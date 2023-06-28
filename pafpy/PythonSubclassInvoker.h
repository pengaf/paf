#pragma once

#include "Utility.h"

BEGIN_PAFPY

struct PythonSubclassInvoker : public paf::SubclassInvoker
{
public:
	PythonSubclassInvoker(PyObject* pyObject);
	~PythonSubclassInvoker();
public:
	virtual paf::ErrorCode invoke(const char* name, paf::Variant* self, paf::Variant* results, size_t numResults, paf::Variant* args, size_t numArgs) override;
public:
	PyObject* m_pyObject;
};

END_PAFPY
