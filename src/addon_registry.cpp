#include "addon_registry.h"

#include <iostream>
#include <string>

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
#include "addons/ASDispose.hpp"
#include "addons/ASException.hpp"
#include "addons/ASJSON.hpp"
#include "addons/ASOptional.hpp"

namespace {
    void ScriptPrint(const std::string &msg) {
        std::cout << msg;
        std::cout.flush();
    }

    void ScriptPrintln(const std::string &msg) {
        std::cout << msg << std::endl;
    }
}

namespace AddonRegistry {

bool RegisterAllAddons(asIScriptEngine* engine) {
    if (!engine) return false;

    int r = 0;

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

    // 6. Console I/O functions
    r = REGISTER_GLOBAL_FUNCTION("void print(const string &in)", asFUNCTION(ScriptPrint), asCALL_CDECL, "Prints a string to the console output.");
    if (r < 0) return false;
    r = REGISTER_GLOBAL_FUNCTION("void println(const string &in)", asFUNCTION(ScriptPrintln), asCALL_CDECL, "Prints a string to the console output followed by a newline.");
    if (r < 0) return false;

    {
        engine->SetDefaultNamespace( "Tests" );
        REGISTER_GLOBAL_PROPERTY("int Passes", &::Tests::Passes, "Number of passed test assertions.");
        REGISTER_GLOBAL_PROPERTY("int Fails", &::Tests::Fails, "Number of failed test assertions.");
        REGISTER_GLOBAL_FUNCTION("bool Expect( const string&in title, bool expected, bool condition )", asFUNCTION(&::Tests::Expect), asCALL_CDECL, "Tests an assertion condition and increments pass/fail counters." );
        engine->SetDefaultNamespace( "" );
    }

    // 7. Dictionary
    RegisterScriptDictionary(engine);

    // 8. Math library
    RegisterScriptMath(engine);

    // 9. Script Handles
    RegisterScriptHandle(engine);

    ASException::Register( engine );
    ASOptional::Register( engine );
    ASDispose::Register( engine );
    ASJSON::Register( engine );

    return true;
}

} // namespace AddonRegistry
