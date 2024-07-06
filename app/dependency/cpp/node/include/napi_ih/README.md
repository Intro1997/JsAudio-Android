# Description

`napi_ih` means "napi inheritance helper" which is a pitch to napi to support inheritance and try to keep the style of napi. The implementation is inspired by https://github.com/ajihyf/node-addon-api-helper and https://mmomtchev.medium.com/-class-inheritance-with-node-api-and-node-addon-api-c180334d9902

# Warning

This is a **VERY SIMPLE TRY** to make an inheritance patch of napi, and this only implements necessary apis for JSAudio-Android project!
This patch **DOESN'T SUPPORT** multi-thread, **DOESN'T COMPATIABLE WITH** origin `napi_api_module()` api and **DOESN'T SUPPORT** multi-inheritance. This is designed to **ONLY BE USED** in JSAudio-Android PROJECT!
If you want to use napi with inheritance more safe and realiable, please try https://github.com/ajihyf/node-addon-api-helper.

# Usage

`napi_ih` only supports be used in inheritance.

## Inheritance

If you what to define cpp class with inheritance, you should make your class inherited from `Napi_IH::IHObjectWrap`

```cpp
#include <napi_ih.hpp>
class Base : public Napi_IH::IHObjectWrap {
public:
  Base(const Napi_IH::IHCallbackInfo &info) : Napi_IH::IHObjectWrap(info) {
    void *data = info.Data();
  }

  Napi::Value GetValue(const Napi::CallbackInfo &info) {
    return Napi::Number::From(info.Env(), value_);
  }
  void SetValue(const Napi::CallbackInfo &info, const Napi::Value &value) {
    if (value.IsNumber()) {
      value_ = value.As<Napi::Number>.Uint32Value();
    }
  }

  static void Init() {
    DefineClass<Base>(
        env, "Base",
        {InstanceAccessor<Base, &Base::GetValue, &Base::SetValue>("value")});
  }

private:
  uint32_t value_;
};

class Derived : public Base {
  Derived(const Napi_IH::IHCallbackInfo &info) : Base(info) {}

  static void Init() { DefineClass<Derived, Base>(env, "Derived", {}); }
};

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  Base::Init(env, exports);
  Derived::Init(env, exports);
  return exports;
}

NAPI_IH_API_MODULE(addon, Init);
```

## Napi_IH::IHCallbackInfo

`Napi_IH::IHCallbackInfo` is a wrapper of `Napi::CallbackInfo`.

### IHCallbackInfo(const Napi::CallbackInfo &info);

You can only create `IHCallbackInfo` by a `Napi::CallbackInfo`, you cannot assign an `IHCallbackInfo` object to other.

### Napi::Env Env() const;

Same usage as `Napi::Env Env() const;` in Napi.

### Napi::Value NewTarget() const;

Same usage as `Napi::Value NewTarget() const;` in Napi.

### bool IsConstructCall() const;

Same usage as `bool IsConstructCall() const;` in Napi.

### size_t Length() const;

Same usage as `size_t Length() const;` in Napi.

### const Napi::Value operator[](size_t index) const;

Same usage as `const Napi::Value operator[](size_t index) const;` in Napi.

### Napi::Value This() const;

Same usage as `Napi::Value This() const;` in Napi.

### void \*Data() const;

Same usage as `void *Data() const;` in Napi.

### const Napi::CallbackInfo &InnerCallbackInfo() const;

We don't implement `napi_callback_info()` here. If you want to call this api, you can use `InnerCallbackInfo()` to get inner `const Napi::CallbackInfo &` object.

## Napi_IH::IHObjectWrap

### Constructor

```cpp
IHObjectWrap(const Napi_IH::IHCallbackInfo &info);
```

- `[in] info`: See `Napi_IH::IHCallbackInfo`

### DefineClass\<T\>(..., initializer_list<PropertyDescriptor>, ...)

```cpp
template <typename T>
static void
DefineClass(Napi::Env env, const char *utf8name,
            const std::initializer_list<PropertyDescriptor> &properties,
            void *data = nullptr);
```

- `[in] T`: Current cpp class type which needs to be defined in js.
- `[in] env`: The environment in which to construct a JavaScript class.
- `[in] utf8name`: Null-terminated string that represents the name of the
  JavaScript constructor function.
- `[in] properties`: initializer_list of class property descriptor describing static and
  instance properties and methods of the class.
  See: `PropertyDescriptor`.
- `[in] data`: User-provided data passed to the constructor callback. You can get the `data`
  value by calling `Napi_IH::IHCallbackInfo::Data()` later in constructor.

### DefineClass\<T\>(..., vector<PropertyDescriptor>, ...)

```cpp
template <typename T>
static void DefineClass(Napi::Env env, const char *utf8name,
                        const std::vector<PropertyDescriptor> &properties,
                        void *data = nullptr);
```

