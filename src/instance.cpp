/*
 * instance.cpp
 *
 *  Created on: Aug 21, 2013
 *      Author: nbingham
 */

#include "instance.h"
#include "message.h"

instance::instance()
{
	this->_kind = "instance";
}

instance::instance(tokenizer &tokens, variable_space &vars)
{
	this->_kind		= "instance";

	parse(tokens, vars);
}

instance::instance(string value)
{
	this->_kind = "instance";
	this->value = value;
}

instance::~instance()
{
}

bool instance::is_next(tokenizer &tokens, size_t i)
{
	string token = tokens.peek(i);

	if (token.size() > 0 && token[0] == '\"' && token[token.size()-1] == '\"')
		return true;

	if (token.size() == 0 || token[0] < 'A' || (token[0] > 'Z' && token[0] != '_' && token[0] < 'a') || token[0] > 'z')
		return false;

	for (size_t j = 1; j < token.size(); j++)
		if (!nc(token[j]))
			return false;

	return true;
}

void instance::parse(tokenizer &tokens, variable_space &vars)
{
	start_token = tokens.index+1;

	string token = tokens.next();
	if (token.size() > 0 && token[0] == '\"' && token[token.size()-1] == '\"')
		token = token.substr(1, token.size()-2);

	bool has_error = false;
	if (token.size() == 0 || token[0] < 'A' || (token[0] > 'Z' && token[0] != '_' && token[0] < 'a') || token[0] > 'z')
		has_error = true;

	for (size_t j = 1; j < token.size(); j++)
		if (!nc(token[j]))
			has_error = true;

	if (!has_error)
		value = token;
	else
		error(tokens, "malformed name", "names must start with either a letter or an underscore and may only contain letters, numbers, or underscores", __FILE__, __LINE__);

	end_token = tokens.index;
}

void instance::print(ostream &os, string newl)
{
	os << value;
}

ostream &operator<<(ostream &os, const instance &i)
{
	os << i.value;

	return os;
}
