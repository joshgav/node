// original source: https://github.com/matthewloring/node/blob/c76caae8de9c689db1018506b2fa8d3ac4d38e3e/src/node_platform.h

#ifndef SRC_NODE_V8PLATFORM_H_
#define SRC_NODE_V8PLATFORM_H_

#include "node_platform.h"
#include "node_mutex.h"

#include "libplatform/libplatform.h"
#include "uv.h"

#include <queue>

namespace node {
namespace platform {

using v8::platform::tracing::TracingController;
using v8::IdleTask;
using v8::Isolate;
using v8::Platform;
using v8::Task;

class NodePlatform : public node::platform::Platform {
 public:
  NodePlatform(int thread_pool_size, uv_loop_t* loop);
  virtual ~NodePlatform();

  bool PumpMessageLoop();

  bool RunIdle(double budgetSeconds);

  // v8::Platform implementation.
  size_t NumberOfAvailableBackgroundThreads() override;
  void CallOnBackgroundThread(Task* task,
                              ExpectedRuntime expected_runtime) override;
  void CallOnForegroundThread(v8::Isolate* isolate, Task* task) override;
  void CallDelayedOnForegroundThread(Isolate* isolate, Task* task,
                                     double delay_in_seconds) override;
  void CallIdleOnForegroundThread(Isolate* isolate, IdleTask* task) override;
  bool IdleTasksEnabled(Isolate* isolate) override;
  double MonotonicallyIncreasingTime() override;
  const uint8_t* GetCategoryGroupEnabled(const char* name) override;
  const char* GetCategoryGroupName(
      const uint8_t* category_enabled_flag) override;
  uint64_t AddTraceEvent(
      char phase, const uint8_t* category_enabled_flag, const char* name,
      const char* scope, uint64_t id, uint64_t bind_id, int32_t num_args,
      const char** arg_names, const uint8_t* arg_types,
      const uint64_t* arg_values,
      std::unique_ptr<v8::ConvertableToTraceFormat>* arg_convertables,
      unsigned int flags) override;
  void UpdateTraceEventDuration(const uint8_t* category_enabled_flag,
                                const char* name, uint64_t handle) override;
  void SetTracingController(TracingController* tracing_controller);

  void AddTraceStateObserver(TraceStateObserver* observer) override;
  void RemoveTraceStateObserver(TraceStateObserver* observer) override;

 private:
  Task* PopTaskInMainThreadQueue();
  Task* PopTaskInMainThreadDelayedQueue();
  IdleTask* PopTaskInMainThreadIdleQueue();

  int threads_;
  uv_loop_t* loop_;

  Mutex mutex_;

  std::queue<Task*> main_thread_queue_;

  typedef std::pair<double, Task*> DelayedEntry;
  std::priority_queue<DelayedEntry, std::vector<DelayedEntry>,
                               std::greater<DelayedEntry> > 
      main_thread_delayed_queue_;

  std::queue<IdleTask*> main_thread_idle_queue_;

  std::unique_ptr<TracingController> tracing_controller_;
};

}  // namespace platform
}  // namespace node

#endif  // SRC_NODE_PLATFORM_H_
