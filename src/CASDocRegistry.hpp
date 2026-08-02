#pragma once

#include <angelscript.h>
#include <assert.h>
#include <vector>
#include <iostream>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

// GeneratePredefined
#include "as_predefined.h"

class CASDocRegistry;

inline std::vector<CASDocRegistry*> g_ASRegistry;

#define CASDocRegisterInterface(T) \
inline CASDocRegistry* gp##T = g_ASRegistry.emplace_back(new T())->__SetName__( #T )
//inline InterfaceType* gp##T = static_cast<T*>(g_ASRegistry.emplace_back(new T()))->__SetName__( #T )

class CASDocRegistry
{
private:

    std::string Name;

protected:

public:

    /**
     * @brief Register angelscript API and documentation
     */
    virtual bool Register() { return false; }

    virtual ~CASDocRegistry() = default;

    // The AngelScript script engine
    static inline asIScriptEngine* Engine;

    // Set to true before registering the application to register documentation.
    static inline bool GenerateDocumentation = false;

    // Set to true before registering the application to register as.predefined.
    static inline bool GeneratePredefined = false;

    // Whatever to print information on what's happening. uses std::cout.
    static inline bool Verbose = false;

    CASDocRegistry() {}

    // Builder pattern because child classes would need to implement constructors with arguments for the register macro :/
    CASDocRegistry* __SetName__( const char* name ) { this->Name = std::string( name ); return this; }

    const char* GetName() { return Name.c_str(); }

    static bool RegisterObjectType( const char* obj, int byteSize, asDWORD flags )
    {
        int r = Engine->RegisterObjectType( obj, byteSize, flags );

        if( Verbose )
        {
            std::cout << "Registering \"" << obj << "\" asERetCodes: " << r << std::endl;;
        }

        assert( r >= 0 );

        return ( r >= 0 );
    }

    bool RegisterObjectType( std::string_view docString, const char* obj, int byteSize, asDWORD flags )
    {
        int r = RegisterObjectType( obj, byteSize, flags );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            if( Verbose )
            {
                std::cout << "Documentation: " << docString << std::endl;
            }

            ASDoc::RegisterObjectTypeComment( obj, comment, this->GetName() );
        }

        assert( !docString.empty() );

        return ( r >= 0 );
    }

    static bool RegisterObjectBehaviour( const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    {
        int r = Engine->RegisterObjectBehaviour( obj, behaviour, declaration, funcPointer, callConv );

        if( Verbose )
        {
            std::cout << "Registering \"" << obj << "::" << declaration << "\" asERetCodes: " << r << std::endl;
        }

        assert( r >= 0 );

        return ( r >= 0 );
    }

    bool RegisterObjectBehaviour( std::string_view docString, const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    {
        int r = RegisterObjectBehaviour( obj, behaviour, declaration, funcPointer, callConv );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            if( Verbose )
            {
                std::cout << "Documentation: " << comment << std::endl;;
            }

            asITypeInfo* type = Engine->GetTypeInfoByDecl(obj);
            asIScriptFunction* func = Engine->GetFunctionById(r);

            if( func != nullptr )
            {
                ASDoc::RegisterScopedFunctionComment( type, func, comment, this->GetName() );
            }
            // -TODO Register object behaviour needed. if it's Factory we need to modify the "obj" string to pass the correct arguments based on function.
            ASDoc::RegisterObjectTypeComment( obj, comment, this->GetName() );
        }

        assert( !docString.empty() );

        return ( r >= 0 );
    }

    static bool RegisterObjectMethod( const char* obj, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    {
        int r = Engine->RegisterObjectMethod( obj, declaration, funcPointer, callConv );

        if( Verbose )
        {
            std::cout << "Registering \"" << obj << "::" << declaration << "\" asERetCodes: " << r << std::endl;
        }

        assert( r >= 0 );

        return ( r >= 0 );
    }

    bool RegisterObjectMethod( std::string_view docString, const char* obj, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    {
        int r = RegisterObjectMethod( obj, declaration, funcPointer, callConv );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            if( Verbose )
            {
                std::cout << "Documentation: " << comment << std::endl;;
            }

            asITypeInfo* type = Engine->GetTypeInfoByDecl(obj);
            asIScriptFunction* func = type ? type->GetMethodByDecl(declaration) : nullptr;

           if( func == nullptr )
           {
                func = Engine->GetFunctionById(r);
           }

            if( func != nullptr )
            {
                ASDoc::RegisterScopedFunctionComment( type, func, comment, this->GetName() );
            }
        }

        assert( !docString.empty() );

        return ( r >= 0 );
    }

    /**
     * @brief Register all the declared addons.
     * @return std::pair<int, int> first int is the number of registered addons, second int is the number of failed to register addons.
     */
    static std::pair<int, int> RegisterInterfaces()
    {
        assert( Engine != nullptr );

        int fails = 0;
        int passes = 0;

        for( auto reg : g_ASRegistry )
        {
            if( reg->Register() )
            {
                if( Verbose )
                {
                    std::cout << "Registered addon \"" << reg->GetName() << "\"" << std::endl;
                }
                passes++;
            }
            else
            {
                if( Verbose )
                {
                    std::cout << "Failed to register addon \"" << reg->GetName() << "\"" << std::endl;
                }
                fails++;
            }
        }

        if( Verbose )
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

    /**
     * @brief Shut down and delete all instances of CASDocRegistry
     */
    static void Shutdown()
    {
        for( auto reg : g_ASRegistry )
        {
            delete reg;
        }
        g_ASRegistry.clear();
    }
};
