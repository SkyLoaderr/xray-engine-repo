// CDemoPlay.cpp: implementation of the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"

#include "fdemoplay.h"
#include "xr_ioconsole.h"
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
	strcpy		(nm,name);	if (strext(nm))	strcpy(strext(nm),".anm");
	if (FS.exist(fn,"$level$",nm))
	{
		m_pMotion				= xr_new<COMotion> ();
		m_pMotion->LoadMotion	(fn);
		m_MParam.Set			(m_pMotion, true);
	}else{
		if (!FS.exist(name)) {
			g_pGameLevel->Cameras.RemoveEffector(cefDemo);
			return;
		}
		IReader*	fs	= FS.r_open	(name);
		u32 sz			= fs->length();
		R_ASSERT		(sz%sizeof(Fmatrix) == 0);
		
		seq.resize		(sz/sizeof(Fmatrix));
		m_count			= seq.size();
		Memory.mem_copy	(&*seq.begin(),fs->pointer(),sz);
		FS.r_close		(fs);
		Log				("! Total frames: ",m_count);
	}
	stat_Start		();

	Device.PreCache	(50);
}

CDemoPlay::~CDemoPlay()
{
	xr_delete	(m_pMotion);
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
	u32	dwFramesTotal	= Device.dwFrame-dwStartFrame;
	u32	dwTimeTotal		= Device.dwTimeGlobal-dwStartTime;

	Msg("* [DEMO] FPS archivied: %f",float(1000.0 * double(dwFramesTotal)/double(dwTimeTotal)));
}

#define FIX(a) while (a>=m_count) a-=m_count

void spline1( float t, Fvector *p, Fvector *ret )
{
	float     t2  = t * t;
	float     t3  = t2 * t;
	float     m[4];

	ret->x=0.0f;
	ret->y=0.0f;
	ret->z=0.0f;
	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
	m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
	m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
	m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );

	for( int i=0; i<4; i++ )
	{
		ret->x += p[i].x * m[i];
		ret->y += p[i].y * m[i];
		ret->z += p[i].z * m[i];
	}
}

BOOL CDemoPlay::Process(Fvector &P, Fvector &D, Fvector &N, float& fFov, float& fFar, float& fAspect)
{
	if (m_pMotion)
	{
		Fvector R;
		Fmatrix mRotate;
		m_pMotion->_Evaluate(m_MParam.Frame(),P,R);
		if (!m_MParam.Update(Device.fTimeDelta)) fLifeTime-=Device.fTimeDelta;
		if (m_MParam.bWrapped)	{ stat_Stop(); stat_Start(); }
		mRotate.setHPB	(-R.x,-R.y,-R.z);
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
			if (!bCycle)	{ return FALSE; }
			stat_Stop		();
			stat_Start		();
		}
		
		int f1=frame; FIX(f1);
		int f2=f1+1;  FIX(f2);
		int f3=f2+1;  FIX(f3);
		int f4=f3+1;  FIX(f4);
		
		Fmatrix *m1,*m2,*m3,*m4;
		Fvector v[4];
		m1 = (Fmatrix *) &seq[f1];
		m2 = (Fmatrix *) &seq[f2];
		m3 = (Fmatrix *) &seq[f3];
		m4 = (Fmatrix *) &seq[f4];
		
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
	return TRUE;
}
