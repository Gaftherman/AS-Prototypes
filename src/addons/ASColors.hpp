/**
* @file ASColors.hpp
* @brief Various color types for AngelScript.
* @version 0.1
**/

// AngelScript namespace where API will be exposed
#define ASCOLOR_NAMESPACE ""

// Register RGB class
#define ASCOLOR_RGB
// Register HEX class
#define ASCOLOR_HEX

#ifdef ASCOLOR_RGB
#include "addons/Colors/ASRGB.hpp"
#endif

#ifdef ASCOLOR_HEX
#include "addons/Colors/ASHEX.hpp"
#endif

namespace ASColors
{
    bool Register( asIScriptEngine* engine )
    {
        engine->SetDefaultNamespace( ASCOLOR_NAMESPACE );
#ifdef ASCOLOR_RGB
        ASRGB::Register( engine );
#endif
#ifdef ASCOLOR_HEX
        ASHEX::Register( engine );
#endif
        engine->SetDefaultNamespace( "" );
    }
}
