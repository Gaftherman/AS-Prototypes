#pragma once

#include <cstdarg>
#include <functional>
#include <cstring>
#include <cassert>
#include <algorithm>

#include "StringUtils.h"

namespace String
{
enum CompareType
{
	CaseSensitive,
	CaseInsensitive
};

static const CompareType DEFAULT_COMPARE = CaseSensitive;

int CompareCharacters( const char char1, const char char2, const CompareType compare = DEFAULT_COMPARE );

int Compare( const char* pszString1, const char* pszString2, const CompareType compare = DEFAULT_COMPARE );
int CompareN( const char* pszString1, const char* pszString2, const size_t uiCount, const CompareType compare = DEFAULT_COMPARE );

char BoolToCharacter( const bool fValue );

char* TrimTrailingCharacters( char* pszString, const char character );
char* TrimTrailingCharacters( char* pszString, const char character, size_t uiLength );

char* TrimLeadingCharacters( char* pszString, const char character );
char* TrimLeadingCharacters( char* pszString, const char character, size_t uiCount );

char* TrimCharacters( char* pszString, const char character );
}

/*
* String class
* Contains a small stack buffer to allow small strings to be stored without using dynamic allocation
*/
class CString
{
public:
	typedef size_t size_type;
	typedef const char* const_iterator;
	typedef char* iterator;

	static const size_type INVALID_INDEX;

	//Minimum size a buffer needs to be to printf a single variable, like int or float
	static const size_type MINIMUM_PRINTF_BUFFER_SIZE = 512;
	static const size_type PRINTF_BUFFER_SIZE = 16384;

	static const CString EMPTY_STRING;
	static const CString WHITESPACE_CHARACTERS;

private:
	static const size_type BUFFER_SIZE = 20;	//At least this much memory is needed before dynamic allocation is required

	static const size_type STATIC_BIT = 31;
	static const size_type STATIC_MASK = static_cast<size_type>( 1 << 31 );
	static const size_type ALLOC_MASK = STATIC_MASK - 1;

public:
	CString();

	CString( const char* pszString );

	CString( const char* pszString, size_type uiBegin, size_type uiCount = INVALID_INDEX );
	CString( const CString& other, size_type uiBegin, size_type uiCount = INVALID_INDEX );

	CString( const CString& other );

	//Allows the string object to take ownership of the pointer.
	CString( char* pszString, const bool bTakeOwnership );

	CString& TakeOwnership( char* pszString );

	CString( const bool fValue );
	CString( const char character );
	CString( const int iValue );
	CString( const unsigned int uiValue );
	CString( const long long int iValue );
	CString( const unsigned long long int uiValue );
	CString( const float flValue );
	CString( const double flValue );

	CString& operator=( const char* pszString );

	CString& Assign( const char* pszString, size_type uiBegin, size_type uiCount );
	CString& Assign( const CString& other, size_type uiBegin, size_type uiCount );

	CString& operator=( const CString& other );

	CString& operator=( const bool fValue );
	CString& operator=( const char character );
	CString& operator=( const int iValue );
	CString& operator=( const unsigned int uiValue );
	CString& operator=( const long long int iValue );
	CString& operator=( const unsigned long long int uiValue );
	CString& operator=( const float flValue );
	CString& operator=( const double flValue );

	~CString();

	bool operator<( const CString& rhs ) const
	{
		return Compare( rhs ) < 0;
	}

	inline const char* CStr() const { return m_pszString; }
	inline char* CStr() { return m_pszString; }

	inline const_iterator Begin() const { return m_pszString; }
	inline iterator Begin() { return m_pszString; }

	inline const_iterator End() const { return m_pszString + Length(); }
	inline iterator End() { return m_pszString + Length(); }

	inline size_type Length() const { return m_uiLength; }

	size_type GetCapacity() const { return m_uiCapacity & ALLOC_MASK; }

	size_type GetDynamicAllocation() const;

	inline bool IsEmpty() const { return !m_uiLength; }

	void Resize( size_type iNewSize );

	void Reserve( size_type iMinimum );

	void Clear();

	void ClearMemory();

	void RecalculateLength();

	char CharAt( size_type uiIndex ) const;

	void SetCharAt( size_type uiIndex, char character );

	//Direct access, no bounds checking
	const char& operator[]( size_t uiIndex ) const;
	char& operator[]( size_t uiIndex );

	CString& operator+=( const char* pszString );

	CString& Append( const char* pszString, size_type uiBegin, size_type uiCount );
	CString& Append( const CString& other, size_type uiBegin, size_type uiCount );

	CString& operator+=( const CString& other );

	CString& operator+=( const bool fValue );
	CString& operator+=( const char character );
	CString& operator+=( const int iValue );
	CString& operator+=( const unsigned int uiValue );
	CString& operator+=( const long long int iValue );
	CString& operator+=( const unsigned long long int uiValue );
	CString& operator+=( const float flValue );
	CString& operator+=( const double flValue );

	int Compare( const char* pszString ) const;
	int Compare( const CString& other ) const;
	int CompareN( const char* pszString, const size_type uiCount ) const;
	int CompareN( const CString& other, const size_type uiCount ) const;

