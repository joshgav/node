#ifndef SRC_TTY_WRAP_H_
#define SRC_TTY_WRAP_H_

#include "env.h"
#include "handle_wrap.h"
#include "stream_wrap.h"
#include "node_ni.h"

namespace node {

using namespace node::ni;

class TTYWrap : public StreamWrap {
 public:
  static void Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context);

  uv_tty_t* UVHandle();

  size_t self_size() const override { return sizeof(*this); }

 private:
  TTYWrap(Environment* env,
          Local<Object> object,
          int fd,
          bool readable);

  static void GuessHandleType(const FunctionCallbackInfo<Value>& args);
  static void IsTTY(const FunctionCallbackInfo<Value>& args);
  static void GetWindowSize(const FunctionCallbackInfo<Value>& args);
  static void SetRawMode(const FunctionCallbackInfo<Value>& args);
  static void New(const FunctionCallbackInfo<Value>& args);

  uv_tty_t handle_;
};

}  // namespace node

#endif  // SRC_TTY_WRAP_H_
