//=============================================================================
//  �������� ��������� ������ � ���������� ������ ��������
//=============================================================================

#ifndef UI_TEXT_BANNER_H_
#define UI_TEXT_BANNER_H_

// #pragma once;

#include "UIStatic.h"

//-----------------------------------------------------------------------------/
//  ����� ���������� �������
//-----------------------------------------------------------------------------/

struct EffectParams
{
	friend class	CUITextBanner;
	float			fPeriod;
	int				iAdditionalParam;
	float			fAdditionalParam;

	// Constructor
	EffectParams()
		:	fPeriod				(0.0f),
			fTimePassed			(0.0f),
			iEffectStage		(0),
			fAdditionalParam	(0.0f),
			iAdditionalParam	(0)
	{}
private:
	float			fTimePassed;
	int				iEffectStage;

};

//-----------------------------------------------------------------------------/
//  ����� ������������� �������
//-----------------------------------------------------------------------------/


class CUITextBanner
{
public:
	enum TextBannerStyles
	{
		tbsNone		= 0,
		tbsFlicker	= 1,
		tbsFade		= 1 << 1
	};
	// Ctor and Dtor
	CUITextBanner				();
	~CUITextBanner				();

	virtual void	Update		();
	void			Out			(float x, float y, const char *fmt, ...);

	// ���������� ��������� ������������ �������. ����� ��. ������������ TextBannerStyles
	void SetStyleParams			(const TextBannerStyles styleName, const float period);

	// Font
	void SetFont				(CGameFont *pFont)	{ m_pFont = pFont; }

	// Color
	void SetTextColor			(u32 cl);
	u32  GetTextColor			();

	// ���/���� ��������
	void	PlayAnimation		()					{ m_bAnimate = true;	}
	void	StopAnimation		()					{ m_bAnimate = false;	}

protected:
	// ���������� ������� ��� ������� �� ������.
	// � ����:	first	- ����������� ������ ������� (���������� �������������)
	//			second	- ��������� ����� � �������� �������
	typedef std::map<TextBannerStyles, EffectParams>	StyleParams;
	typedef StyleParams::iterator						StyleParams_it;
	StyleParams											m_StyleParams;

	// ��������� ��������� ���������� ������� ��� ��������
	void EffectFade();
	void EffectFlicker();

	// ����, ������� ���������� ��������� ��������
	bool		m_bAnimate;

	// Font
	CGameFont	*m_pFont;

	// Letters color
	u32			m_Cl;
};

#endif
