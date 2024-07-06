// desc: napi_ih means "napi inheritance helper" which is a pitch to
//       napi to support inheritance and try to keep the style of napi.
//       The implementation is inspired by
//       https://github.com/ajihyf/node-addon-api-helper and
//       https://mmomtchev.medium.com/c-class-inheritance-with-node-api-and-node-addon-api-c180334d9902

// WARN: This is a VERY SIMPLE TRY to make an inheritance patch of napi, and
//       this only implements necessary apis for JSAudio-Android project!
//       This patch doesn't support multi-thread, does not compatiable with
//       origin NAPI_API_MODULE() api and does not support multi-inheritance
//       this is designed to only be used in JSAudio PROJECT!
//       If you want to use napi with inheritance more safe and realiable,
//       please try https://github.com/ajihyf/node-addon-api-helper

#ifndef NAPI_IH_HPP
#define NAPI_IH_HPP

#include "logger.hpp"
#include <functional>
#include <map>
#include <napi/napi.h>

namespace Napi_IH {
class MethodWrapper;
class IHObjectWrap;

class IHCallbackInfo {
public:
  IHCallbackInfo(const Napi::CallbackInfo &info, void *user_data = nullptr);

  NAPI_DISALLOW_ASSIGN_COPY(IHCallbackInfo)

  Napi::Env Env() const;
  Napi::Value NewTarget() const;
  bool IsConstructCall() const;
  size_t Length() const;
  const Napi::Value operator[](size_t index) const;
  Napi::Value This() const;
  void *Data() const;
  const Napi::CallbackInfo &InnerCallbackInfo() const;

private:
  const Napi::CallbackInfo &info_;
  void *user_data_;
};

using ConstructorFunc =
    std::unique_ptr<IHObjectWrap> (*)(const Napi_IH::IHCallbackInfo &);

struct ClassMetaInfo {
  const char *name = "";
  ClassMetaInfo *parent = nullptr;
  ConstructorFunc ctor;
  std::vector<Napi::ClassPropertyDescriptor<MethodWrapper>> descriptors;
  void *data = nullptr;
};

class Registration {
public:
  static void StartRegistration(Napi::Env env, Napi::Object exports);
  static void AddClassMetaInfo(const char *name, ClassMetaInfo *info);
  template <typename T> static Napi::Function FindClass();

private:
  static void ProcessClassMetaInfo(
      Napi::Env env, Napi::Object exports, ClassMetaInfo *info,
      std::map<ClassMetaInfo *, Napi::FunctionReference> &ctor_table);
  static void ClearClassMetaInfoMap(ClassMetaInfo *info);

  static std::map<const char *, ClassMetaInfo *> meta_info_table_;
  static std::map<ClassMetaInfo *, Napi::FunctionReference> ctor_table_;
};

class IHObjectWrap {
  friend Registration;

public:
  IHObjectWrap(const Napi_IH::IHCallbackInfo &info);

  using PropertyDescriptor = Napi::ClassPropertyDescriptor<MethodWrapper>;
  using NonBase = IHObjectWrap;

  template <typename T>
  static void
  DefineClass(Napi::Env env, const char *utf8name,
              const std::initializer_list<PropertyDescriptor> &properties,
              void *data = nullptr);

  template <typename T>
  static void DefineClass(Napi::Env env, const char *utf8name,
                          const std::vector<PropertyDescriptor> &properties,
                          void *data = nullptr);

  template <typename T, typename Base>
  static void
  DefineClass(Napi::Env env, const char *utf8name,
              const std::initializer_list<PropertyDescriptor> &properties,
              void *data = nullptr);

  template <typename T, typename Base>
  static void DefineClass(Napi::Env env, const char *utf8name,
                          const std::vector<PropertyDescriptor> &properties,
                          void *data = nullptr);
  template <typename T>
  using InstanceVoidMethodCallback = void (T::*)(const Napi::CallbackInfo &);
  template <typename T>
  using InstanceMethodCallback = Napi::Value (T::*)(const Napi::CallbackInfo &);
  template <typename T>
  using InstanceGetterCallback = Napi::Value (T::*)(const Napi::CallbackInfo &);
  template <typename T>
  using InstanceSetterCallback = void (T::*)(const Napi::CallbackInfo &,
                                             const Napi::Value &);
  template <typename T>
  static PropertyDescriptor
  InstanceMethod(const char *utf8name, InstanceMethodCallback<T> method,
                 napi_property_attributes attributes = napi_default,
                 void *data = nullptr);
  template <typename T, InstanceGetterCallback<T> getter,
            InstanceSetterCallback<T> setter = nullptr>
  static PropertyDescriptor
  InstanceAccessor(const char *utf8name,
                   napi_property_attributes attributes = napi_default,
                   void *data = nullptr);
  template <typename T> static Napi::Function FindClass();

private:
  template <typename T, typename Base = NonBase>
  static void DefineClass(Napi::Env env, const char *utf8name,
                          const size_t props_count,
                          const napi_property_descriptor *descriptors,
                          void *data = nullptr);

protected:
  static Napi::FunctionReference js_class_constructor_;
  static ClassMetaInfo class_meta_info_;
  std::function<void *(Napi::CallbackInfo &info)> init_parent_;
};

} // namespace Napi_IH

#define NAPI_IH_API_MODULE(modname, regfunc)                                   \
  static napi_value __napi_##regfunc(napi_env env, napi_value exports) {       \
    napi_value ret = Napi::RegisterModule(env, exports, regfunc);              \
    Napi::details::WrapCallback([&] {                                          \
      Napi_IH::Registration::StartRegistration(Napi::Env(env),                 \
                                               Napi::Object(env, exports));    \
      return nullptr;                                                          \
    });                                                                        \
    return ret;                                                                \
  }                                                                            \
  NAPI_MODULE(modname, __napi_##regfunc)

#include "napi_ih_inl.hpp"
#endif // NAPI_IH_HPP