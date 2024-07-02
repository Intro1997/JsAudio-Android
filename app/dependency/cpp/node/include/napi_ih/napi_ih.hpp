// desc: napi_ih means "napi inheritance helper" which is a pitch to napi
//       to support inheritance. The implementation method is referenced
//       from https://github.com/ajihyf/node-addon-api-helper and
//       https://mmomtchev.medium.com/c-class-inheritance-with-node-api-and-node-addon-api-c180334d9902

// WARN: This is a VERY SIMPLE TRY to make an inheritance patch of napi!
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

struct ClassMetaInfo {
public:
  /**
   * name: we will not delete this memroy
   * parent: we will not delete this memroy
   * class_ctor: we will do Reset() after proecss
   * descriptors: we will do clear() after process
   */
  ClassMetaInfo(Napi::FunctionReference &ctor)
      : name(nullptr), parent(nullptr), class_ctor(ctor), descriptors({}) {}

  const char *name;
  ClassMetaInfo *parent;
  Napi::FunctionReference &class_ctor;
  std::vector<Napi::PropertyDescriptor> descriptors;
  std::function<Napi::Function(ClassMetaInfo &, Napi::Env env)>
      define_class_func;
};

class Registration {
public:
  static void StartRegistration(Napi::Env env);
  static void AddClassMetaInfo(const char *name, ClassMetaInfo *info);

private:
  static void ProcessClassMetaInfo(Napi::Env env, ClassMetaInfo *info);
  static void ClearClassMetaInfoMap(ClassMetaInfo *info);
  static std::map<const char *, ClassMetaInfo *> &GetClassMetaInfoMap();
};

template <typename T> class IHObjectWrap : public Napi::ObjectWrap<T> {
public:
  template <typename Base>
  static void
  DefineClass(Napi::Env env, const char *utf8name,
              const std::initializer_list<Napi::PropertyDescriptor> &properties,
              void *data = nullptr);

  template <typename Base>
  static void
  DefineClass(Napi::Env env, const char *utf8name,
              const std::vector<Napi::PropertyDescriptor> &properties,
              void *data = nullptr);

private:
  static ClassMetaInfo &GetMetaInfoRef();
  template <typename Base>
  static void DefineClass(Napi::Env env, const char *utf8name,
                          const size_t props_count,
                          const napi_property_descriptor *descriptors,
                          void *data = nullptr);

  static void SetMetaInfoName(const char *name);
  template <typename Base> static void SetMetaInfoParent();
  static void
  AddMetaInfoDescriptor(const size_t props_count,
                        const napi_property_descriptor *descriptors);
  static void SetMetaInfoDefineClassFunc(
      const std::function<Napi::Function(ClassMetaInfo &, Napi::Env env)>
          &define_class_func);

  static ClassMetaInfo class_meta_info_;
  static Napi::FunctionReference js_class_constructor_;
};

} // namespace Napi_IH

#define NAPI_IH_API_MODULE(modname, regfunc)                                   \
  static napi_value __napi_##regfunc(napi_env env, napi_value exports) {       \
    napi_value ret = Napi::RegisterModule(env, exports, regfunc);              \
    Napi_IH::Registration::StartRegistration(Napi::Env(env));                  \
    return ret;                                                                \
  }                                                                            \
  NAPI_MODULE(modname, __napi_##regfunc)

#include "napi_ih_inl.hpp"
#endif // NAPI_IH_HPP
