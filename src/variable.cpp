/*
 * variable.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: Ned Bingham and Nicholas Kramer
 *
 *  DO NOT DISTRIBUTE
 */

#include "variable.h"
#include "variable_space.h"

variable::variable()
{
}

variable::variable(string name)
{
	this->name.push_back(name);
}

variable::~variable()
{
}

void variable::add(string n)
{
	name.push_back(n);
	sort(name.begin(), name.end());
	name.resize(unique(name.begin(), name.end()) - name.begin());
}

bool variable::contains(string s)
{
	for (size_t i = 0; i < name.size(); i++)
		if (name[i].find(s) != string::npos)
			return true;

	return false;
}

variable &variable::operator=(variable v)
{
	name = v.name;
	return *this;
}

