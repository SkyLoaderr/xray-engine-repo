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
#include "bottombar.h"
#include "topbar.h"
#include "scene.h"
#include "UI_Tools.h"
#include "FrameEvent.h"
// export to game
#include "net_utils.h"
#include "xrMessages.h"

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
void CEvent::SForm::Select( int flag )
{
	m_Selected = (flag==-1)?(m_Selected?false:true):flag;
};
void CEvent::SForm::GetTransform(Fmatrix& M){
    // update transform matrix
	Fmatrix			mScale,mTranslate,mRotate;
	mRotate.setHPB	(vRotate.y, vRotate.x, vRotate.z);
	mScale.scale	(vSize);
	mTranslate.translate(vPosition);
	M.mul			(mTranslate,mRotate);
	M.mulB			(mScale);
}

void CEvent::SForm::GetTransformRP(Fmatrix& M){
    // update transform matrix
	Fmatrix			mTranslate,mRotate;
	mRotate.setHPB	(vRotate.y, vRotate.x, vRotate.z);
	mTranslate.translate(vPosition);
	M.mul			(mTranslate,mRotate);
}

void CEvent::SForm::RenderBox(const Fmatrix& parent, bool bAlpha){
	Fmatrix T; GetTransform(T);
    T.mulA(parent);
	Fmatrix Trp; GetTransformRP(Trp);
    Trp.mulA(parent);
	// render
	FVF::L v;
	FLvertexVec V;
	DWORD C;
	if (bAlpha){
        C=D3DCOLOR_RGBA( 0, 255, 0, 24 );
		Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
        Device.SetTransform(D3DTS_WORLD,T);
	    Device.SetShader(Device.m_SelectionShader);
        DU::DrawIdentBox(true,false,&C);
		Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
    }else{
        C=D3DCOLOR_RGBA( m_Selected?255:32,m_Selected?255:32,m_Selected?255:32, 255 );
        Device.RenderNearer(0.0003);
        Device.SetTransform(D3DTS_WORLD,T);
	    Device.SetShader(Device.m_WireShader);
        Device.ResetNearer();
        DU::DrawIdentBox(false,true,&C);
		if (fraBottomBar->miDrawObjectsPivot->Checked&&m_Selected){
    		Device.SetShader(Device.m_WireShader);
    		DU::DrawObjectAxis(Trp);
	    }
    }
}

void CEvent::SForm::Render(const Fmatrix& parent, bool bAlpha)
{
	switch(m_eType){
    case efBox: RenderBox(parent,bAlpha); break;
    };
}

void CEvent::SForm::GetBox(Fbox& bb)
{
	Fmatrix T;		GetTransform(T);
	bb.min.set		(-0.5f,-0.5f,-0.5f);
	bb.max.set		( 0.5f, 0.5f, 0.5f);
    bb.xform		(T);
}

bool CEvent::SForm::RayPick(const Fmatrix& parent, float& distance, Fvector& start, Fvector& direction)
{
	bool bPick=false;
    float range;
	if (m_eType==efBox){
		Fmatrix T; 	GetTransform(T);
        T.mulA(parent);
        Fvector v[3];
        for (int i=0; i<12; i++){
            GetBoxTri(i,v);
            T.transform_tiny(v[0]);
            T.transform_tiny(v[1]);
            T.transform_tiny(v[2]);
            if (CDB::TestRayTri2(start,direction,v,range)){
                if ((range>=0)&&(range<distance)){
                    distance=range;
                    bPick=true;
                }
            }
        }
    }
    return bPick;
}

