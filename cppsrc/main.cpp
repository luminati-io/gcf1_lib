#include <napi.h>
#include "gcf1/client.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return client::Init(env, exports);
}

NODE_API_MODULE(testaddon, InitAll)
