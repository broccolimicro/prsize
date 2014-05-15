/*
 * chp.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: Ned Bingham and Nicholas Kramer
 *
 *  DO NOT DISTRIBUTE
 */

#include "common.h"
#include "message.h"
#include "process.h"

int main(int argc, char **argv)
{
	srand(time(0));

	vector<string> files;
	bool transistor = false;
	bool gate = false;

	for (int i = 1; i < argc; i++)
	{
		string arg = argv[i];
		if (arg == "--help" || arg == "-h")			// Help
		{
			cout << "Usage: size [options] file" << endl;
			cout << "Options:" << endl;
			cout << " -h,--help\t\t\t\tDisplay this information" << endl;
			cout << "    --version\t\t\t\tDisplay compiler version information" << endl;
			cout << " -v,--verbose\t\t\t\tPrint all steps taken during execution" << endl;
			cout << " -t,--transistor\t\t\t\tGenerate the transistor level sizing" << endl;
			cout << " -g,--gate\t\t\t\tGenerate the gate level sizing using logical effort" << endl;
			return 0;
		}
		else if (arg == "--version")	// Version Information
		{
			cout << "size 1.0.0" << endl;
			cout << "Copyright (C) 2013 Sol Union." << endl;
			cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
			cout << endl;
			return 0;
		}
		else if (arg == "--transistor" || arg == "-t")	// Version Information
			transistor = true;
		else if (arg == "--gate" || arg == "-g")	// Version Information
			gate = true;
		else if (arg == "--verbose" || arg == "-v")
			set_verbose();
		else
			files.push_back(arg);
	}

	vector<process> processes;

	if (files.size() > 0)
	{
		for (size_t i = 0; i < files.size(); i++)
		{
			ifstream fin;
			fin.open(files[i].c_str(), ios::binary | ios::in);
			if (!fin.is_open())
			{
				error("", "file not found '" + files[i] + "'", "", __FILE__, __LINE__);
				return false;
			}
			else
			{
				fin.seekg(0, fin.end);
				size_t size = fin.tellg();
				string buffer(size, ' ');
				fin.seekg(0, fin.beg);
				fin.read(&buffer[0], size);
				fin.close();
				tokenizer tokens(files[i], buffer);

				processes.push_back(process(tokens));
			}
		}
	}
	else
	{
		string pipe = "";
		string line = "";
		while (getline(cin, line))
			pipe += line + "\n";

		tokenizer tokens("stdin", pipe);
		processes.push_back(process(tokens));
	}

	if (is_clean())
	{
		for (size_t i = 0; i < processes.size(); i++)
		{
			if (transistor)
				processes[i].generate_transistor_level_sizing();

			if (gate)
				processes[i].generate_gate_level_sizing();

			processes[i].print_rules();
		}
	}

	complete();

	return 0;
}
