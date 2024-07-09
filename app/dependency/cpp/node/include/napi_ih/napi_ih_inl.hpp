#ifndef NAPI_IH_INL_HPP
#define NAPI_IH_INL_HPP

#include "napi_ih.hpp"

namespace Napi_IH {
inline IHCallbackInfo::IHCallbackInfo(const Napi::CallbackInfo &info,
                                      void *user_data)
    : info_(info), user_data_(user_data) {}

inline Napi::Env IHCallbackInfo::Env() const { return info_.Env(); }

inline Napi::Value IHCallbackInfo::NewTarget() const {
  return info_.NewTarget();
}

inline bool IHCallbackInfo::IsConstructCall() const {
  return info_.IsConstructCall();
}

inline size_t IHCallbackInfo::Length() const { return info_.Length(); }

inline const Napi::Value IHCallbackInfo::operator[](size_t index) const {
  return info_.operator[](index);
}

inline Napi::Value IHCallbackInfo::This() const { return info_.This(); }

inline void *IHCallbackInfo::Data() const {
  return user_data_ ? user_data_ : info_.Data();
}

inline const Napi::CallbackInfo &IHCallbackInfo::InnerCallbackInfo() const {
  return info_;
}

template <typename T> class ClassMetaInfoInstance {
public:
  static ClassMetaInfo &GetInstance() { return meta_info_; }

private:
  static ClassMetaInfo meta_info_;
};

template <typename T> ClassMetaInfo ClassMetaInfoInstance<T>::meta_info_;

inline std::map<ClassMetaInfo *, Napi::FunctionReference>
    Registration::ctor_table_;

inline std::map<const char *, ClassMetaInfo *> Registration::meta_info_table_;

class MethodWrapper : public Napi::ObjectWrap<MethodWrapper> {
  using PropertyDescriptor = Napi::ClassPropertyDescriptor<MethodWrapper>;

public:
  MethodWrapper(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<MethodWrapper>(info) {
    ClassMetaInfo *meta_info = reinterpret_cast<ClassMetaInfo *>(info.Data());
    if (meta_info->ctor) {
      wrapped_ = meta_info->ctor({info, meta_info->data});
    }
  }

  template <typename T>
  static std::unique_ptr<IHObjectWrap>
  ConstructObject(const Napi_IH::IHCallbackInfo &info) {
    if (!std::is_base_of<IHObjectWrap, T>::value) {
      return nullptr;
    }
    return std::move(std::make_unique<T>(info));
  }

  static Napi::Function
  DefineClass(Napi::Env env, const char *name,
              const std::vector<PropertyDescriptor> descriptors,
              void *data = nullptr) {
    return Napi::ObjectWrap<MethodWrapper>::DefineClass(env, name, descriptors,
                                                        data);
  }

  template <typename T, void (T::*method)(const Napi::CallbackInfo &)>
  void CallInstanceVoidMethodCallback(const Napi::CallbackInfo &info) {
    if (wrapped_ && std::is_base_of<IHObjectWrap, T>::value) {
      T *origin_ptr = reinterpret_cast<T *>(wrapped_.get());
    }
  }

  template <typename T, Napi::Value (T::*method)(const Napi::CallbackInfo &)>
  Napi::Value CallInstanceMethodCallback(const Napi::CallbackInfo &info) {
    if (wrapped_ && std::is_base_of<IHObjectWrap, T>::value) {
      T *origin_ptr = reinterpret_cast<T *>(wrapped_.get());
      return (origin_ptr->*method)(info);
    }
    return {};
  }

  template <typename T, Napi::Value (T::*method)(const Napi::CallbackInfo &)>
  Napi::Value CallInstanceGetterCallback(const Napi::CallbackInfo &info) {
    if (wrapped_ && std::is_base_of<IHObjectWrap, T>::value) {
      T *origin_ptr = reinterpret_cast<T *>(wrapped_.get());
      return (origin_ptr->*method)(info);
    }
    return {};
  }

  template <typename T,
            void (T::*method)(const Napi::CallbackInfo &, const Napi::Value &)>
  void CallInstanceSetterCallback(const Napi::CallbackInfo &info,
                                  const Napi::Value &value) {
    if (wrapped_ && std::is_base_of<IHObjectWrap, T>::value) {
      T *origin_ptr = reinterpret_cast<T *>(wrapped_.get());
      (origin_ptr->*method)(info, value);
    }
  }

  template <typename T, Napi::Value (T::*method)(const Napi::CallbackInfo &)>
  static PropertyDescriptor
  InstanceMethod(const char *utf8name,
                 napi_property_attributes attributes = napi_default,
                 void *data = nullptr) {
    return ObjectWrap<MethodWrapper>::InstanceMethod(
        utf8name, &MethodWrapper::CallInstanceMethodCallback<T, method>,
        attributes, data);
  }

  template <typename T,
            Napi::Value (T::*getter)(const Napi::CallbackInfo &info),
            void (T::*setter)(const Napi::CallbackInfo &info,
                              const Napi::Value &value)>
  static PropertyDescriptor
  InstanceAccessor(const char *utf8name,
                   napi_property_attributes attributes = napi_default,
                   void *data = nullptr) {
    return ObjectWrap<MethodWrapper>::InstanceAccessor(
        utf8name, &MethodWrapper::CallInstanceGetterCallback<T, getter>,
        &MethodWrapper::CallInstanceSetterCallback<T, setter>, attributes,
        data);
  }

private:
  std::unique_ptr<IHObjectWrap> wrapped_;
};

inline IHObjectWrap::IHObjectWrap(const Napi_IH::IHCallbackInfo &) {}

template <typename T>
void IHObjectWrap::DefineClass(
    Napi::Env env, const char *utf8name,
    const std::initializer_list<IHObjectWrap::PropertyDescriptor> &properties,
    bool need_export, void *data) {
  DefineClass<T>(
      env, utf8name, properties.size(),
      reinterpret_cast<const napi_property_descriptor *>(properties.begin()),
      need_export, data);
}

template <typename T>
void IHObjectWrap::DefineClass(
    Napi::Env env, const char *utf8name,
    const std::vector<IHObjectWrap::PropertyDescriptor> &properties,
    bool need_export, void *data) {
  DefineClass<T>(
      env, utf8name, properties.size(),
      reinterpret_cast<const napi_property_descriptor *>(properties.data()),
      need_export, data);
}

template <typename T, typename Base>
inline void IHObjectWrap::DefineClass(
    Napi::Env env, const char *utf8name,
    const std::initializer_list<IHObjectWrap::PropertyDescriptor> &properties,
    bool need_export, void *data) {
  DefineClass<T, Base>(
      env, utf8name, properties.size(),
      reinterpret_cast<const napi_property_descriptor *>(properties.begin()),
      need_export, data);
}

template <typename T, typename Base>
inline void IHObjectWrap::DefineClass(
    Napi::Env env, const char *utf8name,
    const std::vector<IHObjectWrap::PropertyDescriptor> &properties,
    bool need_export, void *data) {
  DefineClass<T, Base>(env, utf8name, properties.size(), properties.data(),
                       need_export, data);
}

template <typename T, IHObjectWrap::InstanceMethodCallback<T> method>
inline IHObjectWrap::PropertyDescriptor
IHObjectWrap::InstanceMethod(const char *utf8name,
                             napi_property_attributes attributes, void *data) {
  return MethodWrapper::InstanceMethod<T, method>(utf8name, attributes, data);
}

template <typename T, IHObjectWrap::InstanceGetterCallback<T> getter,
          IHObjectWrap::InstanceSetterCallback<T> setter>
inline IHObjectWrap::PropertyDescriptor IHObjectWrap::InstanceAccessor(
    const char *utf8name, napi_property_attributes attributes, void *data) {
  return MethodWrapper::InstanceAccessor<T, getter, setter>(utf8name,
                                                            attributes, data);
}

template <typename T> inline Napi::Function IHObjectWrap::FindClass() {
  return Registration::FindClass<T>();
}

template <typename T, typename Base>
void IHObjectWrap::DefineClass(Napi::Env env, const char *utf8name,
                               const size_t props_count,
                               const napi_property_descriptor *descriptors,
                               bool need_export, void *data) {
  ClassMetaInfo &info_handle = ClassMetaInfoInstance<T>::GetInstance();

  info_handle.name = utf8name;
  if (!std::is_same<Base, NonBase>::value) {
    info_handle.parent = &(ClassMetaInfoInstance<Base>::GetInstance());
  }
  info_handle.ctor = MethodWrapper::ConstructObject<T>;
  info_handle.descriptors = {descriptors, descriptors + props_count};
  info_handle.data = data;
  info_handle.need_export = need_export;
  Registration::AddClassMetaInfo(utf8name, &info_handle);
}

inline void Registration::StartRegistration(Napi::Env env,
                                            Napi::Object exports) {
  auto &class_meta_infos = meta_info_table_;
  auto &ctor_table = ctor_table_;
  for (auto &info : class_meta_infos) {
    ProcessClassMetaInfo(env, exports, info.second, ctor_table);
  }
  for (auto &info : class_meta_infos) {
    ClearClassMetaInfoMap(info.second);
  }
  class_meta_infos.clear();
}

inline void Registration::ClearClassMetaInfoMap(ClassMetaInfo *info) {
  info->name = nullptr;
  info->descriptors.clear();
  info->parent = nullptr;
}

inline void Registration::AddClassMetaInfo(const char *name,
                                           ClassMetaInfo *info) {
  auto &class_meta_infos = meta_info_table_;
  if (class_meta_infos.find(name) != class_meta_infos.end()) {
    return;
  }
  class_meta_infos[name] = info;
}

template <typename T> Napi::Function Registration::FindClass() {
  const auto &ctor_element =
      ctor_table_.find(&(ClassMetaInfoInstance<T>::GetInstance()));
  if (ctor_element == ctor_table_.end()) {
    return {};
  }
  return ctor_element->second.Value();
}

inline void Registration::ProcessClassMetaInfo(
    Napi::Env env, Napi::Object exports, ClassMetaInfo *info,
    std::map<ClassMetaInfo *, Napi::FunctionReference> &ctor_table) {
  if (ctor_table_.find(info) != ctor_table_.end()) {
    return;
  }
  ClassMetaInfo *parent = info->parent;
  if (parent != nullptr) {
    ProcessClassMetaInfo(env, exports, parent, ctor_table);
    // TODO: this can be optimized mem space by while loop
    info->descriptors.reserve(info->descriptors.size() +
                              parent->descriptors.size());
    info->descriptors.insert(info->descriptors.end(),
                             parent->descriptors.begin(),
                             parent->descriptors.end());
  }

  Napi::Function clazz =
      MethodWrapper::DefineClass(env, info->name, info->descriptors, info);

  auto parent_element_in_map = ctor_table_.find(info->parent);

  if (info->parent && parent_element_in_map != ctor_table_.end()) {
    Napi::Object global = env.Global();
    Napi::Object Object = global.Get("Object").As<Napi::Object>();
    Napi::Function setPrototypeOf =
        Object.Get("setPrototypeOf").As<Napi::Function>();
    Napi::Function parent_clazz = parent_element_in_map->second.Value();

    setPrototypeOf.Call({clazz, parent_clazz});
    setPrototypeOf.Call(
        {clazz.Get("prototype"), parent_clazz.Get("prototype")});
  }

  ctor_table_[info] = Napi::Persistent(clazz);
  if (info->need_export) {
    exports.Set(info->name, ctor_table_[info].Value());
  }
}

} // namespace Napi_IH

#endif // NAPI_IH_INL_HPP