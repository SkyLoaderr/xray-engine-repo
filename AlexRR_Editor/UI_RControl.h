//----------------------------------------------------
// file: UI_RControl.h
//----------------------------------------------------

#ifndef _INCDEF_UI_RControl_H_
#define _INCDEF_UI_RControl_H_

#include "UI_ViewControl.h"

//----------------------------------------------------

class UI_RControl : public UI_ViewControl {
protected:
	IMatrix m_SaveTransform;
	IVector m_RCenter;

public:
	UI_RControl();
	virtual ~UI_RControl();
	virtual void MouseStartMove();
	virtual void MouseEndMove();
	virtual void MouseMove();
};

//----------------------------------------------------

#endif /*_INCDEF_UI_RControl_H_*/
