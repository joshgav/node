#ifndef SRC_NODE_DTRACE_H_
#define SRC_NODE_DTRACE_H_

#include "node.h"
#include "env.h"
#include "node_ni.h"

extern "C" {
/*
 * The following structures are passed directly to DTrace when probes are fired.
 * Translators in node.d translate these structures into the corresponding D
 * structures, taking care of dealing with the user process data model (32-bit
 * or 64-bit) and structure versions (see node_dtrace_http_server_request_t
 * below).
 */

typedef struct {
  int32_t fd;
  int32_t port;
  char* remote;
  int32_t buffered;
} node_dtrace_connection_t;

typedef struct {
  char* url;
  char* method;
} node_dtrace_http_client_request_t;

/*
 * The original version of this structure contained only a url and method, just
 * like the client request above.  To add the new forwardedFor field, the
 * structure layout was changed to begin with an integer version.  The
 * translator knows whether it's looking at an old- or new-version structure
 * based on whether the version field's value is a reasonable pointer (i.e.
 * address greater than 4K).  No doubt this is filthy, but there's not much else
 * we can do, and it works reliably.
 *
 * This version of the structure also contains padding that should be zeroed out
 * by the consumer so that future versions of the translator can simply check if
 * a field is present by checking it against nullptr.
 */
typedef struct {
  union {
    uint32_t version;
    uintptr_t unused;  /* for compat. with old 64-bit struct */
  } _un;
  char* url;
  char* method;
  char* forwardedFor;
  char* _pad[8];
} node_dtrace_http_server_request_t;

}  // extern "C"

namespace node {

using namespace node::ni;

void InitDTrace(Environment* env, Local<Object> target);

}  // namespace node

#endif  // SRC_NODE_DTRACE_H_
