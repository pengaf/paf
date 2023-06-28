#pragma once

#ifdef _DEBUG
#undef _DEBUG
#include "Python.h"
#define _DEBUG 1
#else
#include "Python.h"
#endif
#include "../pafcore/Utility.h"

#if defined PAFPY_EXPORTS
	#define PAFPY_EXPORT __declspec(dllexport)
#else
	#define PAFPY_EXPORT __declspec(dllimport)
#endif

#define BEGIN_PAFPY namespace pafpy {
#define END_PAFPY }
