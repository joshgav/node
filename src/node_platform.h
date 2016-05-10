#ifndef SRC_NODE_PLATFORM_H_
#define SRC_NODE_PLATFORM_H_

#include "v8-platform.h"
#include "src/libplatform/default-platform.h"  // is this okay?

namespace node {
namespace platform {

v8::Platform* CreateNodePlatform(int thread_pool_size);

class NodePlatform : public v8::platform::DefaultPlatform {

public: 
  NodePlatform();
  ~NodePlatform();

  const uint8_t* GetCategoryGroupEnabled(const char* name) override;
  const char* GetCategoryGroupName(
      const uint8_t* category_enabled_flag) override;
  uint64_t AddTraceEvent(char phase, const uint8_t* category_enabled_flag,
                         const char* name, /* const char* scope, */ uint64_t id,
                         uint64_t bind_id, int32_t num_args,
                         const char** arg_names, const uint8_t* arg_types,
                         const uint64_t* arg_values,
                         unsigned int flags) override;
  void UpdateTraceEventDuration(const uint8_t* category_enabled_flag,
                                const char* name, uint64_t handle) override;

  DISALLOW_COPY_AND_ASSIGN(NodePlatform);
};

} // namespace platform
} // namespace node

#endif // SRC_NODE_PLATFORM_H_

