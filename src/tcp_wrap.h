#ifndef SRC_TCP_WRAP_H_
#define SRC_TCP_WRAP_H_

#include "node_ni.h"
#include "async-wrap.h"
#include "env.h"
#include "stream_wrap.h"

namespace node {

using namespace node::ni;

class TCPWrap : public StreamWrap {
 public:
  static Local<Object> Instantiate(Environment* env, AsyncWrap* parent);
  static void Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context);

  uv_tcp_t* UVHandle();

  size_t self_size() const override { return sizeof(*this); }

 private:
  typedef uv_tcp_t HandleType;

  template <typename T,
            int (*F)(const typename T::HandleType*, sockaddr*, int*)>
  friend void GetSockOrPeerName(const FunctionCallbackInfo<Value>&);

  TCPWrap(Environment* env, Local<Object> object, AsyncWrap* parent);
  ~TCPWrap();

  static void New(const FunctionCallbackInfo<Value>& args);
  static void SetNoDelay(const FunctionCallbackInfo<Value>& args);
  static void SetKeepAlive(const FunctionCallbackInfo<Value>& args);
  static void Bind(const FunctionCallbackInfo<Value>& args);
  static void Bind6(const FunctionCallbackInfo<Value>& args);
  static void Listen(const FunctionCallbackInfo<Value>& args);
  static void Connect(const FunctionCallbackInfo<Value>& args);
  static void Connect6(const FunctionCallbackInfo<Value>& args);
  static void Open(const FunctionCallbackInfo<Value>& args);

#ifdef _WIN32
  static void SetSimultaneousAccepts(
      const FunctionCallbackInfo<Value>& args);
#endif

  static void OnConnection(uv_stream_t* handle, int status);
  static void AfterConnect(uv_connect_t* req, int status);

  uv_tcp_t handle_;
};


}  // namespace node


#endif  // SRC_TCP_WRAP_H_
