#pragma once
#include "Utility.h"
#include <stack>

BEGIN_PAFCORE

#ifdef _DEBUG
#define PAF_FILE_LINE ::pafcore::FileLineStack::Tag PAF_CONCAT(fileLineTag_, __LINE__) (PAF_WIDE(__FILE__), __LINE__)
#else
#define PAF_FILE_LINE
#endif // _DEBUG

#ifdef _DEBUG

class PAFCORE_EXPORT FileLineStack
{
public:
	class Tag
	{
		Tag(const Tag&) = delete;
		Tag(Tag&&) = delete;
	public:
		Tag(const wchar_t* file, unsigned int line)
		{
			FileLineStack::GetInstance().push(this, file, line);
		}
		~Tag()
		{
			FileLineStack::GetInstance().pop(this);
		}
	};
public:
	struct FileLine
	{
		Tag* tag;
		const wchar_t* file;
		unsigned int line;
	};
public:
	FileLineStack(const FileLineStack&) = delete;
	FileLineStack(FileLineStack&&) = delete;
	FileLineStack() = default;
public:
	void push(Tag* tag, const wchar_t* file, unsigned int line);
	void pop(Tag* tag);
private:
	std::stack<FileLine> m_fileLines;
public:
	static const wchar_t* GetFile();
	static unsigned int GetLine();
public:
	static FileLineStack& GetInstance();
};

#endif // _DEBUG

END_PAFCORE
