/*
 * variable.h
 *
 * This structure describes a variable in the chp program. We need
 * to keep track of a variables name, its type (record, keyword, process),
 * and its bit width.
 *
 */

#include "common.h"

#ifndef variable_h
#define variable_h

struct variable_space;

struct variable
{
	variable();
	variable(string name);
	~variable();

	vector<string> name;		// the name of the instantiated variable

	void add(string n);
	bool contains(string s);
	variable &operator=(variable v);
};

#endif
