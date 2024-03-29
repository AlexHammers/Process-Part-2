#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <memory>
#include "Process.hpp"

int main(int argc, char *argv[])
{
	using namespace std;
	std::vector<char*> args;

	for (int nn = 1; nn < argc; ++nn)
	{
		std::cerr << "Arg: " << argv[nn] << std::endl;
		args.push_back(argv[nn]);
	}
	args.push_back(NULL);

	string line;
	string output;

	{
		Process myproc(args, true);
		//write/read test
		for (int n = 0; n < 5; ++n)
		{
			stringstream ss;
			ss << "1+ " << n << endl;
			cerr << "calling write with line=" << ss.str() << "END" << endl;
			myproc.write(ss.str());
			string output = myproc.read();
			cerr << "output from process: " << output << "END" << endl;
		}

	cerr << "Process object destroyed" << endl;
}
	cerr << "Program exiting. Confirm child process has been cleaned up."
			<< endl;
	return (EXIT_SUCCESS);
}
