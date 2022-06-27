#include <napi.h>
namespace client {
    Napi::String get_path(const Napi::CallbackInfo& info);
    Napi::String set_path(const Napi::CallbackInfo& info);
    Napi::String get_error(const Napi::CallbackInfo& info);
    Napi::Number set_method(const Napi::CallbackInfo& info);
    Napi::Number set_second_method(const Napi::CallbackInfo& info);
    Napi::Boolean activate_second_method(const Napi::CallbackInfo& info);
    Napi::Boolean deactivate_second_method(const Napi::CallbackInfo& info);
    Napi::Array check_categorize(const Napi::CallbackInfo& info);
    Napi::Object download_db(const Napi::CallbackInfo& info);
    Napi::Object update_db(const Napi::CallbackInfo& info);
    Napi::Object Init(Napi::Env env, Napi::Object exports);
}
