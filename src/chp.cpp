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
#include "rule.h"

vector<vector<float> > get_fanout(vector<vector<int> > program, vector<float> sizes)
{
	vector<float> node_capacitance(program.size(), 0.0);
	for (size_t i = 0; i < program.size(); i++)
		for (size_t j = 0; j < program[i].size(); j++)
			node_capacitance[i] += sizes[j]*(float)program[i][j];

	vector<vector<float> > fanout(program.size(), vector<float>(program.size(), 0.0));
	for (size_t i = 0; i < program.size(); i++)
		for (size_t j = 0; j < program[i].size(); j++)
			fanout[j][i] = program[i][j] == 0 ? 0.0 : node_capacitance[j]/node_capacitance[i];

	return fanout;
}

float get_distance(vector<vector<float> > fanout)
{
	float distance = 0.0f;
	for (size_t i = 0; i < fanout.size(); i++)
		for (size_t j = 0; j < fanout[i].size(); j++)
			if (fanout[i][j] != 0.0)
				distance += (fanout[i][j] - 1.0)*(fanout[i][j] - 1.0);

	return distance;
}

int main(int argc, char **argv)
{
	srand(time(0));

	vector<string> files;
	bool transistor = false;
	bool gate = false;

	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "--help", 6) == 0 || strncmp(argv[i], "-h", 2) == 0)			// Help
		{
			cout << "Usage: size [options] file" << endl;
			cout << "Options:" << endl;
			cout << " -h,--help\t\t\t\tDisplay this information" << endl;
			cout << " -v,--version\t\t\t\tDisplay compiler version information" << endl;
			cout << " -t,--transistor\t\t\t\tGenerate the transistor level sizing" << endl;
			cout << " -g,--gate\t\t\t\tGenerate the gate level sizing using logical effort" << endl;
			return 0;
		}
		else if (strncmp(argv[i], "--version", 9) == 0 || strncmp(argv[i], "-v", 2) == 0)	// Version Information
		{
			cout << "size 1.0.0" << endl;
			cout << "Copyright (C) 2013 Sol Union." << endl;
			cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
			cout << endl;
			return 0;
		}
		else if (strncmp(argv[i], "--transistor", 12) == 0 || strncmp(argv[i], "-t", 2) == 0)	// Version Information
			transistor = true;
		else if (strncmp(argv[i], "--gate", 6) == 0 || strncmp(argv[i], "-g", 2) == 0)	// Version Information
			gate = true;
		else
			files.push_back(argv[i]);
	}



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

				vector<rule> rules;
				variable_space vars;
				while (tokens.peek_next() != "")
					rules.push_back(rule(tokens, vars));

				if (is_clean())
				{
					if (transistor)
					{
						for (size_t j = 0; j < rules.size(); j++)
						{
							rules[j].left.clear_sizes();
							rules[j].left.calculate_transistor_sizes();
						}

						cout << "Transistor Level Sizing:" << endl;
						for (size_t j = 0; j < rules.size(); j++)
							cout << rules[j] << endl;
						cout << endl;
					}

					if (gate)
					{
						vector<vector<int> > program;
						for (size_t j = 0; j < rules.size(); j++)
						{
							vector<vector<int> > temp = rules[j].generate_program();
							if (program.size() < temp.size())
								program.resize(temp.size(), vector<int>());
							for (size_t k = 0; k < temp.size(); k++)
							{
								if (program[k].size() < temp[k].size())
									program[k].resize(temp[k].size(), 0);

								for (size_t l = 0; l < temp[k].size(); l++)
									program[k][l] += temp[k][l];
							}
						}

						program.resize(vars.globals.size(), vector<int>());
						for (size_t j = 0; j < program.size(); j++)
							program[j].resize(vars.globals.size(), 0);

						cout << "Program" << endl;
						for (size_t j = 0; j < program.size(); j++)
						{
							cout << vars.globals[j].name[0] << " =";
							bool first = true;
							for (size_t k = 0; k < program[j].size(); k++)
							{
								if (program[j][k] != 0)
								{
									if (!first)
										cout << " +";
									else
										first = false;

									cout << " " << program[j][k] << "*" << vars.globals[k].name[0];
								}
							}
							cout << endl;
						}
						cout << endl;

						vector<float> sizes = vector<float>(vars.globals.size(), 1.0);
						vector<vector<float> > fanout = get_fanout(program, sizes);
						vector<vector<float> > initial_fanout = fanout;

						vector<vector<float> > test_sizes;
						vector<vector<vector<float> > > test_fanout;
						vector<float> test_distances;
						float distance = get_distance(fanout);
						int progress = 0;
						do
						{
							test_sizes.assign(vars.globals.size()*2, sizes);
							test_distances.assign(vars.globals.size()*2, 0.0);
							test_fanout.assign(vars.globals.size()*2, vector<vector<float> >());
							for (size_t j = 0; j < vars.globals.size(); j++)
							{
								test_sizes[j*2 + 0][j]++;
								if (test_sizes[j*2+1][j] > 1)
									test_sizes[j*2 + 1][j]--;

								test_fanout[j*2 + 0] = get_fanout(program, test_sizes[j*2 + 0]);
								test_fanout[j*2 + 1] = get_fanout(program, test_sizes[j*2 + 1]);
								test_distances[j*2 + 0] = get_distance(test_fanout[j*2 + 0]);
								test_distances[j*2 + 1] = get_distance(test_fanout[j*2 + 1]);
							}

							bool found = false;
							for (size_t j = 0; j < test_distances.size(); j++)
								if (test_distances[j] < distance)
								{
									distance = test_distances[j];
									sizes = test_sizes[j];
									fanout = test_fanout[j];
									found = true;
								}

							if (!found)
								progress++;
							else
								progress = 0;

						} while (progress < 500);


						cout << "Fanout\t\tInitial\tFinal" << endl;
						for (size_t i = 0; i < fanout.size(); i++)
							for (size_t j = 0; j < fanout[i].size(); j++)
								if (fanout[i][j] > 0.0)
									printf("%s/%s:%s%5.3f\t%5.3f\n", vars.globals[i].name[0].c_str(), vars.globals[j].name[0].c_str(), string(2 - (vars.globals[i].name[0].size() + 2 + vars.globals[j].name[0].size())/8, '\t').c_str(), initial_fanout[i][j], fanout[i][j]);
						cout << endl;

						cout << "Resulting Scale Factors:" << endl;
						for (size_t j = 0; j < sizes.size(); j++)
							cout << vars.globals[j].name[0] << ":" << string(2 - vars.globals[j].name[0].size()/8, '\t') << sizes[j] << endl;
						cout << endl;

						for (size_t j = 0; j < rules.size(); j++)
							rules[j].set_sizes(sizes);

						cout << "Gate Level Sizing:" << endl;
						for (size_t j = 0; j < rules.size(); j++)
							cout << rules[j] << endl;
						cout << endl;
					}
				}
			}
		}



		complete();
	}
	else
	{
		cout << "Usage: size [options] file" << endl;
		cout << "Options:" << endl;
		cout << " -h,--help\t\t\t\tDisplay this information" << endl;
		cout << " -v,--version\t\t\t\tDisplay compiler version information" << endl;
		cout << " -t,--transistor\t\t\t\tGenerate the transistor level sizing" << endl;
		cout << " -g,--gate\t\t\t\tGenerate the gate level sizing using logical effort" << endl;
	}

	return 0;
}
