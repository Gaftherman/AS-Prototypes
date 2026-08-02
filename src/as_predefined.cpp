// Reference: https://github.com/sashi0034/angel-lsp/blob/main/examples/generate_as_predefined.cpp

#include "as_predefined.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <filesystem>
#include <unordered_map>
#include <map>
#include <vector>

namespace ASDoc
{
    static std::unordered_map<std::string, std::string> g_objectTypeComments; // Key: obj name
    static std::unordered_map<std::string, std::string> g_enumComments; // Key: enum name
    static std::unordered_map<std::string, std::string> g_enumValueComments; // Key: "EnumName::ValName"
    static std::unordered_map<const asIScriptFunction*, std::string> g_globalFunctionComments;
    static std::unordered_map<std::string, std::string> g_scopedMethodComments; // Key: "TypeName::Decl"
    static std::unordered_map<std::string, std::string> g_propertyComments; // Key: "TypeName::PropName"
    static std::unordered_map<std::string, std::string> g_globalPropertyComments; // Key: "PropName"

    static std::unordered_map<std::string, std::string> g_typeAddons;
    static std::unordered_map<const asIScriptFunction*, std::string> g_functionAddons;
    static std::unordered_map<std::string, std::string> g_namespaceAddons;

    std::string ExtractAddonName(const char* functionSig)
    {
        if (!functionSig) return "";
        std::string sig(functionSig);
        size_t pos = sig.rfind("::");
        if (pos != std::string::npos)
        {
            size_t start = sig.rfind(' ', pos);
            if (start == std::string::npos) start = 0;
            else start++;
            std::string addon = sig.substr(start, pos - start);
            if (addon != "AddonRegistry") return addon;
        }
        return "";
    }

    template <class Stream>
    void printComment(Stream& stream, const std::string& comment, const std::string& indent = "")
    {
        if (comment.empty()) return;
        std::istringstream iss(comment);
        std::string line;
        while (std::getline(iss, line))
        {
            stream << indent << "/// " << line << "\n";
        }
    }

    void RegisterObjectTypeComment(const std::string& obj, const std::string& comment, const char* funcSig)
    {
        if (!obj.empty())
        {
            if (!comment.empty()) g_objectTypeComments[obj] = comment;
            std::string addon = ExtractAddonName(funcSig);
            if (!addon.empty()) g_typeAddons[obj] = addon;
        }
    }

    void RegisterEnumComment(const std::string& enumName, const std::string& comment, const char* /*funcSig*/)
    {
        if (!enumName.empty() && !comment.empty())
        {
            g_enumComments[enumName] = comment;
        }
    }

    void RegisterEnumValueComment(const std::string& enumName, const std::string& valName, const std::string& comment, const char* /*funcSig*/)
    {
        if (!enumName.empty() && !valName.empty() && !comment.empty())
        {
            std::string key = enumName + "::" + valName;
            g_enumValueComments[key] = comment;
        }
    }

    void RegisterFunctionComment(asIScriptFunction* func, const std::string& comment, const char* funcSig)
    {
        if (func)
        {
            std::string addon = ExtractAddonName(funcSig);
            if (!addon.empty())
            {
                g_functionAddons[func] = addon;
                const char* ns = func->GetNamespace();
                if (ns && *ns) g_namespaceAddons[ns] = addon;
            }
            if (!comment.empty())
            {
                if (func->GetObjectType() != nullptr)
                {
                    std::string key = std::string(func->GetObjectType()->GetName()) + "::" + func->GetDeclaration(false, true, true);
                    g_scopedMethodComments[key] = comment;
                }
                else
                {
                    g_globalFunctionComments[func] = comment;
                }
            }
        }
    }

    void RegisterScopedFunctionComment(const asITypeInfo* type, asIScriptFunction* func, const std::string& comment, const char* /*funcSig*/)
    {
        if (type && !comment.empty())
        {
            std::string key = std::string(type->GetName()) + "::" + (func ? func->GetDeclaration(false, true, true) : "");
            g_scopedMethodComments[key] = comment;
        }
    }

