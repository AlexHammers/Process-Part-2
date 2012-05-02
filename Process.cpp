//This is Process.cpp
#include "Process.hpp"
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>

Process::Process(const std::vector<char*>& args, bool verbose) :
		verbose(verbose),
		m_name(args[0]),
		m_pid((pid_t) NULL),
		m_writepipe{ -1, -1 },
		m_readpipe{ -1, -1 },
		m_pwrite((FILE*) NULL),
		m_pread((FILE*) NULL)
{
	if (pipe(m_writepipe) < 0)
	{
		perror("Pipe Write");
		throw std::string("Pipe Write");
	}
	if (pipe(m_readpipe) < 0)
	{
		perror("Pipe Read");
		throw std::string("Pipe Read");
	}

	if ((m_pid = fork()) < 0)
	{
		perror("Fork error: ");
		throw std::string("Fork error");
	}
	else if (m_pid == 0)	//Child Process
	{
		if (close(PARENT_READ) < 0)

		{
			perror("child process: close parent read ");
			throw std::string("child process: close parent read");
		}
		if (close(PARENT_WRITE) < 0)
		{
			perror("child process: close parent write ");
			throw std::string("child process: close parent write");
		}

		if (dup2(CHILD_READ, 0) < 0)	//Duplicate file descriptor
		{
			perror("child process: dup read ");
			throw std::string("child process: dup read");
		}
		if (close(CHILD_READ) < 0)		//Clear file descriptor for so it can be reused
		{
			perror("child process: close read ");
			throw std::string("child process: close read");
		}

		if (dup2(CHILD_WRITE, 1) < 0)	//Duplicate file descriptor
		{
			perror("child process: dup write ");
			throw std::string("child process: dup write");
		}
		if (close(CHILD_WRITE) < 0)		//Clear file descriptor for so it can be reused
		{
			perror("child process: close write ");
			throw std::string("child process: close write");
		}

		char** argss = new char*[args.size()];
		copy(args.begin(), args.end(), argss);
		execvp(argss[0], argss);
		perror("Process execvp");
		throw std::string("Process execvp");
	}
	else	//Parent Process
	{
		if (verbose)
			std::cerr << "Process " << m_name << ": forked PID " << m_pid
					<< std::endl;

		if (close(CHILD_READ) < 0)
		{
			perror("child process: close read ");
			throw std::string("child process: close read");
		}
		if (close(CHILD_WRITE) < 0)
		{
			perror("child process: close write ");
			throw std::string("child process: close write");
		}

		m_pread = fdopen(PARENT_READ, "r");
		m_pwrite = fdopen(PARENT_WRITE, "w");
	}
}

Process::~Process()
{
	if (verbose)
		std::cerr << "Process " << m_name << ": Entering ~Process()"
				<< std::endl;
	fclose(m_pwrite);
	fclose(m_pread);
	kill(m_pid, SIGTERM);
	int status;
	pid_t pid = waitpid(m_pid, &status, 0);
	if (pid < 0)
		perror("~Process: Waitpid error: ");

	if (verbose)
		std::cerr << "Process " << m_name << ": Leaving ~Process()"
				<< std::endl;
}

void Process::write(const std::string& line)
{

	fputs(line.c_str(), m_pwrite);

	if (fflush(m_pwrite) < 0)
	{
		perror("Error: writing ");
		throw std::string("Error: writing");
	}
}

std::string Process::read()
{

	std::string readLine;
	char* str = NULL;
	size_t num_bytes;

	if (getline(&str, &num_bytes, m_pread) < 0)
	{
		perror("Error: reading ");
		throw std::string("Error: reading");
	}
	readLine = str;
	return readLine;
}
