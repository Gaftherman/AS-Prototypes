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

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

// Our add-ons prototypes
#include "addons/Console.hpp"
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

#ifndef NDEBUG
    CASDocRegistry::Verbose = true;
    CASDocRegistry::GenerateDocumentation = true;
    CASDocRegistry::GeneratePredefined = true;
#endif

    CASDocRegistry::Engine = engine;
    CASDocRegistry::RegisterInterfaces();

    Console::Register( engine );

    ASException::Register( engine );
    ASDispose::Register( engine );

    ASJSON::Register( engine );
    {
        ASJSON::FILESYSTEM_LOAD_CALLBACK = []( std::filesystem::path& path, std::string& content, std::string& err ) -> bool
        {
            if( std::ifstream file(path); file.is_open() )
            {
                std::stringstream buffer;
                buffer << file.rdbuf();
                content = buffer.str(); 
                return true;
            }
            err = "Unexistent file";
            return false;
        };
        ASJSON::FILESYSTEM_DUMP_CALLBACK = []( std::filesystem::path& path, std::string& content, std::string& err ) -> bool
        {
            if( std::ofstream file(path); file.is_open() )
            {
                file << content;
                return true;
            }
            err = "File is read-only";
            return false;
        };
    }
    return true;
}

} // namespace AddonRegistry
