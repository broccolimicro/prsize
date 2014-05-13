/*
 * instruction.h
 *
 *  Created on: Oct 28, 2012
 *      Author: Ned Bingham
 */

#include "common.h"
#include "tokenizer.h"
#include "variable_space.h"

#ifndef instruction_h
#define instruction_h

/** This structure is the abstract base class for all CHP syntaxes.
 *  Its primarily used for two things: polymorphism (having a list
 *  of generic instruction is nice), and distributing function calls
 *  to the correct child by checking the value of the _kind field.
 */
struct instruction
{
protected:
	/** Used for type checking during runtime. All classes that inherit
	 *  from instruction must define their _kind field as their name in
	 *  all of their constructors.
	 */
	string _kind;

public:
	instruction();
	virtual ~instruction();

	/** The range of tokens from which this instruction was parsed.
	 */
	int start_token;
	int end_token;

	string kind();

	void parse(tokenizer &tokens, variable_space &vars);
	void print(ostream &os = cout, string newl = "\n");
};

ostream &operator<<(ostream &os, instruction* i);

#endif