- `[in] T`: Current cpp class type which needs to be defined in js.
- `[in] env`: The environment in which to construct a JavaScript class.
- `[in] utf8name`: Null-terminated string that represents the name of the
  JavaScript constructor function.
- `[in] properties`: vector of class property descriptor describing static and
  instance properties and methods of the class.
  See: `PropertyDescriptor`.
- `[in] data`: User-provided data passed to the constructor callback. You can get the `data`
  value by calling `Napi_IH::IHCallbackInfo::Data()` later in constructor.

### DefineClass\<T, Base\>(..., initializer_list<PropertyDescriptor>, ...)

```cpp
template <typename T, typename Base>
static void
DefineClass(Napi::Env env, const char *utf8name,
            const std::initializer_list<PropertyDescriptor> &properties,
            void *data = nullptr);
```

- `[in] T`: Current cpp class type which needs to be defined in js.
- `[in] Base`: Base cpp class type of current class T.
- `[in] env`: The environment in which to construct a JavaScript class.
- `[in] utf8name`: Null-terminated string that represents the name of the
  JavaScript constructor function.
- `[in] properties`: initializer_list of class property descriptor describing static and
  instance properties and methods of the class.
  See: `PropertyDescriptor`.
- `[in] data`: User-provided data passed to the constructor callback. You can get the `data`
  value by calling `Napi_IH::IHCallbackInfo::Data()` later in constructor.

### DefineClass\<T, Base\>(..., vector<PropertyDescriptor>, ...)

```cpp
template <typename T, typename Base>
static void DefineClass(Napi::Env env, const char *utf8name,
                        const std::vector<PropertyDescriptor> &properties,
                        void *data = nullptr);
```

- `[in] T`: Current cpp class type which needs to be defined in js.
- `[in] Base`: Base cpp class type of current class T.
- `[in] env`: The environment in which to construct a JavaScript class.
- `[in] utf8name`: Null-terminated string that represents the name of the
  JavaScript constructor function.
- `[in] properties`: vector of class property descriptor describing static and
  instance properties and methods of the class.
  See: `PropertyDescriptor`.
- `[in] data`: User-provided data passed to the constructor callback. You can get the `data`
  value by calling `Napi_IH::IHCallbackInfo::Data()` later in constructor.

### PropertyDescriptor\<T\>(..., vector<PropertyDescriptor>, ...)

```cpp
template <typename T>
static PropertyDescriptor
InstanceMethod(const char *utf8name, InstanceMethodCallback<T> method,
                napi_property_attributes attributes = napi_default,
                void *data = nullptr);
```

- `[in] T`: Current cpp class type which the `method` belongs to.
- `[in] utf8name`: Null-terminated string that represents the name of the method
  provided by instances of the class.
- `[in] method`: The native function that represents a method provided by the
  add-on.
- `[in] attributes`: The attributes associated with the property. One or more of
  `napi_property_attributes`.
- `[in] data`: User-provided data passed into the method when it is invoked.

Returns a `Napi_IH::PropertyDescriptor` object that represents a method
provided by instances of the class.

The `method` must be of the form

```cpp
void MethodName(const Napi::CallbackInfo& info);
```

### InstanceAccessor\<T, getter, setter = nullptr\>(..., vector<PropertyDescriptor>, ...)

```cpp
template <typename T, InstanceGetterCallback<T> getter,
          IHObjectWrap::InstanceSetterCallback<T> setter = nullptr>
static PropertyDescriptor
InstanceAccessor(const char *utf8name,
                  napi_property_attributes attributes = napi_default,
                  void *data = nullptr);
```

- `[in] T`: Current cpp class type which the `setter` and `getter` belong to.
- `[in] getter`: The native function to call when a get access to the property
  is performed.
- `[in] setter`: The native function to call when a set access to the property
  is performed.
- `[in] utf8name`: Null-terminated string that represents the name of the method
  provided by instances of the class.
- `[in] attributes`: The attributes associated with the property. One or more of
  `napi_property_attributes`.
- `[in] data`: User-provided data passed into the getter or the setter when it
  is invoked.

Returns a `Napi::ClassPropertyDescriptor<T>` object that represents an instance
accessor property provided by instances of the class.

The `getter` must be of the form:

```cpp
Napi::Value MethodName(const Napi::CallbackInfo& info) {}
```

The `setter` must be of the form:

```cpp
void MethodName(const Napi::CallbackInfo &, const Napi::Value &) {}
```

### FindClass\<T\>()

```cpp
template <typename T> static Napi::Function FindClass();
```

- `[in] T`: The cpp class of the js class that you want to find.

Returns `Napi::Function` object that represents js class constructor.

# Thanks

- Based on [node-addon-api](https://github.com/nodejs/node-addon-api/tree/main).
- Inspired by [node-addon-api-helper](https://github.com/ajihyf/node-addon-api-helper).
