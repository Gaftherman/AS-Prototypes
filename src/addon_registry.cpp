#include "addon_registry.h"

// AngelScript add-ons headers
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptmath/scriptmath.h>
#include <scriptfile/scriptfile.h>
#include <scriptfile/scriptfilesystem.h>
#include <scripthandle/scripthandle.h>
#include <datetime/datetime.h>

// Our add-ons prototypes
#include "addons/ASConsole.hpp"
#include "addons/ASDispose.hpp"
#include "addons/ASException.hpp"
#include "addons/ASJSON.hpp"
#include "addons/ASOptional.hpp"

namespace AddonRegistry {

bool RegisterAllAddons(asIScriptEngine* engine) {
    if (!engine) return false;

    // 1. Base std::string
    RegisterStdString(engine);

    // 2. Dynamic Array template (Required before RegisterStdStringUtils)
    RegisterScriptArray(engine, true);

    // 3. std::string utilities (Requires array<string>)
    RegisterStdStringUtils(engine);

    // 4. DateTime (Required before RegisterScriptFileSystem)
    RegisterScriptDateTime(engine);

    // 5. File & Filesystem I/O
    RegisterScriptFile(engine);
    RegisterScriptFileSystem(engine);

    // 7. Dictionary
    RegisterScriptDictionary(engine);

    // 8. Math library
    RegisterScriptMath(engine);

    // 9. Script Handles
    RegisterScriptHandle(engine);

    ASConsole::Register( engine );

    ASException::Register( engine );
    ASOptional::Register( engine );
    ASDispose::Register( engine );
    ASJSON::Register( engine );

    return true;
}

} // namespace AddonRegistry
