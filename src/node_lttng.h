#ifndef SRC_NODE_LTTNG_H_
#define SRC_NODE_LTTNG_H_

#include "node.h"
#include "env.h"
#include "node_ni.h"

extern "C" {
typedef struct {
  int32_t fd;
  int32_t port;
  const char* remote;
  int32_t buffered;
} node_lttng_connection_t;

typedef struct {
  const char* url;
  const char* method;
} node_lttng_http_client_request_t;

typedef struct {
  union {
    uint32_t version;
    uintptr_t unused;  /* for compat. with old 64-bit struct */
  } _un;
  const char* url;
  const char* method;
  const char* forwarded_for;
  const char* _pad[8];
} node_lttng_http_server_request_t;

}  // extern "C"

namespace node {

using namespace node::ni;

void InitLTTNG(Environment* env, Local<Object> target);

}  // namespace node

#endif  // SRC_NODE_LTTNG_H_
