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

// for static - selects from dynamic lights
void CLightsController::Select(Fvector &pos, float fRadius)
{
//	for (vecI_it it=Selected.begin(); it!=Selected.end(); it++)
//		Disable(*it);
	for (vecI_it it=SelectedDynamic.begin(); it!=SelectedDynamic.end(); it++)
	{
		int		num = *it;
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
		/*
		if (Lights[i].flags&XRLIGHT_PROCEDURAL) {
			DWORD first_key = Lights[i].p_key_start; 
			R_ASSERT(first_key<Keyframes.size());
			CopyMemory(
				&(Lights[i]),
				&Keyframes[first_key],
				sizeof(Flight));
		}
		*/

		Lights[i].specular.set(Lights[i].diffuse);
		Lights[i].specular.mul_rgb(0.2f);
		CHK_DX(HW.pDevice->SetLight(i, Lights[i].d3d()) );
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
		if (::Render.ViewBase.testSphereDirty(L.position, L.range))
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
		
		if ((T.type == D3DLIGHT_DIRECTIONAL)||(::Render.View->visibleSphereNC(T.position, T.range))) 
		{
			/*
			if (T.flags&XRLIGHT_PROCEDURAL)
			{
				SelectedProcedural.push_back(ID);
				T.interpolate(Device.fTimeDelta,Keyframes.begin());
				CHK_DX(HW.pDevice->SetLight(ID,T.d3d()));
			} else {
				Selected.push_back(ID);
			}
			*/
			Selected.push_back(ID);
			T.dwFrame=Device.dwFrame;
		}
	}
};

void CLightsController::Render(void) 
{
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
