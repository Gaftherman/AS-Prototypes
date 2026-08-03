#define ASCONSOLE 1

#pragma once
#include <angelscript.h>

#include <string>
#include <iostream>
#include <sstream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <type_traits>
template <typename T, typename = void>
struct is_streamable : std::false_type {};

template <typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> 
    : std::true_type {};

template <typename T>
inline constexpr bool is_streamable_v = is_streamable<T>::value;

class ASConsole
{
public:

    ASConsole()
    {
#if defined(_WIN32) || defined(_WIN64)
        HANDLE hOut = GetStdHandle( STD_OUTPUT_HANDLE );

        if( hOut != INVALID_HANDLE_VALUE )
        {
            DWORD dwMode = 0;

            if( GetConsoleMode(hOut, &dwMode) )
            {
                dwMode |= 0x0004; 
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    class Color
    {
        private:
            const std::string Default;
            const std::string Target;
            std::stringstream Current;
            ASConsole* Owner;

        public:

            Color( const char* d, const char* t, ASConsole* owner ) : Default(d), Target(t)
            {
                Owner = owner;
            }

            const std::string GetColor()
            {
                return Current.str();
            }

            const std::string& GetDefault()
            {
                return Default;
            }

            ASConsole* rgb( int Red, int Green, int Blue )
            {
                Current << Target << std::to_string(Red) << ";" << std::to_string(Green) << ";" << std::to_string(Blue) << "m";
                std::cout << Current.str() << std::flush;
                return Owner;
            }

            ASConsole* Reset()
            {
                Current << Default;
                std::cout << Default << std::flush;
                return Owner;
            }
    };

    Color* Fore = new Color( "\033[39m", "\033[38;2;", this );
    Color* Back = new Color( "\033[49m", "\033[48;2;", this );

    ASConsole* ResetColor( Color* target = nullptr )
    {
        if( target != nullptr )
        {
            target->Reset();
            return this;
        }

        Fore->Reset();
        Back->Reset();
        return this;
    }

    ~ASConsole()
    {
        ResetColor();
        delete Fore;
        delete Back;
    }

    template <typename T>
    ASConsole* Write( T&& msg )
    {
        static_assert(is_streamable_v<std::decay_t<T>>, "The given type is not compatible with std::cout<<" );
        std::cout << Fore->GetColor() << Back->GetColor() << msg << std::flush;
        return this;
    }

    template <typename T>
    ASConsole* WriteLine( T&& msg )
    {
        static_assert(is_streamable_v<std::decay_t<T>>, "The given type is not compatible with std::cout<<" );
        std::cout << Fore->GetColor() << Back->GetColor() << msg << Back->GetDefault() << std::endl;
        return this;
    }

    ASConsole* WriteLine()
    {
        std::cout << Back->GetDefault() << std::endl;
        return this;
    }

    ASConsole* __ScriptWrite__( const std::string& msg )
    {
        return Write(msg);
    }

    ASConsole* __ScriptWriteLineEmpty__()
    {
        return WriteLine();
    }

    ASConsole* __ScriptWriteLine__( const std::string& msg )
    {
        return WriteLine(msg);
    }
};

static inline ASConsole* Console = new ASConsole();
