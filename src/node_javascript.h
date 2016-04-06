#ifndef SRC_NODE_JAVASCRIPT_H_
#define SRC_NODE_JAVASCRIPT_H_

#include "env.h"
#include "node_ni.h"

namespace node {

using namespace node::ni;

void DefineJavaScript(Environment* env, Local<Object> target);
Local<String> MainSource(Environment* env);

}  // namespace node

#endif  // SRC_NODE_JAVASCRIPT_H_
