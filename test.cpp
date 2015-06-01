#include "Command.h"
//clang++ -std=c++11 test.cpp

#define TEST( name, exp, result ) {\
	std::cout << "Testing " << name << std::endl;\
	if( exp == result )\
		std::cout << "\tsuccess!" << std::endl; \
	else{ \
		std::cout << #exp << " returns " << exp << std::endl; \
	} \
}

using namespace std;

int main(int argc, char *argv[])
{
	//string testString = "test { a , b , c , [foo : 1, bar : 2 ] }";
	string testString = "request [target:1]";
	cout << "running tests on Command with init string:" << endl
		<< "\t" << testString << std::endl;
	Command testCmd( testString );
	cout <<"Reseralizes as " << testCmd.text() << endl;

	TEST( "type", testCmd.type(), "test" );
	TEST(	"get value at 1", *testCmd.getStringAt("1"), "b" );
	TEST(	"get value at 3/foo",
			*testCmd.getStringAt("3/foo"), "1" );
	TEST(	"get value at 3/bar",
			*testCmd.getStringAt("3/bar"), "2" );

	string implicidListString = "test x y x";
	Command * cmdPtr = new Command( implicidListString );
	delete cmdPtr; 
	return 0;
}
