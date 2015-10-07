#include "NumpadController.h"
#include <cstdlib>

void NumpadController::Reset()
{
	entry.clear();
	negative = false;
}

bool NumpadController::EntryInProgress() const
{
	return (entry.size() > 0) || negative;
}

NumpadController::Reply NumpadController::KeyPressed(char key, const RpnInstruction *lastInst)
{
	Reply reply;
	bool resetBeforeReturn = false;
	
	if (lastInst == nullptr || lastInst->GetOpcode() != RpnInstruction::OP_PUSH) {
		Reset();
	}
	
	reply.replaceLast = EntryInProgress();
	
	if ('0' <= key && key <= '9') {
		if (entry == "0") entry.clear();
		entry += key;
	} else if (key == '-') {
		if (EntryInProgress()) {
			negative = !negative;
		} else {
			return { RpnInstruction::OP_NEGATE, false };
		}
	} else if (key == '.') {
		if (entry.find('.') == std::string::npos) {
			entry += '.';
		} else {
			resetBeforeReturn = true;
		}
	} else if (key == '\b') {
		if (entry.size() > 0) {
			entry.pop_back();
		} else {
			negative = false;
		}
	}
	
	float value = std::atof(entry.c_str());
	if (negative) value = -value;
	reply.inst = value;
	
	if (resetBeforeReturn) Reset();
	return reply;
}

void NumpadController::GetEntryString(std::string &str) const
{
	str = negative ? "-" : "";
	str += entry;
}