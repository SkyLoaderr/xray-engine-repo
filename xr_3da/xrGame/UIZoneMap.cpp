#include "stdafx.h"
#include "uizonemap.h"
#include "entity.h"
#include "hudmanager.h"

#include "level.h"
#include "levelFogOfWar.h"

#include "targetcs.h"
#include "targetcscask.h"
#include "targetcsbase.h"

#include "InfoPortion.h"
#include "Pda.h"

#define MAX_VIEW_DISTANCE 50.f
#define VIEW_DISTANCE	(MAX_VIEW_DISTANCE/m_fScale)

#define VIEW_DISTANCE2	VIEW_DISTANCE*VIEW_DISTANCE
#define COLOR_ENEMY		0xffff0000
#define COLOR_FRIEND	0xffffffff
#define COLOR_SELF		0xff00ff00
#define COLOR_TARGET	0xFFFFA0FF
#define COLOR_BASE		0xff00a000

#define BASE_LEFT		9
#define BASE_TOP		6
//#define MAP_LEFT		76
//#define MAP_TOP			80
//#define MAP_RADIUS		65
#define MAP_LEFT		89
#define MAP_TOP			93
#define MAP_RADIUS		80

#define LEVEL_DIFF		3.f


#define MAX_SCALE		8.f
#define MIN_SCALE		1.f



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIZoneMap::CUIZoneMap()
{    
	heading = 0;
	m_fScale  = 1.0f;
}
//--------------------------------------------------------------------

CUIZoneMap::~CUIZoneMap()
{    
}

void CUIZoneMap::Init()
{
	DWORD align = alLeft|alTop;
//	back.Init	("ui\\hud_map_back",	"hud\\default",BASE_LEFT,BASE_TOP,align);
//	back.SetRect(0,0,153,148);

	compass.Init("ui\\hud_map_arrow",	"hud\\default",125,118,align);
	entity.Init	("ui\\hud_map_point",	"hud\\default");
	entity_up.Init	("ui\\ui_hud_map_point_up",		"hud\\default");
	entity_down.Init("ui\\ui_hud_map_point_down",	"hud\\default");
	entity.SetRect(0,0,3,3);
	entity.SetAlign(alLeft|alTop);
	entity_up.SetRect	(0,0,3,4);
	entity_up.SetAlign	(alLeft |alTop);
	entity_down.SetRect	(0,0,3,4);
	entity_down.SetAlign(alLeft|alTop);

	HUD().ClientToScreen(map_center,MAP_LEFT+BASE_LEFT,MAP_TOP+BASE_TOP,align);
	map_radius = iFloor(MAP_RADIUS*HUD().GetScale());



	back.Init	("ui\\ui_mg_back_map",	"hud\\default",BASE_LEFT,BASE_TOP,align);
	back.SetRect(0,0,180,180);


	ref_str map_texture;
	if(Level().pLevel->line_exist("level_map","texture"))
        map_texture = Level().pLevel->r_string("level_map","texture");
	else
		map_texture = "ui\\ui_minimap_level3";

	landscape.Init(*map_texture,	"hud\\default",
					map_center.x - map_radius + 1,
					map_center.y - map_radius + 1,
					align);
	
	landscape.SetRect(0,0,2*map_radius,2*map_radius);

#pragma todo("Correct ininializing params")
	if (Level().pLevel->section_exist("level_map"))	
	{
		level_box.x1 = Level().pLevel->r_float("level_map","x1");
		level_box.z1 = Level().pLevel->r_float("level_map","z1");
		level_box.x2 = Level().pLevel->r_float("level_map","x2");
		level_box.z2 = Level().pLevel->r_float("level_map","z2");
	}
	else
	{
		level_box.x1 = level_box.z1 = level_box.x2 = level_box.z2 = 0.f;
	}

}
//--------------------------------------------------------------------

void CUIZoneMap::ConvertToLocal	(const Fmatrix& LM, const Fvector& src, Ivector2& dest)
{
	float k = map_radius/VIEW_DISTANCE;
	
	Fvector p;
	Fvector2 Pt;
	LM.transform_tiny(p,src);
	p.mul(k);
	Pt.set(p.x,p.z);
	/*float r=Pt.magnitude();
	if (r>map_radius) Pt.mul((float)map_radius/r);*/
	
	
	if(Pt.x>(float)map_radius) Pt.x = (float)map_radius;
	else if(Pt.x<(float)-map_radius) Pt.x = (float)-map_radius;
	
	if(Pt.y>(float)map_radius) Pt.y = (float)map_radius;
	else if(Pt.y<(float)-map_radius) Pt.y = (float)-map_radius;

		
	dest.x = iFloor(map_center.x+Pt.x);
	dest.y = iFloor(map_center.y-Pt.y);
}
//--------------------------------------------------------------------
void CUIZoneMap::EntityOut(float diff, u32 color, const Ivector2& pt)
{
	if (diff>LEVEL_DIFF)		entity_up.Out(pt.x,pt.y,color);
	else if (diff<-LEVEL_DIFF)	entity_down.Out(pt.x,pt.y,color);
	else entity.Out	(pt.x,pt.y,color);
}

