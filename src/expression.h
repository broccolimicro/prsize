/*
 * expression.h
 *
 *  Created on: May 12, 2014
 *      Author: nbingham
 */

#include "common.h"
#include "instruction.h"
#include "tokenizer.h"

#ifndef expression_h
#define expression_h

struct expression : instruction
{
	expression();
	expression(tokenizer &tokens, variable_space &vars);
	expression(tokenizer &tokens, variable_space &vars, int i);
	~expression();

	vector<instruction*> terms;
	vector<string> operators;
	int level;

	static bool is_next(tokenizer &tokens, size_t i = 1);
	void parse(tokenizer &tokens, variable_space &vars);
	void print(ostream &os = cout, string newl = "\n");

	vector<vector<float> > generate_program(size_t uid);
	void set_sizes(float size);
	void clear_sizes();
	float size();
	void calculate_transistor_sizes();
};

ostream &operator<<(ostream &os, expression expr);

#endif
