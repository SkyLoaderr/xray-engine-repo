//----------------------------------------------------
// file: UI_SControl.h
//----------------------------------------------------

#ifndef _INCDEF_UI_SControl_H_
#define _INCDEF_UI_SControl_H_

#include "UI_ViewControl.h"

//----------------------------------------------------

class UI_SControl : public UI_ViewControl {
protected:
	IMatrix m_SaveTransform;
	IVector m_SCenter;
public:
	UI_SControl();
	virtual ~UI_SControl();
	virtual void MouseStartMove();
	virtual void MouseEndMove();
	virtual void MouseMove();
};

//----------------------------------------------------

#endif /*_INCDEF_UI_SControl_H_*/
