#include "stdafx.h"
#include "fstaticrender.h"

// Adds dynamic Light - return handle
int FLightsController::Add(Flight &L)
{
	LightsDynamic.push_back	(L);
	Enabled.push_back		(FALSE);
	Distance.push_back		(0);
	return LightsDynamic.size()-1;
}
// Removes dynamic Light
void FLightsController::Remove(int handle)
{
	for (int j=handle+Lights.size(); j<Enabled.size(); j++) Disable(j);
	LightsDynamic.erase	(LightsDynamic.begin()+handle);
	Enabled.erase		(handle+Lights.size()+Enabled.begin());
	Distance.erase		(handle+Lights.size()+Distance.begin());
}

// Disables all lights
void FLightsController::UnselectAll(void) {
	vecIIT it;
	for (it=Selected.begin(); it!=Selected.end(); it++)
		Disable(*it);
	for (it=SelectedDynamic.begin(); it!=SelectedDynamic.end(); it++)
		Disable(*it);
	dwLastLights=SelectedDynamic.size()+Selected.size();

	Selected.clear();
	SelectedDynamic.clear();
}

// for static - selects from dynamic lights
void FLightsController::Select(Fvector &pos, float fRadius)
{
	for (vecIIT it=Selected.begin(); it!=Selected.end(); it++)
		Disable(*it);
	for (it=SelectedDynamic.begin(); it!=SelectedDynamic.end(); it++)
	{
		int		num = *it;
		Flight &L	= LightsDynamic[num-Lights.size()];
		float	R	= fRadius+L.range;
		if (pos.distance_to_sqr(L.position) < R*R)	Enable(num);
		else										Disable(num);
	}
}

// for dynamic
void	FLightsController::SelectDynamic(Fvector &pos, float fRadius)
{
	// for all dynamic objects we apply not only dynamic but static lights too.
	for (vecIIT it=Selected.begin(); it!=Selected.end(); it++)
	{
		int		num	= *it;
		Flight &L	= Lights[num];
		float	R	= fRadius+L.range;
		if (pos.distance_to_sqr(L.position) < R*R)	Enable(num);
		else										Disable(num);
	}
	for (it=SelectedDynamic.begin(); it!=SelectedDynamic.end(); it++)
	{
		int		num	= *it;
		Flight &L	= LightsDynamic[num-Lights.size()];
		float	R	= fRadius+L.range;
		if (pos.distance_to_sqr(L.position) < R*R)	Enable(num);
		else										Disable(num);
	}
}

void FLightsController::Load(CStream *fs) {
	DWORD size	= fs->Length();
	DWORD count	= size/sizeof(Flight);
	R_ASSERT(count*sizeof(Flight) == size);
	Lights.resize(count);
	fs->Read(Lights.begin(),size);

	Distance.resize	(count+LightsDynamic.size());
	Enabled.resize	(count+LightsDynamic.size());

	for (DWORD i=0; i<count; i++) {
		Lights[i].specular.set(Lights[i].diffuse);
		Lights[i].specular.mul_rgb(0.2f);
		CHK_DX(HW.pDevice->SetLight(i, Lights[i].d3d()) );
		Enabled[i]=FALSE;
	}
	Selected.clear();
	Log("* Total number of light sources on level: ",count);
}

void FLightsController::Unload(void)
{
	for (int i=0; i<Lights.size(); i++) Disable(i);

	Lights.clear			();
	Distance.clear			();
	Enabled.clear			();
	Selected.clear			();
}

IC bool lights_compare(int a, int b) {
	FLightsController &L = pRender->Lights;
	return L.Distance[a]<L.Distance[b];
}
void FLightsController::BuildSelection()
{
	int		i;
	// ***** dynamic
	// first
	for (i=0; i<LightsDynamic.size(); i++) {
		Flight &L = LightsDynamic[i];
		if (pRender->ViewBase.testSphereDirty(L.position, L.range))
		{
			DWORD idx = i+Lights.size();
			Distance[idx]=iROUND(1000.0f*Device.vCameraPosition.distance_to_sqr(L.position));
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

void FLightsController::Render(void) {
/*
ID3DXSimpleShape *	Sphere;
int					i;
Fmatrix				matrix;
Fmaterial			material;

  Device.SetMode(vZTest);
  CHK_DX(D3DXCreateSphere(HW.pDevice,1.f,D3DX_DEFAULT,D3DX_DEFAULT,1,&Sphere));
  Device.Texture.Set(0);
  CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_FILLMODE,		D3DFILL_WIREFRAME	));
  material.Set(1,1,1,1);
  //	CHK_DX(HW.pDevice->SetMaterial(material.d3d()));
  CHK_DX(HW.pDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE));
  for (i=0; i<Selected.count; i++) {
		// color
		CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_AMBIENT, Lights[Selected[i]].diffuse.get_ideal() ));

		  // translate & scale
		  matrix.translate(Lights[Selected[i]].position);
		  matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = Lights[Selected[i]].range;
		  CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, matrix.d3d()));

			// render
			Sphere->Draw();
			}
			for (i=0; i<SelectedDynamic.count; i++) {
			// color
			CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_AMBIENT, LightsDynamic[SelectedDynamic[i]-Lights.count]->diffuse.get_ideal() ));

			  // translate & scale
			  matrix.translate(LightsDynamic[SelectedDynamic[i]-Lights.count]->position);
			  matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = LightsDynamic[SelectedDynamic[i]-Lights.count]->range;
			  CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, matrix.d3d()));

				// render
				Sphere->Draw();
				}
				if (RM.dwRenderState&rsWireframe)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_FILLMODE,		D3DFILL_WIREFRAME	)); }
				else								{ CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_FILLMODE,		D3DFILL_SOLID		)); }
				CHK_DX(HW.pDevice->SetRenderState	( D3DRENDERSTATE_AMBIENT,	0			));
				CHK_DX(HW.pDevice->SetRenderState	( D3DRENDERSTATE_CULLMODE,D3DCULL_CCW	));
				_RELEASE(Sphere);
	*/
}
