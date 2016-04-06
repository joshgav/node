#ifndef SRC_NODE_INTERNALS_H_
#define SRC_NODE_INTERNALS_H_

#include "node.h"
#include "util.h"
#include "util-inl.h"
#include "uv.h"

#include "node_ni.h"

#include <stdint.h>
#include <stdlib.h>

struct sockaddr;

// Variation on NODE_DEFINE_CONSTANT that sets a String value.
#define NODE_DEFINE_STRING_CONSTANT(target, name, constant)                   \
  do {                                                                        \
    Isolate* isolate = target->GetIsolate();                              \
    Local<String> constant_name =                                     \
        String::NewFromUtf8(isolate, name);                               \
    Local<String> constant_value =                                    \
        String::NewFromUtf8(isolate, constant);                           \
    PropertyAttribute constant_attributes =                               \
        static_cast<PropertyAttribute>(ReadOnly | DontDelete);    \
    target->DefineOwnProperty(isolate->GetCurrentContext(),                   \
                              constant_name,                                  \
                              constant_value,                                 \
                              constant_attributes).FromJust();                \
  } while (0)

namespace node {

using namespace node::ni;

// Forward declaration
class Environment;

// If persistent.IsWeak() == false, then do not call persistent.Reset()
// while the returned Local<T> is still in scope, it will destroy the
// reference to the object.
template <class TypeName>
inline Local<TypeName> PersistentToLocal(
    Isolate* isolate,
    const Persistent<TypeName>& persistent);

// Call with valid HandleScope and while inside Context scope.
Local<Value> MakeCallback(Environment* env,
                                   Local<Object> recv,
                                   const char* method,
                                   int argc = 0,
                                   Local<Value>* argv = nullptr);

// Call with valid HandleScope and while inside Context scope.
Local<Value> MakeCallback(Environment* env,
                                   Local<Object> recv,
                                   uint32_t index,
                                   int argc = 0,
                                   Local<Value>* argv = nullptr);

// Call with valid HandleScope and while inside Context scope.
Local<Value> MakeCallback(Environment* env,
                                   Local<Object> recv,
                                   Local<String> symbol,
                                   int argc = 0,
                                   Local<Value>* argv = nullptr);

// Call with valid HandleScope and while inside Context scope.
Local<Value> MakeCallback(Environment* env,
                                   Local<Value> recv,
                                   Local<Function> callback,
                                   int argc = 0,
                                   Local<Value>* argv = nullptr);

// Convert a struct sockaddr to a { address: '1.2.3.4', port: 1234 } JS object.
// Sets address and port properties on the info object and returns it.
// If |info| is omitted, a new object is returned.
Local<Object> AddressToJS(
    Environment* env,
    const sockaddr* addr,
    Local<Object> info = Local<Object>());

template <typename T, int (*F)(const typename T::HandleType*, sockaddr*, int*)>
void GetSockOrPeerName(const FunctionCallbackInfo<Value>& args) {
  T* const wrap = Unwrap<T>(args.Holder());
  CHECK(args[0]->IsObject());
  sockaddr_storage storage;
  int addrlen = sizeof(storage);
  sockaddr* const addr = reinterpret_cast<sockaddr*>(&storage);
  const int err = F(&wrap->handle_, addr, &addrlen);
  if (err == 0)
    AddressToJS(wrap->env(), addr, args[0].As<Object>());
  args.GetReturnValue().Set(err);
}

#ifdef _WIN32
// emulate snprintf() on windows, _snprintf() doesn't zero-terminate the buffer
// on overflow...
// VS 2015 added a standard conform snprintf
#if defined( _MSC_VER ) && (_MSC_VER < 1900)
#include <stdarg.h>
inline static int snprintf(char *buffer, size_t n, const char *format, ...) {
  va_list argp;
  va_start(argp, format);
  int ret = _vscprintf(format, argp);
  vsnprintf_s(buffer, n, _TRUNCATE, format, argp);
  va_end(argp);
  return ret;
}
#endif
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#define arraysize(a) (sizeof(a) / sizeof(*a))  // Workaround for VS 2013.
#else
template <typename T, size_t N>
constexpr size_t arraysize(const T(&)[N]) { return N; }
#endif

#ifndef ROUND_UP
# define ROUND_UP(a, b) ((a) % (b) ? ((a) + (b)) - ((a) % (b)) : (a))
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
# define MUST_USE_RESULT __attribute__((warn_unused_result))
# define NO_RETURN __attribute__((noreturn))
#else
# define MUST_USE_RESULT
# define NO_RETURN
#endif

bool IsExceptionDecorated(Environment* env, Local<Value> er);

void AppendExceptionLine(Environment* env,
                         Local<Value> er,
                         Local<Message> message);

NO_RETURN void FatalError(const char* location, const char* message);

Local<Value> BuildStatsObject(Environment* env, const uv_stat_t* s);

enum Endianness {
  kLittleEndian,  // _Not_ LITTLE_ENDIAN, clashes with endian.h.
  kBigEndian
};

inline enum Endianness GetEndianness() {
  // Constant-folded by the compiler.
  const union {
    uint8_t u8[2];
    uint16_t u16;
  } u = {
    { 1, 0 }
  };
  return u.u16 == 1 ? kLittleEndian : kBigEndian;
}

inline bool IsLittleEndian() {
  return GetEndianness() == kLittleEndian;
}

inline bool IsBigEndian() {
  return GetEndianness() == kBigEndian;
}

// parse index for external array data
inline MUST_USE_RESULT bool ParseArrayIndex(Local<Value> arg,
                                            size_t def,
                                            size_t* ret) {
  if (arg->IsUndefined()) {
    *ret = def;
    return true;
  }

  int32_t tmp_i = arg->Uint32Value();

  if (tmp_i < 0)
    return false;

  *ret = static_cast<size_t>(tmp_i);
  return true;
}

void ThrowError(Isolate* isolate, const char* errmsg);
void ThrowTypeError(Isolate* isolate, const char* errmsg);
void ThrowRangeError(Isolate* isolate, const char* errmsg);
void ThrowErrnoException(Isolate* isolate,
                         int errorno,
                         const char* syscall = nullptr,
                         const char* message = nullptr,
                         const char* path = nullptr);
void ThrowUVException(Isolate* isolate,
                      int errorno,
                      const char* syscall = nullptr,
                      const char* message = nullptr,
                      const char* path = nullptr,
                      const char* dest = nullptr);

class ArrayBufferAllocator : public ArrayBuffer::Allocator {

