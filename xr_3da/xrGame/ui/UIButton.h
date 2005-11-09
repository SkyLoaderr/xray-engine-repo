#pragma once

#include "UIStatic.h"
#include "../script_export_space.h"


class CUIButton : public CUIStatic
{
private:
	typedef			CUIStatic				inherited;
public:
					CUIButton				();
	virtual			~CUIButton				();

	virtual void	Init					(LPCSTR tex_name, float x, float y, float width, float height);
	virtual void	Init					(float x, float y, float width, float height);

	virtual bool	OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual void	OnClick					();

	//���������� ����
	virtual void	DrawTexture				();
	virtual void	DrawText				();
	virtual void	DrawHighlightedText		();

	virtual void	Update					();
	virtual void	Enable					(bool status);

	virtual void	OnFocusLost				();

	//������ � ������� ����� �������� ������
	typedef enum{NORMAL_PRESS, //������ ���������� ��� 
							   //������� � ���������� �� ��� ����
				 DOWN_PRESS    //����� ��� �������
			} E_PRESS_MODE;

	void			SetPressMode			(E_PRESS_MODE ePressMode)	{m_ePressMode = ePressMode;}
	E_PRESS_MODE	GetPressMode			()							{return m_ePressMode;}

	//������ ����������� ���������
    virtual void	Reset					();

	void			SetPushOffsetX			(float offset_x)			{m_iPushOffsetX = offset_x;}
	void			SetPushOffsetY			(float offset_y)			{m_iPushOffsetY = offset_y;}
	float			GetPushOffsetX			()							{return m_iPushOffsetX;}
	float			GetPushOffsetY			()							{return m_iPushOffsetY;}

	//��������� �� ����� �� ������
	// �������������� ���������
	virtual void	HighlightItem			(bool bHighlight)			{m_bCursorOverWindow = bHighlight; }
	// ���� ���������

	//��������� � ������� ��������� ������
	typedef enum{BUTTON_NORMAL, //������ ����� �� �������������
		BUTTON_PUSHED, //� ������� ��������
		BUTTON_UP      //��� ������������ ������ ���� 
	} E_BUTTON_STATE;

	// ��������� ��������� ������: ��������, �� ��������
	void			SetButtonMode			(E_BUTTON_STATE eBtnState)	{ m_eButtonState = eBtnState; }

	// ��������� ������ ��� ������������� ����������� ���� ������ � ������ NORMAL_PRESS
	void			SetButtonAsSwitch		(bool bAsSwitch)			{ m_bIsSwitch = bAsSwitch; }

	// ������ � �������������
	// ��� ������������ ������� �� ����� dinput.h, �� DirectX SDK.
	// ��������: ������ A - ��� 0x1E(DIK_A)
	void			SetAccelerator			(u32 uAccel)				{ m_uAccelerator = uAccel; }
	const u32		GetAccelerator			() const					{ return m_uAccelerator; }
	
	// �������� ��������� ������ ������������ ������ ������. ����� ��� ��� ����� ������� ����
	void			SetShadowOffset			(float offsetX, float offsetY) { m_iShadowOffsetX = offsetX; m_iShadowOffsetY = offsetY; }
	shared_str			m_hint_text;
protected:
	
	E_BUTTON_STATE		m_eButtonState;

	bool				m_bIsSwitch;

	//���� ������ ���� ������ ��� ������
	bool				m_bButtonClicked;


	//����� � ������� ���������� ������
	E_PRESS_MODE		m_ePressMode;

	//�������� ������ ��� �������
	float				m_iPushOffsetX;
	float				m_iPushOffsetY;

	// ��� ������������
	u32					m_uAccelerator;

	// �������� ��������� ������
	float				m_iShadowOffsetX;
	float				m_iShadowOffsetY;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};


add_to_type_list(CUIButton)
#undef script_type_list
#define script_type_list save_type_list(CUIButton)