    void RegisterPropertyComment(const asITypeInfo* type, const std::string& propName, const std::string& comment, const char* /*funcSig*/)
    {
        if (type && !propName.empty() && !comment.empty())
        {
            std::string key = std::string(type->GetName()) + "::" + propName;
            g_propertyComments[key] = comment;
        }
    }

    void RegisterGlobalPropertyComment(const std::string& propName, const std::string& comment, const char* /*funcSig*/)
    {
        if (!propName.empty() && !comment.empty())
        {
            g_globalPropertyComments[propName] = comment;
        }
    }

    std::string GetObjectTypeComment(const asITypeInfo* type)
    {
        if (!type) return "";
        std::string name = type->GetName();
        auto it = g_objectTypeComments.find(name);
        if (it != g_objectTypeComments.end()) return it->second;
        return "";
    }

    std::string GetEnumComment(const asITypeInfo* enumType)
    {
        if (!enumType) return "";
        std::string name = enumType->GetName();
        auto it = g_enumComments.find(name);
        if (it != g_enumComments.end()) return it->second;
        return "";
    }

    std::string GetEnumValueComment(const asITypeInfo* enumType, const std::string& valName)
    {
        if (!enumType || valName.empty()) return "";
        std::string key = std::string(enumType->GetName()) + "::" + valName;
        auto it = g_enumValueComments.find(key);
        if (it != g_enumValueComments.end()) return it->second;
        return "";
    }

    std::string GetFunctionComment(const asIScriptFunction* func)
    {
        if (!func) return "";
        if (func->GetObjectType() == nullptr)
        {
            auto it = g_globalFunctionComments.find(func);
            if (it != g_globalFunctionComments.end()) return it->second;
        }
        return "";
    }

    std::string GetScopedFunctionComment(const asITypeInfo* type, const asIScriptFunction* func)
    {
        if (!type || !func) return "";
        std::string key = std::string(type->GetName()) + "::" + func->GetDeclaration(false, true, true);
        auto itKey = g_scopedMethodComments.find(key);
        if (itKey != g_scopedMethodComments.end()) return itKey->second;
        return "";
    }

    std::string GetPropertyComment(const asITypeInfo* type, const std::string& propName)
    {
        if (!type) return "";
        std::string key = std::string(type->GetName()) + "::" + propName;
        auto it = g_propertyComments.find(key);
        if (it != g_propertyComments.end()) return it->second;
        return "";
    }

    std::string GetGlobalPropertyComment(const std::string& propName)
    {
        auto it = g_globalPropertyComments.find(propName);
        if (it != g_globalPropertyComments.end()) return it->second;
        return "";
    }

    std::string GetTypeAddon(const asITypeInfo* type)
    {
        if (!type) return "";
        auto it = g_typeAddons.find(type->GetName());
        if (it != g_typeAddons.end()) return it->second;
        return "";
    }

    std::string GetFunctionAddon(const asIScriptFunction* func)
    {
        if (!func) return "";
        auto it = g_functionAddons.find(func);
        if (it != g_functionAddons.end()) return it->second;
        return "";
    }

    std::string GetNamespaceAddon(const std::string& ns)
    {
        if (ns.empty()) return "";
        auto it = g_namespaceAddons.find(ns);
        if (it != g_namespaceAddons.end()) return it->second;
        return "";
    }
}

int RegisterObjectTypeWithComment(asIScriptEngine* engine, const char* obj, int byteSize, asDWORD flags, const char* comment, const char* funcSig)
{
    int r = engine->RegisterObjectType(obj, byteSize, flags);
    if (r >= 0)
    {
        ASDoc::RegisterObjectTypeComment(obj, comment ? comment : "", funcSig);
    }
    assert( r >= 0 );
    return r;
}

int RegisterEnumWithComment(asIScriptEngine* engine, const char* type, const char* comment, const char* funcSig)
{
    int r = engine->RegisterEnum(type);
    if (r >= 0)
    {
        ASDoc::RegisterEnumComment(type, comment ? comment : "", funcSig);
    }
    assert( r >= 0 );
    return r;
}

