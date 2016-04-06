#ifndef SRC_PIPE_WRAP_H_
#define SRC_PIPE_WRAP_H_

#include "async-wrap.h"
#include "env.h"
#include "stream_wrap.h"

#include "node_ni.h"

namespace node {

using namespace node::ni;

class PipeWrap : public StreamWrap {
 public:
  uv_pipe_t* UVHandle();

  static Local<Object> Instantiate(Environment* env, AsyncWrap* parent);
  static void Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context);

  size_t self_size() const override { return sizeof(*this); }

 private:
  PipeWrap(Environment* env,
           Local<Object> object,
           bool ipc,
           AsyncWrap* parent);

  static void New(const FunctionCallbackInfo<Value>& args);
  static void Bind(const FunctionCallbackInfo<Value>& args);
  static void Listen(const FunctionCallbackInfo<Value>& args);
  static void Connect(const FunctionCallbackInfo<Value>& args);
  static void Open(const FunctionCallbackInfo<Value>& args);

#ifdef _WIN32
  static void SetPendingInstances(
      const FunctionCallbackInfo<Value>& args);
#endif

  static void OnConnection(uv_stream_t* handle, int status);
  static void AfterConnect(uv_connect_t* req, int status);

  uv_pipe_t handle_;
};


}  // namespace node


#endif  // SRC_PIPE_WRAP_H_
