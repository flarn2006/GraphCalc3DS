#pragma once
#include <iostream>
#include <vector>
#include <string>

class RpnInstruction
{
	friend std::ostream &operator<<(std::ostream &os, const RpnInstruction &inst);
	
public:
	enum Opcode {
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
		S_UNDERFLOW,
		S_UNDEFINED
	};
	
	typedef float (*func_t)(float);
	
private:
	Opcode op;
	union {
		float value;
		float *var;
		struct {
			func_t func;
			const char *name;
		};
	};
	
public:
	RpnInstruction(Opcode opcode);
	RpnInstruction(float value);
	RpnInstruction(float *var);
	RpnInstruction(func_t func, const char *name);
	
	Status Execute(std::vector<float> &stack) const;
};

std::ostream &operator<<(std::ostream &os, const RpnInstruction &inst);

RpnInstruction::Status ExecuteRpn(const std::vector<RpnInstruction> instructions, float &resultOut);