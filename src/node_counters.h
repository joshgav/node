#ifndef SRC_NODE_COUNTERS_H_
#define SRC_NODE_COUNTERS_H_

#include "node.h"

#ifdef HAVE_PERFCTR
#include "node_win32_perfctr_provider.h"
#else
#define NODE_COUNTER_ENABLED() (false)
#define NODE_COUNT_GC_PERCENTTIME(percent) do { } while (false)
#define NODE_COUNT_GET_GC_RAWTIME() do { } while (false)
#define NODE_COUNT_HTTP_CLIENT_REQUEST() do { } while (false)
#define NODE_COUNT_HTTP_CLIENT_RESPONSE() do { } while (false)
#define NODE_COUNT_HTTP_SERVER_REQUEST() do { } while (false)
#define NODE_COUNT_HTTP_SERVER_RESPONSE() do { } while (false)
#define NODE_COUNT_NET_BYTES_RECV(bytes) do { } while (false)
#define NODE_COUNT_NET_BYTES_SENT(bytes) do { } while (false)
#define NODE_COUNT_PIPE_BYTES_RECV(bytes) do { } while (false)
#define NODE_COUNT_PIPE_BYTES_SENT(bytes) do { } while (false)
#define NODE_COUNT_SERVER_CONN_CLOSE() do { } while (false)
#define NODE_COUNT_SERVER_CONN_OPEN() do { } while (false)
#endif

#include "env.h"
#include "node_ni.h"

namespace node {

using namespace node::ni;

void InitPerfCounters(Environment* env, Local<Object> target);
void TermPerfCounters(Local<Object> target);

}  // namespace node

#endif  // SRC_NODE_COUNTERS_H_
