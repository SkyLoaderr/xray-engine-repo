#pragma once

#include "uiwindow.h"
#include "uipointergage.h"


class CUICarPanel : public CUIWindow
{
private:
	typedef CUIWindow inherited;

	CUIStatic			UIStaticCarHealth;
	CUIProgressBar		UICarHealthBar;
	CUIPointerGage		UIPointerGage;
public: 

	// Установить 
	void				SetCarHealth	(float value);
	void				SetSpeed		(float speed);
	void				SetRPM			(float rmp);
	void				Init			(int x, int y, int width, int height);
};