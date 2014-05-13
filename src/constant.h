/*
 * constant.h
 *
 *  Created on: Aug 21, 2013
 *      Author: nbingham
 */

#include "instruction.h"

#ifndef constant_h
#define constant_h

struct constant : instruction
{
	constant();
	constant(tokenizer &tokens, variable_space &vars);
	~constant();

	int value;
	int width;

	static bool is_next(tokenizer &tokens, size_t i = 1);
	void parse(tokenizer &tokens, variable_space &vars);
	void print(ostream &os = cout, string newl = "\n");
};

ostream &operator<<(ostream &os, const constant &c);

#endif
