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

#if ASCONSOLE
#include "addons/Console.hpp"
#endif

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

#if ASCONSOLE
#define PRINT_REG( obj, res ) if(Verbose){Console->Write( "Registering: \"" )->Fore->rgb(0,255,0)->Write(obj)->Fore->Reset() \
->Write( "\" asERetCodes: " )->Fore->rgb( res >= 0 ? 0 : 255, res >= 0 ? 255 : 0, 0 )->Write( res )->Fore->Reset()->WriteLine();}
#define PRINT_DOC( docString ) if(Verbose && !docString.empty()){Console->Write("Documentation: ")->Back->rgb(50,50,50)->Fore->rgb(135, 135, 1)->Write(docString)->ResetColor()->WriteLine();}
#else
#define PRINT_REG( obj, res ) std::cout << "Registering: \"" << obj << "\" asERetCodes: " << res << std::endl;
#define PRINT_DOC( docString ) if(Verbose && !docString.empty()){ std::cout << "Documentation: " << docString << std::endl;}
#endif

// Generate docstring for "declaration" and call "method" for asIScriptEngine. does assertion check, prints messages and returns true/false if no assertion.
#define APIREG( declaration, method ) \
{ \
    int r = Engine->method; \
    PRINT_REG( declaration, r ) \
    assert( r >= 0 ); \
    return ( r >= 0 ); \
}

    static bool SetDefaultNamespace( const char* nameSpace )
    APIREG( nameSpace, SetDefaultNamespace( nameSpace ) )

    bool SetDefaultNamespace( std::string_view docString, const char* nameSpace )
    {
        int r = SetDefaultNamespace( nameSpace );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            PRINT_DOC(docString);

            // -TODO Set a namespace and all the methods registered later will fill into it until namespace changed.
        }

        assert( !docString.empty() );

        return ( r >= 0 );
    }

    static bool RegisterObjectProperty( const char* obj, const char* declaration, int byteOffset )
    APIREG( declaration, RegisterObjectProperty( obj, declaration, byteOffset ) )

    bool RegisterObjectProperty( std::string_view docString, const char* obj, const char* declaration, int byteOffset )
    {
        int r = RegisterObjectProperty( obj, declaration, byteOffset );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            PRINT_DOC(docString);

            asITypeInfo* type = Engine->GetTypeInfoByDecl(obj);

            if( type != nullptr )
            {
                std::string declStr(declaration);
                size_t spacePos = declStr.rfind(' ');
                std::string propName = (spacePos != std::string::npos) ? declStr.substr(spacePos + 1) : declStr;
                ASDoc::RegisterPropertyComment(type, propName, comment, this->GetName() );
            }
        }

        assert( !docString.empty() );

        return ( r >= 0 );
    }

    static bool RegisterGlobalProperty( const char* declaration, void* pointer )
    APIREG( declaration, RegisterGlobalProperty( declaration, pointer ) )

    bool RegisterGlobalProperty( std::string_view docString, const char* declaration, void* pointer )
    {
        int r = RegisterGlobalProperty( declaration, pointer );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            PRINT_DOC(docString);

            std::string declStr( declaration );
            size_t spacePos = declStr.rfind(' ');
            std::string propName = ( spacePos != std::string::npos ) ? declStr.substr( spacePos + 1 ) : declStr;
            ASDoc::RegisterGlobalPropertyComment( propName, comment, this->GetName() );
        }

        assert( !docString.empty() );

        return ( r >= 0 );
    }

    static bool RegisterGlobalFunction( const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    APIREG( declaration, RegisterGlobalFunction( declaration, funcPointer, callConv ) )

    bool RegisterGlobalFunction( std::string_view docString, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    {
        int r = RegisterGlobalFunction( declaration, funcPointer, callConv );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            PRINT_DOC(docString);

            asIScriptFunction* func = Engine->GetGlobalFunctionByDecl( declaration );

            if( func )
            {
                ASDoc::RegisterFunctionComment(func, comment, this->GetName() );
            }
        }

        assert( !docString.empty() );

        return ( r >= 0 );
    }

    static bool RegisterObjectType( const char* obj, int byteSize, asDWORD flags )
    APIREG( obj, RegisterObjectType( obj, byteSize, flags ) )

    bool RegisterObjectType( std::string_view docString, const char* obj, int byteSize, asDWORD flags )
    {
        int r = RegisterObjectType( obj, byteSize, flags );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            PRINT_DOC(docString);

            ASDoc::RegisterObjectTypeComment( obj, comment, this->GetName() );
        }

        assert( !docString.empty() );

        return ( r >= 0 );
    }

    static bool RegisterObjectBehaviour( const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    APIREG( obj, RegisterObjectBehaviour( obj, behaviour, declaration, funcPointer, callConv ) )

    bool RegisterObjectBehaviour( std::string_view docString, const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    {
        int r = RegisterObjectBehaviour( obj, behaviour, declaration, funcPointer, callConv );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            PRINT_DOC(docString);

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
    APIREG( obj, RegisterObjectMethod( obj, declaration, funcPointer, callConv ) )

    bool RegisterObjectMethod( std::string_view docString, const char* obj, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv )
    {
        int r = RegisterObjectMethod( obj, declaration, funcPointer, callConv );

        if( GeneratePredefined )
        {
            std::string comment( docString );

            PRINT_DOC(docString);

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

        if( fails == 0 )
        {
            if( GeneratePredefined )
            {
#ifndef NDEBUG
                GenerateScriptPredefined( Engine, "../as.predefined" );
#else
                GenerateScriptPredefined( Engine, "as.predefined" );
#endif
            }
            if( GenerateDocumentation )
            {
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