int RegisterEnumValueWithComment(asIScriptEngine* engine, const char* type, const char* valName, int val, const char* comment, const char* funcSig)
{
    int r = engine->RegisterEnumValue(type, valName, val);
    if (r >= 0)
    {
        ASDoc::RegisterEnumValueComment(type, valName, comment ? comment : "", funcSig);
    }
    assert( r >= 0 );
    return r;
}

int RegisterObjectMethodWithComment(asIScriptEngine* engine, const char* obj, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment, const char* funcSig)
{
    int r = engine->RegisterObjectMethod(obj, declaration, funcPointer, callConv);
    if (r >= 0)
    {
        asITypeInfo* type = engine->GetTypeInfoByDecl(obj);
        asIScriptFunction* func = type ? type->GetMethodByDecl(declaration) : nullptr;
        if (!func) func = engine->GetFunctionById(r);
        if (func) ASDoc::RegisterScopedFunctionComment(type, func, comment ? comment : "", funcSig);
    }
    assert( r >= 0 );
    return r;
}

int RegisterGlobalFunctionWithComment(asIScriptEngine* engine, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment, const char* funcSig)
{
    int r = engine->RegisterGlobalFunction(declaration, funcPointer, callConv);
    if (r >= 0)
    {
        asIScriptFunction* func = engine->GetGlobalFunctionByDecl(declaration);
        if (func) ASDoc::RegisterFunctionComment(func, comment ? comment : "", funcSig);
    }
    assert( r >= 0 );
    return r;
}

int RegisterObjectBehaviourWithComment(asIScriptEngine* engine, const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, const char* comment, const char* funcSig)
{
    int r = engine->RegisterObjectBehaviour(obj, behaviour, declaration, funcPointer, callConv);
    if (r >= 0)
    {
        asITypeInfo* type = engine->GetTypeInfoByDecl(obj);
        asIScriptFunction* func = engine->GetFunctionById(r);
        if (func) ASDoc::RegisterScopedFunctionComment(type, func, comment ? comment : "", funcSig);
    }
    assert( r >= 0 );
    return r;
}

int RegisterObjectPropertyWithComment(asIScriptEngine* engine, const char* obj, const char* declaration, int byteOffset, const char* comment, const char* funcSig)
{
    int r = engine->RegisterObjectProperty(obj, declaration, byteOffset);
    if (r >= 0 && comment && *comment)
    {
        asITypeInfo* type = engine->GetTypeInfoByDecl(obj);
        if (type)
        {
            std::string declStr(declaration);
            size_t spacePos = declStr.rfind(' ');
            std::string propName = (spacePos != std::string::npos) ? declStr.substr(spacePos + 1) : declStr;
            ASDoc::RegisterPropertyComment(type, propName, comment, funcSig);
        }
    }
    assert( r >= 0 );
    return r;
}

int RegisterGlobalPropertyWithComment(asIScriptEngine* engine, const char* declaration, void* pointer, const char* comment, const char* funcSig)
{
    int r = engine->RegisterGlobalProperty(declaration, pointer);
    if (r >= 0 && comment && *comment)
    {
        std::string declStr(declaration);
        size_t spacePos = declStr.rfind(' ');
        std::string propName = (spacePos != std::string::npos) ? declStr.substr(spacePos + 1) : declStr;
        ASDoc::RegisterGlobalPropertyComment(propName, comment, funcSig);
    }
    assert( r >= 0 );
    return r;
}

namespace
{
    struct GlobalPropInfo
    {
        std::string name;
        std::string typeDecl;
    };

    struct NamespaceGroup
    {
        std::vector<const asITypeInfo*> enums;
        std::vector<const asITypeInfo*> classes;
        std::vector<const asIScriptFunction*> functions;
        std::vector<GlobalPropInfo> properties;
        std::vector<const asITypeInfo*> typedefs;
    };

