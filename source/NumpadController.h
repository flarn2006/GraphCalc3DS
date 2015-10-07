#pragma once
#include <string>
#include "RpnInstruction.h"

class NumpadController
{
public:
	struct Reply
	{
		RpnInstruction inst;
		bool replaceLast;
	};
	
private:
	std::string entry;
	bool negative;

public:
	void Reset();
	bool EntryInProgress() const;
	Reply KeyPressed(char key, const RpnInstruction *lastInst);
	void GetEntryString(std::string &str) const;
};