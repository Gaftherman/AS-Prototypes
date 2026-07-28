#ifndef ADDON_REGISTRY_H
#define ADDON_REGISTRY_H

#include <angelscript.h>

namespace AddonRegistry {
    // Registers all AngelScript standard add-ons and console print functions
    bool RegisterAllAddons(asIScriptEngine* engine);
}

#endif // ADDON_REGISTRY_H
