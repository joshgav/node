// original source:
// https://github.com/matthewloring/node/blob/c76caae8de9c689db1018506b2fa8d3ac4d38e3e/src/node_platform.cc

#include "node_v8platform.h"

namespace node {
namespace platform {

NodePlatform::NodePlatform(int thread_pool_size, uv_loop_t* loop)
    : threads_(thread_pool_size), loop_(loop) {}

NodePlatform::~NodePlatform() {}

Task* NodePlatform::PopTaskInMainThreadQueue() {
  if (main_thread_queue_.empty()) {
    return NULL;
  }
  Task* task = main_thread_queue_.front();
  main_thread_queue_.pop();
  return task;
}


Task* NodePlatform::PopTaskInMainThreadDelayedQueue() {
  if (main_thread_delayed_queue_.empty()) {
    return NULL;
  }
  double now = MonotonicallyIncreasingTime();
  std::pair<double, Task*> deadline_and_task = main_thread_delayed_queue_.top();
  if (deadline_and_task.first > now) {
    return NULL;
  }
  main_thread_delayed_queue_.pop();
  return deadline_and_task.second;
}

IdleTask* NodePlatform::PopTaskInMainThreadIdleQueue() {
  if (main_thread_idle_queue_.empty()) {
    return NULL;
  }
  IdleTask* task = main_thread_idle_queue_.front();
  main_thread_idle_queue_.pop();
  return task;
}

bool NodePlatform::PumpMessageLoop() {
  Task* task = NULL;
  {
    Mutex::ScopedLock scoped_lock(mutex_);

    // Move delayed tasks that hit their deadline to the main queue.
    task = PopTaskInMainThreadDelayedQueue();
    while (task != NULL) {
      main_thread_queue_.push(task);
      task = PopTaskInMainThreadDelayedQueue();
    }

    task = PopTaskInMainThreadQueue();

    if (task == NULL) {
      return false;
    }
  }
  task->Run();
  delete task;
  return true;
}

bool NodePlatform::RunIdle(double budgetSeconds) {
  IdleTask* task = NULL;
  {
    task = PopTaskInMainThreadIdleQueue();

    if (task == NULL) {
      return false;
    }
  }
  task->Run(MonotonicallyIncreasingTime() + budgetSeconds);
  delete task;
  return true;
}

size_t NodePlatform::NumberOfAvailableBackgroundThreads() {
  return static_cast<size_t>(threads_);
}

void RunTask(uv_work_t *req) {
  Task* task = (Task*) req->data;
  task->Run();
  delete task;
}

void AfterTask(uv_work_t *req, int status) {
  delete req;
}

void NodePlatform::CallOnBackgroundThread(Task* task,
                                          ExpectedRuntime expected_runtime) {
  uv_work_t* req = new uv_work_t;
  req->data = (void*) task;
  uv_queue_work(loop_, req, RunTask, AfterTask);
}

void NodePlatform::CallOnForegroundThread(v8::Isolate* isolate, Task* task) {
  Mutex::ScopedLock scoped_lock(mutex_);
  main_thread_queue_.push(task);
}

void NodePlatform::CallDelayedOnForegroundThread(Isolate* isolate,
                                                 Task* task,
                                                 double delay_in_seconds) {
  Mutex::ScopedLock scoped_lock(mutex_);
  double deadline = MonotonicallyIncreasingTime() + delay_in_seconds;
  main_thread_delayed_queue_.push(std::make_pair(deadline, task));
}

void NodePlatform::CallIdleOnForegroundThread(Isolate* isolate,
                                              IdleTask* task) {
  Mutex::ScopedLock scoped_lock(mutex_);
  main_thread_idle_queue_.push(task);
}

bool NodePlatform::IdleTasksEnabled(Isolate* isolate) { return true; }

double NodePlatform::MonotonicallyIncreasingTime() {
  // Convert nanos to seconds
  return uv_hrtime() / 1000000000.0;
}

const uint8_t* NodePlatform::GetCategoryGroupEnabled(const char* name) {
  if (tracing_controller_) {
    return tracing_controller_->GetCategoryGroupEnabled(name);
  }
  static uint8_t no = 0;
  return &no;
}

const char* NodePlatform::GetCategoryGroupName(
    const uint8_t* category_enabled_flag) {
  static const char dummy[] = "dummy";
  return dummy;
}

uint64_t NodePlatform::AddTraceEvent(
    char phase, const uint8_t* category_enabled_flag, const char* name,
    const char* scope, uint64_t id, uint64_t bind_id, int num_args,
    const char** arg_names, const uint8_t* arg_types,
    const uint64_t* arg_values,
    std::unique_ptr<v8::ConvertableToTraceFormat>* arg_convertables,
    unsigned int flags) {
  if (tracing_controller_) {
    return tracing_controller_->AddTraceEvent(
        phase, category_enabled_flag, name, scope, id, bind_id, num_args,
        arg_names, arg_types, arg_values, arg_convertables, flags);
  }

  return 0;
}

void NodePlatform::UpdateTraceEventDuration(
    const uint8_t* category_enabled_flag, const char* name, uint64_t handle) {
  if (tracing_controller_) {
    tracing_controller_->UpdateTraceEventDuration(category_enabled_flag, name,
                                                  handle);
  }
}

void NodePlatform::SetTracingController(
    TracingController* tracing_controller) {
  tracing_controller_.reset(tracing_controller);
}

void NodePlatform::AddTraceStateObserver(TraceStateObserver* observer) {
  if (!tracing_controller_) return;
  tracing_controller_->AddTraceStateObserver(observer);
}

void NodePlatform::RemoveTraceStateObserver(TraceStateObserver* observer) {
  if (!tracing_controller_) return;
  tracing_controller_->RemoveTraceStateObserver(observer);
}

}  // namespace platform
}  // namespace node
