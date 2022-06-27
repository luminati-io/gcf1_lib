#include "client.h"
#include "gcf1.h"

Napi::Array make_js_obj(Napi::Env& env, HCTC& handle,
    categorize_result_s& result, Napi::String url);
Napi::Object make_uncategorized_obj(Napi::Env& env,
    std::string label, Napi::String url);
Napi::String get_category_name(HCTC handle, Napi::Env env,
    int opt, int ind);
int get_full_method(const Napi::CallbackInfo& info);
int get_method_int(std::string method);
bool check_for_errors(categorize_result_s& result);

std::string config_path = "/usr/local/gcf1/etc";
std::string error = "";
int categorize_method = GCF1_OPT_CATEGORIZE_DEFAULT_MATCHING |
    GCF1_OPT_CATEGORIZE_EMBEDDED_URL;
int categorize_method2 = 0;
bool second_categorization = false;
const int OPT1 = GCF1_OPT_CATEGORIZE_URLDB_ONLY;
const int OPT = OPT1 | GCF1_OPT_CATEGORIZE_IPDB_ONLY;
const int UPDATE_DB_OPT = GCF1_OPT_DBMNG_UPDATE_DEFAULT | GCF1_OPT_DBMNG_CLEAR_CACHEDB;


Napi::String client::get_path(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    return Napi::String::New(env, config_path);
}

Napi::String client::set_path(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Expected 1 argument of type string")
            .ThrowAsJavaScriptException();
    }
    else
    {
        Napi::String str = info[0].As<Napi::String>();
        config_path = str.Utf8Value();
    }
    return Napi::String::New(env, config_path);
}

Napi::String client::get_error(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    return Napi::String::New(env, error);
}

Napi::Number client::set_method(const Napi::CallbackInfo& info)
{
    int method = get_full_method(info);
    Napi::Env env = info.Env();
    if (method != -1)
    {
        categorize_method = method;
    }
    return Napi::Number::New(env, method);
}

Napi::Number client::set_second_method(const Napi::CallbackInfo& info)
{
    int method = get_full_method(info);
    Napi::Env env = info.Env();
    if (method != -1)
    {
        categorize_method2 = method;
    }
    return Napi::Number::New(env, method);
}

Napi::Boolean client::activate_second_method(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    second_categorization = true;
    return Napi::Boolean::New(env, true);
}

Napi::Boolean client::deactivate_second_method(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    second_categorization = false;
    return Napi::Boolean::New(env, true);
}

int get_full_method(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Expected first argument to be a string")
            .ThrowAsJavaScriptException();
        return -1;
    }
    Napi::String opt = info[0].As<Napi::String>();
    std::string str = opt.Utf8Value();
    int flag = get_method_int(str);
    return GCF1_OPT_CATEGORIZE_EMBEDDED_URL | flag;
}

Napi::Array client::check_categorize(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::Array res;
    HCTC handle;
    if (info.Length() < 1 || !info[0].IsArray())
    {
        Napi::TypeError::New(env, "Expected 1 argument of type array")
            .ThrowAsJavaScriptException();
        res = Napi::Array::New(env, 0);
        return res;
    }
    int ret = gcf1_check_begin(config_path.c_str(), 0, &handle);
    if (ret != 0)
    {
        error = gcf1_error_string(ret);
        Napi::TypeError::New(env, "Couldn't create handle: "+error)
            .ThrowAsJavaScriptException();
        res = Napi::Array::New(env, 0);
        return res;
    }
    Napi::Array input = info[0].As<Napi::Array>();
    Napi::Array full_res = Napi::Array::New(env, input.Length());
    struct categorize_result_s result;
    for (int i = 0; i<(int)input.Length(); i++)
    {
        Napi::Value c_input_init = input[i];
        Napi::String c_input = c_input_init.As<Napi::String>();
        std::string str_url = c_input.Utf8Value();
        const char* url = str_url.c_str();
        /* run daemon */
        memset(&result, 0, sizeof(struct categorize_result_s));
        result.size = sizeof(struct categorize_result_s);
        ret = gcf1_categorize(handle, url, categorize_method, OPT, &result);
        /* handle result */
        if (ret == 0 && second_categorization && check_for_errors(result))
        {
            ret = gcf1_categorize(handle, url, categorize_method2, OPT,
                &result);
        }
        if (ret != 0)
        {
            res = Napi::Array::New(env, 1);
            Napi::Object err = make_uncategorized_obj(env, "Error", c_input);
            err.Set("error_code", ret);
            err.Set("error", Napi::String::New(env, gcf1_error_string(ret)));
            res.Set((int)0, err);
        }
        else
        {
            res = make_js_obj(env, handle, result, c_input);
        }
        full_res.Set(i, res);
    }
    /* done */
    gcf1_check_end(handle, 0);
    return full_res;
}

