#include "stdafx.h"
#include "fstaticrender.h"

const float DE_Speed=0.5f;

// Adds dynamic Light - return handle
int CLightsController::Add(xrLIGHT &L)
{
	LightsDynamic.push_back	(L);
	Enabled.push_back		(FALSE);
	Distance.push_back		(0);
	return LightsDynamic.size()-1;
}
// Removes dynamic Light
void CLightsController::Remove(int handle)
{
	for (DWORD j=handle+Lights.size(); j<Enabled.size(); j++) Disable(j);
	LightsDynamic.erase	(LightsDynamic.begin()+handle);
	Enabled.erase		(handle+Lights.size()+Enabled.begin());
	Distance.erase		(handle+Lights.size()+Distance.begin());
}

// Disables all lights
void CLightsController::UnselectAll(void) {
	vecI_it it;
	for (it=Selected.begin(); it!=Selected.end(); it++)
		Disable(*it);
	for (it=SelectedDynamic.begin(); it!=SelectedDynamic.end(); it++)
		Disable(*it);
	for (it=SelectedProcedural.begin(); it!=SelectedProcedural.end(); it++)
		Disable(*it);
//	dwLastLights=SelectedDynamic.size()+Selected.size();

	Selected.clear();
	SelectedDynamic.clear();
}

// for dynamic
void	CLightsController::SelectDynamic(Fvector &pos, float fRadius)
{
	// for all dynamic objects we apply not only dynamic but static lights too.
	for (vecI_it it=Selected.begin(); it!=Selected.end(); it++)
	{
		int		num	= *it;
		xrLIGHT &L	= Lights[num];
		if (L.type == D3DLIGHT_DIRECTIONAL)				Enable(num);
		else {
			float	R	= fRadius+L.range;
			if (pos.distance_to_sqr(L.position) < R*R)	Enable(num);
			else										Disable(num);
		}
	}
	for (it=SelectedDynamic.begin(); it!=SelectedDynamic.end(); it++)
	{
		int		num	= *it;
		xrLIGHT &L	= LightsDynamic[num-Lights.size()];
		float	R	= fRadius+L.range;
		if (pos.distance_to_sqr(L.position) < R*R)	Enable(num);
		else										Disable(num);
	}
	for (it = SelectedProcedural.begin(); it!=SelectedProcedural.end(); it++)
	{
		int		num = *it;
		xrLIGHT &L	= Lights[num];
		float	R	= fRadius+L.range;
		if (pos.distance_to_sqr(L.position) < R*R)	Enable	(num);
		else										Disable	(num);
	}
}

void CLightsController::Load(CStream *fs) 
{
	DWORD size	= fs->Length();
	DWORD count	= size/sizeof(xrLIGHT);
	R_ASSERT(count*sizeof(xrLIGHT) == size);
	Lights.resize(count);
	fs->Read(Lights.begin(),size);

	Distance.resize	(count+LightsDynamic.size());
	Enabled.resize	(count+LightsDynamic.size());

	for (DWORD i=0; i<count; i++) 
	{
		Lights[i].specular.set		(Lights[i].diffuse);
		Lights[i].specular.mul_rgb	(0.2f);
		CHK_DX(HW.pDevice->SetLight	(i, Lights[i].d3d()) );
		Enabled[i]=FALSE;
	}
	Log("* Total number of light sources on level: ",count);
}

void CLightsController::Unload(void)
{
	for (DWORD i=0; i<Lights.size(); i++) Disable(i);

	Lights.clear			();
	Distance.clear			();
	Enabled.clear			();
}

IC BOOL lights_compare(int a, int b) {
	CLightsController &L = Render.Lights;
	return L.Distance[a]<L.Distance[b];
}
IC float spline(float *m, float p0, float p1, float p2, float p3)
{	return p0 * m[0] + p1 * m[1] + p2 * m[2] + p3 * m[3]; }

IC void t_spline(float t, float *m) 
{
	float     t2  = t * t;
	float     t3  = t2 * t;
	
	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
	m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
	m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
	m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );
}
void CLightsController::BuildSelection()
{
	DWORD	i;

	// ***** dynamic
	// first
	for (i=0; i<LightsDynamic.size(); i++) {
		xrLIGHT &L = LightsDynamic[i];
		if (::Render.ViewBase.testSphere_dirty(L.position, L.range))
		{
			DWORD idx=i+Lights.size();
			Distance[idx]=iFloor(1000.0f*Device.vCameraPosition.distance_to_sqr(L.position));
			SelectedDynamic.push_back(idx);
			CHK_DX(HW.pDevice->SetLight(idx,L.d3d()));
		} else {
			Disable(i+Lights.size());
		}
	}
	std::sort(
		SelectedDynamic.begin(),
		SelectedDynamic.end(),
		lights_compare);
}

void	CLightsController::add_sector_lights(vector<WORD> &L)
{
	for (vector<WORD>::iterator I=L.begin(); I!=L.end(); I++)
	{
		WORD ID		= *I;
		xrLIGHT&  T	= Lights[ID];
		if (T.dwFrame==Device.dwFrame) continue;
		
		if ((T.type == D3DLIGHT_DIRECTIONAL)||(::Render.View->testSphere_dirty	(T.position, T.range))) 
		{
			Selected.push_back(ID);
			T.dwFrame=Device.dwFrame;
		}
	}
};

