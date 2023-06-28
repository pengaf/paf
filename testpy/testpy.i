//tutorial
#import "Utility.i"
#import "String.i"

#{
#include <stdio.h>
#include "../pafcore/SmartPtr.h"
#}

namespace tutorial
{
	struct Test
	{
		static paf::String Run();
		static int, int Sort2(int a, int b=#-3LL);
	};
#{
	inline paf::String Test::Run()
	{
		return "Hello World!";
	}
	inline int Test::Sort2(int& max, int a, int b)
	{
		max = a > b ? a : b;
		return a < b ? a : b;
	}
#}


	struct Point
	{
		float x;
		float y;
		nocode void getX(float& f);
		nocode paf::string_t test();
		nocode Point();
#{
		void getX(float& f)
		{
			f = x;
		}
		paf::string_t test()
		{
			return "test";
		}
#}
	};
}
