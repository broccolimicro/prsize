/*
 * variable_name.cpp
 *
 *  Created on: Aug 21, 2013
 *      Author: nbingham
 */

#include "variable_name.h"
#include "message.h"

variable_name::variable_name()
{
	this->_kind = "variable name";
	size = NULL;
}

variable_name::variable_name(tokenizer &tokens, variable_space &vars)
{
	this->_kind		= "variable name";
	size = NULL;

	parse(tokens, vars);
}

variable_name::~variable_name()
{
}

bool variable_name::is_next(tokenizer &tokens, size_t i)
{
	return instance::is_next(tokens, i);
}

void variable_name::parse(tokenizer &tokens, variable_space &vars)
{
	start_token = tokens.index+1;

	tokens.increment();
	tokens.push_bound("<");
	tokens.push_expected("[instance]");
	tokens.syntax(__FILE__, __LINE__);
	tokens.decrement();

	if (instance::is_next(tokens))
	{
		name.parse(tokens, vars);
		uid = vars.find(name.value);
		if (uid == vars.globals.size())
			vars.globals.push_back(variable(name.value));
	}

	if (tokens.peek_next() == "<")
	{
		tokens.increment();
		tokens.push_expected("<");
		tokens.push_bound("[constant]");
		tokens.syntax(__FILE__, __LINE__);
		tokens.decrement();

		tokens.increment();
		tokens.push_expected("[constant]");
		tokens.push_bound(">");
		tokens.syntax(__FILE__, __LINE__);
		if (constant::is_next(tokens))
			size = new constant(tokens, vars);
		tokens.decrement();

		tokens.increment();
		tokens.push_expected(">");
		tokens.syntax(__FILE__, __LINE__);
		tokens.decrement();
	}

	end_token = tokens.index;
}

void variable_name::print(ostream &os, string newl)
{
	name.print(os, newl);
	if (size != NULL)
	{
		os << "<";
		size->print(os, newl);
		os << ">";
	}
}

ostream &operator<<(ostream &os, variable_name v)
{
	os << v.name;
	if (v.size != NULL)
		os << "<" << *v.size << ">";
	return os;
}
