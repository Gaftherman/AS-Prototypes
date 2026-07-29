#pragma once

#include <cstring>
#include <cctype>
#include <cstddef>

#ifdef _WIN32
#ifndef stricmp
#define stricmp _stricmp
#endif
#ifndef strnicmp
#define strnicmp _strnicmp
#endif
#else
#ifndef stricmp
#define stricmp strcasecmp
#endif
#ifndef strnicmp
#define strnicmp strncasecmp
#endif
#endif

inline size_t StringHash( const char* str )
{
    if (!str) return 0;
    size_t hash = 5381;
    int c;
    while ((c = static_cast<unsigned char>(*str++)))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

inline bool PrintfSuccess( int iLength, size_t uiBufferSize )
{
    return iLength >= 0 && static_cast<size_t>(iLength) < uiBufferSize;
}