    template <class Stream>
    void printEnum(const asITypeInfo* e, Stream& stream, const std::string& indent)
    {
        std::string comment = ASDoc::GetEnumComment(e);
        ASDoc::printComment(stream, comment, indent);

        stream << indent << "enum " << e->GetName() << "\n" << indent << "{\n";
        std::string valIndent = indent + "    ";
        for (asUINT j = 0; j < e->GetEnumValueCount(); ++j)
        {
            const char* valName = e->GetEnumValueByIndex(j, nullptr);
            if (valName)
            {
                ASDoc::printComment(stream, ASDoc::GetEnumValueComment(e, valName), valIndent);
            }
            stream << valIndent << (valName ? valName : "");
            if (j < e->GetEnumValueCount() - 1) stream << ",";
            stream << "\n";
        }
        stream << indent << "}\n";
    }

    template <class Stream>
    void printClassType(const asITypeInfo* t, Stream& stream, const std::string& indent)
    {
        std::string addon = ASDoc::GetTypeAddon(t);
        if (!addon.empty())
        {
            stream << indent << "/// Addon: " << addon << "\n";
        }
        std::string comment = ASDoc::GetObjectTypeComment(t);
        ASDoc::printComment(stream, comment, indent);

        stream << indent << "class " << t->GetName();
        if (t->GetSubTypeCount() > 0)
        {
            stream << "<";
            for (asUINT sub = 0; sub < t->GetSubTypeCount(); ++sub)
            {
                if (sub < t->GetSubTypeCount() - 1) stream << ", ";
                const auto st = t->GetSubType(sub);
                stream << st->GetName();
            }
            stream << ">";
        }

        stream << "\n" << indent << "{\n";
        std::string memberIndent = indent + "    ";
        for (asUINT j = 0; j < t->GetBehaviourCount(); ++j)
        {
            asEBehaviours behaviours;
            const auto f = t->GetBehaviourByIndex(j, &behaviours);
            if (behaviours == asBEHAVE_CONSTRUCT || behaviours == asBEHAVE_DESTRUCT)
            {
                ASDoc::printComment(stream, ASDoc::GetScopedFunctionComment(t, f), memberIndent);
                stream << memberIndent << f->GetDeclaration(false, true, true) << ";\n";
            }
        }
        for (asUINT j = 0; j < t->GetMethodCount(); ++j)
        {
            const auto m = t->GetMethodByIndex(j);
            ASDoc::printComment(stream, ASDoc::GetScopedFunctionComment(t, m), memberIndent);
            stream << memberIndent << m->GetDeclaration(false, true, true) << ";\n";
        }
        for (asUINT j = 0; j < t->GetPropertyCount(); ++j)
        {
            const char* propName = nullptr;
            t->GetProperty(j, &propName);
            if (propName)
            {
                ASDoc::printComment(stream, ASDoc::GetPropertyComment(t, propName), memberIndent);
            }
            stream << memberIndent << t->GetPropertyDeclaration(j, true) << ";\n";
        }
        for (asUINT j = 0; j < t->GetChildFuncdefCount(); ++j)
        {
            stream << memberIndent << "funcdef " << t->GetChildFuncdef(j)->GetFuncdefSignature()->GetDeclaration(false) << ";\n";
        }
        stream << indent << "}\n";
    }
}

