/*
 * rule.h
 *
 *  Created on: May 12, 2014
 *      Author: nbingham
 */

#include "expression.h"
#include "variable_name.h"
#include "instruction.h"

#ifndef rule_h
#define rule_h

struct rule : instruction
{
	rule();
	rule(tokenizer &tokens, variable_space &vars);
	~rule();

	expression left;
	variable_name right;
	bool direction;

	static bool is_next(tokenizer &tokens, size_t i = 1);
	void parse(tokenizer &tokens, variable_space &vars);
	void print(ostream &os, string newl);

	vector<vector<int> > generate_program();
	void set_sizes(vector<float> sizes);
	void clear_sizes();
	void calculate_transistor_sizes();
};

ostream &operator<<(ostream &os, rule r);

#endif