void CUIZoneMap::UpdateRadar(CEntity* Actor, CTeam& Team)
{
	CActor* pActor = dynamic_cast<CActor*>(Actor);

	entity.Clear		();
	entity_up.Clear		();
	entity_down.Clear	();
	Ivector2 P;

	Fmatrix LM,T;
	T.rotateY			(heading); 
	T.translate_over	(Actor->Position());
	LM.invert			(T);

	// render enemy
	/*
	for (u32 i=0; i<Team.KnownEnemys.size(); ++i){
		CEntity* E = dynamic_cast<CEntity*>(Team.KnownEnemys[i].key);
		if (E->IsVisibleForHUD()){
			ConvertToLocal(LM,E->Position(),P);
			entity.Out	(P.x,P.y,COLOR_ENEMY,alLeft|alTop);
		}
	}
	*/

	// render friend
	BOOL	bRender = FALSE;
	switch (GameID())
	{
	case GAME_SINGLE:		bRender = TRUE; break;
	case GAME_DEATHMATCH:	bRender = TRUE; break;
	case GAME_ASSAULT:		bRender = TRUE;	break;
	case GAME_CTF:			bRender = TRUE;	break;
	case GAME_CS:			bRender = TRUE;	break;
	case GAME_TEAMDEATHMATCH:	bRender = TRUE; break;
	case GAME_ARTEFACTHUNT:	bRender = TRUE; break;
	}
	
	if (bRender && (
					GameID() == GAME_DEATHMATCH || 
					GameID() == GAME_TEAMDEATHMATCH ||
					GameID() == GAME_ARTEFACTHUNT
					)
		)
	{
			for(u32 i=0; i<Level().Objects.objects.size(); i++)
			{
				CObject* pObj = Level().Objects.objects[i];
				if (!pObj) continue;
				CActor* pObjActor = dynamic_cast<CActor*>(pObj);
				if (GameID() == GAME_TEAMDEATHMATCH ||
					GameID() == GAME_ARTEFACTHUNT
					)
				{
					if (pObjActor && pObjActor != Level().CurrentEntity() && pObjActor->g_Alive())
					{
						ConvertToLocal(LM,pObjActor->Position(),P);

						if (pActor->id_Team == pObjActor->id_Team)
							EntityOut(pObjActor->Position().y-Actor->Position().y,COLOR_SELF,P);
						continue;
					};
				};
				CInventoryItem* pItem = dynamic_cast<CInventoryItem*>(pObj);
				if (pItem && !pItem->H_Parent())
				{
					if (GameID() == GAME_ARTEFACTHUNT)
					{
						CArtifact* pArtefact = dynamic_cast<CArtifact*>(pObj);
						if (pArtefact)
						{
							ConvertToLocal(LM,pItem->Position(),P);
							EntityOut(pItem->Position().y-Actor->Position().y,COLOR_FRIEND,P);
							continue;
						};
					};
					ConvertToLocal(LM,pItem->Position(),P);
					EntityOut(pItem->Position().y-Actor->Position().y,COLOR_TARGET,P);
					continue;
				};
			};
	};
/*	if (bRender)
	{
		for (u32 i=0; i<Team.Squads.size(); ++i){
			CSquad& S = Team.Squads[i];
			for (u32 j=0; j<S.Groups.size(); ++j){
				CGroup& G = S.Groups[j];
				for (u32 k=0; k<G.Members.size(); ++k){
					if (G.Members[k]->IsVisibleForHUD()){
						ConvertToLocal(LM,G.Members[k]->Position(),P);
						EntityOut(G.Members[k]->Position().y-Actor->Position().y,COLOR_FRIEND,P);
					}
				}
			}
		}
		for(u32 i=0; i<Game().targets.size(); ++i) {
			// draw artifacts
			CObject* T = dynamic_cast<CTargetCS*>(Game().targets[i]);
			if (T){
				if (T->H_Parent()) {
					CTargetCSCask *l_TC = dynamic_cast<CTargetCSCask*>(T->H_Parent());
					if(l_TC) T = T->H_Parent();
					else continue;
				}
				ConvertToLocal(LM,T->Position(),P);
				EntityOut(T->Position().y-Actor->Position().y,COLOR_TARGET,P);
			}else{
				// draw artifacts
				CTargetCSBase* TB = dynamic_cast<CTargetCSBase*>(Game().targets[i]);
				if (TB){
					if (TB->g_Team()==Actor->g_Team()){
						ConvertToLocal(LM,TB->Position(),P);
						EntityOut(TB->Position().y-Actor->Position().y,COLOR_BASE,P);
					}
				}
			}
		}
	}*/


	if(!pActor->g_Alive())return;
#pragma todo("Костя то Юра : я вставил здесь проверку, чтобы не вылетало при смерти актера - надо разобраться ...............")
	////////////////////////////////////////////
	//добавить локации, имеющиеся в памяти PDA
	////////////////////////////////////////////
	
	CObject* pObject = NULL;
	Fvector  world_pos;
	Fvector  src;


	if (pActor->GetPDA())
	{
#pragma todo("Mad Max то Юра : я вставил здесь проверку, чтобы не вылетало если нет PDA надо разобраться..............")
	for(KNOWN_INFO_PAIR_IT it = pActor->GetPDA()->m_mapKnownInfo.begin();
		pActor->GetPDA()->m_mapKnownInfo.end() != it;
		++it)
	{
		//подгрузить кусочек информации с которым мы работаем
		CInfoPortion info_portion;
		info_portion.Load((*it).first);

		//цвет по умолчанию
		u32 entity_color = COLOR_BASE;

		//локации заданные в порцие информации	
		for(int i=0; i<info_portion.GetLocationsNum(); i++)
		{
			SMapLocation& map_location = info_portion.GetLocation(i);
			if(map_location.attached_to_object)
			{
				pObject = Level().Objects.net_Find	(map_location.object_id);
				
				CEntity* pEntity = NULL;
				if(pObject)
					pEntity = dynamic_cast<CEntity*>(pObject);

				if(pEntity)
				{
					if(pEntity->g_Team() == pActor->g_Team())
						entity_color = COLOR_FRIEND;
					else
						entity_color = COLOR_ENEMY;
				}

				src.x = pObject->Position().x;
				src.y = 0;
				src.z = pObject->Position().z;
			}
			else
			{
				src.x = map_location.x;
				src.y = 0;
				src.z = map_location.y;

				pObject = NULL;
				world_pos.set(map_location.x,0.f,map_location.y);
			}

			ConvertToLocal(LM, src, P);
			entity.Out(P.x,P.y,entity_color);
		}
	}
	};



	// draw self
	ConvertToLocal	(LM,Actor->Position(),P);
	entity.Out		(map_center.x,P.y,COLOR_SELF);

	/////////////////////
	// calc coord for the part of the landscape texture to show
	float map_x = Actor->Position().x;
    float map_y = Actor->Position().z;
	
	float width = level_box.x2 - level_box.x1;
	float height = level_box.z2 - level_box.z1;

	float center_x = level_box.x1 + width/2;
	float center_y = level_box.z1 + height/2;


	float x = width/2  + map_x - center_x;
	float y = height/2 - map_y + center_y;

	float w =  2*VIEW_DISTANCE;
	float h =  2*VIEW_DISTANCE;

	Fvector2 LTt,RBt, LBt, RTt;
	Fvector p;
	Fmatrix m;
	m.identity();
	m.rotateY(-heading);

	src.x =  - w/2;
	src.y = 0;
	src.z =  + h/2;
	m.transform_tiny(p,src);
	LBt.set(x + p.x, y +  p.z);

	src.x =  - w/2;
	src.y = 0;
	src.z =  - h/2;
	m.transform_tiny(p,src);
	LTt.set(x+ p.x, y + p.z);

	src.x =  + w/2;
	src.y = 0;
	src.z =  + h/2;
	m.transform_tiny(p,src);
	RBt.set(x+ p.x, y + p.z);

	src.x =  + w/2;
	src.y = 0;
	src.z =  - h/2;
	m.transform_tiny(p,src);
	RTt.set(x+p.x, y + p.z);

	landscape_x1 = LTt.x/width;
	landscape_y1 = LTt.y/height;
	landscape_x2 = LBt.x/width;
	landscape_y2 = LBt.y/height;
	landscape_x3 = RBt.x/width;
	landscape_y3 = RBt.y/height;
	landscape_x4 = RTt.x/width;
	landscape_y4 = RTt.y/height;

	//draw scale sign
	CGameFont* l_pF = HUD().pFontMedium;
	l_pF->SetColor(0xffffffff);
	string256 buf;
	sprintf(buf, "%dx", int(m_fScale));
	l_pF->Out(float(BASE_LEFT + 12), 
			  float(map_center.y + map_radius - l_pF->CurrentHeight()),  buf);
}
//--------------------------------------------------------------------

void CUIZoneMap::Render()
{
	back.Render			();

 	landscape.Render(landscape_x1, landscape_y1,
		             landscape_x2, landscape_y2,
					 landscape_x3, landscape_y3,
					 landscape_x4, landscape_y4);
	
	//////////////////////////////////////////
	compass.Render		(heading);
	entity.Render		();
	entity_up.Render	();
	entity_down.Render	();
}
//--------------------------------------------------------------------

void CUIZoneMap::SetHeading(float angle){
	heading			= angle;
}
//--------------------------------------------------------------------
bool CUIZoneMap::ZoomIn()
{
	if(m_fScale<MAX_SCALE)
	{
		m_fScale += 1;
		return true;
	}

	return false;
}
bool CUIZoneMap::ZoomOut()
{
	if(m_fScale>MIN_SCALE)
	{
		m_fScale -= 1;
		return true;
	}
	return false;
}