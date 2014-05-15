/*
 * process.h
 *
 *  Created on: May 15, 2014
 *      Author: nbingham
 */

#include "common.h"
#include "rule.h"
#include "variable_space.h"
#include "tokenizer.h"

#ifndef process_h
#define process_h

struct gate_net
{
	gate_net();
	~gate_net();

	/* connections[i][j] represents the sum of the gate widths
	 * of the transistors that connect the output of node i to
	 * the input of node j. So the sum of connections[i][j] for
	 * all j in a particular i represents the total load capacitance
	 * for node i.
	 */
	vector<vector<float> > connections;

	vector<vector<float> > fanout(vector<float> sizes);
	float distance(vector<vector<float> > fanout, float desired_fanout = 1.0f);
};

struct process
{
	process();
	process(tokenizer &tokens);
	~process();

	vector<rule> rules;
	variable_space vars;
	gate_net net;

	void parse(tokenizer &tokens);
	void generate_transistor_level_sizing();
	void generate_gate_level_sizing();

	void generate_gate_net();

	void print_rules();
};

#endif
