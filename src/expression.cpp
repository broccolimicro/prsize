/*
 * expression.cpp
 *
 *  Created on: May 12, 2014
 *      Author: nbingham
 */

#include "expression.h"
#include "variable_name.h"

expression::expression()
{
	_kind = "expression";
	level = 0;
}

expression::expression(tokenizer &tokens, variable_space &vars)
{
	_kind = "expression";
	level = 0;
	parse(tokens, vars);
}

expression::expression(tokenizer &tokens, variable_space &vars, int i)
{
	_kind = "expression";
	level = i;
	parse(tokens, vars);
}

expression::~expression()
{

}

bool expression::is_next(tokenizer &tokens, size_t i)
{
	return (variable_name::is_next(tokens) || tokens.peek(i) == "(" || tokens.peek(i) == "~");
}

void expression::parse(tokenizer &tokens, variable_space &vars)
{
	string ops[2] = {"|", "&"};

	if (level <= 1)
	{
		bool done = false;
		while (!done)
		{
			tokens.increment();
			tokens.push_expected("[expression]");
			tokens.push_bound(ops[level]);
			tokens.syntax(__FILE__, __LINE__);
			if (expression::is_next(tokens))
				terms.push_back(new expression(tokens, vars, level+1));
			tokens.decrement();

			if (tokens.peek_next() != ops[level])
				done = true;
			else
				operators.push_back(tokens.next());
		}
	}
	else if (level == 2)
	{
		if (tokens.peek_next() == "~")
		{
			operators.push_back(tokens.next());
			terms.push_back(new expression(tokens, vars, level));
		}
		else
			terms.push_back(new expression(tokens, vars, level+1));
	}
	else if (level == 3)
	{
		tokens.increment();
		tokens.push_expected("(");
		tokens.push_expected("[variable name]");
		string token = tokens.syntax(__FILE__, __LINE__);
		tokens.decrement();

		if (token == "(")
		{
			tokens.increment();
			tokens.push_bound(")");
			tokens.push_expected("[expression]");
			tokens.syntax(__FILE__, __LINE__);
			if (expression::is_next(tokens))
				terms.push_back(new expression(tokens, vars, 0));
			tokens.decrement();

			tokens.increment();
			tokens.push_expected(")");
			tokens.syntax(__FILE__, __LINE__);
			tokens.decrement();
		}
		else if (variable_name::is_next(tokens))
			terms.push_back(new variable_name(tokens, vars));
	}
}

void expression::print(ostream &os, string newl)
{
	for (size_t i = 0; i < terms.size(); i++)
	{
		if (i != 0 && level <= 1)
			os << " " << operators[i-1] << " ";
		else if (i == 0 && level == 2 && operators.size() > 0)
			os << operators[i];

		if (level == 3 && terms[i]->kind() == "expression")
		{
			os << "(";
			terms[i]->print(os, newl);
			os << ")";
		}
		else
			terms[i]->print(os, newl);
	}
}

vector<vector<float> > expression::generate_program(size_t uid)
{
	vector<vector<float> > result;
	for (size_t i = 0; i < terms.size(); i++)
	{
		if (terms[i]->kind() == "expression")
		{
			vector<vector<float> > temp = ((expression*)terms[i])->generate_program(uid);
			if (result.size() < temp.size())
				result.resize(temp.size(), vector<float>());
			for (size_t j = 0; j < temp.size(); j++)
			{
				if (result[j].size() < temp[j].size())
					result[j].resize(temp[j].size(), 0);

				for (size_t k = 0; k < temp[j].size(); k++)
					result[j][k] += temp[j][k];
			}
		}
		else if (terms[i]->kind() == "variable name")
		{
			variable_name *temp = (variable_name*)terms[i];
			if (result.size() < temp->uid+1)
				result.resize(temp->uid+1, vector<float>());

			if (result[temp->uid].size() < uid+1)
				result[temp->uid].resize(uid+1, 0.0);

			if (temp->size != NULL)
				result[temp->uid][uid] += (float)temp->size->value;
		}
	}

	return result;
}

void expression::set_sizes(float size)
{
	for (size_t i = 0; i < terms.size(); i++)
	{
		if (terms[i]->kind() == "expression")
			((expression*)terms[i])->set_sizes(size);
		else if (terms[i]->kind() == "variable name")
		{
			variable_name *temp = (variable_name*)terms[i];
			if (temp->size == NULL)
			{
				temp->size = new constant();
				temp->size->value = 1;
			}

			temp->size->value *= (int)size;
		}
	}
}

void expression::clear_sizes()
{
	for (size_t i = 0; i < terms.size(); i++)
	{
		if (terms[i]->kind() == "expression")
			((expression*)terms[i])->clear_sizes();
		else if (terms[i]->kind() == "variable name")
		{
			variable_name *temp = (variable_name*)terms[i];
			if (temp->size == NULL)
				temp->size = new constant();

			temp->size->value = 1;
		}
	}
}

float expression::size()
{
	float result = 9E10;
	for (size_t i = 0; i < terms.size(); i++)
	{
		float s = 0;
		if (terms[i]->kind() == "expression")
			s = ((expression*)terms[i])->size();
		else if (terms[i]->kind() == "variable name")
		{
			variable_name *temp = (variable_name*)terms[i];
			if (temp->size == NULL)
			{
				temp->size = new constant();
				temp->size->value = 1;
			}

			s = (float)temp->size->value;
		}

		if (level <= 1 && s < result)
			result = s;
		else if (level == 2 && operators.size() > 0)
			result = s/3.0;
		else if (level == 2 || level == 3)
			result = s;
	}

	if (level == 1)
		result /= (float)terms.size();

	return result;
}

void expression::calculate_transistor_sizes()
{
	for (size_t i = 0; i < terms.size(); i++)
	{
		if (terms[i]->kind() == "expression")
		{
			((expression*)terms[i])->calculate_transistor_sizes();

			float s = ((expression*)terms[i])->size();
			if (s < 1.0)
				((expression*)terms[i])->set_sizes(1.0/s);
		}
	}
}

ostream &operator<<(ostream &os, expression expr)
{
	for (size_t i = 0; i < expr.terms.size(); i++)
	{
		if (i != 0 && expr.level <= 1)
			os << " " << expr.operators[i-1] << " ";
		else if (i == 0 && expr.level == 2 && expr.operators.size() > 0)
			os << expr.operators[i];

		if (expr.level == 3 && expr.terms[i]->kind() == "expression")
			os << "(" << expr.terms[i] << ")";
		else
			os << expr.terms[i];
	}
	return os;
}
