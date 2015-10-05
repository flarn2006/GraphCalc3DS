#pragma once
#include <functional>
#include "Control.h"

class Button : public Control
{
public:
	typedef std::function<void(Button&)> callback_t;
	
private:
	callback_t callback;
	
protected:
	virtual void Click();

public:
	virtual void Draw(int x, int y, int w, int h);
	void SetAction(callback_t callback);
};