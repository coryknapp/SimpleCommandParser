#include "Command.h"
//clang++ -std=c++11 test.cpp

int main(int argc, char *argv[])
{
	std::string testString( "test { a, b, c, [foo:1, bar:2 ] }" );

	std::cout << "running tests on Command with init string:" << std::endl
		<< "\t" << testString << std::endl;
	Command testCmd( testString );	


	std::cout << "Testing type()" << std::endl;
	if( testCmd.type() == "test" )
		std::cout<<"\tsuccess!" << std::endl;
	else
		std::cout << "\tfail :(" << std::endl;


	std::cout << "Testing getValueAt(\"0\")" << std::endl;
	if( testCmd.getValueAt<std::string>("0") == "a" )
		std::cout<<"\tsuccess!" << std::endl;
	else{
		std::cout << "\tfail :(" << std::endl;
		std::cout << "getValueAt<std::string>(\"0\") returns \""
		  << testCmd.getValueAt<std::string>("0") << "\"" << std::endl;
	}


	std::cout << "Testing getValueAt(\"1\")" << std::endl;
	if( testCmd.getValueAt<std::string>("1") == "a" )
		std::cout<<"\tsuccess!" << std::endl;
	else{
		std::cout << "\tfail :(" << std::endl;
		std::cout << "getValueAt<std::string>(\"1\") returns \""
		  << testCmd.getValueAt<std::string>("1") << "\"" << std::endl;
	}


	std::cout << "Testing getValueAt(\"3/foo\")" << std::endl;
	if( testCmd.getValueAt<std::string>("3/foo") == "a" )
		std::cout<<"\tsuccess!" << std::endl;
	else{
		std::cout << "\tfail :(" << std::endl;
		std::cout << "getValueAt<std::string>(\"3/foo\") returns \""
		  << testCmd.getValueAt<std::string>("3/foo") << "\"" << std::endl;
	}
	return 0;
}
