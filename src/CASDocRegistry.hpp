#pragma once

#include <angelscript.h>
#include <assert.h>
#include <vector>
#include <iostream>

class CASDocRegistry;

inline std::vector<CASDocRegistry*> g_ASRegistry;

#define CASDocRegisterInterface(InterfaceType) \
    inline InterfaceType* gp##InterfaceType = static_cast<InterfaceType*>( g_ASRegistry.emplace_back( new InterfaceType() ) )

class CASDocRegistry
{
private:

protected:

public:

    virtual ~CASDocRegistry() = default;

    asIScriptEngine* engine;

    // Set to true before registering the application to register documentation.
    static inline bool GenerateDocumentation = false;

    // Set to true before registering the application to register as.predefined.
    static inline bool GeneratePredefined = false;

    CASDocRegistry() { }

    /**
     * @brief Get the Name for the addon
     */
    virtual const char* GetName() { return ""; }

    /**
     * @brief Register angelscript API and documentation
     */
    virtual bool Register() { return false; }

    /**
     * @brief Register all the declared addons.
     * @param engine The AngelScript engine
     * @return std::pair<int, int> first int is the number of registered addons, second int is the number of failed to register addons.
     */
    static std::pair<int, int> RegisterInterfaces( asIScriptEngine* engine, bool verbose = false )
    {
        int fails = 0;
        int passes = 0;

        for( auto reg : g_ASRegistry )
        {
            reg->engine = engine;

            if( reg->Register() )
            {
                if( verbose )
                {
                    std::cout << "Registered addon \"" << reg->GetName() << "\"" << std::endl;
                }
                passes++;
            }
            else
            {
                if( verbose )
                {
                    std::cout << "Failed to register addon \"" << reg->GetName() << "\"" << std::endl;
                }
                fails++;
            }

            delete reg;
        }

        g_ASRegistry.clear();

        if( verbose )
        {
            if( fails > 0 )
            {
                std::cout << "failed registering " << fails << " addons out of " << ( fails + passes ) << std::endl;
            }
            else if( passes > 0 )
            {
                std::cout << "All " << passes << " addons has been registered" << std::endl;
            }
        }

        return { fails, passes };
    }
};