bool CEvent::SForm::FrustumPick(const Fmatrix& parent, const CFrustum& frustum)
{
	if (m_eType==efBox){
		Fmatrix T; 	GetTransform(T);
        T.mulA(parent);
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
    return false;
}

bool CEvent::SForm::FrustumSelect(const Fmatrix& parent, int flag, const CFrustum& frustum)
{
	if (FrustumPick(parent,frustum)){
    	Select(flag);
    	return true;
    }
	return false;
}

void CEvent::SForm::Move( const Fmatrix& parent, const Fmatrix& inv_parent, Fvector& amount )
{
    if (fraTopBar->ebNormalAlignment->Down){
    	Fmatrix Trp; GetTransformRP(Trp);
		Trp.mulA(parent);
        parent.transform_tiny(vPosition);
    	CCustomObject::SnapMove(vPosition,vRotate,Trp,amount);
		GetTransformRP(Trp);
        Trp.mulA(inv_parent);
        Trp.getXYZ(vRotate);
        inv_parent.transform_tiny(vPosition);
    }
}

void CEvent::SForm::MoveTo( Fvector& pos, Fvector& up )
{
    vPosition.set(pos);
    if (fraTopBar->ebNormalAlignment->Down)
    	CCustomObject::NormalAlign(vRotate,up);
}

void CEvent::SForm::RotateLocal( Fvector& axis, float angle )
{
	Fmatrix T; 		GetTransformRP(T);

    Fmatrix 		m;
    Fvector 		r;
    m.rotation		(axis,angle);
    T.mulB			(m);
    T.getXYZ		(r);
    vRotate			= r;
}

void CEvent::SForm::RotateParent( Fvector& axis, float angle )
{
    vRotate.mad(vRotate,axis,angle);
}

void CEvent::SForm::Scale( Fvector& amount )
{
	vSize.add(amount);
    if (vSize.x<EPS) vSize.x=EPS;
    if (vSize.y<EPS) vSize.y=EPS;
    if (vSize.z<EPS) vSize.z=EPS;
}
//------------------------------------------------------------------------------

CEvent::CEvent( char *name ):CCustomObject(){
	Construct();
	Name			= name;
}

CEvent::CEvent():CCustomObject(){
	Construct();
}

CEvent::~CEvent(){
}

void CEvent::Construct(){
	ClassID 		= OBJCLASS_EVENT;
    m_Actions.clear	();
    m_Forms.clear	();
    // create one box
    AppendForm		(efBox);
}

CEvent::SForm* CEvent::AppendForm(EFormType type)
{
    for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->Select(0);
	m_Forms.push_back(SForm());
    SForm& F 	= m_Forms.back();
    F.m_eType   = type;
    F.vSize.set	(1.f,1.f,1.f);
    F.vRotate.set(0.f,0.f,0.f);
	F.vPosition.set(0.f,0.f,0.f);
    F.m_Selected= true;
    return &F;
}

void CEvent::RemoveSelectedForm()
{
	for (int i=0; i<(int)m_Forms.size(); i++)
    	if (m_Forms[i].m_Selected){
			m_Forms.erase(m_Forms.begin()+i);
            i--;
        }
    // remove object from scene if empty?
    if (m_Forms.empty()){
    	Scene.RemoveObject(this,true);
    	delete this;
    }
}

//------------------------------------------------------------------------------------------------
void CEvent::GetRenderBox(Fbox& box){
	// calc for all forms
    box.invalidate();
    Fbox bb;
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++){
    	it->GetBox(bb);
        box.merge(bb);
    }
}
//------------------------------------------------------------------------------------------------

bool CEvent::GetBox( Fbox& box ){
	GetRenderBox(box);
    box.xform(FTransform);
	return true;
}

void CEvent::Render( int priority, bool strictB2F ){
	inherited::Render(priority,strictB2F);
	if (priority==1){
        for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->Render(FTransform,strictB2F);
        if(Selected()&&(false==strictB2F)){
            Fbox bb;
            GetRenderBox(bb);
			DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
			Device.SetTransform(D3DTS_WORLD,FTransform);
			DU::DrawSelectionBox(bb,&clr);
        }
    }
}

bool CEvent::FrustumPick(const CFrustum& frustum){
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++)
    	if (it->FrustumPick(FTransform,frustum)) return true;
    return false;
}

bool CEvent::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
    bool bPick=false;
	for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++)
    	if (it->RayPick(FTransform,distance,start,direction)) bPick=true;
    return bPick;
}

void CEvent::Select(int flag)
{
	inherited::Select(flag);
    if (flag==0) for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->Select(0);
}

bool CEvent::RaySelect(int flag, Fvector& start,Fvector& dir, bool bRayTest)
{
    if (IsFormMode()){
    	if ((bRayTest&&RayPick(UI.ZFar(),start,dir))||!bRayTest) Select(1);
        FormIt nearest=m_Forms.end();
        float dist = UI.ZFar();
		for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++)
			if (it->RayPick(FTransform,dist,start,dir)) nearest=it;
        if (nearest!=m_Forms.end()){
        	nearest->Select(flag);
            return true;
        }
    }else 	return inherited::RaySelect(flag,start,dir,bRayTest);
    return false;
}

