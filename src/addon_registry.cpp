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
#include "addons/ASDispose.hpp"
#include "addons/ASJSON.hpp"

namespace AddonRegistry
{
bool RegisterDefaultAddons( asIScriptEngine* engine )
{
    if( !engine )
        return false;

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

    return true;
}
}

#ifndef NDEBUG
#include "addons/ASException.hpp"
#include "addons/ASJSON.hpp"
#include "addons/ASOptional.hpp"
#define REG_TEST(T)r=T;if(!r){TotalFails++;}else{TotalPasses++;std::cout<<#T" propertly registers."<<std::endl;}assert(r);
namespace Tests
{
void TestGenericRegistry()
{
    asIScriptEngine* engine = asCreateScriptEngine();

    AddonRegistry::RegisterDefaultAddons(engine);

    extern int TotalFails;
    extern int TotalPasses;
    bool r;

    REG_TEST( ASException::Register( engine ) );
    REG_TEST( ASJSON::Register( engine ) );
    REG_TEST( ASOptional::Register( engine, true ) );

    engine->ShutDownAndRelease();
}
}
#undef REG_TEST
#endif

namespace AddonRegistry
{
bool RegisterAllAddons( asIScriptEngine* engine )
{
    if( !engine )
        return false;

    RegisterDefaultAddons(engine);

#ifndef NDEBUG
    static bool FirstTime = true; // Generate these stuff only once and assume it's ok
    if( FirstTime )
        Tests::TestGenericRegistry();
    CASDocRegistry::Verbose = FirstTime;
    CASDocRegistry::GenerateDocumentation = FirstTime;
    CASDocRegistry::GeneratePredefined = FirstTime;
    FirstTime = false;
#endif

    ASDispose::Register( engine );

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

    CASDocRegistry::Engine = engine;
    CASDocRegistry::RegisterInterfaces();

    return true;
}

} // namespace AddonRegistry
