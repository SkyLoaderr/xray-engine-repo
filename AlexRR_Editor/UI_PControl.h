//----------------------------------------------------
// file: UI_PControl.h
//----------------------------------------------------

#ifndef _INCDEF_UI_PControl_H_
#define _INCDEF_UI_PControl_H_

#include "UI_ViewControl.h"

//----------------------------------------------------

class UI_PControl : public UI_ViewControl {
protected:
	IMatrix m_SaveTransform;
	IVector m_Side,m_Up;
public:
	UI_PControl();
	virtual ~UI_PControl();
	virtual void MouseStartMove();
	virtual void MouseEndMove();
	virtual void MouseMove();
};

//----------------------------------------------------

#endif /*_INCDEF_UI_PControl_H_*/
