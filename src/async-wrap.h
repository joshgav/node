#ifndef SRC_ASYNC_WRAP_H_
#define SRC_ASYNC_WRAP_H_

#include "base-object.h"
#include "node_ni.h"
#include <stdint.h>

namespace node {

using namespace node::ni;

#define NODE_ASYNC_ID_OFFSET 0xA1C

#define NODE_ASYNC_PROVIDER_TYPES(V)                                          \
  V(NONE)                                                                     \
  V(CRYPTO)                                                                   \
  V(FSEVENTWRAP)                                                              \
  V(FSREQWRAP)                                                                \
  V(GETADDRINFOREQWRAP)                                                       \
  V(GETNAMEINFOREQWRAP)                                                       \
  V(HTTPPARSER)                                                               \
  V(JSSTREAM)                                                                 \
  V(PIPEWRAP)                                                                 \
  V(PIPECONNECTWRAP)                                                          \
  V(PROCESSWRAP)                                                              \
  V(QUERYWRAP)                                                                \
  V(SHUTDOWNWRAP)                                                             \
  V(SIGNALWRAP)                                                               \
  V(STATWATCHER)                                                              \
  V(TCPWRAP)                                                                  \
  V(TCPCONNECTWRAP)                                                           \
  V(TIMERWRAP)                                                                \
  V(TLSWRAP)                                                                  \
  V(TTYWRAP)                                                                  \
  V(UDPWRAP)                                                                  \
  V(UDPSENDWRAP)                                                              \
  V(WRITEWRAP)                                                                \
  V(ZLIB)

class Environment;

class AsyncWrap : public BaseObject {
 public:
  enum ProviderType {
#define V(PROVIDER)                                                           \
    PROVIDER_ ## PROVIDER,
    NODE_ASYNC_PROVIDER_TYPES(V)
#undef V
  };

  inline AsyncWrap(Environment* env,
                   Local<Object> object,
                   ProviderType provider,
                   AsyncWrap* parent = nullptr);

  inline virtual ~AsyncWrap();

  inline ProviderType provider_type() const;

  inline int64_t get_uid() const;

  // Only call these within a valid HandleScope.
  Local<Value> MakeCallback(const Local<Function> cb,
                                     int argc,
                                     Local<Value>* argv);
  inline Local<Value> MakeCallback(const Local<String> symbol,
                                            int argc,
                                            Local<Value>* argv);
  inline Local<Value> MakeCallback(uint32_t index,
                                            int argc,
                                            Local<Value>* argv);

  virtual size_t self_size() const = 0;

 private:
  inline AsyncWrap();
  inline bool ran_init_callback() const;

  // When the async hooks init JS function is called from the constructor it is
  // expected the context object will receive a _asyncQueue object property
  // that will be used to call pre/post in MakeCallback.
  uint32_t bits_;
  const int64_t uid_;
};

void LoadAsyncWrapperInfo(Environment* env);

}  // namespace node


#endif  // SRC_ASYNC_WRAP_H_
