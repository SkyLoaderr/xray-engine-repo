#include "stdafx.h"
#include "uizonemap.h"
#include "entity.h"
#include "hudmanager.h"


#define VIEW_DISTANCE	50.f
#define VIEW_DISTANCE2	VIEW_DISTANCE*VIEW_DISTANCE
#define COLOR_ENEMY		0xffff0000
#define COLOR_FRIEND	0xffffffff
#define COLOR_SELF		0xff00ff00

#define BASE_LEFT		9.f
#define BASE_TOP		6.f
#define MAP_LEFT		76.f
#define MAP_TOP			80.f
#define MAP_RADIUS		66.f

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

void CUIZoneMap::Init(){
	DWORD align = alLeft|alTop;
	back.Init	("ui\\hud_map_back","font",BASE_LEFT,BASE_TOP,256,256,align);
	back.SetRect(0,0,153,148);
	compass.Init("ui\\hud_map_arrow","font",124,117,32,32,align);
	entity.Init	("ui\\hud_map_point","font",4,4);
	entity.SetRect(0,0,3,3);

	Level().HUD()->ClientToScreen(MAP_LEFT+BASE_LEFT,MAP_TOP+BASE_TOP,map_center,align);
	map_radius = MAP_RADIUS*Level().HUD()->GetScale();
}
//--------------------------------------------------------------------

void CUIZoneMap::ConvertToLocal	(const Fmatrix& LM, const Fvector& src, Fvector2& dest)
{
	float k = map_radius/VIEW_DISTANCE;

	Fvector p;
	Fvector2 Pt;
	LM.transform_tiny(p,src);
	Pt.set(p.x,p.z);
	Pt.mul(k);
	float r=Pt.magnitude();
	if (r>map_radius) Pt.mul(map_radius/r);
	Pt.y*=-1;
	dest.add(map_center,Pt);
}
//--------------------------------------------------------------------

void CUIZoneMap::UpdateRadar(CEntity* Actor, CTeam& Team){
	entity.Clear();
	Fvector2 P;

	Fmatrix LM;
	LM.rotateY(heading);
	LM.translate_over(Actor->Position());
	LM.invert();

	// draw self
	ConvertToLocal	(LM,Actor->Position(),P);
	entity.Out		(P.x,P.y,COLOR_SELF,alLeft|alTop);
	// render enemy
	for (DWORD i=0; i<Team.KnownEnemys.size(); i++){
		CEntity* E = (CEntity*)Team.KnownEnemys[i].key;
		ConvertToLocal(LM,E->Position(),P);
		entity.Out	(P.x,P.y,COLOR_ENEMY,alLeft|alTop);
	}
	// render friend
	for (i=0; i<Team.Squads.size(); i++){
		CSquad& S = Team.Squads[i];
		for (DWORD j=0; j<S.Groups.size(); j++){
			CGroup& G = S.Groups[j];
			for (DWORD k=0; k<G.Members.size(); k++){
				ConvertToLocal(LM,G.Members[k]->Position(),P);
				entity.Out	(P.x,P.y,COLOR_FRIEND,alLeft|alTop);
			}
		}
	}
}
//--------------------------------------------------------------------

void CUIZoneMap::Render(){
	back.Render		();
	compass.Render	(heading);
	entity.Render	();
}
//--------------------------------------------------------------------

void CUIZoneMap::SetHeading(float angle){
	heading			= angle;
}
//--------------------------------------------------------------------
