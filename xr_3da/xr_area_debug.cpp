#include "stdafx.h"
#include "xr_area.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "xr_creator.h"
#include "customactor.h"

//#define _RENDER_COLISION_DATA
//#define _RENDER_COLISION_GRID
//#define _RENDER_COLISION_FORM
//#define _RENDER_COLISION_ONLY_PLAYER
#ifdef DEBUG
#define _RENDER_BBOX_DATA
//#define _RENDER_TRIS_DATA
#endif

/*
void CObjectSpace::AddToTrisList( const CCFModel* model )
{
	VERIFY					( model );
	if ( model->owner ) {
		Fmatrix *pM = &(model->owner->mTransform);
		for ( int i = 0; i < model->model.num_tris; i++)
			AddToTrisList( pM, model->model.tris[i] );
	}else
		tris_list.append( model->model.tris, model->model.num_tris );
}
*/
//----------------------------------------------------------------------

void CObjectSpace::_report()
{
///------------------------------------------
	/*
	pCreator->xrConsole->SetFontK(0.6f);
	int 				ix, iz;
	for (ix=0;ix<x_count;ix++){
		for (iz=0;iz<z_count;iz++){
			if (slot_list(ix,iz).count) pCreator->xrConsole->Add( float(iz*10), float(ix*10), int (slot_list(ix,iz).count) );
			else pCreator->xrConsole->Add( float(iz*10), float(ix*10), "-" );
		}
	}
	*/
///------------------------------------------
}
//----------------------------------------------------------------------

void CObjectSpace::_render()
{
	/*
#ifdef _RENDER_BBOX_DATA
//---------------------------------------------------------------------------------
// draw OBB box
	DWORD C = 0xffffffff;
	Fmatrix T;
	T.set(debug_last_bb_transform);
	Device.Primitive.dbg_DrawOBB(T, debug_last_bb_size, C);
#endif

#ifdef _RENDER_TRIS_DATA
// draw tris
	DWORD TC = 0xff500000;
	Fmatrix TT;
	TT.identity();
	for (DWORD i=0; i<debug_render_tris_list.count; i++)
		Device.Primitive.dbg_DrawTRI(TT,debug_render_tris_list[i].P,TC);
//---------------------------------------------------------------------------------
#endif

#ifdef _RENDER_COLISION_DATA
///------------------------------------------
//	HW.pDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );
//	HW.pDevice->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME );
	HW.pDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, precalc_identity.d3d() );

	Device.SetMode(vZTest|vZWrite|vAlpha);
	Device.Texture.Set(0);
///------------------------------------------
	FLvertex g[2];
// x
	g[0].set(0,0,0,0xFFFF0000,0,0,0);
	g[1].set(10,0,0,0xFFFF0000,0,0,0);
	Device.Draw.PrimitiveSP(D3DPT_LINELIST, FLVERTEX, g, 2);
// y
	g[0].set(0,0,0,0xFF00FF00,0,0,0);
	g[1].set(0,10,0,0xFF00FF00,0,0,0);
	Device.Draw.PrimitiveSP(D3DPT_LINELIST, FLVERTEX, g, 2);
// z
	g[0].set(0,0,0,0xFF0000FF,0,0,0);
	g[1].set(0,0,10,0xFF0000FF,0,0,0);
	Device.Draw.PrimitiveSP(D3DPT_LINELIST, FLVERTEX, g, 2);

	float x, z;
#ifdef _RENDER_COLISION_GRID
	for (x = 0; x < x_count; x++){
		g[0].x = x*2;
		g[0].y = 0;
		g[0].z = 0;
		g[0].color.set( 0xFF404040 );

		g[1].x = x*2;
		g[1].y = 0;
		g[1].z = float(z_count)*2;
		g[1].color.set( 0xFF404040 );

		Device.Draw.PrimitiveSP(D3DPT_LINELIST, FLVERTEX, g, 2);
	}
	for (z = 0; z < z_count; z++){
		g[0].x = 0;
		g[0].y = 0;
		g[0].z = z*2;
		g[0].color.set( 0xFF404040 );

		g[1].x = float(x_count)*2;
		g[1].y = 0;
		g[1].z = z*2;
		g[1].color.set( 0xFF404040 );

		Device.Draw.PrimitiveSP(D3DPT_LINELIST, FLVERTEX, g, 2);
	}
#endif

	tris_list.Clear();
#ifdef _RENDER_COLISION_FORM
#ifdef _RENDER_COLISION_ONLY_PLAYER
	AddToTrisList( pCreator->GetPlayer()->cfModel, NULL );
#else
	for (x=0;x<x_count;x++)
		for (z=0;z<z_count;z++)
			for (int q=0;q<slot_list(x, z).objects.count;q++){
				CCFModel *target	= slot_list(x, z).objects[q];
				AddToTrisList(target, NULL);
			}
#endif
//
// render cfModel from tris_list
	FLvertex* a;

	int cnt = tris_list.count;
	a = new FLvertex[cnt*3];
	for ( int i = 0; i<cnt; i++ ){
		a[i*3+0].x = tris_list[i].p1[0];	a[i*3+0].y = tris_list[i].p1[1];	a[i*3+0].z = tris_list[i].p1[2];
		a[i*3+1].x = tris_list[i].p2[0];	a[i*3+1].y = tris_list[i].p2[1];	a[i*3+1].z = tris_list[i].p2[2];
		a[i*3+2].x = tris_list[i].p3[0];	a[i*3+2].y = tris_list[i].p3[1];	a[i*3+2].z = tris_list[i].p3[2];
		a[i*3+0].color.set( 0x4500FF00 );	a[i*3+1].color.set( 0x4500FF00 );	a[i*3+2].color.set( 0x4500FF00 );
	}
	Device.Draw.PrimitiveSP(D3DPT_TRIANGLELIST, FLVERTEX, a, cnt*3);
	for ( i = 0; i<cnt; i++ ) {
		a[i*3+0].color.set( 0xffFF0000 );	a[i*3+1].color.set( 0xffFF0000 );	a[i*3+2].color.set( 0xffFF0000 );
	}
	HW.pDevice->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME );
	Device.Draw.PrimitiveSP(D3DPT_TRIANGLELIST, FLVERTEX, a, cnt*3);
	HW.pDevice->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID );


	_DELETEARRAY ( a );
///
//	HW.pDevice->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID );
//	HW.pDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CCW );
#endif
#endif
*/
}
//----------------------------------------------------------------------

