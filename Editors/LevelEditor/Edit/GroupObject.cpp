//----------------------------------------------------
// file: GroupObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "GroupObject.h"
#include "Scene.h"
#include "d3dutils.h"
//----------------------------------------------------
//------------------------------------------------------------------------------
// !!! при разворачивании груп использовать prefix если нужно имя !!!
//------------------------------------------------------------------------------

CGroupObject::CGroupObject(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct	(data);
}

void CGroupObject::Construct(LPVOID data)
{
	ClassID		= OBJCLASS_GROUP;
    m_Flags.zero();
    m_BBox.invalidate();
}

CGroupObject::~CGroupObject	()
{
	OnDestroy();
}

void CGroupObject::OnDestroy()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	(*it)->m_pGroupObject=0;	// 
    	_DELETE(*it);
    }
    m_Objects.clear();
}

bool CGroupObject::GetBox(Fbox& bb)
{
	if (m_BBox.is_valid()){
	    bb.xform(m_BBox,_Transform());
        return true;
    }
    return false;
}

void CGroupObject::OnUpdateTransform()
{
	inherited::OnUpdateTransform();
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnUpdateTransform();
}

void CGroupObject::OnFrame()
{
	inherited::OnFrame();
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnFrame();
}

void CGroupObject::AppendObject(CCustomObject* object)
{
	if (!IsGroupClassID(object->ClassID)) return;
    if (object->GetGroup()){
        if (mrNo==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Object '%s' already in group '%s'. Change group?",object->Name,object->GetGroup()->Name))
        	return;
	    object->RemoveFromGroup();
    }
	object->AppendToGroup(this);
	m_Objects.push_back(object);
}
void CGroupObject::UpdateBBoxAndPivot(bool bInitFromFirstObject)
{
	Fbox box;
    Fvector p;
    m_BBox.invalidate();
    // first init
    if (bInitFromFirstObject){
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            if ((*it)->ClassID==OBJCLASS_SCENEOBJECT){
                PPosition = (*it)->PPosition;
                PRotation = (*it)->PRotation;
                PScale	  = (*it)->PScale;
                UpdateTransform(true);
                if ((*it)->GetUTBox(box)) m_BBox.merge(box);
        	    break;
	        }
        }
    }
    if (!m_BBox.is_valid()){
        ObjectIt it=m_Objects.begin();
        Fvector C; C.set((*it)->PPosition); it++;
        for (; it!=m_Objects.end(); it++)
        	C.add((*it)->PPosition);    
        FPosition.div(C,m_Objects.size());
        FRotation.set(0,0,0);
        FScale.set(1.f,1.f,1.f);
        UpdateTransform(true);
        m_BBox.set(0,0,0,0,0,0);
    }
    // update box
    for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        switch((*it)->ClassID){
        case OBJCLASS_SCENEOBJECT:
            if ((*it)->GetBox(box)){
                box.xform(FITransform);
                m_BBox.merge(box);
            }
        break;
        default:
        	FITransform.transform_tiny(p,(*it)->PPosition);
        	m_BBox.modify(p);
        }
    }
}

void CGroupObject::GroupObjects(ObjectList& lst)
{
	R_ASSERT(lst.size());
	for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
    	AppendObject(*it);
    UpdateBBoxAndPivot(true);
}

void CGroupObject::UngroupObjects()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
        (*it)->RemoveFromGroup();
    m_Objects.clear();
}

void CGroupObject::MoveTo(const Fvector& pos, const Fvector& up)
{
	Fvector old_r=FRotation;
	inherited::MoveTo(pos,up);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

    Fvector dr; dr.sub(FRotation,old_r);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector r=(*it)->PRotation; r.add(dr); (*it)->PRotation=r;
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}

void CGroupObject::NumSetPosition(Fvector& pos)
{
	inherited::NumSetPosition(pos);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}
void CGroupObject::NumSetRotation(Fvector& rot)
{
	Fvector old_r=FRotation;
	inherited::NumSetRotation(rot);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

    Fvector dr; dr.sub(FRotation,old_r);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector r=(*it)->PRotation; r.add(dr); (*it)->PRotation=r;
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}
void CGroupObject::NumSetScale(Fvector& scale)
{
	inherited::NumSetScale(scale);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    	(*it)->PScale=scale;
    }
}

