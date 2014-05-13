/*
 * variable_name.h
 *
 *  Created on: Aug 21, 2013
 *      Author: nbingham
 */

#include "instruction.h"
#include "instance.h"
#include "constant.h"

#ifndef variable_name_h
#define variable_name_h

struct variable_name : instruction
{
	variable_name();
	variable_name(tokenizer &tokens, variable_space &vars);
	~variable_name();

	instance name;
	constant *size;
	size_t uid;

	static bool is_next(tokenizer &tokens, size_t i = 1);
	void parse(tokenizer &tokens, variable_space &vars);
	void print(ostream &os = cout, string newl = "\n");

};

ostream &operator<<(ostream &os, variable_name v);

#endif
