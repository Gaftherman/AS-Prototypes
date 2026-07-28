#ifndef ADDON_REGISTRY_H
#define ADDON_REGISTRY_H

#include <angelscript.h>

namespace Tests
{
    inline int Fails = 0;
    inline int Passes = 0;
}

namespace AddonRegistry {
    // Registers all AngelScript standard add-ons and console print functions
    bool RegisterAllAddons(asIScriptEngine* engine);
}

#endif // ADDON_REGISTRY_H
