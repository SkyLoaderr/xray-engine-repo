//=============================================================================
//  Красивый текстовый баннер с множеством стилей
//=============================================================================

#include "stdafx.h"
#include "UITextBanner.h"

CUITextBanner::CUITextBanner()
	:	m_bAnimate(true),
		m_Cl(0xffffffff),
		m_pFont(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////

CUITextBanner::~CUITextBanner()
{

}

////////////////////////////////////////////////////////////////////////////////

void CUITextBanner::SetStyleParams(const TextBannerStyles styleName, const float period)
{
	if (tbsNone == styleName)
	{
		if (!m_StyleParams.empty())
			m_StyleParams.clear();
		return;
	}

	EffectParams	param;
	param.fPeriod	= period;

	m_StyleParams[styleName] = param;
}

////////////////////////////////////////////////////////////////////////////////

void CUITextBanner::Update()
{
	StyleParams_it it = m_StyleParams.begin();

	// Если анимация включена
	if (m_bAnimate)
	{
		for (; it != m_StyleParams.end(); ++it)
		{
			it->second.fTimePassed += Device.fTimeDelta;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CUITextBanner::Out(float x, float y, const char *fmt, ...)
{
	StyleParams_it it = m_StyleParams.begin();

	// Применяем эффекты
	for (; it != m_StyleParams.end(); ++it)
	{
		// Fade effect
		if (it->first & tbsFade)
		{
			EffectFade();
		}
		// Flicker effect
		if (it->first & tbsFlicker)
		{
			EffectFlicker();
		}
	}

	va_list		Print;                                                                  
	string256	msg;

	va_start(Print, fmt);
	vsprintf(msg, fmt, Print);                   
	va_end(Print);

	R_ASSERT(m_pFont);
	m_pFont->SetColor(m_Cl);
	m_pFont->Out(x, y, msg);
}

////////////////////////////////////////////////////////////////////////////////

void CUITextBanner::EffectFade()
{
	EffectParams	&fade = m_StyleParams[tbsFade];

	// Если пришло время сменить направление фейда
	if (fade.fTimePassed > fade.fPeriod)
	{
		if (0 == fade.iEffectStage)
			fade.iEffectStage = 1;
		else
			fade.iEffectStage = 0;
		fade.fTimePassed = 0;
	}

	if (fade.iEffectStage)
	{
		m_Cl = subst_alpha(GetTextColor(), u8(iFloor(255.f*(fade.fTimePassed / fade.fPeriod))));
	}
	else
	{
		m_Cl = subst_alpha(GetTextColor(), u8(iFloor(255.f*(1 - (fade.fTimePassed / fade.fPeriod)))));
	}
}

//////////////////////////////////////////////////////////////////////////

void CUITextBanner::EffectFlicker()
{
	EffectParams	&flicker = m_StyleParams[tbsFlicker];

	// Если пришло время, показать/спрятать надпись
	if (flicker.fTimePassed > flicker.fPeriod)
	{
		if (0 == flicker.iEffectStage)
			flicker.iEffectStage = 1;
		else
			flicker.iEffectStage = 0;
		flicker.fTimePassed = 0;
	}

	if (flicker.iEffectStage)
	{
		m_Cl = subst_alpha(GetTextColor(), 0);
	}
	else
	{
		m_Cl = subst_alpha(GetTextColor(), 255);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUITextBanner::SetTextColor(u32 cl)
{
	R_ASSERT(m_pFont);
	if (!m_pFont) return;

	m_Cl = cl;
}

//////////////////////////////////////////////////////////////////////////

u32  CUITextBanner::GetTextColor()
{
	void SetTextColor			(u32 cl);
	u32  GetTextColor			();

	return m_Cl;
}