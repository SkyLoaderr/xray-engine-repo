// HUDCrosshair.h:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#define HUD_CURSOR_SECTION "hud_cursor"


class CHUDCrosshair:public pureDeviceCreate, public pureDeviceDestroy
{
private:
	int				cross_length;
	int				min_radius;
	int				max_radius;
	
	Ivector2		center;
	Fcolor			cross_color;

	//текущий радиус прицела
	int				radius;
	int				target_radius;
	int				radius_speed;

	ref_geom 		hGeomLine;
	ref_shader		hShader;
public:
					CHUDCrosshair	();
					~CHUDCrosshair	();

			void	OnRender		();
	virtual void 	OnDeviceCreate	();
	virtual void 	OnDeviceDestroy	();

			void	SetDispersion	(float disp);

			void	Load			();
};