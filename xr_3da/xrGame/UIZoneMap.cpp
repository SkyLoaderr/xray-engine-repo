#include "stdafx.h"
#include "uizonemap.h"
#include "entity.h"
#include "hudmanager.h"

#include "targetcs.h"
#include "targetcscask.h"
#include "targetcsbase.h"

#define VIEW_DISTANCE	50.f
#define VIEW_DISTANCE2	VIEW_DISTANCE*VIEW_DISTANCE
#define COLOR_ENEMY		0xffff0000
#define COLOR_FRIEND	0xffffffff
#define COLOR_SELF		0xff00ff00
#define COLOR_TARGET	0xFFFFA0FF
#define COLOR_BASE		0xff00a000

#define BASE_LEFT		9
#define BASE_TOP		6
#define MAP_LEFT		76
#define MAP_TOP			80
#define MAP_RADIUS		65

#define LEVEL_DIFF		3.f
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIZoneMap::CUIZoneMap()
{    
	heading = 0;
}
//--------------------------------------------------------------------

CUIZoneMap::~CUIZoneMap()
{    
}

void CUIZoneMap::Init()
{
	DWORD align = alLeft|alTop;
	back.Init	("ui\\hud_map_back",	"hud\\default",BASE_LEFT,BASE_TOP,align);
	back.SetRect(0,0,153,148);
	compass.Init("ui\\hud_map_arrow",	"hud\\default",125,118,align);
	entity.Init	("ui\\hud_map_point",	"hud\\default");
	entity_up.Init	("ui\\ui_hud_map_point_up",		"hud\\default");
	entity_down.Init("ui\\ui_hud_map_point_down",	"hud\\default");
	entity.SetRect(0,0,3,3);
	entity.SetAlign(alLeft|alTop);
	entity_up.SetRect	(0,0,3,4);
	entity_up.SetAlign	(alLeft|alTop);
	entity_down.SetRect	(0,0,3,4);
	entity_down.SetAlign(alLeft|alTop);

	Level().HUD()->ClientToScreen(map_center,MAP_LEFT+BASE_LEFT,MAP_TOP+BASE_TOP,align);
	map_radius = iFloor(MAP_RADIUS*Level().HUD()->GetScale());
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
	float r=Pt.magnitude();
	if (r>map_radius) Pt.mul((float)map_radius/r);
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
	for (u32 i=0; i<Team.KnownEnemys.size(); i++){
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
	case GAME_DEATHMATCH:	break;
	case GAME_ASSAULT:		bRender = TRUE;	break;
	case GAME_CTF:			bRender = TRUE;	break;
	case GAME_CS:			bRender = TRUE;	break;
	}
	if (bRender)
	{
		for (u32 i=0; i<Team.Squads.size(); i++){
			CSquad& S = Team.Squads[i];
			for (u32 j=0; j<S.Groups.size(); j++){
				CGroup& G = S.Groups[j];
				for (u32 k=0; k<G.Members.size(); k++){
					if (G.Members[k]->IsVisibleForHUD()){
						ConvertToLocal(LM,G.Members[k]->Position(),P);
						EntityOut(G.Members[k]->Position().y-Actor->Position().y,COLOR_FRIEND,P);
					}
				}
			}
		}
		for(u32 i=0; i<Game().targets.size(); i++) {
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
	}

	// draw self
	ConvertToLocal	(LM,Actor->Position(),P);
	entity.Out		(P.x,P.y,COLOR_SELF);
}
//--------------------------------------------------------------------

void CUIZoneMap::Render()
{
	back.Render			();
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
