#include "stdafx.h"
#include "uizonemap.h"
#include "entity.h"
#include "hudmanager.h"


#define VIEW_DISTANCE	50.f
#define VIEW_DISTANCE2	VIEW_DISTANCE*VIEW_DISTANCE
#define COLOR_ENEMY		0xffff0000
#define COLOR_FRIEND	0xffffffff
#define COLOR_SELF		0xff00ff00

#define BASE_LEFT		9
#define BASE_TOP		6
#define MAP_LEFT		76
#define MAP_TOP			80
#define MAP_RADIUS		66

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
	entity.SetRect(0,0,3,3);

	Level().HUD()->ClientToScreen(map_center,MAP_LEFT+BASE_LEFT,MAP_TOP+BASE_TOP,align);
	map_radius = iFloor(MAP_RADIUS*Level().HUD()->GetScale());
}
//--------------------------------------------------------------------

void CUIZoneMap::ConvertToLocal	(const Fmatrix& LM, const Fvector& src, Ivector2& dest)
{
	float k = map_radius/VIEW_DISTANCE;
	Fvector p;
	Ivector2 Pt;
	LM.transform_tiny(p,src);
	p.mul(k);
	Pt.set(iFloor(p.x),iFloor(p.z));
	int r=Pt.magnitude();
	if (r>map_radius) Pt.mul(map_radius/r);
	Pt.y*=-1;
	dest.add(map_center,Pt);
}
//--------------------------------------------------------------------

void CUIZoneMap::UpdateRadar(CEntity* Actor, CTeam& Team)
{
	entity.Clear();
	Ivector2 P;

	Fmatrix LM,T;
	T.rotateY			(heading); 
	T.translate_over	(Actor->Position());
	LM.invert			(T);

	// draw self
	ConvertToLocal	(LM,Actor->Position(),P);
	entity.Out		(P.x,P.y,COLOR_SELF,alLeft|alTop);

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
	}
	if (bRender)
	{
		for (u32 i=0; i<Team.Squads.size(); i++){
			CSquad& S = Team.Squads[i];
			for (u32 j=0; j<S.Groups.size(); j++){
				CGroup& G = S.Groups[j];
				for (u32 k=0; k<G.Members.size(); k++)
				{

					if (G.Members[k]->IsVisibleForHUD()){
						ConvertToLocal(LM,G.Members[k]->Position(),P);
						entity.Out	(P.x,P.y,COLOR_FRIEND,alLeft|alTop);
					}
				}
			}
		}
	}
}
//--------------------------------------------------------------------

void CUIZoneMap::Render()
{
	back.Render		();
	compass.Render	(heading);
	entity.Render	();
}
//--------------------------------------------------------------------

void CUIZoneMap::SetHeading(float angle){
	heading			= angle;
}
//--------------------------------------------------------------------