void CGroupObject::Move(Fvector& amount)
{
	Fvector old_r=FRotation;
	inherited::Move(amount);
    Fmatrix prev; prev.invert(FTransform);
    UpdateTransform(true);

    Fvector dr; dr.sub(FRotation,old_r);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	Fvector r=(*it)->PRotation; r.add(dr); (*it)->PRotation=r;
    	Fvector v=(*it)->PPosition;
        prev.transform_tiny(v);
        FTransform.transform_tiny(v);
    	(*it)->PPosition=v;
    }
}
void CGroupObject::RotateParent(Fvector& axis, float angle )
{
	inherited::RotateParent(axis,angle);
    Fmatrix  m_old;
    m_old.invert(FTransform);
	UpdateTransform(true);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
		(*it)->PivotRotateParent(m_old,FTransform,axis,angle);
}
void CGroupObject::RotateLocal(Fvector& axis, float angle )
{
	inherited::RotateLocal(axis,angle);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
		(*it)->PivotRotateLocal(FTransformRP,FPosition,axis,angle);
}
void CGroupObject::Scale(Fvector& amount )
{
	inherited::Scale(amount);
    Fmatrix  m_old;
    m_old.invert(FTransform);
	UpdateTransform(true);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
		(*it)->PivotScale(m_old,FTransform,amount);
}
void CGroupObject::Render(int priority, bool strictB2F)
{
	inherited::Render(priority, strictB2F);
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	if ((*it)->IsRender()){
	    	switch ((*it)->ClassID){
    	    case OBJCLASS_SCENEOBJECT: (*it)->Render(priority,strictB2F); break;
            default:
                Device.SetShader(strictB2F?Device.m_SelectionShader:Device.m_WireShader);
                Device.SetTransform(D3DTS_WORLD,Fidentity);
                (*it)->Render(priority,strictB2F);
        	}
    	}
    }
	if ((1==priority)&&(false==strictB2F)){
    	if (Selected()&&m_BBox.is_valid()){
            Device.SetShader(Device.m_WireShader);
            Device.SetTransform(D3DTS_WORLD,FTransform);
            DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
            DU::DrawSelectionBox(m_BBox,&clr);
        }
    }
}

bool CGroupObject::FrustumPick(const CFrustum& frustum)
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if ((*it)->FrustumPick(frustum)) return true;
    return false;
}

bool CGroupObject::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf)
{
	bool bPick = false;
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if ((*it)->RayPick(distance,start,direction,pinf)) bPick=true;
    return bPick;
}

void CGroupObject::OnDeviceCreate(){
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceCreate();
}

void CGroupObject::OnDeviceDestroy(){
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceDestroy();
}

//----------------------------------------------------
#define GROUPOBJ_CURRENT_VERSION		0x0011
//----------------------------------------------------
#define GROUPOBJ_CHUNK_VERSION		  	0x0000
#define GROUPOBJ_CHUNK_OBJECT_LIST     	0x0001
#define GROUPOBJ_CHUNK_FLAGS	     	0x0003
//----------------------------------------------------

bool CGroupObject::Load(CStream& F)
{
    DWORD version = 0;
    char buf[1024];
    R_ASSERT(F.ReadChunk(GROUPOBJ_CHUNK_VERSION,&version));
    if (version!=GROUPOBJ_CURRENT_VERSION){
        ELog.DlgMsg( mtError, "CGroupObject: unsupported file version. Object can't load.");
        return false;
    }
	CCustomObject::Load(F);

	// objects
    Scene.ReadObjects(F,GROUPOBJ_CHUNK_OBJECT_LIST,AppendObject);

    F.ReadChunk(GROUPOBJ_CHUNK_FLAGS,&m_Flags);

	// update bounding volume    
	UpdateBBoxAndPivot(m_Flags.is(flInitFromFirstObject));

    return true;
}

void CGroupObject::Save(CFS_Base& F)
{
	CCustomObject::Save(F);

	F.open_chunk	(GROUPOBJ_CHUNK_VERSION);
	F.Wword			(GROUPOBJ_CURRENT_VERSION);
	F.close_chunk	();

    // objects
    Scene.SaveObjects(m_Objects,GROUPOBJ_CHUNK_OBJECT_LIST,F);

    F.write_chunk	(GROUPOBJ_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));
}
//----------------------------------------------------

bool CGroupObject::ExportGame(SExportStreams& data)
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->ExportGame(data);
	return true;
}
//----------------------------------------------------