bool check_for_errors(categorize_result_s& result)
{
    return result.count == 0 ||
        (result.category[0] == 0 && result.type[0] == OPT1);
}

Napi::Array make_js_obj(Napi::Env& env, HCTC& handle,
    categorize_result_s& result, Napi::String url)
{
    int length = result.count;
    /* empty result */
    if (length == 0)
    {
        Napi::Array arr = Napi::Array::New(env, 1);
        arr.Set((int)0, make_uncategorized_obj(env, "Uncategorized", url));
        return arr;
    }
    /* parse results */
    Napi::Array arr = Napi::Array::New(env, length);
    int i;
    for (i = 0; i < length; i++)
    {
        /* uncategorized */
        if (result.category[i] == 0 && result.type[i] == OPT1)
        {
            arr.Set(i, make_uncategorized_obj(env, "Uncategorized1", url));
            continue;
        }
        /* categorized, parse to object */
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("res", Napi::Boolean::New(env, true));
        obj.Set("result", Napi::String::New(env, "Categorized"));
        obj.Set("category_index", Napi::Number::New(env, result.category[i]));
        obj.Set("category", get_category_name(handle, env,
            GCF1_OPT_GET_CATEGORY, result.category[i]));
        obj.Set("secondary_category_index",
            Napi::Number::New(env, result.category_secondary[i]));
        obj.Set("secondary_category", get_category_name(handle, env,
            GCF1_OPT_GET_CATEGORY, result.category_secondary[i]));
        obj.Set("security_index", Napi::Number::New(env,
            result.category_security[i]));
        obj.Set("security", get_category_name(handle, env,
            GCF1_OPT_GET_CATEGORY, result.category_security[i]));
        obj.Set("reputation_index", Napi::Number::New(env,
            result.reputation[i]));
        obj.Set("reputation", get_category_name(handle, env,
            GCF1_OPT_GET_REPUTAION, result.reputation[i]));
        obj.Set("flag", Napi::Number::New(env, result.flag[i]));
        obj.Set("age_rating_index", Napi::Number::New(env,
            result.age_rating[i]));
        obj.Set("age_rating", get_category_name(handle, env,
            GCF1_OPT_GET_AGE_RATING, result.age_rating[i]));
        obj.Set("volume_index", Napi::Number::New(env,
            result.volume_index[i]));
        if (result.type[i] == GCF1_OPT_CATEGORIZE_IPDB_ONLY)
        {
            obj.Set("country_code", Napi::String::New(env,
                result.country_code[i]));
            obj.Set("location_string", Napi::String::New(env,
                result.location_string[i]));
        }
        int j;
        Napi::Object category_group = Napi::Object::New(env);
        for (j = 0; j < result.category_group_count[i]; j++)
        {
            category_group.Set(get_category_name(handle, env,
                GCF1_OPT_GET_CATEGORY_GROUP, j+1),
                result.category_group[i][j]);
        }
        obj.Set("category_group", category_group);
        obj.Set("url", url);
        /* push object */
        arr.Set(i, obj);
    }
    /* done */
    return arr;
}

Napi::String get_category_name(HCTC handle, Napi::Env env, int opt, int ind)
{
    int ret;
    std::string res;
    struct category_info_s info;
    ret = gcf1_get_info_detail(handle, opt, ind, &info);
    if (ret == 0)
    {
        res = info.name;
    }
    else
    {
        res = "";
    }
    return Napi::String::New(env, res);
}

Napi::Object make_uncategorized_obj(Napi::Env& env, std::string label,
    Napi::String url)
{
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("res", Napi::Boolean::New(env, false));
    obj.Set("result", Napi::String::New(env, label));
    obj.Set("url", url);
    return obj;
}

