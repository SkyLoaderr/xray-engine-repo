// UIMapSpot.cpp:  пятно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapSpot.h"

#include "../actor.h"
#include "../level.h"


#include "../xrserver.h"
#include "../xrServer_Objects_ALife.h"

const char * const	ARROW_TEX			= "ui\\ui_map_arrow_04";
const int			ARROW_DIMENTIONS	= 32;

CUIMapSpot::CUIMapSpot()
	:	m_bArrowEnabled		(false),
		m_bArrowVisible		(false)
{
	m_our_level_id = 0xffff;
	m_object_id = 0xffff;
	m_vWorldPos.set(0,0,0);

	m_eAlign = eNone;

	m_sDescText.SetText("");
	m_sNameText.SetText("");

	m_fHeading = 0.f;
	m_bHeading = false;

	ClipperOn();

	m_Arrow.CreateShader(ARROW_TEX, "hud\\default");

	arrow_color = 0xffffffff;
}
CUIMapSpot::~CUIMapSpot()
{
}

void CUIMapSpot::Draw()
{
	if(m_bHeading)
	{
		RECT rect = GetAbsoluteRect();
		if(m_bClipper) TextureClipper();
		m_UIStaticItem.SetPos(rect.left, rect.top);
		m_UIStaticItem.Render(m_fHeading);
 	}
	else
		CUIStatic::Draw();

	// Если нужно рисовать стрелочку-указатель
	if (m_bArrowEnabled)
	{
		// Определяем, видимы ли на парентовом окне, и если нет, то рисуем стрелочку-указатель
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

		// Рисуем стрелку, только если иконка не видна
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
			m_Arrow.SetColor(arrow_color);
			m_Arrow.Render(arrowHeading);
		}
	}
}

void CUIMapSpot::Update()
{
	CUIStatic::Update();
}

Fvector CUIMapSpot::MapPos()
{
	if(0xffff == m_object_id)
		return m_vWorldPos;

	Fvector src = {0,0,0};

	CSE_Abstract* E = Level().Server->game->get_entity_from_eid(m_object_id);
	CSE_ALifeObject* O = NULL;
	if(E) O = smart_cast<CSE_ALifeObject*>(E);
	if(O)
	{
		//объект в оффлайне на нашем уровне
		if(m_our_level_id ==
			ai().game_graph().vertex(O->m_tGraphID)->level_id())
		{
			src.x = O->position().x;
			src.y = 0;
			src.z = O->position().z;
		}
	}


	CObject* pObject =  Level().Objects.net_Find(m_object_id);
	//объект в онлайне и в инвентаре у актера, то ничего не показывать
	if(pObject && pObject->H_Parent() == Level().CurrentEntity())
		Show(false);
	else
		Show(true);
	

		
	/*		float dx = src.x - pObject->Position().x;
		float dz = src.z - pObject->Position().z;

		dx = dx;
		dz = dz;
		
		src.x = pObject->Position().x;
		src.y = 0;
		src.z = pObject->Position().z;
	}
*/

	return src;
}	

void CUIMapSpot::SetObjectID(u16 id)
{
	m_object_id = id;
	m_our_level_id = ai().game_graph().vertex(smart_cast<CGameObject*>(Level().CurrentEntity())->game_vertex_id())->level_id();
}