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
#include <weakref/weakref.h>

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

    RegisterStdString(engine);
    RegisterScriptArray(engine, true);
    RegisterStdStringUtils(engine);
    RegisterScriptDateTime(engine);
    RegisterScriptFile(engine);
    RegisterScriptFileSystem(engine);
    RegisterScriptDictionary(engine);
    RegisterScriptMath(engine);
    RegisterScriptHandle(engine);
    RegisterScriptWeakRef(engine);

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

#ifndef NDEBUG
    static bool FirstTime = true; // Geneate these stuff only once and assume it's ok
    CASDocRegistry::Verbose = FirstTime;
    CASDocRegistry::GenerateDocumentation = FirstTime;
    CASDocRegistry::GeneratePredefined = FirstTime;
    FirstTime = false;
#endif

    CASDocRegistry::Engine = engine;
    CASDocRegistry::RegisterInterfaces();

    return true;
}

} // namespace AddonRegistry
