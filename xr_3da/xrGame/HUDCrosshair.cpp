// HUDCrosshair.cpp:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HUDCrosshair.h"



CHUDCrosshair::CHUDCrosshair()
{
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);

	OnDeviceCreate();

	//вычислить и запомнить центр экрана
	center.set(int(Device.dwWidth)/2,int(Device.dwHeight)/2);
	radius = 0;
}


CHUDCrosshair::~CHUDCrosshair()
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);


	OnDeviceDestroy();
}

void CHUDCrosshair::OnDeviceCreate()
{
	hGeomLine.create			(FVF::F_TL0uv,RCache.Vertex.Buffer(),0);
	hShader.create				("hud\\cursor","ui\\cursor");
}

void CHUDCrosshair::OnDeviceDestroy()
{
	hGeomLine.destroy				();
	hShader.destroy					();
}

void CHUDCrosshair::Load ()
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


	disp_scale = pSettings->r_float (HUD_CURSOR_SECTION, "disp_scale");
}



//выставляет radius от min_radius до max_radius
void CHUDCrosshair::SetDispersion(float disp)
{ 
	float factor = disp/disp_scale;
	target_radius = min_radius + iFloor(0.5f+factor*float(max_radius-min_radius));
}

void CHUDCrosshair::OnRender ()
{
	RCache.OnFrameEnd();

	// draw back
	u32			dwOffset,dwCount;
	FVF::TL0uv* pv_start				= (FVF::TL0uv*)RCache.Vertex.Lock(8,hGeomLine->vb_stride,dwOffset);
	FVF::TL0uv* pv						= pv_start;
	

	u32 color = cross_color.get();

	// base rect
	// 0
	pv->set					(center.x,center.y + min_radius + radius, color); pv++;
	pv->set					(center.x,center.y + min_radius +radius + cross_length, color); pv++;
	// 1
	pv->set					(center.x,center.y - min_radius - radius , color); pv++;
	pv->set					(center.x,center.y - min_radius - radius - cross_length, color); pv++;
	// 2
	pv->set					(center.x + min_radius + radius, center.y, color); pv++;
	pv->set					(center.x + min_radius + radius + cross_length, center.y, color); pv++;
	// 3
	pv->set					(center.x - min_radius - radius, center.y, color); pv++;
	pv->set					(center.x - min_radius - radius - cross_length, center.y, color); pv++;

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