int get_method_int(std::string method)
{
    if (method == "cascade")
    {
        return GCF1_OPT_CATEGORIZE_CASCADE_MATCHING;
    }
    if (method =="longest")
    {
        return GCF1_OPT_CATEGORIZE_LONGEST_MATCHING;
    }
    if (method =="simple")
    {
        return GCF1_OPT_CATEGORIZE_SIMPLE_MATCHING;
    }
    if (method =="domainonly")
    {
        return GCF1_OPT_CATEGORIZE_DOMAINONLY_MATCHING;
    }
    if (method =="hybrid")
    {
        return GCF1_OPT_CATEGORIZE_HYBRID_DEFAULT_MATCHING;
    }
    if (method =="hybrid_local")
    {
        return GCF1_OPT_CATEGORIZE_HYBRID_DEFAULT_LOCAL_MATCHING;
    }
    if (method =="hybrid_cloud")
    {
        return GCF1_OPT_CATEGORIZE_HYBRID_DEFAULT_CLOUD_MATCHING;
    }
    if (method =="hybrid_longest")
    {
        return GCF1_OPT_CATEGORIZE_HYBRID_LONGEST_MATCHING;
    }
    if (method =="hybrid_longest_local")
    {
        return GCF1_OPT_CATEGORIZE_HYBRID_LONGEST_LOCAL_MATCHING;
    }
    if (method =="hybrid_longest_cloud")
    {
        return GCF1_OPT_CATEGORIZE_HYBRID_LONGEST_CLOUD_MATCHING;
    }
    return GCF1_OPT_CATEGORIZE_DEFAULT_MATCHING;
}

Napi::Object client::download_db(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    HCTM handle;
    int ret = gcf1_dbmng_begin(config_path.c_str(), 0, &handle);
    Napi::Object res = Napi::Object::New(env);
    if (ret == 0)
    {
        ret = gcf1_dbmng_download(handle, 0);
        if (ret == GCF1_RET_DL_CANCEL)
        {
            res.Set("result", Napi::String::New(env, "cancel"));
        }
        else if (ret == GCF1_RET_DL_NONEEDED)
        {
            res.Set("result", Napi::String::New(env, "noneeded"));
        }
        else if (ret != 0)
        {
            res.Set("result", Napi::String::New(env, "error"));
        }
    }
    else
    {
        res.Set("result", Napi::String::New(env, "enter_error"));
    }
    if (ret != 0)
    {
        res.Set("error", Napi::String::New(env, gcf1_error_string(ret)));
    }
    res.Set("success", Napi::Boolean::New(env, ret == 0));
    ret = gcf1_dbmng_end(handle, 0);
    if (ret != 0)
    {
        res.Set("exit_error", Napi::String::New(env, gcf1_error_string(ret)));
    }
    return res;
}

Napi::Object client::update_db(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    HCTM handle;
    int ret = gcf1_dbmng_begin(config_path.c_str(), 0, &handle);
    Napi::Object res = Napi::Object::New(env);
    if (ret == 0)
    {
        ret = gcf1_dbmng_update(handle, UPDATE_DB_OPT);
    }
    if (ret != 0)
    {
        res.Set("error", Napi::String::New(env, gcf1_error_string(ret)));
    }
    res.Set("success", Napi::Boolean::New(env, ret == 0));
    ret = gcf1_dbmng_end(handle, 0);
    if (ret != 0)
    {
        res.Set("exit_error", Napi::String::New(env, gcf1_error_string(ret)));
    }
    return res;
}

Napi::Object client::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Array methods = Napi::Array::New(env, 11);
    methods.Set((int)0, "default");
    methods.Set((int)1, "cascade");
    methods.Set((int)2, "longest");
    methods.Set((int)3, "simple");
    methods.Set((int)4, "domainonly");
    methods.Set((int)5, "hybrid");
    methods.Set((int)6, "hybrid_local");
    methods.Set((int)7, "hybrid_cloud");
    methods.Set((int)8, "hybrid_longest");
    methods.Set((int)9, "hybrid_longest_local");
    methods.Set((int)10, "hybrid_longest_cloud");
    exports.Set("methods", methods);
    exports.Set("set_path", Napi::Function::New(env, client::set_path));
    exports.Set("get_path", Napi::Function::New(env, client::get_path));
    exports.Set("get_error", Napi::Function::New(env, client::get_error));
    exports.Set("set_method", Napi::Function::New(env, client::set_method));
    exports.Set("set_second_method", Napi::Function::New(env,
        client::set_second_method));
    exports.Set("activate_second_method", Napi::Function::New(env,
        client::activate_second_method));
    exports.Set("deactivate_second_method", Napi::Function::New(env,
        client::deactivate_second_method));
    exports.Set("categorize", Napi::Function::New(env,
        client::check_categorize));
    exports.Set("download_db", Napi::Function::New(env, client::download_db));
    exports.Set("update_db", Napi::Function::New(env, client::update_db));
    return exports;
}
