#ifndef SRC_NODE_FILE_H_
#define SRC_NODE_FILE_H_

#include "node.h"
#include "node_ni.h"


namespace node {

using namespace node::ni;

void InitFs(Local<Object> target);

}  // namespace node

#endif  // SRC_NODE_FILE_H_
