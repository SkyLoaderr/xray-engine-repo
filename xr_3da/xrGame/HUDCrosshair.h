// HUDCrosshair.h:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#define HUD_CURSOR_SECTION "hud_cursor"


class CHUDCrosshair	
{
private:
	float			cross_length_perc;
	float			min_radius_perc;
	float			max_radius_perc;
	Ivector2		center;
	Fcolor			cross_color;

	//текущий радиус прицела
	int				radius;
	int				target_radius;
	int				radius_speed;
	float			radius_speed_perc; 

	ref_geom 		hGeomLine;
	ref_shader		hShader;
public:
					CHUDCrosshair	();
					~CHUDCrosshair	();

			void	OnRender		();
			void	SetDispersion	(float disp);

			void	Load			();
};