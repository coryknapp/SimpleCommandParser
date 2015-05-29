#include "Command.h"
//clang++ -std=c++11 test.cpp

int main(int argc, char *argv[])
{
	std::string testString( "test { a , b , c , [foo : 1, bar : 2 ] }" );
	std::cout << "VVVVVVVVV" << std::endl;
	std::cout << "running tests on Command with init string:" << std::endl
		<< "\t" << testString << std::endl;
	Command testCmd( testString );
	std::cout <<"Reseralizes as " << testCmd.text() << std::endl;


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
	if( testCmd.getValueAt<std::string>("1") == "b" )
		std::cout<<"\tsuccess!" << std::endl;
	else{
		std::cout << "\tfail :(" << std::endl;
		std::cout << "getValueAt<std::string>(\"1\") returns \""
		  << testCmd.getValueAt<std::string>("1") << "\"" << std::endl;
	}


	std::cout << "Testing getValueAt(\"3/foo\")" << std::endl;
	if( testCmd.getValueAt<std::string>("3/foo") == "1" )
		std::cout<<"\tsuccess!" << std::endl;
	else{
		std::cout << "\tfail :(" << std::endl;
		std::cout << "getValueAt<std::string>(\"3/foo\") returns \""
		  << testCmd.getValueAt<std::string>("3/foo") << "\"" << std::endl;
	}

	std::cout << "Testing getValueAt(\"3/bar\")" << std::endl;
	if( testCmd.getValueAt<std::string>("3/bar") == "2" )
		std::cout<<"\tsuccess!" << std::endl;
	else{
		std::cout << "\tfail :(" << std::endl;
		std::cout << "getValueAt<std::string>(\"3/bar\") returns \""
		  << testCmd.getValueAt<std::string>("3/bar") << "\"" << std::endl;
	}


	return 0;
}
