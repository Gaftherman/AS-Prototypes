#pragma once

// Reference: https://github.com/sashi0034/angel-lsp/blob/main/examples/generate_as_predefined.cpp

#include <string>
#include <angelscript.h>

namespace ASDoc
{
    void RegisterObjectTypeComment(const std::string& obj, const std::string& comment);
    void RegisterEnumComment(const std::string& enumName, const std::string& comment);
    void RegisterEnumValueComment(const std::string& enumName, const std::string& valName, const std::string& comment);
    void RegisterFunctionComment(asIScriptFunction* func, const std::string& comment);
    void RegisterScopedFunctionComment(const asITypeInfo* type, asIScriptFunction* func, const std::string& comment);
    void RegisterPropertyComment(const asITypeInfo* type, const std::string& propName, const std::string& comment);
    void RegisterGlobalPropertyComment(const std::string& propName, const std::string& comment);

    std::string GetObjectTypeComment(const asITypeInfo* type);
    std::string GetEnumComment(const asITypeInfo* enumType);
    std::string GetEnumValueComment(const asITypeInfo* enumType, const std::string& valName);
    std::string GetFunctionComment(const asIScriptFunction* func);
    std::string GetScopedFunctionComment(const asITypeInfo* type, const asIScriptFunction* func);
    std::string GetPropertyComment(const asITypeInfo* type, const std::string& propName);
    std::string GetGlobalPropertyComment(const std::string& propName);
}

int RegisterObjectTypeWithComment(asIScriptEngine* engine, const char* obj, int byteSize, asDWORD flags, const char* comment = nullptr);
int RegisterEnumWithComment(asIScriptEngine* engine, const char* type, const char* comment = nullptr);
int RegisterEnumValueWithComment(asIScriptEngine* engine, const char* type, const char* valName, int val, const char* comment = nullptr);
int RegisterObjectMethodWithComment(asIScriptEngine* engine, const char* obj, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment = nullptr);
int RegisterGlobalFunctionWithComment(asIScriptEngine* engine, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment = nullptr);
int RegisterObjectBehaviourWithComment(asIScriptEngine* engine, const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment = nullptr);
int RegisterObjectPropertyWithComment(asIScriptEngine* engine, const char* obj, const char* declaration, int byteOffset, const char* comment = nullptr);
int RegisterGlobalPropertyWithComment(asIScriptEngine* engine, const char* declaration, void* pointer, const char* comment = nullptr);

#define REGISTER_OBJECT_TYPE(engine, obj, byteSize, flags, comment) \
    RegisterObjectTypeWithComment(engine, obj, byteSize, flags, comment)

#define REGISTER_ENUM(engine, type, comment) \
    RegisterEnumWithComment(engine, type, comment)

#define REGISTER_ENUM_VALUE(engine, type, valName, val, comment) \
    RegisterEnumValueWithComment(engine, type, valName, val, comment)

#define REGISTER_OBJECT_METHOD(engine, obj, decl, funcPtr, callConv, comment) \
    RegisterObjectMethodWithComment(engine, obj, decl, funcPtr, callConv, comment)

#define REGISTER_GLOBAL_FUNCTION(engine, decl, funcPtr, callConv, comment) \
    RegisterGlobalFunctionWithComment(engine, decl, funcPtr, callConv, comment)

#define REGISTER_OBJECT_BEHAVIOUR(engine, obj, behaviour, decl, funcPtr, callConv, comment) \
    RegisterObjectBehaviourWithComment(engine, obj, behaviour, decl, funcPtr, callConv, comment)

#define REGISTER_OBJECT_PROPERTY(engine, obj, decl, byteOffset, comment) \
    RegisterObjectPropertyWithComment(engine, obj, decl, byteOffset, comment)

#define REGISTER_GLOBAL_PROPERTY(engine, decl, pointer, comment) \
    RegisterGlobalPropertyWithComment(engine, decl, pointer, comment)

// Generates as.predefined file containing defined symbols for language server
void GenerateScriptPredefined(const asIScriptEngine* engine, const std::string& path = "as.predefined");
