#ifndef SRC_ASYNC_WRAP_INL_H_
#define SRC_ASYNC_WRAP_INL_H_

#include "node_ni.h"

#include "async-wrap.h"
#include "base-object.h"
#include "base-object-inl.h"
#include "env.h"
#include "env-inl.h"
#include "node_internals.h"
#include "util.h"
#include "util-inl.h"

namespace node {

using namespace node::ni;

inline AsyncWrap::AsyncWrap(Environment* env,
                            Local<Object> object,
                            ProviderType provider,
                            AsyncWrap* parent)
    : BaseObject(env, object), bits_(static_cast<uint32_t>(provider) << 1),
      uid_(env->get_async_wrap_uid()) {
  CHECK_NE(provider, PROVIDER_NONE);
  CHECK_GE(object->InternalFieldCount(), 1);

  // Shift provider value over to prevent id collision.
  persistent().SetWrapperClassId(NODE_ASYNC_ID_OFFSET + provider);

  Local<Function> init_fn = env->async_hooks_init_function();

  // No init callback exists, no reason to go on.
  if (init_fn.IsEmpty())
    return;

  // If async wrap callbacks are disabled and no parent was passed that has
  // run the init callback then return.
  if (!env->async_wrap_callbacks_enabled() &&
      (parent == nullptr || !parent->ran_init_callback()))
    return;

  HandleScope scope(env->isolate());

  Local<Value> argv[] = {
    Integer::New(env->isolate(), get_uid()),
    Int32::New(env->isolate(), provider),
    Null(env->isolate()),
    Null(env->isolate())
  };

  if (parent != nullptr) {
    argv[2] = Integer::New(env->isolate(), parent->get_uid());
    argv[3] = parent->object();
  }

  TryCatch try_catch(env->isolate());

  MaybeLocal<Value> ret =
      init_fn->Call(env->context(), object, arraysize(argv), argv);

  if (ret.IsEmpty()) {
    ClearFatalExceptionHandlers(env);
    FatalException(env->isolate(), try_catch);
  }

  bits_ |= 1;  // ran_init_callback() is true now.
}


inline AsyncWrap::~AsyncWrap() {
  if (!ran_init_callback())
    return;

  Local<Function> fn = env()->async_hooks_destroy_function();
  if (!fn.IsEmpty()) {
    HandleScope scope(env()->isolate());
    Local<Value> uid = Integer::New(env()->isolate(), get_uid());
    TryCatch try_catch(env()->isolate());
    MaybeLocal<Value> ret =
        fn->Call(env()->context(), Null(env()->isolate()), 1, &uid);
    if (ret.IsEmpty()) {
      ClearFatalExceptionHandlers(env());
      FatalException(env()->isolate(), try_catch);
    }
  }
}


inline bool AsyncWrap::ran_init_callback() const {
  return static_cast<bool>(bits_ & 1);
}


inline AsyncWrap::ProviderType AsyncWrap::provider_type() const {
  return static_cast<ProviderType>(bits_ >> 1);
}


inline int64_t AsyncWrap::get_uid() const {
  return uid_;
}


inline Local<Value> AsyncWrap::MakeCallback(
    const Local<String> symbol,
    int argc,
    Local<Value>* argv) {
  Local<Value> cb_v = object()->Get(symbol);
  CHECK(cb_v->IsFunction());
  return MakeCallback(cb_v.As<Function>(), argc, argv);
}


inline Local<Value> AsyncWrap::MakeCallback(
    uint32_t index,
    int argc,
    Local<Value>* argv) {
  Local<Value> cb_v = object()->Get(index);
  CHECK(cb_v->IsFunction());
  return MakeCallback(cb_v.As<Function>(), argc, argv);
}

}  // namespace node

#endif  // SRC_ASYNC_WRAP_INL_H_
