#include "stdafx.h"
#include "uizonemap.h"

#include "hudmanager.h"

#include "levelFogOfWar.h"

#include "InfoPortion.h"
#include "Pda.h"

#include "Grenade.h"
#include "level.h"
#include "game_cl_base.h"

#include "actor.h"
#include "ai_space.h"
#include "game_graph.h"

//////////////////////////////////////////////////////////////////////////

static float		MAX_VIEW_DISTANCE	=		50.f;
#define				VIEW_DISTANCE				(MAX_VIEW_DISTANCE/m_fScale)

#define				VIEW_DISTANCE2				VIEW_DISTANCE*VIEW_DISTANCE
#define				COLOR_ENEMY					0xffff0000
#define				COLOR_FRIEND				0xffffffff
#define				COLOR_SELF					0xff00ff00
#define				COLOR_TARGET				0xFFFFA0FF
#define				COLOR_BASE					0xff00a000
#define				COLOR_ARTEFACT				0xffffff00
#define				BASE_LEFT					9
#define				BASE_TOP					6
#define				MAP_LEFT					89
#define				MAP_TOP						93
#define				MAP_RADIUS					80
#define				LEVEL_DIFF					3.f
#define				MAX_SCALE					8.f
#define				MIN_SCALE					1.f
#define				MAP_AROW					8.f
#define				MIN_SCALE					1.f
#define				MAP_ARROW_TEXTURE			"ui\\ui_map_arrow_02"
#define				MAP_ARROW_ROTATION			-PI
#define				MAP_ARROW_WIDTH				16
#define				MAP_ARROW_HEIGHT			16

#define				MAP_SECTION					"game_map"

// �������� ����� ����� �������� ����� ������
const char * const	mapTextureKey				= "level_map";
// ���������� � ���������� �� ���������� �����
const char * const	mapLocalCoordinatesKey		= "ui_map_coords";
// ���������� � ������� � ������
const char * const	mapDimentions				= "ui_map_dims";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIZoneMap::CUIZoneMap()
{    
	heading = 0;
	m_fScale  = 1.0f;

	MAX_VIEW_DISTANCE = pSettings->r_float("game_map", "max_view_minimap");
}
//--------------------------------------------------------------------

CUIZoneMap::~CUIZoneMap()
{    
}

//////////////////////////////////////////////////////////////////////////

struct FindLevelByName
{
	FindLevelByName(const shared_str &l)
		:	levelName(l)
	{}
	bool operator ()(const GameGraph::LEVEL_MAP::value_type &lhs)
	{
		return 0 == xr_strcmp(levelName, lhs.second.name());
	}
	shared_str levelName;
};

//////////////////////////////////////////////////////////////////////////