	int ICompare( const char* pszString ) const;
	int ICompare( const CString& other ) const;
	int ICompareN( const char* pszString, const size_type uiCount ) const;
	int ICompareN( const CString& other, const size_type uiCount ) const;

	//Case sensitive compare
	bool operator==( const char* pszString ) const;
	bool operator==( const CString& other ) const;

	bool operator!=( const char* pszString ) const;
	bool operator!=( const CString& other ) const;

	//Trim whitespace
	void Trim( const char character = ' ' );

	bool StartsWith( const char* pszString, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	bool StartsWith( const CString& str, const String::CompareType compare = String::DEFAULT_COMPARE ) const;

	bool EndsWith( const char* pszString, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	bool EndsWith( const CString& str, const String::CompareType compare = String::DEFAULT_COMPARE ) const;

	size_type Find( const char character, const size_type uiStartOffset = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	size_type Find( const char* pszString, const size_type uiStartOffset = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	size_type Find( const CString& str, const size_type uiStartOffset = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;

	size_type RFind( const char character, const size_type uiStartOffset = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	size_type RFind( const char* pszString, const size_type uiStartOffset = 0,const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	size_type RFind( const CString& str, const size_type uiStartOffset = 0,const String::CompareType compare = String::DEFAULT_COMPARE ) const;

	size_type FindFirstOf( const char* pszCharacters, const size_type uiStartOffset = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	size_type FindFirstOf( const CString& szCharacters, const size_type uiStartOffset = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;

	size_type FindLastOf( const char* pszCharacters, const size_type uiStartOffset = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	size_type FindLastOf( const CString& szCharacters, const size_type uiStartOffset = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;

	size_type FindFirstNotOf( const char* pszString, size_t uiStartIndex = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	size_type FindFirstNotOf( const CString& str, size_t uiStartIndex = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;

	size_type FindLastNotOf( const char* pszString, size_t uiStartIndex = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;
	size_type FindLastNotOf( const CString& str, size_t uiStartIndex = 0, const String::CompareType compare = String::DEFAULT_COMPARE ) const;

	void Format( const char* pszFormat, ... );

	void VFormat( const char* pszFormat, va_list list );

	CString& ToLowercase();
	CString& ToUppercase();

	CString SubString( const size_type uiBegin, const size_type uiCount = INVALID_INDEX ) const;

	CString& Replace( const char charToReplace, const char charToSet );

	CString& Replace( const char* pszSubString, const char* pszReplacement, const String::CompareType compare = String::DEFAULT_COMPARE );

	CString& Replace( const CString& szSubString, const CString& szReplacement, const String::CompareType compare = String::DEFAULT_COMPARE );

	void Truncate( const size_type uiMaxLength );

protected:
	void SetStaticString( const char* pszString, size_type iLength = INVALID_INDEX );

private:
	void Construct();

	void Assign( const char* pszString, const size_type iLength );

	void Append( const char* pszString, const size_type iLength );

	void SetCapacity( size_type iCapacity )
	{
		m_uiCapacity = ( m_uiCapacity & STATIC_MASK ) | ( iCapacity & ALLOC_MASK );
	}

	bool IsStatic() const { return ( m_uiCapacity & STATIC_MASK ) != 0; }

	void SetStatic( bool fState )
	{
		m_uiCapacity = ( m_uiCapacity & ALLOC_MASK ) | ( ( ( size_type ) fState ) << STATIC_BIT );
	}

private:
	char m_szBuffer[ BUFFER_SIZE ];
	char* m_pszString;
	size_type m_uiLength;
	size_type m_uiCapacity;
};

CString operator+( const CString& string, const char* pszString );

CString operator+( const CString& string, const CString& other );

CString operator+( const CString& string, const bool fValue );
CString operator+( const CString& string, const char character );
CString operator+( const CString& string, const int iValue );
CString operator+( const CString& string, const unsigned int uiValue );
CString operator+( const CString& string, const long long int iValue );
CString operator+( const CString& string, const unsigned long long int uiValue );
CString operator+( const CString& string, const float flValue );
CString operator+( const CString& string, const double flValue );

bool operator==( const char* pszString, const CString& other );
bool operator!=( const char* pszString, const CString& other );

class CStaticString : public CString
{
public:
	CStaticString();
	CStaticString( char* pszString );

private:
	CStaticString( const CStaticString& );
	CStaticString& operator=( const CStaticString& ) = delete;
	CStaticString( CStaticString&& );
	CStaticString& operator=( CStaticString&& ) = delete;
};

struct CStringLess
{
	inline bool operator()( const CString& lhs, const CString& rhs ) const
	{
		return lhs.Compare( rhs ) < 0;
	}
};

struct CStringCaseInsensitiveLess
{
	inline bool operator()( const CString& lhs, const CString& rhs ) const
	{
		return lhs.ICompare( rhs ) < 0;
	}
};

namespace std
{
template<>
struct hash<CString>
{
	std::size_t operator()( const CString& szStr ) const
	{
		return StringHash( szStr.CStr() );
	}
};
}