/// @brief Generate 'as.predefined' file, which contains all defined symbols in C++. It is used by the language server.
void GenerateScriptPredefined(const asIScriptEngine* engine, const std::string& path)
{
    const std::string suffix = "as.predefined";
    assert(path.size() >= suffix.size() && path.compare(path.size() - suffix.size(), suffix.size(), suffix) == 0);

    // Collect symbols into namespace groups
    std::map<std::string, NamespaceGroup> nsGroups;

    // 1. Enums
    for (asUINT i = 0; i < engine->GetEnumCount(); i++)
    {
        const auto e = engine->GetEnumByIndex(i);
        if (!e) continue;
        std::string ns = e->GetNamespace();
        nsGroups[ns].enums.push_back(e);
    }

    // 2. Object types (classes)
    for (asUINT i = 0; i < engine->GetObjectTypeCount(); i++)
    {
        const auto t = engine->GetObjectTypeByIndex(i);
        if (!t) continue;
        std::string ns = t->GetNamespace();
        nsGroups[ns].classes.push_back(t);
    }

    // 3. Global functions
    for (asUINT i = 0; i < engine->GetGlobalFunctionCount(); i++)
    {
        const auto f = engine->GetGlobalFunctionByIndex(i);
        if (!f) continue;
        std::string ns = f->GetNamespace();
        nsGroups[ns].functions.push_back(f);
    }

    // 4. Global properties
    for (asUINT i = 0; i < engine->GetGlobalPropertyCount(); i++)
    {
        const char* name = nullptr;
        const char* ns0 = nullptr;
        int type = 0;
        engine->GetGlobalPropertyByIndex(i, &name, &ns0, &type, nullptr, nullptr, nullptr, nullptr);

        const std::string tStr = engine->GetTypeDeclaration(type, true);
        if (tStr.empty() || !name) continue;

        std::string ns = ns0 ? ns0 : "";
        nsGroups[ns].properties.push_back({ name, tStr });
    }

    // 5. Typedefs
    for (asUINT i = 0; i < engine->GetTypedefCount(); ++i)
    {
        const auto type = engine->GetTypedefByIndex(i);
        if (!type) continue;
        std::string ns = type->GetNamespace();
        nsGroups[ns].typedefs.push_back(type);
    }

    std::ostringstream stream;

    // Process global scope (empty namespace "") first
    auto globalIt = nsGroups.find("");
    if (globalIt != nsGroups.end())
    {
        const auto& group = globalIt->second;
        for (const auto* e : group.enums) printEnum(e, stream, "");
        for (const auto* t : group.classes) printClassType(t, stream, "");
        for (const auto* f : group.functions)
        {
            std::string addon = ASDoc::GetFunctionAddon(f);
            if (!addon.empty())
            {
                stream << "/// Addon: " << addon << "\n";
            }
            ASDoc::printComment(stream, ASDoc::GetFunctionComment(f), "");
            stream << f->GetDeclaration(false, false, true) << ";\n";
        }
        for (const auto& prop : group.properties)
        {
            ASDoc::printComment(stream, ASDoc::GetGlobalPropertyComment(prop.name), "");
            stream << prop.typeDecl << " " << prop.name << ";\n";
        }
        for (const auto* type : group.typedefs)
        {
            stream << "typedef " << engine->GetTypeDeclaration(type->GetTypeId()) << " " << type->GetName() << ";\n";
        }
    }

    // Process named namespaces grouped together
    for (const auto& [ns, group] : nsGroups)
    {
        if (ns.empty()) continue; // Already processed global scope

        std::string addon = ASDoc::GetNamespaceAddon(ns);
        if (!addon.empty())
        {
            stream << "/// Addon: " << addon << "\n";
        }
        stream << "namespace " << ns << "\n{\n";
        std::string indent = "    ";

        for (const auto* e : group.enums) printEnum(e, stream, indent);
        for (const auto* t : group.classes) printClassType(t, stream, indent);
        for (const auto* f : group.functions)
        {
            ASDoc::printComment(stream, ASDoc::GetFunctionComment(f), indent);
            stream << indent << f->GetDeclaration(false, false, true) << ";\n";
        }
        for (const auto& prop : group.properties)
        {
            ASDoc::printComment(stream, ASDoc::GetGlobalPropertyComment(prop.name), indent);
            stream << indent << prop.typeDecl << " " << prop.name << ";\n";
        }
        for (const auto* type : group.typedefs)
        {
            stream << indent << "typedef " << engine->GetTypeDeclaration(type->GetTypeId()) << " " << type->GetName() << ";\n";
        }

        stream << "}\n";
    }

    std::string newContent = stream.str();

    // 1:1 buffer comparison to avoid changing timestamps when content hasn't changed
    if (std::filesystem::exists(path))
    {
        std::ifstream inFile(path, std::ios::binary);
        if (inFile.is_open())
        {
            std::string existingContent((std::istreambuf_iterator<char>(inFile)),
                                         std::istreambuf_iterator<char>());
            if (existingContent == newContent)
            {
                return;
            }
        }
    }

    std::ofstream outFile(path, std::ios::binary);
    if (outFile.is_open())
    {
        outFile << newContent;
    }
}
