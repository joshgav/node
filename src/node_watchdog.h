#ifndef SRC_NODE_WATCHDOG_H_
#define SRC_NODE_WATCHDOG_H_

#include "uv.h"
#include "node_ni.h"

namespace node {

using namespace node::ni;

class Watchdog {
 public:
  explicit Watchdog(Isolate* isolate, uint64_t ms);
  ~Watchdog();

  void Dispose();

  Isolate* isolate() { return isolate_; }

 private:
  void Destroy();

  static void Run(void* arg);
  static void Async(uv_async_t* async);
  static void Timer(uv_timer_t* timer);

  Isolate* isolate_;
  uv_thread_t thread_;
  uv_loop_t* loop_;
  uv_async_t async_;
  uv_timer_t timer_;
  bool destroyed_;
};

}  // namespace node

#endif  // SRC_NODE_WATCHDOG_H_
