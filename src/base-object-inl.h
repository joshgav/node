#ifndef SRC_BASE_OBJECT_INL_H_
#define SRC_BASE_OBJECT_INL_H_

#include "base-object.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"
#include "util-inl.h"
#include "node_ni.h"

namespace node {

using namespace node::ni;

inline BaseObject::BaseObject(Environment* env, Local<Object> handle)
    : handle_(env->isolate(), handle),
      env_(env) {
  CHECK_EQ(false, handle.IsEmpty());
}


inline BaseObject::~BaseObject() {
  CHECK(handle_.IsEmpty());
}


inline Persistent<Object>& BaseObject::persistent() {
  return handle_;
}


inline Local<Object> BaseObject::object() {
  return PersistentToLocal(env_->isolate(), handle_);
}


inline Environment* BaseObject::env() const {
  return env_;
}


template <typename Type>
inline void BaseObject::WeakCallback(
    const WeakCallbackInfo<Type>& data) {
  Type* self = data.GetParameter();
  self->persistent().Reset();
  delete self;
}


template <typename Type>
inline void BaseObject::MakeWeak(Type* ptr) {
  HandleScope scope(env_->isolate());
  Local<Object> handle = object();
  CHECK_GT(handle->InternalFieldCount(), 0);
  Wrap(handle, ptr);
  handle_.MarkIndependent();
  handle_.SetWeak<Type>(ptr, WeakCallback<Type>,
                        WeakCallbackType::kParameter);
}


inline void BaseObject::ClearWeak() {
  handle_.ClearWeak();
}

}  // namespace node

#endif  // SRC_BASE_OBJECT_INL_H_