void CUIZoneMap::Init()
{
	static LPCSTR	map_point_texture			= pSettings->r_string	(MAP_SECTION, "map_point");
	static u32		map_point_size				= pSettings->r_u32		(MAP_SECTION, "map_point_size");
	static LPCSTR	map_point_up_texture		= pSettings->r_string	(MAP_SECTION, "map_point_up");
	static u32		map_point_up_texture_size	= pSettings->r_u32		(MAP_SECTION, "map_point_up_size");
	static LPCSTR	map_point_down_texture		= pSettings->r_string	(MAP_SECTION, "map_point_down");
	static u32		map_point_down_texture_size	= pSettings->r_u32		(MAP_SECTION, "map_point_down_size");
	static LPCSTR	map_point_info_texture		= pSettings->r_string	(MAP_SECTION, "map_point_info");
	static u32		map_point_info_texture_size	= pSettings->r_u32		(MAP_SECTION, "map_point_info_size");
	static LPCSTR	map_arrow_texture			= pSettings->r_string	(MAP_SECTION, "map_arrow");

	DWORD align = alLeft|alTop;
	compass.Init("ui\\hud_map_arrow",	"hud\\default",125,118,align);
	
	entity.Init			(map_point_texture,			"hud\\default");
	entity_story.Init	(map_point_info_texture,	"hud\\default");
	entity_arrow.Init	(map_arrow_texture,			"hud\\default");
	entity_up.Init		(map_point_up_texture,		"hud\\default");
	entity_down.Init	(map_point_down_texture,	"hud\\default");
	
	entity.SetRect(0,0,map_point_size,map_point_size);
	entity.SetAlign(alLeft|alTop);
	entity_story.SetOffset(map_point_size/2, map_point_size/2);
	
	entity_story.SetRect(0,0,map_point_info_texture_size,map_point_info_texture_size);
	entity_story.SetOffset(map_point_info_texture_size/2, map_point_info_texture_size/2);
	entity_story.SetAlign(alLeft|alTop);

	entity_up.SetRect	(0,0,map_point_up_texture_size,map_point_up_texture_size);
	entity_up.SetAlign	(alLeft|alTop);
	entity_up.SetOffset (map_point_up_texture_size/2, map_point_up_texture_size/2);
	
	entity_down.SetRect	(0,0,map_point_down_texture_size,map_point_down_texture_size);
	entity_down.SetAlign(alLeft|alTop);
	entity_up.SetOffset (map_point_down_texture_size/2, map_point_down_texture_size/2);

	UI()->ClientToScreen(map_center,MAP_LEFT+BASE_LEFT,MAP_TOP+BASE_TOP,align);
	map_radius = MAP_RADIUS; //iFloor(MAP_RADIUS*HUD().GetScale());



	back.Init	("ui\\ui_mg_back_map",	"hud\\default",BASE_LEFT,BASE_TOP,align);
	back.SetRect(0,0,180,180);

	// �������� ������ ������� ����
	const GameGraph::LEVEL_MAP &levelMap = ai().game_graph().header().levels();

	FindLevelByName currentLevel(Level().name());
	GameGraph::LEVEL_MAP::const_iterator currLIt = std::find_if(levelMap.begin(), levelMap.end(), currentLevel);

	shared_str				map_texture;
	string256			gameLtxPath;
	FS.update_path					(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile			gameLtx		(gameLtxPath);

	if(currLIt != levelMap.end() &&
	   gameLtx.line_exist(currLIt->second.section(), mapTextureKey) &&
	   gameLtx.line_exist(currLIt->second.section(), mapDimentions))
	{

		map_texture		= gameLtx.r_string		(currLIt->second.section(), mapTextureKey);
		Fvector4 f		= gameLtx.r_fvector4	(currLIt->second.section(), mapDimentions);

		level_box.x1	= f.x;
		level_box.z1	= f.z;
		level_box.x2	= f.y;
		level_box.z2	= f.w;

	}
	else
	{
		if(Level().pLevel->line_exist("level_map", "texture"))
			map_texture = Level().pLevel->r_string("level_map","texture");
		else
			map_texture = "ui\\ui_nomap";

		if (Level().pLevel->section_exist("level_map"))	
		{
			level_box.x1 = Level().pLevel->r_float("level_map","x1");
			level_box.z1 = Level().pLevel->r_float("level_map","z1");
			level_box.x2 = Level().pLevel->r_float("level_map","x2");
			level_box.z2 = Level().pLevel->r_float("level_map","z2");
		}
	}

	landscape.Init(*map_texture,	"hud\\default",
					map_center.x - map_radius + 1,
					map_center.y - map_radius + 1,
					align);
	
	landscape.SetRect(0,0,2*map_radius,2*map_radius);
	landscape.SetColor(subst_alpha(landscape.GetColor(), 128));
}
//--------------------------------------------------------------------

void CUIZoneMap::ConvertToLocalWithoutTransform(const Fmatrix& LM, const Fvector& src, Fvector2 &Pt)
{
	float k = map_radius/VIEW_DISTANCE;

	Fvector p;
	LM.transform_tiny(p,src);
	p.mul(k);
	Pt.set(p.x,p.z);
}

//////////////////////////////////////////////////////////////////////////


void CUIZoneMap::ConvertToLocal	(const Fmatrix& LM, const Fvector& src, Ivector2& dest, int border_radius,  bool& on_border)
{
	Fvector2 Pt;
	ConvertToLocalWithoutTransform(LM, src, Pt);
	/*float r=Pt.magnitude();
	if (r>map_radius) Pt.mul((float)map_radius/r);*/
	
	on_border = false;
	
	if(Pt.x>(float)border_radius) 
	{
		Pt.x = (float)border_radius;
		on_border = true;
	}
	else if(Pt.x<(float)-border_radius) 
	{
		Pt.x = (float)-border_radius;
		on_border = true;
	}
	
	if(Pt.y>(float)border_radius) 
	{
		Pt.y = (float)border_radius;
		on_border = true;
	}
	else if(Pt.y<(float)-border_radius) 
	{
		Pt.y = (float)-border_radius;
		on_border = true;
	}

		
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


int x_m_x = 0;
int x_m_z = 0;
void CUIZoneMap::UpdateRadar(CActor* pActor)
{
	//.hack
	//����� ��������� ��� � ��������� �����
	Level().UpdateMapLocation			();



	entity.Clear		();
	entity_story.Clear	();
	entity_up.Clear		();
	entity_down.Clear	();
	entity_arrow.Clear	();
	Ivector2 P;

	Fmatrix LM,T;
	T.rotateY			(heading); 
	T.translate_over	(pActor->Position());
	LM.invert			(T);


	xr_vector<SZoneMapEntityData> vEnt;
	Level().game->GetMapEntities(vEnt);
	xr_vector<SZoneMapEntityData>::iterator it = vEnt.begin();
	for(; it !=vEnt.end(); ++it){
		float diff = 0.0f;
		Ivector2 pt;
		bool b;
		ConvertToLocal(LM, (*it).pos, pt, map_radius, b);
		diff = (*it).pos.y - pActor->Position().y;
		EntityOut(diff, (*it).color, pt);
	};

	if(!pActor->g_Alive())return;


#pragma todo("����� �� ��� : � ������� ����� ��������, ����� �� �������� ��� ������ ������ - ���� ����������� ...............")
	////////////////////////////////////////////
	//�������� �������, ��������� � ������ PDA
	////////////////////////////////////////////

	CObject* pObject = NULL;
	Fvector  world_pos;
	Fvector  src;


	//���� �� ���������
	u32 entity_color = COLOR_BASE;
	bool draw_entity = true;

	for(LOCATIONS_PTR_VECTOR_IT it = Level().MapLocations().begin();
		Level().MapLocations().end() != it;
		it++)
	{
		draw_entity = true;
		SMapLocation* map_location = (*it);

		if ( Level().name() != map_location->LevelName() ) 
			continue;

		entity_color = map_location->icon_color;
		if(map_location->attached_to_object)
		{
			pObject = Level().Objects.net_Find	(map_location->object_id);
			//���� ������ � �� ��� �� ����� ���� � ���������
			if(pObject && !(pObject->H_Parent() && pObject->H_Parent()->ID() == pActor->ID()))
			{
				src.x = pObject->Position().x;
				src.y = 0;
				src.z = pObject->Position().z;
			}
			else
				draw_entity = false;
		}
		else
		{
			src.x = map_location->x;
			src.y = 0;
			src.z = map_location->y;

			pObject = NULL;
			world_pos.set(map_location->x,0.f,map_location->y);
		}

		bool on_border;
		ConvertToLocal(LM, src, P, map_radius - MAP_ARROW_WIDTH/2, on_border);
		if(draw_entity)
		{
			if(!on_border)
			{
				if(map_location->big_icon)
					entity_story.Out(P.x,P.y,entity_color);
				else
					entity.Out(P.x,P.y,entity_color);
			}
			else if(map_location->marker_show)
			{
				float arrowHeading = 0;
				int x_shift = 0;
				int y_shift = 0;

				Fvector2 Pt;
				ConvertToLocalWithoutTransform(LM, src, Pt);
				Pt.x = map_center.x + Pt.x;
				Pt.y = map_center.y - Pt.y;

				// ��������� �� ����� �� ������ �������� ��������� ����� �������� �����
                float	directionK			= atan((map_center.y - Pt.y) / (map_center.x - Pt.x));

				// ������ �� ����, ��� � - ��� ������� ���� ������� ������ ������������ ��� �,
				// ��������� ����� ������� ��������� (������������ ��� ��������������) �������� �������������� ���
				// ���� �������
				bool	bVerticalClipLine	= ((directionK >= -PI_DIV_4) && (directionK < PI_DIV_4)) || (directionK <= -3 * PI_DIV_4) || (directionK > 3 * PI_DIV_4);

//				int		modifiedMapRadius	= map_radius - MAP_ARROW_WIDTH/2;

				// ���������� ���������� ���������
				if (!bVerticalClipLine)
				{
					P.x = static_cast<int>((P.y - map_center.y) * (Pt.x - map_center.x) / (Pt.y - map_center.y) + map_center.x);
				}
				else
				{
					P.y = static_cast<int>((P.x - map_center.x) * (Pt.y - map_center.y) / (Pt.x - map_center.x) + map_center.y);
				}
				
				// ������� ����� ����������� ������ - ������������ �� ����, � ��������������� �������
				// �������� �����

				if (P.y == map_center.y)
				{
					if (P.x > map_center.x)
						arrowHeading = PI_DIV_2;
					else
						arrowHeading = PI + PI_DIV_2;
				}
				else if (P.x == map_center.x)
				{
					if(P.y > map_center.y)
						arrowHeading = 0;
					else
						arrowHeading = -PI;
				}
				else
				{
					arrowHeading = std::atan(static_cast<float>(P.x - map_center.x) / (P.y - map_center.y));
					if (P.y < map_center.y) arrowHeading += PI;
				}

				arrowHeading += MAP_ARROW_ROTATION;
				entity_arrow.Out(P.x + x_shift - MAP_ARROW_WIDTH, 
					P.y + y_shift - MAP_ARROW_HEIGHT, entity_color, arrowHeading);
			}
		}
	}		


	// draw self
	entity.Out		(map_center.x, map_center.y,COLOR_SELF);

	/////////////////////
	// calc coord for the part of the landscape texture to show

	float map_x = pActor->Position().x;
	float map_y = pActor->Position().z;

	float width = level_box.x2 - level_box.x1;
	float height = level_box.z2 - level_box.z1;

	float center_x = level_box.x1 + width/2 + x_m_x;
	float center_y = level_box.z1 + height/2 + x_m_z;


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
	CGameFont* l_pF = HUD().Font().pFontMedium;
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
	entity_story.Render	();
	entity_up.Render	();
	entity_down.Render	();
	entity_arrow.Render	();
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

