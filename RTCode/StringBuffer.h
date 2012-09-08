//----------------------------------------------
//
//  File: StringBuffer.h
//
//  Created:  awolfe 9/16/11
//
//  Description:  Contains templated fixed size string buffers
//	for both wide and narrow strings.  Does not allocate on the heap
//	unless you specifically "new" one of these objects.  Use anywhere
//	you would normally use a char[] or wchar_t[] string.  Using these
//	guys ensures we use the safe version of string functions, and also
//	makes sure problems don't come up like declaring a char string of
//	size 256 in a header, but in the C code treat it like a 512 buffer (etc).
//
//----------------------------------------------

#ifndef STRINGBUFFER_H
#define STRINGBUFFER_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define CP_UTF8 65001

//The base interface classes that all string buffers derive from so that this base type can be passed around
class StringBufferAnySize
{
public:
	//basic functionality
	virtual unsigned int Capacity() const = 0;
	virtual char *Str() = 0;
	virtual const char *Str() const = 0;

	//string manipulation functionality
	virtual void Concat(const char *szText) = 0;
	virtual void Set(const char *szText) = 0;
	virtual void Set(const StringBufferAnySize &rOther) = 0;
	virtual void Format(const char *szFmt, ...) = 0;
	virtual size_t Length() const = 0;
	virtual bool IsEmpty() const {return Length() == 0;}
	virtual void Clear() {Set("");}

	//this is explicitly named instead of just being another Set() call
	//so that people don't accidentally convert between wide and narrow when they don't
	//intend to, which can cause data loss
	//virtual void SetFromWideString(const wchar_t *wszWideString) = 0;

	//some other functions
	virtual void Concat(const char c)
	{
		char temp[2];
		temp[0] = c;
		temp[1] = 0;
		Concat(temp);
	}
};

class WStringBufferAnySize
{
public:
	//basic functionality
	virtual unsigned int Capacity() const = 0;
	virtual wchar_t *Str() = 0;
	virtual const wchar_t *Str() const = 0;

	//string manipulation functionality
	virtual void Concat(const wchar_t *szText) = 0;
	virtual void Set(const wchar_t *szText) = 0;
	virtual void Set(const WStringBufferAnySize &rOther) = 0;
	virtual void Format(const wchar_t *szFmt, ...) = 0;
	virtual size_t Length() const = 0;
	virtual bool IsEmpty() const {return Length() == 0;}
	virtual void Clear() {Set(L"");}

	//this is explicitly named instead of just being another Set() call
	//so that people don't accidentally convert between wide and narrow when they don't
	//intend to, which can cause data loss
	virtual void SetFromNarrowString(const char *szNarrowString) = 0;
};

//the classes that are used to actually create objects
template<unsigned int size>
class StringBuffer : public StringBufferAnySize
{
public:
	StringBuffer()
	{
		m_szBuffer[0] = 0;
	}

	virtual unsigned int Capacity() const { return size; }
	virtual char *Str() { return m_szBuffer; }
	virtual const char *Str() const { return m_szBuffer; }

	virtual void Concat(const char *szText)
	{
		#ifdef PLATFORM_NACL
			strcat(m_szBuffer,szText);
		#else
			strcat_s(m_szBuffer,size,szText);
		#endif
	}

	virtual void Set(const char *szText)
	{
		#ifdef PLATFORM_NACL
			strcpy(m_szBuffer,szText);
		#else
			strcpy_s(m_szBuffer,size,szText);
		#endif
	}

	virtual void Set(const StringBufferAnySize &rOther)
	{
		Set(rOther.Str());
	}

	virtual void Format(const char *szFormat, ...)
	{
		va_list args;
		va_start (args, szFormat);

		#ifdef PLATFORM_NACL
			vsprintf(m_szBuffer,szFormat,args);
		#else
			vsnprintf_s(m_szBuffer,size,_TRUNCATE,szFormat,args);	
		#endif

		va_end (args);
	}

	virtual size_t Length() const
	{
		return strlen(m_szBuffer);
	}

	/*
	//this is explicitly named instead of just being another Set() call
	//so that people don't accidentally convert between wide and narrow when they don't
	//intend to, which can cause data loss
	virtual void SetFromWideString(const wchar_t *wszWideString)
	{
		//first, clear out the string
		Clear();

		//convert old string to new string - NOTE: if the buffer isn't big enough it fails 100% of the way instead of
		//just converting what it can.  Also note, this isn't QUITE a conversion to narrow string.  it makes it into UTF-8
		//which can be stored in a narrow string but it still retains characters > 255, they are just enceded in UTF-8.
		int nChars = ::WideCharToMultiByte( CP_UTF8, 0, wszWideString, -1, m_szBuffer, size, NULL, NULL );

		//make sure it's null terminated
		m_szBuffer[nChars] = 0;
	}
	*/

	/*
	//this explicity converts to utf8 in case the above function is ever changed to throw away wide character data
	virtual void SetUTF8FromWideString(const wchar_t *wszWideString)
	{
		//first, clear out the string
		Clear();

		//convert old string to new string - NOTE: if the buffer isn't big enough it fails 100% of the way instead of
		//just converting what it can.
		int nChars = ::WideCharToMultiByte( CP_UTF8, 0, wszWideString, -1, m_szBuffer, size, NULL, NULL );

		//make sure it's null terminated
		m_szBuffer[nChars] = 0;
	}
	*/

private:
	char m_szBuffer[size];
};

//the classes that are used to actually create objects
template<unsigned int size>
class WStringBuffer : public WStringBufferAnySize
{
public:
	WStringBuffer()
	{
		m_wszBuffer[0] = 0;
	}

	virtual unsigned int Capacity() const { return size; }
	virtual wchar_t *Str() { return m_wszBuffer; }
	virtual const wchar_t *Str() const { return m_wszBuffer; }

	virtual void Concat(const wchar_t *wszText)
	{
		wcscat_s(m_wszBuffer,size,wszText);
	}

	virtual void Set(const wchar_t *wszText)
	{
		wcscpy_s(m_wszBuffer,size,wszText);
	}

	virtual void Set(const WStringBufferAnySize &rOther)
	{
		Set(rOther.Str());
	}

	//this is explicitly named instead of just being another Set() call
	//so that people don't accidentally convert between wide and narrow when they don't
	//intend to, which can cause data loss
	virtual void SetFromNarrowString(const char *szNarrowString)
	{
		//first, clear out the string
		Clear();

		#ifndef PLATFORM_NACL
			//convert old string to new string - NOTE: if the buffer isn't big enough it fails 100% of the way instead of
			//just converting what it can.
			int nChars = ::MultiByteToWideChar( CP_UTF8, 0, szNarrowString, -1, m_wszBuffer, size );

			//make sure it's null terminated
			m_wszBuffer[nChars] = 0;
		#endif
	}

	virtual void Format(const wchar_t *wszFormat, ...)
	{
		va_list args;
		va_start (args, wszFormat);

		#ifdef PLATFORM_NACL
			vswprintf(m_wszBuffer,wszFormat,args);
		#else
			_vsnwprintf_s(m_wszBuffer,size,_TRUNCATE,wszFormat,args);
		#endif

		va_end (args);
	}

	virtual size_t Length() const
	{
		return wcslen(m_wszBuffer);
	}

private:
	wchar_t m_wszBuffer[size];
};

#endif //STRINGBUFFER_H
