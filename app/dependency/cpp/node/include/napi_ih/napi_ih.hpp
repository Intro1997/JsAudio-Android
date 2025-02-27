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

#include <napi/napi.h>

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

#include "logger.hpp"

namespace Napi_IH {
class MethodWrapper;
class IHObjectWrap;

enum class ClassVisibility {
  kDefault = 0,
  kHideType,
  kHideConstructor,
};

#define NAPI_IH_VERIFY_INSTANCE_OF(napi_obj, napi_type_name) \
  (VerifyExactInstanceType(napi_obj, #napi_type_name) ||     \
   VerifyInstanceOf<JS##napi_type_name>(napi_obj))

inline bool VerifyExactInstanceType(const Napi::Object &object,
                                    const std::string &napi_type_name);
template <typename T>
inline bool VerifyInstanceOf(const Napi::Object &object);

class Error {
 public:
  template <typename... Args>
  static Napi::Error New(Napi::Env env, const char *format, Args... args);

 protected:
  enum class ErrorType { kError = 0, kTypeError = 1, kRangeError = 2 };

  template <typename T>
  static T InnerNew(Napi::Env env, const char *msg);

  template <typename... Args>
  static std::string string_format(const char *format, Args... args);
};
class TypeError : public Error {
 public:
  template <typename... Args>
  static Napi::TypeError New(Napi::Env env, const char *format, Args... args);
};
class RangeError : public Error {
 public:
  template <typename... Args>
  static Napi::RangeError New(Napi::Env env, const char *format, Args... args);
};

class FunctionWrapper {
 public:
  FunctionWrapper() = default;
  FunctionWrapper(const Napi::Function &function);

  // If you want use more member functions of
  // Napi::Function, add them to here, then
  // call them via Napi::Function function_;

  // WARN: All Args here must be default args
  // such as JSAudioNode(const Napi::CallbackInfo& info, int a = 0, char * name
  // = nullptr);
  // the args will be 'a' and 'name'. We can't use this function
  // with the constructor has non-default value
  template <typename T, typename... Args>
  Napi::MaybeOrValue<Napi::Object> NewWithArgs(
      const std::initializer_list<napi_value> &args, Args... ctor_args) const;
  template <typename T, typename... Args>
  Napi::MaybeOrValue<Napi::Object> NewWithArgs(
      const std::vector<napi_value> &args, Args... ctor_args) const;
  template <typename T, typename... Args>
  Napi::MaybeOrValue<Napi::Object> NewWithArgs(size_t argc,
                                               const napi_value *args,
                                               Args... ctor_args) const;

  Napi::MaybeOrValue<Napi::Object> New(
      const std::initializer_list<napi_value> &args) const;
  Napi::MaybeOrValue<Napi::Object> New(
      const std::vector<napi_value> &args) const;
  Napi::MaybeOrValue<Napi::Object> New(size_t argc,
                                       const napi_value *args) const;

  inline Napi::Function InnerFunction() const;

 private:
  Napi::Function function_;
};

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

using WrappedDeleterType =
    std::function<void(std::unique_ptr<IHObjectWrap> &wrapped)>;
using ConstructorFunc = std::unique_ptr<IHObjectWrap> (*)(
    const Napi_IH::IHCallbackInfo &, WrappedDeleterType *);
using ConstructorHelpFunc = std::function<std::unique_ptr<IHObjectWrap>(
    const Napi_IH::IHCallbackInfo &, WrappedDeleterType *)>;

struct ClassMetaInfo {
  using PropertyName = const char *;

  const char *name = "";
  ClassMetaInfo *parent = nullptr;
  ConstructorFunc ctor;
  std::vector<Napi::ClassPropertyDescriptor<MethodWrapper>> descriptors;
  bool export_type = true;
  void *data = nullptr;
  ConstructorHelpFunc ctor_helper;
  std::unordered_map<PropertyName, bool> member_search_map;

  void InitDescriptors(const napi_property_descriptor *start,
                       const size_t &cnt) {
    if (!descriptors.empty()) {
      return;
    }
    const auto *curr = start;
    const auto *end = start + cnt;
    size_t idx = 0;
    descriptors.reserve(cnt);
    while (curr != end) {
      descriptors.push_back(*curr);
      member_search_map[curr->utf8name] = true;
      curr++;
      idx++;
    }
  }

  void AddDescriptors(
      const std::vector<Napi::ClassPropertyDescriptor<MethodWrapper>> &descs,
      const size_t &cnt) {
    descriptors.reserve(cnt);
    for (const napi_property_descriptor &desc : descs) {
      if (!HasProperty(desc.utf8name)) {
        descriptors.push_back(desc);
        member_search_map[desc.utf8name] = true;
      }
    }
  }

  inline bool HasProperty(const char *name) const {
    return !(member_search_map.find(name) == member_search_map.end());
  }
};

class Registration {
 public:
  static void StartRegistration(Napi::Env env, Napi::Object exports);
  static void AddClassMetaInfo(const char *name, ClassMetaInfo *info);
  template <typename T>
  static Napi_IH::FunctionWrapper FindClass();

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
  static void DefineClass(
      Napi::Env env, const char *utf8name,
      const std::initializer_list<PropertyDescriptor> &properties,
      ClassVisibility visibility = ClassVisibility::kDefault,
      void *data = nullptr);

  template <typename T>
  static void DefineClass(
      Napi::Env env, const char *utf8name,
      const std::vector<PropertyDescriptor> &properties,
      ClassVisibility visibility = ClassVisibility::kDefault,
      void *data = nullptr);

  template <typename T, typename Base>
  static void DefineClass(
      Napi::Env env, const char *utf8name,
      const std::initializer_list<PropertyDescriptor> &properties,
      ClassVisibility visibility = ClassVisibility::kDefault,
      void *data = nullptr);

  template <typename T, typename Base>
  static void DefineClass(
      Napi::Env env, const char *utf8name,
      const std::vector<PropertyDescriptor> &properties,
      ClassVisibility visibility = ClassVisibility::kDefault,
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

  template <typename T, IHObjectWrap::InstanceMethodCallback<T> method>
  static PropertyDescriptor InstanceMethod(
      const char *utf8name, napi_property_attributes attributes = napi_default,
      void *data = nullptr);
  template <typename T, InstanceGetterCallback<T> getter,
            InstanceSetterCallback<T> setter = nullptr>
  static PropertyDescriptor InstanceAccessor(
      const char *utf8name, napi_property_attributes attributes = napi_default,
      void *data = nullptr);
  template <typename T>
  static Napi_IH::FunctionWrapper FindClass();

  template <typename T>
  static T *UnWrap(Napi::Object object);

 private:
  template <typename T, typename Base = NonBase>
  static void DefineClass(
      Napi::Env env, const char *utf8name, const size_t props_count,
      const napi_property_descriptor *descriptors,
      ClassVisibility visibility = ClassVisibility::kDefault,
      void *data = nullptr);

 protected:
  static Napi::FunctionReference js_class_constructor_;
  std::function<void *(Napi::CallbackInfo &info)> init_parent_;
};

}  // namespace Napi_IH

// NOTE: This method is deprecated after v18.17.0,
// but we have no other method to register our module.
// To avoid our NAPI_IH_API_MODULE be recognized as
// deprecated by IDE, we redeclare the interface here.
inline auto napi_ih_moudle_register = napi_module_register;

#define NAPI_IH_C_CTOR(fn)                           \
  static void fn(void) __attribute__((constructor)); \
  static void fn(void)

#define NAPI_IH_MODULE_X(modname, regfunc, priv, flags)                      \
  EXTERN_C_START                                                             \
  static napi_module _module = {                                             \
      NAPI_MODULE_VERSION, flags, __FILE__, regfunc, #modname, priv, {0},    \
  };                                                                         \
  NAPI_IH_C_CTOR(_register_##modname) { napi_ih_moudle_register(&_module); } \
  EXTERN_C_END

#define NAPI_IH_MODULE(modname, regfunc)   \
  NAPI_IH_MODULE_X(modname, regfunc, NULL, \
                   0)  // NOLINT (readability/null_usage)

#define NAPI_IH_API_MODULE(modname, regfunc)                                \
  static napi_value __napi_##regfunc(napi_env env, napi_value exports) {    \
    napi_value ret = Napi::RegisterModule(env, exports, regfunc);           \
    Napi::details::WrapCallback([&] {                                       \
      Napi_IH::Registration::StartRegistration(Napi::Env(env),              \
                                               Napi::Object(env, exports)); \
      return nullptr;                                                       \
    });                                                                     \
    return ret;                                                             \
  }                                                                         \
  NAPI_IH_MODULE(modname, __napi_##regfunc)

#include "napi_ih_inl.hpp"
#endif  // NAPI_IH_HPP
