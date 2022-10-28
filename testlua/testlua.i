//tutorial
#import "Utility.i"

#{
#include <stdio.h>
#include "../pafcore/SmartPtr.h"
#}

namespace tutorial
{
	struct Test
	{
		static void Run();
	};
#{
	inline void Test::Run()
	{
		printf("Hello World!");
	}
#}


	struct Point
	{
		float x;
		float y;
		nocode Point();
		nocode void getX(float* f) const;
		nocode paf::string_t test();
#{
		void getX(float* f) const
		{
			*f = x;
		}
		paf::string_t test()
		{
			return "test";
		}
#}
	};
}
