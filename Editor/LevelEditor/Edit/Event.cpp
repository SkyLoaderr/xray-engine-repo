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

#define EVENT_VERSION		   				0x0011
//----------------------------------------------------
#define EVENT_CHUNK_VERSION					0x0310
#define EVENT_CHUNK_FORMS					0x0311
#define EVENT_CHUNK_ACTIONS					0x0312
//----------------------------------------------------
typedef BYTE T_IDX[3];
static const T_IDX idx[12]={{0,4,3},{4,7,3},{7,6,2},{7,2,3},{4,5,6},{6,7,4},
							{6,5,1},{6,1,2},{5,4,0},{5,0,1},{0,3,2},{0,2,1}};

void GetBoxTri(int index, Fvector* v){
	box_identity.getpoint(idx[index][0],v[0]);
	box_identity.getpoint(idx[index][1],v[1]);
	box_identity.getpoint(idx[index][2],v[2]);
}
//----------------------------------------------------
void CEvent::SForm::GetTransform(Fmatrix& M){
    // update transform matrix
	Fmatrix	mScale,mTranslate,mRotate;
	mRotate.setHPB	(vRotate.y, vRotate.x, vRotate.z);
	mScale.scale	(vSize);
	mTranslate.translate(vPosition);
	M.mul			(mTranslate,mRotate);
	M.mul			(mScale);
}

void CEvent::SForm::RenderBox(bool bAlpha){
	Fmatrix T; GetTransform(T);
	// render
	FVF::L v;
	FLvertexVec V;
	DWORD C;
	if (bAlpha){
        C=D3DCOLOR_RGBA( 0, 255, 0, m_Selected?48:24 );
		Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
        Device.SetTransform(D3DTS_WORLD,T);
	    Device.SetShader(Device.m_SelectionShader);
        DU::DrawIdentBox(true,false,&C);
		Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
    }else{
        C=D3DCOLOR_RGBA( 32, 32, 32, 255 );
        Device.RenderNearer(0.0003);
        Device.SetTransform(D3DTS_WORLD,T);
	    Device.SetShader(Device.m_SelectionShader);
        Device.ResetNearer();
        DU::DrawIdentBox(false,true,&C);
    }
}

void CEvent::SForm::Render(bool bAlpha)
{
	switch(m_eType){
    case efBox: RenderBox(bAlpha); break;
    };
}

void CEvent::SForm::GetBox(Fbox& bb)
{
	Fmatrix T; 		GetTransform(T);
	bb.min.set		(-0.5f,-0.5f,-0.5f);
	bb.max.set		( 0.5f, 0.5f, 0.5f);
    bb.transform	(T);
}

