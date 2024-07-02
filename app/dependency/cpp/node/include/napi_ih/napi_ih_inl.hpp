
#include "napi_ih.hpp"

namespace Napi_IH {
template <typename T>
Napi::FunctionReference IHObjectWrap<T>::js_class_constructor_;

template <typename T>
ClassMetaInfo IHObjectWrap<T>::class_meta_info_(js_class_constructor_);

template <typename T>
template <typename Base>
void IHObjectWrap<T>::DefineClass(
    Napi::Env env, const char *utf8name,
    const std::initializer_list<Napi::PropertyDescriptor> &properties,
    void *data) {
  DefineClass<Base>(
      env, utf8name, properties.size(),
      reinterpret_cast<const napi_property_descriptor *>(properties.begin()),
      data);
}

template <typename T> ClassMetaInfo &IHObjectWrap<T>::GetMetaInfoRef() {
  return class_meta_info_;
}

template <typename T>
template <typename Base>
void IHObjectWrap<T>::DefineClass(
    Napi::Env env, const char *utf8name,
    const std::vector<Napi::PropertyDescriptor> &properties, void *data) {
  DefineClass<Base>(
      env, utf8name, properties.size(),
      reinterpret_cast<const napi_property_descriptor *>(properties.data()),
      data);
}

template <typename T>
template <typename Base>
void IHObjectWrap<T>::DefineClass(
    Napi::Env env, const char *utf8name, const size_t props_count,
    const napi_property_descriptor *descriptors, void *data) {
  ClassMetaInfo &info_handle = IHObjectWrap<T>::GetMetaInfoRef();

  IHObjectWrap<T>::SetMetaInfoName(utf8name);
  IHObjectWrap<T>::template SetMetaInfoParent<Base>();
  AddMetaInfoDescriptor(props_count, descriptors);
  SetMetaInfoDefineClassFunc([](ClassMetaInfo &info,
                                Napi::Env env) -> Napi::Function {
    return Napi::ObjectWrap<T>::DefineClass(env, info.name, info.descriptors);
  });
  Registration::AddClassMetaInfo(utf8name, &info_handle);
}

template <typename T>
void IHObjectWrap<T>::SetMetaInfoName(const char *name) {
  GetMetaInfoRef().name = name;
}

template <typename T>
template <typename Base>
void IHObjectWrap<T>::SetMetaInfoParent() {
  ClassMetaInfo &info_handle = IHObjectWrap<T>::GetMetaInfoRef();
  info_handle.parent = &(Base::class_meta_info_);
}

template <typename T>
void IHObjectWrap<T>::AddMetaInfoDescriptor(
    const size_t props_count, const napi_property_descriptor *descriptors) {
  ClassMetaInfo &info_handle = IHObjectWrap<T>::GetMetaInfoRef();
  info_handle.descriptors.reserve(info_handle.descriptors.size() + props_count);
  info_handle.descriptors.insert(info_handle.descriptors.end(), descriptors,
                                 descriptors + props_count);
}

template <typename T>
inline void IHObjectWrap<T>::SetMetaInfoDefineClassFunc(
    const std::function<Napi::Function(ClassMetaInfo &, Napi::Env env)>
        &define_class_func) {
  GetMetaInfoRef().define_class_func = define_class_func;
}

inline void Registration::StartRegistration(Napi::Env env) {
  auto &class_meta_infos = GetClassMetaInfoMap();
  for (auto &info : class_meta_infos) {
    ProcessClassMetaInfo(env, info.second);
  }
  for (auto &info : class_meta_infos) {
    ClearClassMetaInfoMap(info.second);
  }
  class_meta_infos.clear();
}

inline void Registration::ClearClassMetaInfoMap(ClassMetaInfo *info) {
  info->name = nullptr;
  info->descriptors.clear();
  info->class_ctor.Reset();
  info->parent = nullptr;
}

inline void Registration::AddClassMetaInfo(const char *name,
                                           ClassMetaInfo *info) {
  auto &class_meta_infos = GetClassMetaInfoMap();
  if (class_meta_infos.find(name) == class_meta_infos.end()) {
    return;
  }
  class_meta_infos[name] = info;
}

inline void Registration::ProcessClassMetaInfo(Napi::Env env,
                                               ClassMetaInfo *info) {
  // TODO:
  // check if info.class_ctor_ is valid
  if (!info->class_ctor.IsEmpty()) {
    // - if valid, return
    return;
  }
  // if parent is not nullptr
  ClassMetaInfo *parent = info->parent;
  if (parent != nullptr) {
    // - call ProcessClassMetaInfo(*parent)
    ProcessClassMetaInfo(env, parent);
    // - insert parent's descriptors_ to current info.descriptors_
    // TODO: this can be optimized mem space by while loop
    info->descriptors.reserve(info->descriptors.size() +
                              parent->descriptors.size());
    info->descriptors.insert(info->descriptors.end(),
                             parent->descriptors.begin(),
                             parent->descriptors.end());
  }
  // call DefineClass of napi, and set return value to info.class_ctor_
  if (info->define_class_func) {
    info->define_class_func(*info, env);
  }
  if (info->parent) {
    Napi::Object global = env.Global();
    Napi::Object Object = global.Get("Object").As<Napi::Object>();
    Napi::Function setPrototypeOf =
        Object.Get("setPrototypeOf").As<Napi::Function>();
    Napi::Function clazz = info->class_ctor.Value();
    Napi::Function parent_clazz = info->parent->class_ctor.Value();

    setPrototypeOf.Call({clazz, parent_clazz});
    setPrototypeOf.Call(
        {clazz.Get("prototype"), parent_clazz.Get("prototype")});
  }
}

inline std::map<const char *, ClassMetaInfo *> &
Registration::GetClassMetaInfoMap() {
  static std::map<const char *, ClassMetaInfo *> class_meta_info_map_ = {};
  return class_meta_info_map_;
}

} // namespace Napi_IH