//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop
#include "ObjectPreview.h"
#include "Library.h"
#include "SceneObject.h"
#include "UI_Main.h"

//---------------------------------------------------------------------------
__fastcall TObjectPreview::TObjectPreview(bool CreateSuspended, TPanel* p): TThread(CreateSuspended)
{
    Priority = tpNormal;
    FreeOnTerminate = true;
    object = 0;
    if( FAILED( InitD3DX(p->Handle, &m_DX))){
		Log.Msg( "D3D: DirectDrawCreateEx() failed" );
    }

	m_Camera.identity();
    m_World.identity();
    ResizeD3DX( p->Width, p->Height );
    float m_Aspect = (float)p->Height / (float)p->Width;
	float m_FOV = deg2rad( 60.f );
	float m_Znear = 0.1f;
	float m_Zfar = 500.f;
    m_Projection.projection( m_FOV, m_Aspect, m_Znear, m_Zfar );
    CDX( m_DX->pD3DDev->SetTransform(D3DTRANSFORMSTATE_PROJECTION,(LPD3DMATRIX)m_Projection.a) );
    CDX( m_DX->pD3DDev->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)m_World.a) );
}
//---------------------------------------------------------------------------
__fastcall TObjectPreview::~TObjectPreview()
{
    ReleaseD3DX();
}
//---------------------------------------------------------------------------
void _fastcall TObjectPreview::SelectObject(SceneObject* obj)
{
    object = obj;
    object->GetBox(bbox);
    float dz = bbox.vmin.z+bbox.vmax.z-bbox.vmin.z;
    IVector vmove;
    vmove.set( 0,0,-1 );
    vmove.safe_normalize();
    vmove.mul( dz );
	m_Camera.identity();
    m_Camera.c.add( vmove );
}
//---------------------------------------------------------------------------
void __fastcall TObjectPreview::Execute()
{
    do{
        if (object){
            BeginDraw();

            IMatrix viewmatrix;
            viewmatrix.invert( m_Camera );
            CDX( m_DX->pD3DDev->SetTransform(D3DTRANSFORMSTATE_VIEW,(LPD3DMATRIX)viewmatrix.a) );
            IMatrix mat;
            mat.identity();
            object->Render(mat);
            Sleep(25);

            EndDraw();
        }
    }while (!Terminated);
  // ---- Place thread code here ----
}
//---------------------------------------------------------------------------
