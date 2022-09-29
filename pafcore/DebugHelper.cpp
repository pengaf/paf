#include "DebugHelper.h"

BEGIN_PAFCORE

#ifdef _DEBUG

void FileLineStack::push(Tag* tag, const wchar_t* file, unsigned int line)
{
	FileLine fileLine = { tag, file, line };
	m_fileLines.push(fileLine);
}

void FileLineStack::FileLineStack::pop(Tag* tag)
{
	PAF_ASSERT(!m_fileLines.empty() && m_fileLines.top().tag == tag);
	m_fileLines.pop();
}

const wchar_t* FileLineStack::GetFile()
{
	FileLineStack& instance = GetInstance();
	return instance.m_fileLines.empty() ? L"" : instance.m_fileLines.top().file;
}

unsigned int FileLineStack::GetLine()
{
	FileLineStack& instance = GetInstance();
	return instance.m_fileLines.empty() ? 0 : instance.m_fileLines.top().line;
}

FileLineStack& FileLineStack::GetInstance()
{
	static thread_local FileLineStack s_instance;
	return s_instance;
}

#endif // _DEBUG

END_PAFCORE