 public:
  ArrayBufferAllocator() : env_(nullptr) { }

  inline void set_env(Environment* env) { env_ = env; }

  virtual void* Allocate(size_t size);  // Defined in src/node.cc
  virtual void* AllocateUninitialized(size_t size) { return malloc(size); }
  virtual void Free(void* data, size_t) { free(data); }

 private:
  Environment* env_;
};

// Clear any domain and/or uncaughtException handlers to force the error's
// propagation and shutdown the process. Use this to force the process to exit
// by clearing all callbacks that could handle the error.
void ClearFatalExceptionHandlers(Environment* env);

enum NodeInstanceType { MAIN, WORKER };

class NodeInstanceData {
  public:
    NodeInstanceData(NodeInstanceType node_instance_type,
                     uv_loop_t* event_loop,
                     int argc,
                     const char** argv,
                     int exec_argc,
                     const char** exec_argv,
                     bool use_debug_agent_flag)
        : node_instance_type_(node_instance_type),
          exit_code_(1),
          event_loop_(event_loop),
          argc_(argc),
          argv_(argv),
          exec_argc_(exec_argc),
          exec_argv_(exec_argv),
          use_debug_agent_flag_(use_debug_agent_flag) {
      CHECK_NE(event_loop_, nullptr);
    }

    uv_loop_t* event_loop() const {
      return event_loop_;
    }

    int exit_code() {
      CHECK(is_main());
      return exit_code_;
    }

    void set_exit_code(int exit_code) {
      CHECK(is_main());
      exit_code_ = exit_code;
    }

    bool is_main() {
      return node_instance_type_ == MAIN;
    }

    bool is_worker() {
      return node_instance_type_ == WORKER;
    }

    int argc() {
      return argc_;
    }

    const char** argv() {
      return argv_;
    }

    int exec_argc() {
      return exec_argc_;
    }

    const char** exec_argv() {
      return exec_argv_;
    }

    bool use_debug_agent() {
      return is_main() && use_debug_agent_flag_;
    }

  private:
    const NodeInstanceType node_instance_type_;
    int exit_code_;
    uv_loop_t* const event_loop_;
    const int argc_;
    const char** argv_;
    const int exec_argc_;
    const char** exec_argv_;
    const bool use_debug_agent_flag_;

    DISALLOW_COPY_AND_ASSIGN(NodeInstanceData);
};

namespace Buffer {
MaybeLocal<Object> Copy(Environment* env, const char* data, size_t len);
MaybeLocal<Object> New(Environment* env, size_t size);
// Takes ownership of |data|.
MaybeLocal<Object> New(Environment* env,
                               char* data,
                               size_t length,
                               void (*callback)(char* data, void* hint),
                               void* hint);
// Takes ownership of |data|.  Must allocate |data| with malloc() or realloc()
// because ArrayBufferAllocator::Free() deallocates it again with free().
// Mixing operator new and free() is undefined behavior so don't do that.
MaybeLocal<Object> New(Environment* env, char* data, size_t length);
}  // namespace Buffer

}  // namespace node

#endif  // SRC_NODE_INTERNALS_H_
