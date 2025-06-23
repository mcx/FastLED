#ifdef __EMSCRIPTEN__

#include <emscripten/val.h>
#include <emscripten.h>
#include <emscripten/emscripten.h> // Include Emscripten headers
#include <emscripten/html5.h>
#include <emscripten/val.h>

#include "fl/map.h"
#include "fl/singleton.h"
#include "fl/slice.h"

#include "active_strip_data.h"
#include "fl/map.h"
#include "fl/namespace.h"
#include "fl/str.h"
#include "fl/json.h"
#include "js.h"
#include "platforms/wasm/engine_listener.h"

namespace fl {

ActiveStripData &ActiveStripData::Instance() {
    return fl::Singleton<ActiveStripData>::instance();
}

void ActiveStripData::update(int id, uint32_t now, const uint8_t *pixel_data,
                             size_t size) {
    mStripMap.update(id, SliceUint8(pixel_data, size));
}

void ActiveStripData::updateScreenMap(int id, const ScreenMap &screenmap) {
    mScreenMap.update(id, screenmap);
}


Str ActiveStripData::infoJsonString() {
    FLArduinoJson::JsonDocument doc;
    auto array = doc.to<FLArduinoJson::JsonArray>();

    for (const auto &[stripIndex, stripData] : mStripMap) {
        auto obj = array.add<FLArduinoJson::JsonObject>();
        obj["strip_id"] = stripIndex;
        obj["type"] = "r8g8b8";
    }

    Str jsonBuffer;
    serializeJson(doc, jsonBuffer);
    return jsonBuffer;
}

/// WARNING: For some reason the following code must be here, when
/// it was moved to embind.cpp frame data stopped being updated.
// gcc constructor to get the
// ActiveStripData instance created.
__attribute__((constructor)) void __init_ActiveStripData() {
    ActiveStripData::Instance();
}

} // namespace fl

// C binding wrappers
extern "C" {
    EMSCRIPTEN_KEEPALIVE fl::ActiveStripData* getActiveStripData() {
        return &fl::ActiveStripData::Instance();
    }
    
    EMSCRIPTEN_KEEPALIVE uint8_t* getPixelData_Uint8_Raw(int stripIndex, size_t* outSize) {
        fl::ActiveStripData& instance = fl::ActiveStripData::Instance();
        fl::SliceUint8 stripData;
        if (instance.mStripMap.get(stripIndex, &stripData)) {
            *outSize = stripData.size();
            return const_cast<uint8_t*>(stripData.data());
        }
        *outSize = 0;
        return nullptr;
    }
    
    // Compatibility aliases for old embind names (for existing compiled JS files)
    EMSCRIPTEN_KEEPALIVE fl::ActiveStripData* _getActiveStripData() {
        return &fl::ActiveStripData::Instance();
    }
    
    EMSCRIPTEN_KEEPALIVE uint8_t* _getPixelData_Uint8_Raw(int stripIndex, size_t* outSize) {
        return getPixelData_Uint8_Raw(stripIndex, outSize);
    }
}

#endif
