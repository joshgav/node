#include "node_platform.h"
#include "trace/trace_broker.h"

#include "v8.h"
#include "v8-platform.h"
#include "src/libplatform/default-platform.h"
#include "src/tracing/trace-event.h"

#include <map>
#include <string>

namespace node {
namespace platform {

v8::Platform* CreateNodePlatform(int thread_pool_size) {
  NodePlatform* platform = new NodePlatform();
  platform->SetThreadPoolSize(thread_pool_size);
  platform->EnsureInitialized();
  return platform;
}

NodePlatform::NodePlatform() { }

NodePlatform::~NodePlatform() { }

uint64_t NodePlatform::AddTraceEvent(
    char phase, const uint8_t* category_enabled_flag, const char* name,
    /* const char* scope, */ uint64_t id, uint64_t bind_id, int num_args,
    const char** arg_names, const uint8_t* arg_types,
    const uint64_t* arg_values, unsigned int flags) {
    
  std::map<const char*, const char*> trace_map {};

  // TODO iterate through num_args and cast arg_values to corresponding arg_types
  //      then associate with corresponding arg_names
  v8::internal::tracing::TraceValueUnion type_value; 
  type_value.as_uint = arg_values[0];
  const char* arg_value = type_value.as_string;
  trace_map[arg_names[0]] = arg_value;

  diag::trace::TraceBroker::Singleton()->Trace(name, trace_map);
  return 0;
}


void NodePlatform::UpdateTraceEventDuration(
    const uint8_t* category_enabled_flag, const char* name, uint64_t handle) {}


const uint8_t* NodePlatform::GetCategoryGroupEnabled(const char* name) {
  // this is a hack for now, should be able to use strcmp?
  std::string str_name(name);
  std::size_t found = str_name.find("v8");
  std::size_t found2 = str_name.find("V8");

  static uint8_t no = 0;
  static uint8_t yes = 1;
  if ((found != std::string::npos) || (found2 != std::string::npos)) {
    return &no;
  } else {
    return &yes;
  }
}


const char* NodePlatform::GetCategoryGroupName(
    const uint8_t* category_enabled_flag) {
  static const char dummy[] = "dummy";
  return dummy;
}

} // namespace platform
} // namepsace node
