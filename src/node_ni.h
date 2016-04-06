#ifndef SRC_NODE_NI_H_
#define SRC_NODE_NI_H_

#include "v8.h"
#include "v8-debug.h"
#include "v8-platform.h"
#include "v8-profiler.h"
#include "libplatform/libplatform.h"

namespace node {
namespace ni {

using v8::Array;
using v8::ArrayBuffer;
using v8::Boolean;
using v8::False;
using v8::Function;
using v8::Int32;
using v8::Integer;
using v8::Name;
using v8::None;
using v8::Null;
using v8::Number;
using v8::Object;
using v8::PropertyAttribute;
using v8::Script;
using v8::String;
using v8::True;
using v8::Uint32;
using v8::Uint32Array;
using v8::Uint8Array;
using v8::Undefined;
using v8::Value;

using v8::AccessType;
using v8::AccessorSignature;
using v8::Context;
using v8::EscapableHandleScope;
using v8::Eternal;
using v8::Exception;
using v8::External;
using v8::FunctionCallback;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Isolate;
using v8::JitCodeEvent;
using v8::Local;
using v8::Locker;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Message;
using v8::NamedPropertyHandlerConfiguration;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::Platform;
using v8::Private;
using v8::Promise;
using v8::PromiseRejectMessage;
using v8::PropertyCallbackInfo;
using v8::ScriptCompiler;
using v8::ScriptOrigin;
using v8::SealHandleScope;
using v8::Signature;
using v8::TryCatch;
using v8::UnboundScript;
using v8::V8;
using v8::WeakCallbackInfo;
using v8::WeakCallbackType;

using v8::CpuProfiler;
using v8::Debug;
using v8::GCCallbackFlags;
using v8::GCType;
using v8::HeapProfiler;
using v8::HeapSpaceStatistics;
using v8::HeapStatistics;
using v8::RetainedObjectInfo;
using v8::StackFrame;
using v8::StackTrace;

using v8::ArrayBufferCreationMode;
using v8::NewStringType;
using v8::ReadOnly;
using v8::kPromiseRejectWithNoHandler;
using v8::kPromiseHandlerAddedAfterReject;
using v8::DontEnum;
using v8::DontDelete;
using v8::DEFAULT;

}
}

#endif  // SRC_NODE_NI_H_
