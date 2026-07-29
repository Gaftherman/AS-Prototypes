#pragma once

// Reference: https://github.com/sashi0034/angel-lsp/blob/main/examples/generate_as_predefined.cpp

#include <string>
#include <angelscript.h>

namespace ASDoc
{
    std::string ExtractAddonName(const char* functionSig);

    void RegisterObjectTypeComment(const std::string& obj, const std::string& comment, const char* funcSig = nullptr);
    void RegisterEnumComment(const std::string& enumName, const std::string& comment, const char* funcSig = nullptr);
    void RegisterEnumValueComment(const std::string& enumName, const std::string& valName, const std::string& comment, const char* funcSig = nullptr);
    void RegisterFunctionComment(asIScriptFunction* func, const std::string& comment, const char* funcSig = nullptr);
    void RegisterScopedFunctionComment(const asITypeInfo* type, asIScriptFunction* func, const std::string& comment, const char* funcSig = nullptr);
    void RegisterPropertyComment(const asITypeInfo* type, const std::string& propName, const std::string& comment, const char* funcSig = nullptr);
    void RegisterGlobalPropertyComment(const std::string& propName, const std::string& comment, const char* funcSig = nullptr);

    std::string GetObjectTypeComment(const asITypeInfo* type);
    std::string GetEnumComment(const asITypeInfo* enumType);
    std::string GetEnumValueComment(const asITypeInfo* enumType, const std::string& valName);
    std::string GetFunctionComment(const asIScriptFunction* func);
    std::string GetScopedFunctionComment(const asITypeInfo* type, const asIScriptFunction* func);
    std::string GetPropertyComment(const asITypeInfo* type, const std::string& propName);
    std::string GetGlobalPropertyComment(const std::string& propName);

    std::string GetTypeAddon(const asITypeInfo* type);
    std::string GetFunctionAddon(const asIScriptFunction* func);
    std::string GetNamespaceAddon(const std::string& ns);
}

int RegisterObjectTypeWithComment(asIScriptEngine* engine, const char* obj, int byteSize, asDWORD flags, const char* comment = nullptr, const char* funcSig = nullptr);
int RegisterEnumWithComment(asIScriptEngine* engine, const char* type, const char* comment = nullptr, const char* funcSig = nullptr);
int RegisterEnumValueWithComment(asIScriptEngine* engine, const char* type, const char* valName, int val, const char* comment = nullptr, const char* funcSig = nullptr);
int RegisterObjectMethodWithComment(asIScriptEngine* engine, const char* obj, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment = nullptr, const char* funcSig = nullptr);
int RegisterGlobalFunctionWithComment(asIScriptEngine* engine, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment = nullptr, const char* funcSig = nullptr);
int RegisterObjectBehaviourWithComment(asIScriptEngine* engine, const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment = nullptr, const char* funcSig = nullptr);
int RegisterObjectPropertyWithComment(asIScriptEngine* engine, const char* obj, const char* declaration, int byteOffset, const char* comment = nullptr, const char* funcSig = nullptr);
int RegisterGlobalPropertyWithComment(asIScriptEngine* engine, const char* declaration, void* pointer, const char* comment = nullptr, const char* funcSig = nullptr);

#define REGISTER_OBJECT_TYPE(obj, byteSize, flags, comment) \
    RegisterObjectTypeWithComment(engine, obj, byteSize, flags, comment, __FUNCTION__)

#define REGISTER_ENUM(type, comment) \
    RegisterEnumWithComment(engine, type, comment, __FUNCTION__)

#define REGISTER_ENUM_VALUE(type, valName, val, comment) \
    RegisterEnumValueWithComment(engine, type, valName, val, comment, __FUNCTION__)

#define REGISTER_OBJECT_METHOD(obj, decl, funcPtr, callConv, comment) \
    RegisterObjectMethodWithComment(engine, obj, decl, funcPtr, callConv, comment, __FUNCTION__)

#define REGISTER_GLOBAL_FUNCTION(decl, funcPtr, callConv, comment) \
    RegisterGlobalFunctionWithComment(engine, decl, funcPtr, callConv, comment, __FUNCTION__)

#define REGISTER_OBJECT_BEHAVIOUR(obj, behaviour, decl, funcPtr, callConv, comment) \
    RegisterObjectBehaviourWithComment(engine, obj, behaviour, decl, funcPtr, callConv, comment, __FUNCTION__)

#define REGISTER_OBJECT_PROPERTY(obj, decl, byteOffset, comment) \
    RegisterObjectPropertyWithComment(engine, obj, decl, byteOffset, comment, __FUNCTION__)

#define REGISTER_GLOBAL_PROPERTY(decl, pointer, comment) \
    RegisterGlobalPropertyWithComment(engine, decl, pointer, comment, __FUNCTION__)

// Generates as.predefined file containing defined symbols for language server
void GenerateScriptPredefined(const asIScriptEngine* engine, const std::string& path = "as.predefined");
