#ifndef SRC_JS_STREAM_H_
#define SRC_JS_STREAM_H_

#include "async-wrap.h"
#include "env.h"
#include "stream_base.h"

#include "node_ni.h"

namespace node {

using namespace node::ni;

class JSStream : public StreamBase, public AsyncWrap {
 public:
  static void Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context);

  ~JSStream();

  void* Cast() override;
  bool IsAlive() override;
  bool IsClosing() override;
  int ReadStart() override;
  int ReadStop() override;

  int DoShutdown(ShutdownWrap* req_wrap) override;
  int DoWrite(WriteWrap* w,
              uv_buf_t* bufs,
              size_t count,
              uv_stream_t* send_handle) override;

  size_t self_size() const override { return sizeof(*this); }

 protected:
  JSStream(Environment* env, Local<Object> obj, AsyncWrap* parent);

  AsyncWrap* GetAsyncWrap() override;

  static void New(const FunctionCallbackInfo<Value>& args);
  static void DoAlloc(const FunctionCallbackInfo<Value>& args);
  static void DoRead(const FunctionCallbackInfo<Value>& args);
  static void DoAfterWrite(const FunctionCallbackInfo<Value>& args);
  static void ReadBuffer(const FunctionCallbackInfo<Value>& args);
  static void EmitEOF(const FunctionCallbackInfo<Value>& args);

  template <class Wrap>
  static void Finish(const FunctionCallbackInfo<Value>& args);
};

}  // namespace node

#endif  // SRC_JS_STREAM_H_