bool CEvent::SForm::Pick(float& distance, Fvector& start, Fvector& direction)
{
	bool bPick=false;
    float range;
	if (m_eType==efBox){
		Fmatrix T; 	GetTransform(T);
        Fvector v[3];
        for (int i=0; i<12; i++){
            GetBoxTri(i,v);
            T.transform_tiny(v[0]);
            T.transform_tiny(v[1]);
            T.transform_tiny(v[2]);
            range=UI.ZFar();
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

bool CEvent::SForm::FrustumPick( const CFrustum& frustum )
{
	if (m_eType==efBox){
		Fmatrix T; 	GetTransform(T);
        Fvector v[3];
        for (int i=0; i<12; i++){
            GetBoxTri(i,v);
            T.transform_tiny(v[0]);
            T.transform_tiny(v[1]);
            T.transform_tiny(v[2]);
            sPoly s(v,3);
            if (frustum.testPoly(s)) return true;
        }
    }
}

void CEvent::SForm::Move( Fvector& amount )
{
	vPosition.add(amount);
}

void CEvent::SForm::Rotate( Fvector& center, Fvector& axis, float angle )
{
	Fmatrix m;
	m.rotation( axis, angle );

	vPosition.sub( center );
    m.transform_tiny(vPosition);
	vPosition.add( center );

    vRotate.direct(vRotate,axis,angle);
}

void CEvent::SForm::Scale( Fvector& center, Fvector& amount )
{
	vSize.add(amount);
    if (vSize.x<EPS) vSize.x=EPS;
    if (vSize.y<EPS) vSize.y=EPS;
    if (vSize.z<EPS) vSize.z=EPS;
}

void CEvent::SForm::LocalRotate( Fvector& axis, float angle )
{
    vRotate.direct(vRotate,axis,angle);
}

void CEvent::SForm::LocalScale( Fvector& amount )
{
	vSize.add(amount);
    if (vSize.x<EPS) vSize.x=EPS;
    if (vSize.y<EPS) vSize.y=EPS;
    if (vSize.z<EPS) vSize.z=EPS;
}
//------------------------------------------------------------------------------

CEvent::CEvent( char *name ):CCustomObject(){
	Construct();
	strcpy( m_Name, name );
}

CEvent::CEvent():CCustomObject(){
	Construct();
}

CEvent::~CEvent(){
}

void CEvent::Construct(){
	m_ClassID 		= OBJCLASS_EVENT;
    m_Actions.clear	();
    m_Forms.clear	();
    // create one box
    AppendForm		(efBox);
}

void CEvent::AppendForm(EFormType type)
{
	m_Forms.push_back(SForm());
    SForm& F 	= m_Forms.back();
    F.m_eType   = type;
    F.vSize.set	(1.f,1.f,1.f);
    F.vRotate.set(0.f,0.f,0.f);
	F.vPosition.set(0.f,0.f,0.f);
    F.m_Selected= false;
}

void CEvent::RemoveSelectedForm()
{
	for (int i=0; i<m_Forms.size(); i++)
    	if (m_Forms[i].m_Selected){
			m_Forms.erase(m_Forms.begin()+i);
            i--;
        }
    // remove object from scene if empty?
    //S
}

//------------------------------------------------------------------------------------------------
/*
void CEvent::UpdateTransform(){
    // update transform matrix
	Fmatrix	mScale,mTranslate,mRotate;

	mRotate.setHPB	(vRotate.y, vRotate.x, vRotate.z);

	mScale.scale	(vScale);
	mTranslate.translate(vPosition);
	mTransform.mul	(mTranslate,mRotate);
	mTransform.mul	(mScale);
}
*/

bool CEvent::GetBox( Fbox& box ){
	// calc for all forms
    box.invalidate();
    Fbox bb;
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++){
    	it->GetBox(bb);
        box.merge(bb);
    }
	return true;
}

void CEvent::Render( int priority, bool strictB2F ){
	if (priority==1){
        for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->Render(strictB2F);
        if(Selected()&&(false==strictB2F)){
            Fbox bb;
            GetBox(bb);
			DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
			Device.SetTransform(D3DTS_WORLD,precalc_identity);
			DU::DrawSelectionBox(bb,&clr);
        }
    }
}

bool CEvent::FrustumPick(const CFrustum& frustum){
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++)
    	if (it->FrustumPick(frustum)) return true;
    return false;
}

bool CEvent::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
    bool bPick=false;
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++)
    	if (it->Pick(distance,start,direction)) bPick=true;
    return bPick;
}

void CEvent::Move(Fvector& amount){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    //S
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->Move(amount);
    UI.UpdateScene();
}

void CEvent::Rotate(Fvector& center, Fvector& axis, float angle){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    //S
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->Rotate(center, axis, angle);
    UI.UpdateScene();
}

void CEvent::LocalRotate(Fvector& axis, float angle){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->LocalRotate(axis, angle);
    UI.UpdateScene();
}

void CEvent::Scale( Fvector& center, Fvector& amount ){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->Scale(center, amount);
    UI.UpdateScene();
}

void CEvent::LocalScale( Fvector& amount ){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->LocalScale(amount);
    UI.UpdateScene();
}
//----------------------------------------------------

bool CEvent::Load(CStream& F){
	DWORD version = 0;
    string4096 buf;

    R_ASSERT(F.ReadChunk(EVENT_CHUNK_VERSION,&version));
    if( version!=EVENT_VERSION ){
        ELog.DlgMsg( mtError, "Event: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

	R_ASSERT(F.FindChunk(EVENT_CHUNK_FORMS));
	m_Forms.resize	(F.Rdword());
    F.Read			(m_Forms.begin(),sizeof(SForm)*m_Forms.size());

	R_ASSERT(F.FindChunk(EVENT_CHUNK_ACTIONS));
	m_Actions.resize(F.Rdword());
	for (ActionIt it=m_Actions.begin(); it!=m_Actions.end(); it++){
    	it->type	= F.Rdword();
    	it->count	= F.Rword();
    	F.Read		(&it->clsid,sizeof(it->clsid));
    	F.RstringZ	(buf); it->event=buf;
    }

    return true;
}

void CEvent::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(EVENT_CHUNK_VERSION);
	F.Wword			(EVENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(EVENT_CHUNK_FORMS);
	F.Wdword		(m_Forms.size());
    F.write			(m_Forms.begin(),sizeof(SForm)*m_Forms.size());
	F.close_chunk	();

	F.open_chunk	(EVENT_CHUNK_ACTIONS);
	F.Wdword		(m_Actions.size());
	for (ActionIt it=m_Actions.begin(); it!=m_Actions.end(); it++){
    	F.Wdword	(it->type);
    	F.Wword		(it->count);
    	F.write		(&it->clsid,sizeof(it->clsid));
    	F.WstringZ	(it->event.c_str());
    }
	F.close_chunk	();
}
//----------------------------------------------------

