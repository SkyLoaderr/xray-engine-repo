#pragma once

class CUIXmlInit;
class CUIMainIngameWnd;
class CUICarPanel 
{
		CUIStatic			UIStaticCarHealth;
		CUIProgressBar		UICarHealthBar;
public: 
		// Установить 
		void				SetCarHealth(float value);
		// Показать/спрятать 
		void				ShowCarHealth(bool on);
		void				Init(CUIMainIngameWnd* wnd, CUIXml& uiXml,CUIXmlInit& xml_init);
};