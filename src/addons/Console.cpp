#include "addons/Console.hpp"
#include "CASDocRegistry.hpp"

class CASDocConsole : public CASDocRegistry
{
    bool Register() override
    {
        return
        RegisterObjectType(
            "Access the CLI console"sv,
            "console_t",
            0,
            asOBJ_REF | asOBJ_NOCOUNT
        ) &&
        RegisterObjectMethod(
            "Print to the console output"sv,
            "console_t",
            "console_t@ Write( const string&in msg )",
            asMETHOD(ASConsole, __ScriptWrite__),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Print line to the console output"sv,
            "console_t",
            "console_t@ WriteLine( const string&in msg )",
            asMETHOD(ASConsole, __ScriptWriteLine__),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Print line to the console output"sv,
            "console_t",
            "console_t@ WriteLine()",
            asMETHOD(ASConsole, __ScriptWriteLineEmpty__),
            asCALL_THISCALL
        ) &&
        RegisterObjectType(
            "Console text & background color."sv,
            "console_color_t",
            0,
            asOBJ_REF | asOBJ_NOCOUNT
        ) &&
        RegisterObjectMethod(
            "Reset the color to default."sv,
            "console_color_t",
            "console_t@ Reset()",
            asMETHOD(ASConsole::Color, Reset),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Set the color to Red, Green and Blue."sv,
            "console_color_t",
            "console_t@ rgb( int red, int green, int blue )",
            asMETHOD(ASConsole::Color, rgb),
            asCALL_THISCALL
        ) &&
        RegisterObjectProperty(
            "Reset the color."sv,
            "console_t",
            "console_color_t@ Fore",
            asOFFSET(ASConsole, Fore)
        ) &&
        RegisterObjectProperty(
            "Reset the color."sv,
            "console_t",
            "console_color_t@ Back",
            asOFFSET(ASConsole, Back)
        ) &&
        RegisterObjectMethod(
            "Reset the color."sv,
            "console_t",
            "console_t@ ResetColor( console_color_t@ target = null )",
            asMETHOD(ASConsole, ResetColor),
            asCALL_THISCALL
        ) &&
        RegisterGlobalProperty(
            "Access the CLI console"sv,
            "console_t@ Console",
            &Console
        );
    }
};

CASDocRegisterInterface(CASDocConsole);
