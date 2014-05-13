/*
 * rule.cpp
 *
 *  Created on: May 12, 2014
 *      Author: nbingham
 */

#include "rule.h"

rule::rule()
{
	_kind = "rule";
	direction = false;
}

rule::rule(tokenizer &tokens, variable_space &vars)
{
	_kind = "rule";
	direction = false;
	parse(tokens, vars);
}

rule::~rule()
{

}

bool rule::is_next(tokenizer &tokens, size_t i)
{
	return expression::is_next(tokens, i);
}

void rule::parse(tokenizer &tokens, variable_space &vars)
{
	tokens.increment();
	tokens.push_expected("[expression]");
	tokens.push_bound("->");
	tokens.syntax(__FILE__, __LINE__);
	if (expression::is_next(tokens))
		left.parse(tokens, vars);
	tokens.decrement();

	tokens.increment();
	tokens.push_expected("->");
	tokens.push_bound("[variable name]");
	tokens.syntax(__FILE__, __LINE__);
	tokens.decrement();

	tokens.increment();
	tokens.push_expected("[variable name]");
	tokens.push_bound("+");
	tokens.push_bound("-");
	tokens.syntax(__FILE__, __LINE__);
	if (variable_name::is_next(tokens))
		right.parse(tokens, vars);
	tokens.decrement();

	tokens.increment();
	tokens.push_expected("+");
	tokens.push_expected("-");
	string token = tokens.syntax(__FILE__, __LINE__);
	if (token == "+")
		direction = true;
	else if (token == "-")
		direction = false;
	tokens.decrement();
}

void rule::print(ostream &os, string newl)
{
	left.print(os, newl);
	os << " -> ";
	right.print(os, newl);
	if (direction)
		os << "+";
	else
		os << "-";
}

vector<vector<int> > rule::generate_program()
{
	return left.generate_program(right.uid);
}

void rule::set_sizes(vector<float> sizes)
{
	left.set_sizes(sizes[right.uid]);
}

void rule::clear_sizes()
{
	left.clear_sizes();
}

void rule::calculate_transistor_sizes()
{
	left.calculate_transistor_sizes();
}

ostream &operator<<(ostream &os, rule r)
{
	os << r.left << " -> " << r.right << (r.direction ? "+" : "-");
	return os;
}
