#include "Command.h"
//clang++ -std=c++11 test.cpp

#include <boost/optional/optional_io.hpp>

#define TEST( name, expression, expected ) {\
	std::cout << "Testing " << name << std::endl;\
	auto result = expression;\
	if( result ){\
		if( expected == *result )\
			std::cout << "\tsuccess!" << std::endl; \
		else{ \
			std::cout << "\tFAIL :(" << endl << \
			"\t" << #expression << " returns " << *result << std::endl; \
		} \
	} else { \
		std::cout << "\tnothing found at address" << std::endl; \
	} \
}

#define TEST_FOR_NOTHING( name, expression ) {\
	std::cout << "Testing " << name << std::endl;\
	auto result = expression;\
	if( result ){\
		std::cout << "\tFAIL :(" << endl << \
		"\t" << #expression << " returns " << *result << std::endl; \
	} else {\
		std::cout << "\tsuccess!" << std::endl; \
	} \
}

using namespace std;

int main(int argc, char *argv[])
{
	string testString = "test { a , b , c , [foo : 1, bar : \"x y\" ] }";
	//string testString = "request [target:1]";
	cout << "running tests on Command with init string:" << endl
		<< "\t" << testString << std::endl;
	Command testCmd( testString );
	cout <<"Reseralizes as " << testCmd.text() << endl;

	//TEST( "type", testCmd.type(), "test" );
	TEST(	"get value at 1", testCmd.getValueAt<string>("1"), "b" );
	TEST(	"get value at 3/foo",
			testCmd.getValueAt<string>("3/foo"), "1" );
	TEST(	"get value at 3/bar",
			testCmd.getValueAt<string>("3/bar"), "x y" );
	TEST_FOR_NOTHING( "Get 3/bazz", testCmd.getValueAt<string>("3/bazz") );

	string implicidListString = "test x y x";
	Command * cmdPtr = new Command( implicidListString, true );
	TEST(	"get value at 2 (implcid)",
			cmdPtr->getValueAt<string>("1"), "y" );

	delete cmdPtr;
	return 0;
}
