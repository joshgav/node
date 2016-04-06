#ifndef SRC_NODE_STAT_WATCHER_H_
#define SRC_NODE_STAT_WATCHER_H_

#include "node.h"
#include "async-wrap.h"
#include "env.h"
#include "uv.h"
#include "node_ni.h"

namespace node {

using namespace node::ni;

class StatWatcher : public AsyncWrap {
 public:
  virtual ~StatWatcher() override;

  static void Initialize(Environment* env, Local<Object> target);

 protected:
  StatWatcher(Environment* env, Local<Object> wrap);

  static void New(const FunctionCallbackInfo<Value>& args);
  static void Start(const FunctionCallbackInfo<Value>& args);
  static void Stop(const FunctionCallbackInfo<Value>& args);

  size_t self_size() const override { return sizeof(*this); }

 private:
  static void Callback(uv_fs_poll_t* handle,
                       int status,
                       const uv_stat_t* prev,
                       const uv_stat_t* curr);
  void Stop();

  uv_fs_poll_t* watcher_;
};

}  // namespace node
#endif  // SRC_NODE_STAT_WATCHER_H_
