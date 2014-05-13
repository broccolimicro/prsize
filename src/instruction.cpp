/*
 * instruction.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: Ned Bingham and Nicholas Kramer
 */

#include "instruction.h"
#include "rule.h"
#include "constant.h"
#include "expression.h"
#include "instance.h"
#include "variable_name.h"
#include "message.h"

instruction::instruction()
{
	_kind = "instruction";
	start_token = 0;
	end_token = 0;
}

instruction::~instruction()
{
	_kind = "instruction";
}

/** For type checking during runtime.
 */
string instruction::kind()
{
	return _kind;
}

/** This calls the parse function of the class given in _kind.
 *  It does NOT call the parse function dependent upon what is
 *  next in the token stream.
 *
 *  Classes inheriting from instruction should define a parse
 *  function with this interface that consumes some tokens from
 *  the token stream in order to fill data within the class. The
 *  function must start with:	start_token = tokens.index+1;
 *  and end with:				end_token = tokens.index;
 *  So that higher order errors can be flagged.
 */
void instruction::parse(tokenizer &tokens, variable_space &vars)
{
	if (this == NULL)
		internal(tokens, "attempting to call the parse function of a null pointer", __FILE__, __LINE__);
	else if (this->kind() == "expression")
		((expression*)this)->parse(tokens, vars);
	else if (this->kind() == "rule")
		((rule*)this)->parse(tokens, vars);
	else if (this->kind() == "variable name")
		((variable_name*)this)->parse(tokens, vars);
	else if (this->kind() == "instance")
		((instance*)this)->parse(tokens, vars);
	else if (this->kind() == "constant")
		((constant*)this)->parse(tokens, vars);
	else
		internal(tokens, "unrecognized instruction of kind '" + this->kind() + "'", __FILE__, __LINE__);
}

/** This calls the print function of the class given in _kind.
 *
 *  Classes inheriting from instruction should define a print
 *  function with this interface that prints the HSE in a normal
 *  human readable format. This is defined so that people may
 *  check to make sure the parser didn't screw something up.
 */
void instruction::print(ostream &os, string newl)
{
	if (this == NULL)
		os << "null";
	else if (this->kind() == "expression")
		((expression*)this)->print(os, newl);
	else if (this->kind() == "rule")
		((rule*)this)->print(os, newl);
	else if (this->kind() == "variable name")
		((variable_name*)this)->print(os, newl);
	else if (this->kind() == "instance")
		((instance*)this)->print(os, newl);
	else if (this->kind() == "constant")
		((constant*)this)->print(os, newl);
}

/** In all cases, this is the same as print, however
 *  the spacing is not as good. This is just used if
 *  you want to do some quick debugging.
 */
ostream &operator<<(ostream &os, instruction* i)
{
	if (i == NULL)
		os << "null";
	else if (i->kind() == "expression")
		os << *((expression*)i);
	else if (i->kind() == "rule")
		os << *((rule*)i);
	else if (i->kind() == "variable name")
		os << *((variable_name*)i);
	else if (i->kind() == "instance")
		os << *((instance*)i);
	else if (i->kind() == "constant")
		os << *((constant*)i);

	return os;
}
