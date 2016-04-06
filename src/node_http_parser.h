#ifndef SRC_NODE_HTTP_PARSER_H_
#define SRC_NODE_HTTP_PARSER_H_

#include "http_parser.h"
#include "node_ni.h"

namespace node {

using namespace node::ni;

void InitHttpParser(Local<Object> target);

}  // namespace node

#endif  // SRC_NODE_HTTP_PARSER_H_
