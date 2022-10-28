#include "String.h"
#include "String.mh"
#include "String.ic"
#include "String.mc"
#include <string.h>

BEGIN_PAF

char* String::s_emptyString = "";

String::String() : m_str(s_emptyString)
{}

String::String(const String& arg)
{
	if (s_emptyString != arg.m_str)
	{
		size_t len = strlen(arg.m_str) + 1;
		m_str = new char[len];
		memcpy(m_str, arg.m_str, len * sizeof(char));
	}
	else
	{
		m_str = s_emptyString;
	}
}

String::String(String&& arg)
{
	m_str = arg.m_str;
	arg.m_str = s_emptyString;
}

String::String(const char* str)
{
	if (str && str[0])
	{
		size_t len = strlen(str) + 1;
		m_str = new char[len];
		memcpy(m_str, str, len * sizeof(char));
	}
	else
	{
		m_str = s_emptyString;
	}
}

String::~String()
{
	if (s_emptyString != m_str)
	{
		delete[]m_str;
	}
}

size_t String::length() const
{
	return strlen(m_str);
}

void String::assign(string_t str)
{
	if (s_emptyString != m_str)
	{
		delete[]m_str;
	}
	if (0 == str || 0 == *str)
	{
		m_str = s_emptyString;
	}
	else
	{
		size_t len = strlen(str) + 1;
		m_str = new char[len];
		memcpy(m_str, str, len * sizeof(char));
	}
}

void String::append(string_t str)
{
	if (0 != str && 0 != *str)
	{
		size_t len1 = strlen(m_str);
		size_t len2 = strlen(str) + 1;
		char* newStr = new char[len1 + len2];
		memcpy(newStr, m_str, len1 * sizeof(char));
		memcpy(newStr + len1, str, len2 * sizeof(char));
		delete[]m_str;
		m_str = newStr;
	}
}


END_PAF
