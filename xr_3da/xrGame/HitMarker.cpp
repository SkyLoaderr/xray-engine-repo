// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:12:50 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "HitMarker.h"

static Fvector2			as_PC[5];
static Fvector2			as_TC[5];
const static u32		as_id[4*3] = {0,1,4,  1,2,4,  2,3,4,  3,0,4};

//--------------------------------------------------------------------
CHitMarker::CHitMarker()
{
	ZeroMemory		(fHitMarks,sizeof(float)*4);
	as_PC[0].set	(-0.5f,-0.67f);
	as_PC[1].set	(0.5f,-0.67f);
	as_PC[2].set	(0.5f,0.67f);
	as_PC[3].set	(-0.5f,0.67f);
	as_PC[4].set	(0.f,0.f);
	as_TC[0].set	(0.f,0.f);
	as_TC[1].set	(1.f,0.f);
	as_TC[2].set	(0.f,0.f);
	as_TC[3].set	(1.f,0.f);
	as_TC[4].set	(.5f,1.f);
	hShader			= 0;
	hVS				= 0;
	OnDeviceCreate	();

	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
} 
//--------------------------------------------------------------------
CHitMarker::~CHitMarker()
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);

	hVS				= 0;
	OnDeviceDestroy	();
} 
//--------------------------------------------------------------------

void CHitMarker::OnDeviceCreate()
{
	REQ_CREATE	();
	hShader		= Device.Shader.Create		("hud\\hitmarker","ui\\hud_hitmarker");
	hVS			= Device.Shader.CreateGeom	(FVF::F_TL, RCache.Vertex.Buffer(), NULL);
}
void CHitMarker::OnDeviceDestroy()
{
	Device.Shader.DeleteGeom	(hVS);
	Device.Shader.Delete		(hShader);
}

const static float fShowTime = 0.2f;
void CHitMarker::Render()
{
	if (fHitMarks[0]>0 || fHitMarks[1]>0 || fHitMarks[2]>0 || fHitMarks[3]>0)	
	{
		float		w_2		= float(::Render->getTarget()->get_width())/2;
		float		h_2		= float(::Render->getTarget()->get_height())/2;

		u32			dwOffset;
		FVF::TL* D		= (FVF::TL*)RCache.Vertex.Lock(12,hVS->vb_stride,dwOffset);
		FVF::TL* Start	= D;
		for (int i=0; i<4; i++){
			if (fHitMarks[i]>0){
				u32 Alpha = iFloor((fHitMarks[i]/float(fShowTime))*255.f);
				u32 Color = D3DCOLOR_RGBA(255,255,255,Alpha);
				const u32* idx = as_id+i*3;
				
				const Fvector2& P1 = as_PC[idx[0]]; const Fvector2& T1 = as_TC[idx[0]]; D->set((P1.x+1)*w_2,(P1.y+1)*h_2,.0001f,.9999f, Color, T1.x, T1.y); D++;
				const Fvector2& P2 = as_PC[idx[1]]; const Fvector2& T2 = as_TC[idx[1]]; D->set((P2.x+1)*w_2,(P2.y+1)*h_2,.0001f,.9999f, Color, T2.x, T2.y); D++;
				const Fvector2& P3 = as_PC[idx[2]]; const Fvector2& T3 = as_TC[idx[2]]; D->set((P3.x+1)*w_2,(P3.y+1)*h_2,.0001f,.9999f, Color, T3.x, T3.y); D++;
				fHitMarks[i] -= Device.fTimeDelta;
			}
		}
		u32 Count						= (u32)(D - Start);
		RCache.Vertex.Unlock	(Count,hVS->vb_stride);
		if (Count)
		{
			RCache.set_Shader	(hShader);
			RCache.set_Geometry (hVS);
			RCache.Render		(D3DPT_TRIANGLELIST,dwOffset,Count/3);
		}
	}
}
//--------------------------------------------------------------------

void CHitMarker::Hit(int id){
	VERIFY((id>=0)&&(id<4));
	fHitMarks[id] = fShowTime;
}
//--------------------------------------------------------------------
