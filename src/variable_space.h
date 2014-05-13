/*
 * variable_space.h
 *
 * A variable space collects information about all the variables in a given program.
 * It includes a list variables, as well as types and labels for those variables.
 * The concept behind variable_space is to consolidate information about variables into one
 * easy to access place. variable_space is primarily used in program.
 */

#include "common.h"
#include "variable.h"
#include "tokenizer.h"

#ifndef variable_space_h
#define variable_space_h

struct variable_space
{
	variable_space();
	~variable_space();

	vector<variable> globals;

	size_t find(string name);

	bool contains(string name);

	void merge(size_t i, size_t j);
	variable_space &operator=(variable_space s);
	variable &at(size_t i);

	string variable_list();
};

#endif
