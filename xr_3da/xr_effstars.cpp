#include "stdafx.h"
#pragma hdrstop

#include "xr_effstars.h"
#include "xr_effects.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "environment.h"
#include "CustomActor.h"

CEffectStars::CEffectStars	( )
{
	stars			= NULL;
	rnd_value		= NULL;
	opacity			= NULL;

// инициализируемые переменные
	stars_count		= 250;	// кол-во объектов
	vis_radius		= 1000;	// видимость в метрах

	srand			( timeGetTime() );

	matTranslate.identity( );
	matTranslate.m [0][0] = -1;
}

CEffectStars::~CEffectStars	( ) {
	delete			[] stars;
	delete			[] rnd_value;
	delete			[] opacity;
}

void CEffectStars::GenerateStars	( ) {
	/*
	float     		dist, s_a, x, y, z, c1;
	float     		v_r2 = vis_radius*vis_radius;
	float     		t_r  = pCreator->pEnvironment->fTemperature / 100;

	for ( DWORD i = 0; i < stars_count; i++ ){
		c1			= rnd();
		dist		= c1 * vis_radius;
		s_a			= PI_MUL_2*rnd();

		x			= (dist - 1) * cosf(s_a);
		z			= (dist - 1) * sinf(s_a);
		y			= sqrtf(v_r2 - x*x - z*z) / 5;

		stars[i].x	= x;
		stars[i].y	= y;
		stars[i].z	= z;
		opacity[i]	= sqrtf(1-c1)*rnd()*0.85f + 0.15f;
		stars[i].color.set(0xFF, 0xFF, 0xFF, BYTE(255 * opacity[i]) {;

		rnd_value[i]=  t_r * rnd();
	}
	*/
}


BOOL CEffectStars::Load		( LPSTR section, void* data ) {
// Read data
	stars_count		= pCreator->pIniLevel->ReadDWORD ( section, "stars_count" );

// Generate stars
	stars			= new FLvertex	[stars_count];
	rnd_value		= new float     [stars_count];
	opacity			= new float		[stars_count];

  	GenerateStars	( );

	_OK;
}


void CEffectStars::OnMove	( ) {
// расчет координат
	Fvector			pos;
	pos.set			( pCreator->GetPlayer()->basic_pos );
	pos.y			+= pCreator->Effects.deltaY;

	matTranslate.m [3][0] = pos.x;
	matTranslate.m [3][1] = pos.y;
	matTranslate.m [3][2] = pos.z;

	float     	op;
	DWORD		d	= DWORD( rnd() * stars_count );
	DWORD		offs;

	for (DWORD i = 0; i < stars_count; i++){
		offs		= (i^d);
		if ( offs >= stars_count ) offs = offs - stars_count;
		op			= 1 - pCreator->Environment.fDayLight - rnd_value[offs];
		if( op < 0 ) op	  = 0.01f;
		op			= opacity[i] * op;
//		stars[i].color	  = (stars[i].color & 0x00ffffff) | (BYTE(op) << 24);
		stars[i].color.set((DWORD)D3DRGBA(op, op, op, op));
	}
}

void CEffectStars::RenderAlpha( ) {
	// установим параметры рендера
//	HW.pDevice->SetTransform	( D3DTRANSFORMSTATE_WORLD, matTranslate.d3d() );
//	Device.Texture.Set			(0);
//	HW.pDevice->DrawPrimitive	( D3DPT_POINTLIST, FLVERTEX, stars, stars_count, NULL);
}

void CEffectStars::Update( ) {
}
