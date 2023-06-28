#include "PythonSubclassInvoker.h"
#include "../pafcore/Variant.h"

BEGIN_PAFPY

const size_t max_param_count = 20;

PyObject* VariantToPython(paf::Variant* variant);
paf::Variant* PythonToVariant(paf::Variant* value, PyObject* pyObject);

struct VariantWrapper
{
    PyObject_HEAD
	char m_var[sizeof(paf::Variant)];
};

PythonSubclassInvoker::PythonSubclassInvoker(PyObject* pyObject)
{
	m_pyObject = pyObject;
	Py_INCREF(pyObject);
}

PythonSubclassInvoker::~PythonSubclassInvoker()
{
	Py_DECREF(m_pyObject);
}

paf::ErrorCode PythonSubclassInvoker::invoke(const char* name, paf::Variant* self, paf::Variant* results, size_t numResults, paf::Variant* args, size_t numArgs)
{
	PyObject* pyFunc = PyObject_GetAttrString(m_pyObject, name);
	if(pyFunc)
	{
		if(PyCallable_Check(pyFunc))
		{
			PyObject* pyArgs[max_param_count];
			PyObject* pyTuple = 0;
			if(numArgs)
			{
				pyTuple = PyTuple_New(numArgs);
				for(size_t i = 0; i < numArgs; ++i)
				{
					pyArgs[i] = VariantToPython(&args[i]);
					PyTuple_SetItem(pyTuple, i, pyArgs[i]);
				}
			}
			PyObject* pyResult = PyObject_CallObject(pyFunc, pyTuple);
			if(pyResult)
			{
				paf::Variant* value = PythonToVariant(results, pyResult);
				if(value != results)
				{
					//result->move(*value);
				}
			}
			for(size_t i = 0; i < numArgs; ++i)
			{
				//args[i].move(*(paf::Variant*)((VariantWrapper*)pyArgs[i])->m_var);
			}
			Py_XDECREF(pyTuple);
			Py_XDECREF(pyResult);
		}
		Py_DECREF(pyFunc);
		return paf::ErrorCode::s_ok;
	}
	else 
	{
		PyErr_Clear();
		return paf::ErrorCode::e_script_dose_not_override;
	}
}

END_PAFPY
