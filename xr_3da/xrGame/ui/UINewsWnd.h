//=============================================================================
//  Filename:   UINewsWnd.h
//  News subwindow in PDA dialog
//=============================================================================

#ifndef UINEWSWND_H_
#define UINEWSWND_H_

// #pragma once

#include "UIDialogWnd.h"

class CUINewsWnd: public CUIDialogWnd
{
public:
	// Ctor and Dtor
	CUINewsWnd();
	virtual ~CUINewsWnd();

	virtual void Init();
};

#endif