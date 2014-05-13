/*
 * variable_space.cpp
 *
 *  Created on: Feb 9, 2013
 *      Author: nbingham
 */

#include "variable_space.h"

variable_space::variable_space()
{
}

variable_space::~variable_space()
{
}

size_t variable_space::find(string name)
{
	for (size_t i = 0; i < globals.size(); i++)
		if (::find(globals[i].name.begin(), globals[i].name.end(), name) != globals[i].name.end() || ::find(globals[i].name.begin(), globals[i].name.end(), "this." + name) != globals[i].name.end())
			return i;

	return globals.size();
}

bool variable_space::contains(string name)
{
	return (find(name) != globals.size());
}

void variable_space::merge(size_t i, size_t j)
{
	globals[i].name.insert(globals[i].name.end(), globals[j].name.begin(), globals[j].name.end());
	sort(globals[i].name.begin(), globals[i].name.end());
	globals[i].name.resize(unique(globals[i].name.begin(), globals[i].name.end()) - globals[i].name.begin());
	globals.erase(globals.begin() + j);
}

variable_space &variable_space::operator=(variable_space s)
{
	this->globals = s.globals;
	return *this;
}

variable &variable_space::at(size_t i)
{
	if (i < globals.size())
		return globals[i];
	else
	{
		size_t idx = find("null");
		if (idx == globals.size())
			globals.push_back(variable("null"));
		return globals[idx];
	}
}

string variable_space::variable_list()
{
	string result;
	for (size_t i = 0; i < globals.size(); i++)
	{
		if (i != 0)
			result += ",";
		result += globals[i].name[0];
	}
	return result;
}
