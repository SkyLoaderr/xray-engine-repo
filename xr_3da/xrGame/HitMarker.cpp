#include "stdafx.h"
#include "HitMarker.h"

static Fvector2	as_PC[5];
static Fvector2	as_TC[5];
const static DWORD		as_id[4*3] = {0,1,4,  1,2,4,  2,3,4,  3,0,4};

//--------------------------------------------------------------------
CHitMarker::CHitMarker(){
	hShader			= Device.Shader.Create("hud_hitmarker","hud_hitmarker");
	ZeroMemory		(fHitMarks,sizeof(float)*4);
	VS				= Device.Streams.Create(FVF::F_TL,4*3);
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
} 
//--------------------------------------------------------------------

CHitMarker::~CHitMarker(){
	VS				= 0;
	Device.Shader.Delete(hShader);
} 
//--------------------------------------------------------------------

const static float fShowTime = 0.2f;
void CHitMarker::Render()
{
	if (fHitMarks[0]>0 || fHitMarks[1]>0 || fHitMarks[2]>0 || fHitMarks[3]>0)	
	{
		DWORD			dwOffset;
		FVF::TL* D		= (FVF::TL*)VS->Lock(12,dwOffset);
		FVF::TL* Start	= D;
		for (int i=0; i<4; i++){
			if (fHitMarks[i]>0){
				DWORD Alpha = iFloor((fHitMarks[i]/float(fShowTime))*255.f);
				DWORD Color = D3DCOLOR_RGBA(255,255,255,Alpha);
				const DWORD* idx = as_id+i*3;
				
				const Fvector2& P1 = as_PC[idx[0]]; const Fvector2& T1 = as_TC[idx[0]]; D->set(Device._x2real(P1.x),Device._y2real(P1.y),.0001f,.9999f, Color, T1.x, T1.y); D++;
				const Fvector2& P2 = as_PC[idx[1]]; const Fvector2& T2 = as_TC[idx[1]]; D->set(Device._x2real(P2.x),Device._y2real(P2.y),.0001f,.9999f, Color, T2.x, T2.y); D++;
				const Fvector2& P3 = as_PC[idx[2]]; const Fvector2& T3 = as_TC[idx[2]]; D->set(Device._x2real(P3.x),Device._y2real(P3.y),.0001f,.9999f, Color, T3.x, T3.y); D++;
				fHitMarks[i] -= Device.fTimeDelta;
			}
		}
		DWORD Count = D-Start;
		VS->Unlock(Count);
		Device.Shader.Set		(hShader);
		Device.Primitive.Draw	(VS,Count/3,dwOffset);
	}
}
//--------------------------------------------------------------------

void CHitMarker::Hit(int id){
	VERIFY((id>=0)&&(id<4));
	fHitMarks[id] = fShowTime;
}
//--------------------------------------------------------------------
