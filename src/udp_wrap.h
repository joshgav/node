#ifndef SRC_UDP_WRAP_H_
#define SRC_UDP_WRAP_H_

#include "async-wrap.h"
#include "env.h"
#include "handle_wrap.h"
#include "req-wrap.h"
#include "req-wrap-inl.h"
#include "uv.h"
#include "node_ni.h"

namespace node {

using namespace node::ni;

class UDPWrap: public HandleWrap {
 public:
  static void Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context);
  static void GetFD(Local<String>,
                    const PropertyCallbackInfo<Value>&);
  static void New(const FunctionCallbackInfo<Value>& args);
  static void Bind(const FunctionCallbackInfo<Value>& args);
  static void Send(const FunctionCallbackInfo<Value>& args);
  static void Bind6(const FunctionCallbackInfo<Value>& args);
  static void Send6(const FunctionCallbackInfo<Value>& args);
  static void RecvStart(const FunctionCallbackInfo<Value>& args);
  static void RecvStop(const FunctionCallbackInfo<Value>& args);
  static void GetSockName(const FunctionCallbackInfo<Value>& args);
  static void AddMembership(const FunctionCallbackInfo<Value>& args);
  static void DropMembership(const FunctionCallbackInfo<Value>& args);
  static void SetMulticastTTL(const FunctionCallbackInfo<Value>& args);
  static void SetMulticastLoopback(
      const FunctionCallbackInfo<Value>& args);
  static void SetBroadcast(const FunctionCallbackInfo<Value>& args);
  static void SetTTL(const FunctionCallbackInfo<Value>& args);

  static Local<Object> Instantiate(Environment* env, AsyncWrap* parent);
  uv_udp_t* UVHandle();

  size_t self_size() const override { return sizeof(*this); }

 private:
  typedef uv_udp_t HandleType;

  template <typename T,
            int (*F)(const typename T::HandleType*, sockaddr*, int*)>
  friend void GetSockOrPeerName(const FunctionCallbackInfo<Value>&);

  UDPWrap(Environment* env, Local<Object> object, AsyncWrap* parent);

  static void DoBind(const FunctionCallbackInfo<Value>& args,
                     int family);
  static void DoSend(const FunctionCallbackInfo<Value>& args,
                     int family);
  static void SetMembership(const FunctionCallbackInfo<Value>& args,
                            uv_membership membership);

  static void OnAlloc(uv_handle_t* handle,
                      size_t suggested_size,
                      uv_buf_t* buf);
  static void OnSend(uv_udp_send_t* req, int status);
  static void OnRecv(uv_udp_t* handle,
                     ssize_t nread,
                     const uv_buf_t* buf,
                     const struct sockaddr* addr,
                     unsigned int flags);

  uv_udp_t handle_;
};

}  // namespace node

#endif  // SRC_UDP_WRAP_H_
