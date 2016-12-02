#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "TinyExpr.h"

class RpnInstruction
{
	friend std::ostream &operator<<(std::ostream &os, const RpnInstruction &inst);
	
public:
	enum Opcode {
		OP_NULL,
		OP_PUSH,
		OP_PUSHVAR,
		OP_ADD,
		OP_SUBTRACT,
		OP_MULTIPLY,
		OP_DIVIDE,
		OP_MODULO,
		OP_POWER,
		OP_NEGATE,
		OP_FUNCTION,
		OP_DUP,
		OP_EXPR
	};
	
	enum Status {
		S_OK,
		S_OVERFLOW,
		S_UNDERFLOW,
		S_UNDEFINED
	};
	
	enum {
		D_ZERO = 1,
		D_POSITIVE = 2,
		D_NEGATIVE = 4,
		D_ALL = D_ZERO | D_POSITIVE | D_NEGATIVE
	};
	
	typedef double (*func_t)(double);
	
private:
	Opcode op;
	// Not all of these will be used at the same time.
	// Which one(s) depends on the opcode.
	double value;
	const double *var;
	func_t func;
	int domain;
	TinyExpr expr;
	std::string name;

	bool IsInDomain(double value) const;
	
public:
	RpnInstruction();
	RpnInstruction(Opcode opcode);
	RpnInstruction(double value);
	RpnInstruction(const double *var, const char *name);
	RpnInstruction(func_t func, const char *name, int domain = D_ALL);
	RpnInstruction(const char *expression);
	
	Opcode GetOpcode() const;
	const std::string &GetName() const;
	Status Execute(std::vector<double> &stack) const;
};

std::ostream &operator<<(std::ostream &os, const RpnInstruction &inst);

RpnInstruction::Status ExecuteRpn(const std::vector<RpnInstruction> instructions, double &resultOut);
