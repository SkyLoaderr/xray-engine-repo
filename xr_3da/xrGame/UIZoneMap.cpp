#include "stdafx.h"
#include "uizonemap.h"

#include "hudmanager.h"

//#include "levelFogOfWar.h"

#include "InfoPortion.h"
#include "Pda.h"

#include "Grenade.h"
#include "level.h"
#include "game_cl_base.h"

#include "actor.h"
#include "ai_space.h"
#include "game_graph.h"

#include "ui/UIMapWnd.h"

//////////////////////////////////////////////////////////////////////////

CUIZoneMap::CUIZoneMap()
{}

CUIZoneMap::~CUIZoneMap()
{
	
}

void CUIZoneMap::Init()
{
	string256			gameLtxPath;
	FS.update_path		(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile			gameLtx		(gameLtxPath);


	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH, "zone_map.xml");
	R_ASSERT3(xml_result, "xml file not found", "zone_map.xml");

	// load map background
	CUIXmlInit xml_init;
	xml_init.InitStatic(uiXml, "minimap:background", 0, &m_background);

	xml_init.InitStatic(uiXml, "minimap:background:level_frame", 0, &m_clipFrame);
	m_background.AttachChild(&m_clipFrame);

	xml_init.InitStatic(uiXml, "minimap:center", 0, &m_center);
	

	
	m_activeMap = xr_new<CUIMiniMap>();
	m_activeMap->SetAutoDelete(true);
	m_activeMap->Init(Level().name(),gameLtx);
	m_clipFrame.AttachChild(m_activeMap);
	m_activeMap->SetClipRect( m_clipFrame.GetAbsoluteRect() );
	m_activeMap->SetZoomFactor( float(m_clipFrame.GetWndRect().width())/100.0f );
	m_activeMap->EnableHeading(true);  
	xml_init.InitStatic(uiXml, "minimap:compass", 0, &m_compass);
	m_compass.EnableHeading(true);
//	m_background.AttachChild(&m_compass);

	m_clipFrame.AttachChild(&m_center);
	m_center.SetWndPos(m_clipFrame.GetWidth()/2,m_clipFrame.GetHeight()/2);
}

void CUIZoneMap::Render			()
{
	m_background.Draw();
	m_compass.Draw();
}

void CUIZoneMap::SetHeading		(float angle)
{
	m_activeMap->SetHeading(angle);
	m_compass.SetHeading(angle);
};

void CUIZoneMap::UpdateRadar		(CActor* Actor)
{
	m_background.Update();
	m_activeMap->SetActivePoint( Actor->Position() );
}

bool CUIZoneMap::ZoomIn()
{
	return true;
}

bool CUIZoneMap::ZoomOut()
{
	return true;
}


//////////////////////////////////////////////////////////////////////////
/*
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

// Ќазвание ключа имени текстуры карты уровн€
const char * const	mapTextureKey				= "level_map";
// јналогично еЄ координаты на глобальной карте
const char * const	mapLocalCoordinatesKey		= "ui_map_coords";

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
	LPCSTR	map_point_texture			= pSettings->r_string	(MAP_SECTION, "map_point");
	u32		map_point_size				= pSettings->r_u32		(MAP_SECTION, "map_point_size");
	LPCSTR	map_point_up_texture		= pSettings->r_string	(MAP_SECTION, "map_point_up");
	u32		map_point_up_texture_size	= pSettings->r_u32		(MAP_SECTION, "map_point_up_size");
	LPCSTR	map_point_down_texture		= pSettings->r_string	(MAP_SECTION, "map_point_down");
	u32		map_point_down_texture_size	= pSettings->r_u32		(MAP_SECTION, "map_point_down_size");
	LPCSTR	map_point_info_texture		= pSettings->r_string	(MAP_SECTION, "map_point_info");
	u32		map_point_info_texture_size	= pSettings->r_u32		(MAP_SECTION, "map_point_info_size");
	LPCSTR	map_arrow_texture			= pSettings->r_string	(MAP_SECTION, "map_arrow");

	DWORD align = alLeft|alTop;
	compass.Init("ui\\hud_map_arrow",	"hud\\default",150,150,align);
	
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

	// ѕолучаем список уровней игры
	const GameGraph::LEVEL_MAP &levelMap = ai().game_graph().header().levels();

	FindLevelByName currentLevel(Level().name());
	GameGraph::LEVEL_MAP::const_iterator currLIt = std::find_if(levelMap.begin(), levelMap.end(), currentLevel);

	shared_str				map_texture;
	string256			gameLtxPath;
	FS.update_path					(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile			gameLtx		(gameLtxPath);

	if((currLIt != levelMap.end())&&gameLtx.line_exist(currLIt->second.section(), mapTextureKey)){
		map_texture		= gameLtx.r_string		(currLIt->second.section(), mapTextureKey);
	}else{
		if(Level().pLevel->line_exist("level_map", "texture"))
			map_texture = Level().pLevel->r_string("level_map","texture");
		else
			map_texture = "ui\\ui_nomap";
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
	//нужно обновл€ть раз в некоторое врем€
	Level().UpdateMapLocation			();
	const Fbox& level_box	= Level().ObjectSpace.GetBoundingVolume();

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


#pragma todo(" ост€ то ёра : € вставил здесь проверку, чтобы не вылетало при смерти актера - надо разобратьс€ ...............")
	////////////////////////////////////////////
	//добавить локации, имеющиес€ в пам€ти PDA
	////////////////////////////////////////////

	CObject* pObject = NULL;
	Fvector  world_pos;
	Fvector  src;


	//цвет по умолчанию
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
			//если объект и мы его не вз€ли себе в инвентарь
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

				// ќпредел€м на какой из сторон квадрата миникарты лежит проекци€ точки
                float	directionK			= atan((map_center.y - Pt.y) / (map_center.x - Pt.x));

				// »сход€ из того, что к - это тангенс угла наклона пр€мой относительно оси ’,
				// вычисл€ем какие стороны миникарты (вертикальные или горизонтальные) €вл€ютс€ ограничител€ми дл€
				// хода стрелки
				bool	bVerticalClipLine	= ((directionK >= -PI_DIV_4) && (directionK < PI_DIV_4)) || (directionK <= -3 * PI_DIV_4) || (directionK > 3 * PI_DIV_4);

//				int		modifiedMapRadius	= map_radius - MAP_ARROW_WIDTH/2;

				// ќпредел€ем координаты стрелочки
				if (!bVerticalClipLine)
				{
					P.x = static_cast<int>((P.y - map_center.y) * (Pt.x - map_center.x) / (Pt.y - map_center.y) + map_center.x);
				}
				else
				{
					P.y = static_cast<int>((P.x - map_center.x) * (Pt.y - map_center.y) / (Pt.x - map_center.x) + map_center.y);
				}
				
				// Ќаходим точку пересечени€ пр€мой - направлением на цель, и соответствующей стороны
				// квадрата карты

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
	if (GameID() == GAME_SINGLE)
		l_pF->Out(float(BASE_LEFT + 12),205.0f*UI()->GetScaleY(), buf); 
//		float(map_center.y + map_radius - l_pF->CurrentHeight()),  buf);
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

*/