/*
 * process.cpp
 *
 *  Created on: May 15, 2014
 *      Author: nbingham
 */

#include "process.h"
#include "message.h"

gate_net::gate_net()
{

}

gate_net::~gate_net()
{

}

vector<vector<float> > gate_net::fanout(vector<float> sizes)
{
	vector<float> node_capacitance(connections.size(), 0.0);
	for (size_t i = 0; i < connections.size(); i++)
		for (size_t j = 0; j < connections[i].size(); j++)
			node_capacitance[i] += sizes[j]*(float)connections[i][j];

	vector<vector<float> > fanout(connections.size(), vector<float>(connections.size(), 0.0));
	for (size_t i = 0; i < connections.size(); i++)
		for (size_t j = 0; j < connections[i].size(); j++)
			fanout[j][i] = connections[i][j] == 0 ? 0.0 : node_capacitance[j]/node_capacitance[i];

	return fanout;
}

float gate_net::distance(vector<vector<float> > fanout, float desired_fanout)
{
	float result = 0.0f;
	for (size_t i = 0; i < fanout.size(); i++)
		for (size_t j = 0; j < fanout[i].size(); j++)
			if (fanout[i][j] != 0.0)
				result += (fanout[i][j] - desired_fanout)*(fanout[i][j] - desired_fanout);

	return result;
}

process::process()
{

}

process::process(tokenizer &tokens)
{
	parse(tokens);
}

process::~process()
{

}

void process::parse(tokenizer &tokens)
{
	while (tokens.peek_next() != "")
	{
		if (tokens.peek_next() == "=" && tokens.next() == "=")
		{
			string n0 = tokens.next();
			string n1 = tokens.next();

			if (n0.size() > 0 && (n0[0] == '\"' || n0[0] == '\''))
				n0 = n0.substr(1, n0.size()-2);

			if (n1.size() > 0 && (n1[0] == '\"' || n1[0] == '\''))
				n1 = n1.substr(1, n1.size()-2);

			size_t i0 = vars.find(n0);
			size_t i1 = vars.find(n1);

			if (i0 < vars.globals.size() && i1 < vars.globals.size())
			{
				if (i0 != i1)
					vars.merge(i0, i1);
			}
			else if (i0 < vars.globals.size())
			{
				vars.globals[i0].name.push_back(n1);
				sort(vars.globals[i0].name.begin(), vars.globals[i0].name.end());
				vars.globals[i0].name.resize(unique(vars.globals[i0].name.begin(), vars.globals[i0].name.end()) - vars.globals[i0].name.begin());
			}
			else if (i1 < vars.globals.size())
			{
				vars.globals[i1].name.push_back(n0);
				sort(vars.globals[i1].name.begin(), vars.globals[i1].name.end());
				vars.globals[i1].name.resize(unique(vars.globals[i1].name.begin(), vars.globals[i1].name.end()) - vars.globals[i1].name.begin());
			}
			else
			{
				vars.globals.push_back(variable());
				vars.globals.back().name.push_back(n0);
				vars.globals.back().name.push_back(n1);
				sort(vars.globals.back().name.begin(), vars.globals.back().name.end());
				vars.globals.back().name.resize(unique(vars.globals.back().name.begin(), vars.globals.back().name.end()) - vars.globals.back().name.begin());
			}
		}
		else
			rules.push_back(rule(tokens, vars));
	}
}

void process::generate_transistor_level_sizing()
{
	for (size_t j = 0; j < rules.size(); j++)
	{
		rules[j].left.clear_sizes();
		rules[j].left.calculate_transistor_sizes();
	}
}

void process::generate_gate_level_sizing()
{
	generate_gate_net();

	vector<float>					sizes			= vector<float>(vars.globals.size(), 1.0);
	vector<vector<float> >			fanout			= net.fanout(sizes);
	float							distance		= net.distance(fanout);

	vector<vector<float> >			initial_fanout	= fanout;

	vector<vector<float> >			test_sizes;
	vector<vector<vector<float> > >	test_fanout;
	vector<float>					test_distances;
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

			test_fanout[j*2 + 0] = net.fanout(test_sizes[j*2 + 0]);
			test_fanout[j*2 + 1] = net.fanout(test_sizes[j*2 + 1]);
			test_distances[j*2 + 0] = net.distance(test_fanout[j*2 + 0]);
			test_distances[j*2 + 1] = net.distance(test_fanout[j*2 + 1]);
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

	size_t max_name_length = 0;
	size_t min_name_length = (size_t)-1;
	for (size_t i = 0; i < vars.globals.size(); i++)
	{
		if (vars.globals[i].name[0].size() > max_name_length)
			max_name_length = vars.globals[i].name[0].size();
		if (vars.globals[i].name[0].size() < min_name_length)
			min_name_length = vars.globals[i].name[0].size();
	}

	log("", "Fanout\t\tInitial\tFinal", __FILE__, __LINE__);
	for (size_t i = 0; i < fanout.size(); i++)
		for (size_t j = 0; j < fanout[i].size(); j++)
			if (fanout[i][j] > 0.0)
				log("", vars.globals[i].name[0] + "/" + vars.globals[j].name[0] + ":" + string((2*max_name_length + 2)/8 + 1 - (vars.globals[i].name[0].size() + 2 + vars.globals[j].name[0].size())/8, '\t') + to_string(initial_fanout[i][j], 5) + "\t" + to_string(fanout[i][j], 5), __FILE__, __LINE__);
	log("", "", __FILE__, __LINE__);

	log("", "Resulting Scale Factors:", __FILE__, __LINE__);
	for (size_t j = 0; j < sizes.size(); j++)
		log("", vars.globals[j].name[0] + ":" + string(max_name_length/8 + 1 - vars.globals[j].name[0].size()/8, '\t') + to_string(sizes[j]), __FILE__, __LINE__);
	log("", "", __FILE__, __LINE__);

	for (size_t j = 0; j < rules.size(); j++)
		rules[j].set_sizes(sizes);
}

void process::generate_gate_net()
{
	for (size_t j = 0; j < rules.size(); j++)
	{
		vector<vector<float> > temp = rules[j].generate_program();
		if (net.connections.size() < temp.size())
			net.connections.resize(temp.size(), vector<float>());

		for (size_t k = 0; k < temp.size(); k++)
		{
			if (net.connections[k].size() < temp[k].size())
				net.connections[k].resize(temp[k].size(), 0.0f);

			for (size_t l = 0; l < temp[k].size(); l++)
				net.connections[k][l] += temp[k][l];
		}
	}

	net.connections.resize(vars.globals.size(), vector<float>());
	for (size_t j = 0; j < net.connections.size(); j++)
		net.connections[j].resize(vars.globals.size(), 0.0f);

	log("", "Gate Net", __FILE__, __LINE__);
	for (size_t j = 0; j < net.connections.size(); j++)
	{
		string line = vars.globals[j].name[0] + " =";
		bool first = true;
		for (size_t k = 0; k < net.connections[j].size(); k++)
		{
			if (net.connections[j][k] != 0.0)
			{
				if (!first)
					line += " +";
				else
					first = false;

				line += " " + to_string(net.connections[j][k]) + "*" + vars.globals[k].name[0];
			}
		}
		log("", line, __FILE__, __LINE__);
	}
	log("", "", __FILE__, __LINE__);
}

void process::print_rules()
{
	for (size_t j = 0; j < rules.size(); j++)
		cout << rules[j] << endl;
	cout << endl;
}
