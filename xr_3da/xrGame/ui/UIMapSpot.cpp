// UIMapSpot.cpp:  ����� �� �����, 
// ��� ������� ���� �������� ��������� ��������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapSpot.h"

#include "../actor.h"

const char * const	ARROW_TEX			= "ui\\ui_map_arrow_04";
const int			ARROW_DIMENTIONS	= 32;

CUIMapSpot::CUIMapSpot()
	:	m_bArrowEnabled		(false),
		m_bArrowVisible		(false)
{
	m_pObject = NULL;
	m_vWorldPos.set(0,0,0);

	m_eAlign = eNone;

	m_sDescText.SetText("");
	m_sNameText.SetText("");

	m_fHeading = 0.f;
	m_bHeading = false;

	ClipperOn();

	m_Arrow.CreateShader(ARROW_TEX, "hud\\default");
}
CUIMapSpot::~CUIMapSpot()
{
}

void CUIMapSpot::Draw()
{
	if(dynamic_cast<CActor*>(this->m_pObject))
	{
		int a =0;
		a++;
	}
	if(m_bHeading)
	{
		RECT rect = GetAbsoluteRect();
		if(m_bClipper) TextureClipper();
		m_UIStaticItem.SetPos(rect.left, rect.top);
		m_UIStaticItem.Render(m_fHeading);
 	}
	else
		CUIStatic::Draw();

	// ���� ����� �������� ���������-���������
	if (m_bArrowEnabled)
	{
		// ����������, ������ �� �� ���������� ����, � ���� ���, �� ������ ���������-���������
		CUIWindow *p = GetParent();

		R_ASSERT(p);
		if (!p) return;

		m_bArrowVisible = false;

		RECT parentRect = p->GetAbsoluteRect();
		RECT ourRect	= GetAbsoluteRect(), ourRect2 = GetAbsoluteRect();

		// Clamp x coordinate
		clamp(ourRect.left, parentRect.left - ourRect.right + ourRect.left, parentRect.right);
		// Clamp y coordinate
		clamp(ourRect.top, parentRect.top - ourRect.bottom + ourRect.top, parentRect.bottom);

		// ������ �������, ������ ���� ������ �� �����
		if (ourRect.left != ourRect2.left || ourRect2.top != ourRect.top)
		{

			m_bArrowVisible = true;
			float arrowHeading = 0;
			if (ourRect2.top == ourRect.top)
			{
				if (ourRect.left > ourRect2.left)
					arrowHeading = PI + PI_DIV_2;
				else
					arrowHeading = PI_DIV_2;
			}
			else if (ourRect2.left == ourRect.left && ourRect.top > ourRect2.top)
			{
				arrowHeading = PI;
			}
			else
			{
				arrowHeading = std::atan(static_cast<float>(ourRect2.left - ourRect.left) / (ourRect2.top - ourRect.top));
				if (ourRect2.top < ourRect.top) arrowHeading += PI;
			}

			// Some small coordinate fixes
			clamp(ourRect.left, parentRect.left, parentRect.right - ARROW_DIMENTIONS);
			// Clamp y coordinate
			clamp(ourRect.top, parentRect.top, parentRect.bottom - ARROW_DIMENTIONS);

			m_Arrow.SetPos(ourRect.left, ourRect.top);
			m_Arrow.Render(arrowHeading);
		}
	}
}

void CUIMapSpot::Update()
{
	CUIStatic::Update();
}