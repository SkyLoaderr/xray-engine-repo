//=============================================================================
//  Filename:   UIMultiTextStatic.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Статик контрол на котором можно выводить множество надписей различными
//	шрифтами, цветами и даже с анимацией
//=============================================================================

#ifndef UI_MULTITEXT_STATIC_H_
#define UI_MULTITEXT_STATIC_H_

#pragma once

#include "UITextBanner.h"

class CUIMultiTextStatic: public CUIStatic
{
	typedef CUIStatic inherited;
public:
	typedef struct SPh
	{
		float			outX;
		float			outY;
		CUITextBanner	effect;
		ref_str			str;

		void			SetText	(const char *fmt, ...);

		// Ctor
		SPh						();
	} SinglePhrase;

	typedef xr_vector<SinglePhrase>	Phrases;
	typedef Phrases::iterator		Phrases_it;
protected:
	Phrases							m_vPhrases;
public:
	virtual void Draw();
	virtual void Update();
	// Добавить надпись
	// Return:	Указатель на добавленную запись
	SinglePhrase * AddPhrase();
};


#endif	//UI_MULTITEXT_STATIC_H_