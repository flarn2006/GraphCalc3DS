#pragma once
#include <iostream>
#include <vector>
#include <string>

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
		OP_FUNCTION
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
	
	typedef float (*func_t)(float);
	
private:
	Opcode op;
	union {
		float value;
		struct {
			union {
				float *var;
				struct {
					func_t func;
					int domain;
				};
			};
			const char *name;
		};
	}; // Yes, I know that was complex. :)
	
	bool IsInDomain(float value) const;
	
public:
	RpnInstruction();
	RpnInstruction(Opcode opcode);
	RpnInstruction(float value);
	RpnInstruction(float *var, const char *name);
	RpnInstruction(func_t func, const char *name, int domain = D_ALL);
	
	Opcode GetOpcode() const;
	Status Execute(std::vector<float> &stack) const;
};

std::ostream &operator<<(std::ostream &os, const RpnInstruction &inst);

RpnInstruction::Status ExecuteRpn(const std::vector<RpnInstruction> instructions, float &resultOut);