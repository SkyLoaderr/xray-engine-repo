// CDemoPlay.cpp: implementation of the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fdemoplay.h"
#include "xr_ioconsole.h"
#include "xr_creator.h"
#include "Motion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDemoPlay::CDemoPlay(const char *name, float ms, BOOL bc, float life_time) : CEffector(cefDemo,life_time,FALSE)
{
	Msg("! Playing demo: %s",name);
	Console.Execute("hud_weapon 0");
	fStartTime	= 0;
	fSpeed		= ms;
	bCycle      = bc;

	m_pMotion	= 0;
	char		nm[255],fn[255];
	strcpy	(nm,name);	if (strext(nm))	strcpy(strext(nm),".anm");
	if (Engine.FS.Exist(fn,Path.Current,nm))
	{
		m_pMotion	= new COMotion();
		m_pMotion->LoadMotion(fn);
		m_MParam.Set(m_pMotion, true);
	}else{
		if (!Engine.FS.Exist(name)) {
			pCreator->Cameras.RemoveEffector(cefDemo);
			return;
		}
		DWORD sz	= 0;
		void *data	= FileDownload(name,&sz);
		VERIFY(sz%sizeof(Fmatrix) == 0);
		
		seq.resize	(sz/sizeof(Fmatrix));
		m_count		= seq.size();
		PSGP.memCopy(&*seq.begin(),data,sz);
		_FREE		(data);
		Log			("! Total frames: ",m_count);
	}
	stat_Start	();
}

CDemoPlay::~CDemoPlay()
{
	_DELETE(m_pMotion);
	stat_Stop	();
	Console.Execute("hud_weapon 1");
}

void CDemoPlay::stat_Start()
{
	dwStartTime				= Device.dwTimeGlobal;
	dwStartFrame			= Device.dwFrame;
}

void CDemoPlay::stat_Stop()
{
	DWORD	dwFramesTotal	= Device.dwFrame-dwStartFrame;
	DWORD	dwTimeTotal		= Device.dwTimeGlobal-dwStartTime;

	Msg("* [DEMO] FPS archivied: %f",float(1000.0 * double(dwFramesTotal)/double(dwTimeTotal)));
}

#define FIX(a) while (a>=m_count) a-=m_count

extern void spline1(float,Fvector*,Fvector*);
void CDemoPlay::Process(Fvector &P, Fvector &D, Fvector &N)
{
	if (m_pMotion)
	{
		Fvector R;
		Fmatrix mRotate;
		m_pMotion->Evaluate(m_MParam.Frame(),P,R);
		if (!m_MParam.Update(Device.fTimeDelta)) fLifeTime-=Device.fTimeDelta;
		if (m_MParam.bWrapped)	{ stat_Stop(); stat_Start(); }
		mRotate.setHPB	(-R.x,-R.y,R.z);
		D.set			(mRotate.k);
		N.set			(mRotate.j);
	}
	else
	{
		fStartTime		+=	Device.fTimeDelta;
		
		float	ip;
		float	p		=	fStartTime/fSpeed;
		float	t		=	modff(p, &ip);
		int		frame	=	iFloor(ip);
		VERIFY	(t>=0);
		
		if (frame>=m_count)
		{
			if (!bCycle)	{ pCreator->Cameras.RemoveEffector(cefDemo); return; }
			stat_Stop		();
			stat_Start		();
		}
		
		int f1=frame; FIX(f1);
		int f2=f1+1;  FIX(f2);
		int f3=f2+1;  FIX(f3);
		int f4=f3+1;  FIX(f4);
		
		Fmatrix *m1,*m2,*m3,*m4;
		Fvector v[4];
		m1 = (Fmatrix *) seq[f1].d3d();
		m2 = (Fmatrix *) seq[f2].d3d();
		m3 = (Fmatrix *) seq[f3].d3d();
		m4 = (Fmatrix *) seq[f4].d3d();
		
		for (int i=0; i<4; i++) {
			v[0].x = m1->m[i][0]; v[0].y = m1->m[i][1];  v[0].z = m1->m[i][2];
			v[1].x = m2->m[i][0]; v[1].y = m2->m[i][1];  v[1].z = m2->m[i][2];
			v[2].x = m3->m[i][0]; v[2].y = m3->m[i][1];  v[2].z = m3->m[i][2];
			v[3].x = m4->m[i][0]; v[3].y = m4->m[i][1];  v[3].z = m4->m[i][2];
			spline1	( t, &(v[0]), (Fvector *) &(Device.mView.m[i][0]) );
		}
		
		Fmatrix mInvCamera;
		mInvCamera.invert(Device.mView);
		N.set( mInvCamera._21, mInvCamera._22, mInvCamera._23 );
		D.set( mInvCamera._31, mInvCamera._32, mInvCamera._33 );
		P.set( mInvCamera._41, mInvCamera._42, mInvCamera._43 );
		
		fLifeTime-=Device.fTimeDelta;
	}
}
