#pragma once

class Control
{
public:
	enum TouchState {
		TS_INACTIVE,
		TS_ACTIVE,
		TS_TOUCHING
	};
	
private:
	TouchState touchState;
	
protected:
	Control();
	virtual void Click();
	virtual void TouchStart(int x, int y);
	virtual void TouchEnd();
	
public:
	virtual ~Control(); //for deleting without knowing control type

	virtual void Draw(int x, int y, int w, int h);
	virtual void TouchingInside(int x, int y);
	virtual void TouchingOutside(int x, int y);
	virtual void NotTouching();
	
	TouchState GetTouchState() const;
};