#ifndef SRC_BASE_OBJECT_H_
#define SRC_BASE_OBJECT_H_

#include "node_ni.h"

namespace node {

using namespace node::ni;

class Environment;

class BaseObject {
 public:
  inline BaseObject(Environment* env, Local<Object> handle);
  inline virtual ~BaseObject();

  // Returns the wrapped object.  Returns an empty handle when
  // persistent.IsEmpty() is true.
  inline Local<Object> object();

  // The parent class is responsible for calling .Reset() on destruction
  // when the persistent handle is strong because there is no way for
  // BaseObject to know when the handle goes out of scope.
  // Weak handles have been reset by the time the destructor runs but
  // calling .Reset() again is harmless.
  inline Persistent<Object>& persistent();

  inline Environment* env() const;

  // The handle_ must have an internal field count > 0, and the first
  // index is reserved for a pointer to this class. This is an
  // implicit requirement, but Node does not have a case where it's
  // required that MakeWeak() be called and the internal field not
  // be set.
  template <typename Type>
  inline void MakeWeak(Type* ptr);

  inline void ClearWeak();

 private:
  BaseObject();

  template <typename Type>
  static inline void WeakCallback(
      const WeakCallbackInfo<Type>& data);

  Persistent<Object> handle_;
  Environment* env_;
};

}  // namespace node

#endif  // SRC_BASE_OBJECT_H_
