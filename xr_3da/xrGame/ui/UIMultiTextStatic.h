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
		float						outX;
		float						outY;
		int							maxWidth;
		CUIStatic::EElipsisPosition elipsisPos;
		CUITextBanner				effect;
		shared_str						str;

		void						SetText	(const char *fmt, ...);

		// Ctor		
		SPh							();
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
	// Получить запись по номеру
	SinglePhrase * GetPhraseByIndex(u32 idx);


};

class CUICaption :protected CUIMultiTextStatic
{
	typedef CUIMultiTextStatic inherited;
	xr_map<shared_str,u32>		m_indices;
public:
	virtual void		Draw();
	void				addCustomMessage(const shared_str& msg_name, float x, float y, float font_size, CGameFont *pFont, CGameFont::EAligment al, u32 color, LPCSTR def_str="");
	EffectParams*		customizeMessage(const shared_str& msg_name, const CUITextBanner::TextBannerStyles styleName);
	void				setCaption(const shared_str& msg_name, LPCSTR message_to_out, u32 color=0, bool replaceColor=false);

};
#endif	//UI_MULTITEXT_STATIC_H_