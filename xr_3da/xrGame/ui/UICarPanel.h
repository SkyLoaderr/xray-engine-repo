#pragma once

class CUIXmlInit;
class CUIMainIngameWnd;
class CUICarPanel 
{
		CUIStatic			UIStaticCarHealth;
		CUIProgressBar		UICarHealthBar;
public: 
		// ���������� 
		void				SetCarHealth(float value);
		// ��������/�������� 
		void				ShowCarHealth(bool on);
		void				Init(CUIMainIngameWnd* wnd, CUIXml& uiXml,CUIXmlInit& xml_init);
};