bool CEvent::FrustumSelect(int flag, const CFrustum& frustum)
{
    if (IsFormMode()){
    	Select(1);
		for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) it->FrustumSelect(FTransform,flag,frustum);
        return true;
    }else 	return inherited::FrustumSelect(flag,frustum);
}

void CEvent::Move(Fvector& amount){
    if (IsFormMode()){
		for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++)
        	if (it->m_Selected) it->Move(FTransform,FITransform,amount);
    }else inherited::Move(amount);
}

void CEvent::MoveTo(const Fvector& pos, const Fvector& up){
	if (IsFormMode()){
    	Fvector p,u;
        FITransform.transform_tiny(p,pos);
        FITransform.transform_dir(u,up);
		for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++)
        	if (it->m_Selected) it->MoveTo(p,u);
    }else inherited::MoveTo(pos,up);
}

void CEvent::RotateParent(Fvector& axis, float angle){
    if (IsFormMode()){
		for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) if (it->m_Selected) it->RotateParent(axis,angle);
    }else	inherited::RotateParent(axis,angle);

}
void CEvent::RotateLocal(Fvector& axis, float angle){
    if (IsFormMode()){
	    for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) if (it->m_Selected) it->RotateLocal(axis,angle);
    }else	inherited::RotateLocal(axis,angle);
}
void CEvent::Scale(Fvector& amount){
    if (IsFormMode()){
	    for (FormIt it=m_Forms.begin(); it!=m_Forms.end(); it++) if (it->m_Selected) it->Scale(amount);
    }else	inherited::Scale(amount);
}
void CEvent::PivotRotateParent(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& axis, float angle ){
	inherited::PivotRotateParent(prev_inv, current, axis, angle);
}
void CEvent::PivotRotateLocal(const Fmatrix& parent, Fvector& pivot, Fvector& axis, float angle ){
	inherited::PivotRotateLocal(parent, pivot, axis, angle);
}
void CEvent::PivotScale( const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount ){
	inherited::PivotScale(prev_inv, current, amount);
}

bool CEvent::IsFormMode(){
	if (Tools.GetTargetClassID()==OBJCLASS_EVENT){
		TfraEvent* frame=(TfraEvent*)Tools.GetFrame(); R_ASSERT(frame);
        return frame->ebFormMode->Down;
    }
    return false;
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
    	it->type	= (EEventType)F.Rdword();
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

bool CEvent::ExportSpawn( CFS_Base& F, int chunk_id )
{
    NET_Packet Packet;
    Packet.w_begin		(M_SPAWN);
    Packet.w_string		("g_event");
    Packet.w_string		(Name);
    Packet.w_u8 		(0xFE);
    Packet.w_vec3		(PPosition);
    Fvector a; a.set	(0,0,0);
    Packet.w_vec3		(a);
    Packet.w_u16		(0);
    WORD fl 			= M_SPAWN_OBJECT_ACTIVE;
    Packet.w_u16		(fl);

    u32	position		= Packet.w_tell	();
    Packet.w_u16		(0);
    // spawn info
    // cform's
    // -count
    Packet.w_u8 		(u8(m_Forms.size()));
    // --elements
    for (CEvent::FormIt f_it=m_Forms.begin(); f_it!=m_Forms.end(); f_it++){
        Packet.w_u8 	(u8(f_it->m_eType));
        switch (f_it->m_eType){
        case CEvent::efSphere:	{ Fsphere S; S.set(f_it->vPosition,f_it->vSize.magnitude());Packet.w_vec3(S.P);Packet.w_float(S.R); }break;
        case CEvent::efBox:		{ Fmatrix T; f_it->GetTransform(T); Packet.w_matrix(T); }break;
        }
    }
    // action's
    // -count
    Packet.w_u8 		(u8(m_Actions.size()));
    // --elements
    for (CEvent::ActionIt a_it=m_Actions.begin(); a_it!=m_Actions.end(); a_it++){
        Packet.w_u8 	(u8(a_it->type));
        Packet.w_u16 	(a_it->count);
        Packet.w_u64 	(a_it->clsid);
        Packet.w_string	(a_it->event.c_str());
    }
    // data size
    u16 size			= u16(Packet.w_tell()-position);
    Packet.w_seek		(position,&size,sizeof(u16));

    F.open_chunk		(chunk_id);
    F.write				(Packet.B.data,Packet.B.count);
    F.close_chunk		();

    return true;
}

