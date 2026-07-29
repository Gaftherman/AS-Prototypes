#ifndef ADDON_REGISTRY_H
#define ADDON_REGISTRY_H

#include <angelscript.h>
#include "addons/CString.h"
#include "as_predefined.h"

#include <iostream>

namespace Tests
{
    inline int Fails = 0;
    inline int Passes = 0;
    
    inline bool Expect( const CString& title, bool expected, bool condition )
    {
        asIScriptContext* ctx = asGetActiveContext();

        if( ctx != nullptr && expected == condition )
        {
            std::cout << "Passed test \"" << title << "\"" << "\n";
            Tests::Passes++;
            return true;
        }
        std::cerr << "Failed test \"" << title << "\"" << "\n";
        Tests::Fails++;
        return false;
    }
}

namespace AddonRegistry {
    // Registers all AngelScript standard add-ons and console print functions
    bool RegisterAllAddons(asIScriptEngine* engine);
}

#endif // ADDON_REGISTRY_H
