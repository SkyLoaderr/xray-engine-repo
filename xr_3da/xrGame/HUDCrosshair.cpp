// HUDCrosshair.cpp:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HUDCrosshair.h"

CHUDCrosshair::CHUDCrosshair	()
{
	hGeomLine.create			(FVF::F_TL0uv,RCache.Vertex.Buffer(),0);
	hShader.create				("hud\\cursor","ui\\cursor");

	//вычислить и запомнить центр экрана
	center.set(int(Device.dwWidth)/2,int(Device.dwHeight)/2);
	radius = 0;
}


CHUDCrosshair::~CHUDCrosshair	()
{
	hGeomLine.destroy			();
	hShader.destroy				();
}

void CHUDCrosshair::Load		()
{
	//все размеры в процентах от длины экрана
	//длина крестика 
	float cross_length_perc = pSettings->r_float (HUD_CURSOR_SECTION, "cross_length");
	cross_length = iFloor(0.5f + cross_length_perc*float(Device.dwWidth));

	float min_radius_perc = pSettings->r_float (HUD_CURSOR_SECTION, "min_radius");
	min_radius = iFloor(0.5f + min_radius_perc*float(Device.dwWidth));

	float max_radius_perc = pSettings->r_float (HUD_CURSOR_SECTION, "max_radius");
	max_radius = iFloor(0.5f + max_radius_perc*float(Device.dwWidth));

	cross_color = pSettings->r_fcolor (HUD_CURSOR_SECTION, "cross_color");


	float radius_speed_perc = pSettings->r_float (HUD_CURSOR_SECTION, "radius_lerp_speed");
	radius_speed = iFloor(0.5f + radius_speed_perc*float(Device.dwWidth));
	if(radius_speed<=0)
		radius_speed = 1;
}

//выставляет radius от min_radius до max_radius
void CHUDCrosshair::SetDispersion	(float disp)
{ 
	Fvector E={0,0,0};
	Fvector D={0,0,1}, R={1,0,0};
	E.mad(D,_cos(disp));
	E.mad(R,_sin(disp));
	Device.mProject.transform_tiny(E);

    int radius_pixels = iFloor(0.5f + _abs(E.x)*Device.fWidth_2);
	clamp(radius_pixels, min_radius, max_radius);

	target_radius = radius_pixels; 
}

void CHUDCrosshair::OnRender ()
{
	RCache.OnFrameEnd();

	// draw back
	u32			dwOffset,dwCount;
	FVF::TL0uv* pv_start				= (FVF::TL0uv*)RCache.Vertex.Lock(8,hGeomLine->vb_stride,dwOffset);
	FVF::TL0uv* pv						= pv_start;
	
	u32 color = cross_color.get			();

	int x_min = min_radius + radius		;
	int x_max = x_min + cross_length	;

	int y_min = x_min;
	int y_max = x_max;


	//int y_min = iFloor(0.5f + float(x_min)*Device.fASPECT);
	//int y_max = iFloor(0.5f + float(x_max)*Device.fASPECT);

	// 0
	pv->set					(center.x,center.y + y_min, color); pv++;
	pv->set					(center.x,center.y + y_max, color); pv++;
	// 1
	pv->set					(center.x,center.y - y_min , color); pv++;
	pv->set					(center.x,center.y - y_max, color); pv++;
	// 2
	pv->set					(center.x + x_min, center.y, color); pv++;
	pv->set					(center.x + x_max, center.y, color); pv++;
	// 3
	pv->set					(center.x - x_min, center.y, color); pv++;
	pv->set					(center.x - x_max, center.y, color); pv++;

	// render	
	dwCount 				= u32(pv-pv_start);
	RCache.Vertex.Unlock	(dwCount,hGeomLine->vb_stride);

	//RCache.set_Shader		(hShader);
	RCache.set_Geometry		(hGeomLine);
	RCache.Render	   		(D3DPT_LINELIST,dwOffset,dwCount/2);

	
	if(target_radius - radius>radius_speed)
		radius += radius_speed;
	else if(radius - target_radius >radius_speed)
		radius -= radius_speed;
	else 
		radius = target_radius;
}