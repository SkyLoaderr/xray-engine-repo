//=============================================================================
//  Красивый текстовый баннер с множеством стилей анимации
//=============================================================================

#ifndef UI_TEXT_BANNER_H_
#define UI_TEXT_BANNER_H_

// #pragma once;

#include "UIStatic.h"

//-----------------------------------------------------------------------------/
//  Класс параметров эффекта
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
//  Класс анимироанного баннера
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

	// Установить параметры визуализации баннера. Флаги см. перечисление TextBannerStyles
	void SetStyleParams			(const TextBannerStyles styleName, const float period);

	// Font
	void SetFont				(CGameFont *pFont)	{ m_pFont = pFont; }

	// Color
	void SetTextColor			(u32 cl);
	u32  GetTextColor			();

	// Вкл/выкл анимации
	void	PlayAnimation		()					{ m_bAnimate = true;	}
	void	StopAnimation		()					{ m_bAnimate = false;	}

protected:
	// Переменные времени для каждого из стилей.
	// В паре:	first	- контрольный период эффекта (задаваемый пользователем)
	//			second	- прошедшее время с текущего апдейта
	typedef std::map<TextBannerStyles, EffectParams>	StyleParams;
	typedef StyleParams::iterator						StyleParams_it;
	StyleParams											m_StyleParams;

	// Процедуры изменения параметров надписи для эффектов
	void EffectFade();
	void EffectFlicker();

	// Флаг, который определяет состояние анимации
	bool		m_bAnimate;

	// Font
	CGameFont	*m_pFont;

	// Letters color
	u32			m_Cl;
};

#endif
