////////////////////////////////////////////////
// UITalkDialogWnd.h
// ���� ������� �� ��������� � ������ ���������
////////////////////////////////////////////////

#pragma once


#include "UIStatic.h"
#include "UIButton.h"
#include "UIFrameWindow.h"

#include "../InfoPortion.h"

#include "UICharacterInfo.h"
#include "UIItemInfo.h"

#include "UIWndCallback.h"

class CUIScrollView;
class CUIXml;

class CUITalkDialogWnd: public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow inherited;
	CUIXml*			m_uiXml;
public:
	CUITalkDialogWnd();
	virtual ~CUITalkDialogWnd();
	

	virtual void Init(float x, float y, float width, float height);
	
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Show();
	virtual void Hide();

	u32			GetHeaderColor()		{ return m_iNameTextColor; }
	CGameFont *	GetHeaderFont()			{ return m_pNameTextFont; }
	u32			GetOurReplicsColor()	{ return m_uOurReplicsColor; }

	//����� ���������� �������
	int m_iClickedQuestion;

	//������ ��������, ������� �� ����� �������� ���������

	//�������� ���������� �������
	CUIFrameWindow		UIDialogFrame;
	CUIFrameWindow		UIOurPhrasesFrame;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

	CUIButton			UIToTradeButton;

	//���������� � ���������� 
	CUIStatic			UIOurIcon;
	CUIStatic			UIOthersIcon;
	CUICharacterInfo	UICharacterInfoLeft;
	CUICharacterInfo	UICharacterInfoRight;

	void				AddQuestion(const char* str, int value);
	void				AddAnswer(const char* SpeakerName, const char* str, bool bActor);
	void				AddIconedAnswer(LPCSTR text, LPCSTR texture_name, Frect texture_rect);
	void				ClearAll();
	void				ClearQuestions();
private:
	CUIScrollView*			UIQuestionsList;
	CUIScrollView*			UIAnswersList;

	// ����� � ���� ������ � ������ ���������
	CGameFont			*m_pNameTextFont;
	u32					m_iNameTextColor;
	// ���� �e���� � ����� ����� ������
	u32					m_uOurReplicsColor;

	void				OnTradeClicked			();
	void				OnQuestionClicked		(CUIWindow* w, void*);
	
};


class CUIQuestionItem :public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	float			m_min_height;
public:
	CUI3tButton*	m_text;
	int				m_value;
					CUIQuestionItem			(CUIXml* xml_doc, LPCSTR path);
	void			Init					(int val, LPCSTR text);

	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			void	OnTextClicked();
};

class CUIAnswerItem :public CUIWindow
{
	typedef CUIWindow inherited;

	float			m_min_height;
	float			m_bottom_footer;
	CUIStatic*		m_text;
	CUIStatic*		m_name;
public:
					CUIAnswerItem			(CUIXml* xml_doc, LPCSTR path);
	void			Init					(LPCSTR text, LPCSTR name);
};

class CUIAnswerItemIconed :public CUIAnswerItem
{
	typedef CUIAnswerItem inherited;
	CUIStatic*		m_icon;

public:
					CUIAnswerItemIconed		(CUIXml* xml_doc, LPCSTR path);
	void			Init					(LPCSTR text, LPCSTR texture_name, Frect texture_rect);

};
