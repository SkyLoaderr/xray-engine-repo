//----------------------------------------------------
// file: Event.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Event.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "frustum.h"
#include "cl_intersect.h"
#include "texture.h"
#include "D3DUtils.h"

#define EVENT_VERSION		   				0x0010
//----------------------------------------------------
#define EVENT_CHUNK_VERSION					0x0310
#define EVENT_CHUNK_TYPE					0x0311
#define EVENT_PARAMS                        0x0312
#define EVENT_CHUNK_BOX						0x0313
#define EVENT_CHUNK_EXECUTE_ONCE			0x0314
//----------------------------------------------------

CEvent::CEvent( char *name ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

CEvent::CEvent():SceneObject(){
	Construct();
}

CEvent::~CEvent(){
}

void CEvent::Construct(){
	m_ClassID = OBJCLASS_EVENT;
    eEventType= eetBox;

	mTransform.identity();
    vScale.set(1,1,1);
    vRotate.set(0,0,0);
    vPosition.set(0,0,0);

    bExecuteOnce = true;
}
//------------------------------------------------------------------------------------------------
void CEvent::UpdateTransform(){
    // update transform matrix
	Fmatrix	mScale,mTranslate,mRotate;

	mRotate.setHPB			(vRotate.y, vRotate.x, vRotate.z);

	mScale.scale			(vScale);
	mTranslate.translate	(vPosition);
	mTransform.mul			(mTranslate,mRotate);
	mTransform.mul			(mScale);
}

bool CEvent::GetBox( Fbox& box ){
	box.min.set(-0.5f,-0.5f,-0.5f);
	box.max.set( 0.5f, 0.5f, 0.5f);
    box.transform(mTransform);
	return true;
}

void CEvent::RenderBox(bool bAlpha ){
	FVF::L v;
	FLvertexVec V;
	DWORD C;
	if (bAlpha){
        C=RGBA_MAKE( 0, 255, 0, 96 );
//		UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
        UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,mTransform);
	    UI->Device.Shader.Set(UI->Device.m_SelectionShader);
        DU::DrawIdentBox(false,&C);
//		UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
    }else{
        C=RGBA_MAKE( 32, 32, 32, 255 );
        UI->Device.RenderNearer(0.0003);
        UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,mTransform);
	    UI->Device.Shader.Set(UI->Device.m_WireShader);
        UI->Device.ResetNearer();
        DU::DrawIdentBox(true,&C);
        if(Selected()){
            Fbox bb; 
			bb.min.set(-0.5f,-0.5f,-0.5f);
			bb.max.set( 0.5f, 0.5f, 0.5f);
			DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
			DU::DrawSelectionBox(bb,&clr);
        }
    }
}

void CEvent::Render( Fmatrix& parent, ERenderPriority flag ){
	switch(eEventType){
    case eetBox: RenderBox(flag!=rpNormal); break;
    }
}

typedef BYTE T_IDX[3];
static const T_IDX idx[12]={{0,4,3},{4,7,3},{7,6,2},{7,2,3},{4,5,6},{6,7,4},
							{6,5,1},{6,1,2},{5,4,0},{5,0,1},{0,3,2},{0,2,1}};
                            
void GetBoxTri(int index, Fvector* v){
	box_identity.getpoint(idx[index][0],v[0]);
	box_identity.getpoint(idx[index][1],v[1]);
	box_identity.getpoint(idx[index][2],v[2]);
}

bool CEvent::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
	if (eEventType==eetBox){
        Fvector v[3];
        for (int i=0; i<12; i++){
            GetBoxTri(i,v);
            mTransform.transform_tiny(v[0]);
            mTransform.transform_tiny(v[1]);
            mTransform.transform_tiny(v[2]);
            sPoly s(v,3);
            if (frustum.testPoly(s)) return true;
        }
    }
    return false;
}

bool CEvent::RayPick(float& distance, Fvector& start, Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf){
    float range;
    bool bPick=false;

	if (eEventType==eetBox){
        Fvector v[3];
        for (int i=0; i<12; i++){
            GetBoxTri(i,v);
            mTransform.transform_tiny(v[0]);
            mTransform.transform_tiny(v[1]);
            mTransform.transform_tiny(v[2]);
            range=UI->ZFar();
            if (RAPID::TestRayTri2(start,direction,v,range)){
                if ((range>=0)&&(range<distance)){
                    distance=range;
                    bPick=true;
                }
            }
        }
    }
    return bPick;
}

void CEvent::Move(Fvector& amount){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	vPosition.add( amount );
    UpdateTransform();
}

void CEvent::Rotate(Fvector& center, Fvector& axis, float angle){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();     

	Fmatrix m;
	m.rotation( axis, angle );

	vPosition.sub( center );
    m.transform_tiny(vPosition);
	vPosition.add( center );

    vRotate.direct(vRotate,axis,angle);

    UpdateTransform();
}

void CEvent::LocalRotate(Fvector& axis, float angle){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
    vRotate.direct(vRotate,axis,angle);
    UpdateTransform();
}

void CEvent::Scale( Fvector& center, Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	vScale.add(amount);
    if (vScale.x<EPS) vScale.x=EPS;
    if (vScale.y<EPS) vScale.y=EPS;
    if (vScale.z<EPS) vScale.z=EPS;
    UpdateTransform();
}

void CEvent::LocalScale( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	vScale.add(amount);
    if (vScale.x<EPS) vScale.x=EPS;
    if (vScale.y<EPS) vScale.y=EPS;
    if (vScale.z<EPS) vScale.z=EPS;
    UpdateTransform();
}
//----------------------------------------------------

bool CEvent::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(EVENT_CHUNK_VERSION,&version));
    if( version!=EVENT_VERSION ){
        Log->DlgMsg( mtError, "Event: Unsupported version.");
        return false;
    }

	SceneObject::Load(F);

    R_ASSERT(F.ReadChunk(EVENT_CHUNK_TYPE,&eEventType));
    
    switch(eEventType){
    case eetBox:
	    R_ASSERT(F.FindChunk(EVENT_CHUNK_BOX));
    	F.Rvector(vPosition);
	    F.Rvector(vRotate);
    	F.Rvector(vScale);
    break;
    }
    
    char buf[4096];
    R_ASSERT(F.FindChunk(EVENT_PARAMS));
    F.RstringZ		(buf); sTargetClass = buf;
    F.RstringZ		(buf); sOnEnter 	= buf;
    F.RstringZ		(buf); sOnExit 		= buf;

    if(F.FindChunk(EVENT_CHUNK_EXECUTE_ONCE))
		bExecuteOnce = F.Rdword();

    UpdateTransform();
    return true;
}

void CEvent::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(EVENT_CHUNK_VERSION);
	F.Wword			(EVENT_VERSION);
	F.close_chunk	();

    F.write_chunk	(EVENT_CHUNK_TYPE,&eEventType,sizeof(EEventType));
    
    switch(eEventType){
    case eetBox:
	    F.open_chunk(EVENT_CHUNK_BOX);
    	F.Wvector(vPosition);
	    F.Wvector(vRotate);
    	F.Wvector(vScale);
		F.close_chunk	();
    break;
    }

    F.open_chunk	(EVENT_PARAMS);
    F.WstringZ		(sTargetClass.c_str());
    F.WstringZ		(sOnEnter.c_str());
    F.WstringZ		(sOnExit.c_str());
	F.close_chunk	();

	F.open_chunk	(EVENT_CHUNK_EXECUTE_ONCE);
    F.Wdword		(bExecuteOnce);
	F.close_chunk	();
}
//----------------------------------------------------

