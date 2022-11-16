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
