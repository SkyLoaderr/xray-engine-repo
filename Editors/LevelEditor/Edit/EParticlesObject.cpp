//----------------------------------------------------
// file: EParticlesObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "EParticlesObject.h"
#include "d3dutils.h"
#include "PSLibrary.h"
#include "PropertiesListHelper.h"
#include "ui_main.h"

#define CPSOBJECT_VERSION  				0x0011
//----------------------------------------------------
#define CPSOBJECT_CHUNK_VERSION			0x0001
#define CPSOBJECT_CHUNK_REFERENCE		0x0002
#define CPSOBJECT_CHUNK_PARAMS			0x0003
//----------------------------------------------------

//using namespace PS;

EParticlesObject::EParticlesObject(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct(data);
}
//----------------------------------------------------

void EParticlesObject::Construct(LPVOID data)
{
	ClassID   	= OBJCLASS_PS;
}
//----------------------------------------------------

EParticlesObject::~EParticlesObject()
{
}
//----------------------------------------------------

bool EParticlesObject::GetBox( Fbox& box )
{
	box.set( PPosition, PPosition );
	box.min.x -= PSOBJECT_SIZE;
	box.min.y -= PSOBJECT_SIZE;
	box.min.z -= PSOBJECT_SIZE;
	box.max.x += PSOBJECT_SIZE;
	box.max.y += PSOBJECT_SIZE;
	box.max.z += PSOBJECT_SIZE;
    return true;
}
//----------------------------------------------------

void EParticlesObject::OnUpdateTransform()
{
	inherited::OnUpdateTransform	();
	Fvector v={0.f,0.f,0.f};
	m_PE.UpdateParent(_Transform(),v);
}
//----------------------------------------------------

void EParticlesObject::OnFrame()
{
	inherited::OnFrame();
    Fbox bb; GetBox(bb);
    if (::Render->occ_visible(bb))
	    m_PE.OnFrame(Device.dwTimeDelta);
}
//----------------------------------------------------

void EParticlesObject::Render(int priority, bool strictB2F)
{
	inherited::Render(priority,strictB2F);
    Fbox bb; GetBox(bb);
	if (::Render->occ_visible(bb)){
	    if (1==priority){
            if (false==strictB2F){
                // draw emitter
                u32 C = 0xFFFFEBAA;
                DU.DrawLineSphere(PPosition, PSOBJECT_SIZE/10, C, true);
                if( Selected() ){
                    Fbox bb; GetBox(bb);
                    u32 clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                    DU.DrawSelectionBox(bb,&clr);
                }
            }
        }
	    Device.Models.Render(&m_PE,_Transform(),priority,strictB2F,1.f);
    }
}
//----------------------------------------------------

void EParticlesObject::RenderSingle()
{
	Render(1,false);
	Render(1,true);
}
//----------------------------------------------------

bool EParticlesObject::FrustumPick(const CFrustum& frustum)
{
    return (frustum.testSphere_dirty(PPosition,PSOBJECT_SIZE))?true:false;
}
//----------------------------------------------------

bool EParticlesObject::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	Fvector pos,ray2;
    pos.set(PPosition);
	ray2.sub( pos, start );

    float d = ray2.dotproduct(direction);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (PSOBJECT_SIZE*PSOBJECT_SIZE)) && (d>PSOBJECT_SIZE) ){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
        }
    }
	return false;
}
//----------------------------------------------------

void EParticlesObject::Play()
{
	m_PE.Play();
}
//----------------------------------------------------

void EParticlesObject::Stop()
{
	m_PE.Stop();
}
//----------------------------------------------------

bool EParticlesObject::Load(IReader& F)
{
	u32 version = 0;

    R_ASSERT(F.r_chunk(CPSOBJECT_CHUNK_VERSION,&version));
    if( version!=CPSOBJECT_VERSION ){
        ELog.DlgMsg( mtError, "PSObject: Unsupported version.");
        return false;
    }

	inherited::Load(F);

    R_ASSERT(F.find_chunk(CPSOBJECT_CHUNK_REFERENCE));
    F.r_stringZ(m_RefName);
    if (!Compile(m_RefName.c_str())){
        ELog.DlgMsg( mtError, "EParticlesObject: '%s' not found in library", m_RefName.c_str() );
        return false;
    }

    if (F.find_chunk(CPSOBJECT_CHUNK_PARAMS)){
    	m_PE.m_RT_Flags.set(F.r_u32());
    }

    return true;
}
//----------------------------------------------------

void EParticlesObject::Save(IWriter& F)
{
	CCustomObject::Save(F);

	F.open_chunk	(CPSOBJECT_CHUNK_VERSION);
	F.w_u16			(CPSOBJECT_VERSION);
	F.close_chunk	();

    VERIFY(m_PE.GetDefinition());
	F.open_chunk	(CPSOBJECT_CHUNK_REFERENCE);
    F.w_stringZ		(m_PE.GetDefinition()->m_Name);
	F.close_chunk	();

	F.open_chunk	(CPSOBJECT_CHUNK_PARAMS);
	F.w_u32			(m_PE.m_RT_Flags.get());
	F.close_chunk	();
}
//----------------------------------------------------

void EParticlesObject::OnDeviceCreate()
{
}

void EParticlesObject::OnDeviceDestroy()
{
}

bool EParticlesObject::ExportGame(SExportStreams& F)
{
	SExportStreamItem& I	= F.pe_static;
	I.stream.open_chunk		(I.chunk++);
    I.stream.w_stringZ		(m_RefName.c_str());
    I.stream.w				(&_Transform(),sizeof(Fmatrix));
    I.stream.close_chunk	();
    return true;
}
//----------------------------------------------------

bool EParticlesObject::Compile(LPCSTR ref_name)
{
    PS::CPEDef* def 		= ::Render->PSLibrary.FindPED(ref_name);
    if (m_PE.Compile(def)){
		m_RefName			= ref_name;
		UpdateTransform		();
        return true;
    }
    return false;
}

void __fastcall EParticlesObject::OnRefChange(PropValue* V)
{
	if (!Compile(m_RefName.c_str())){
        ELog.Msg( mtError, "Can't compile particle system '%s'", m_RefName.c_str() );
    }else{
		m_RefName	= m_PE.GetDefinition()?m_PE.GetDefinition()->m_Name:"";
        UI.Command(COMMAND_REFRESH_PROPERTIES);
    }
}

void EParticlesObject::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref, items);
    PropValue* V;
    V=PHelper.CreateALibPE	(items,FHelper.PrepareKey(pref, "Reference"),&m_RefName);
    V->OnChangeEvent		= OnRefChange;
}
//----------------------------------------------------

bool EParticlesObject::GetSummaryInfo(SSceneSummary* inf)
{
	if (!m_RefName.IsEmpty()&&m_PE.GetDefinition()){ 
    	inf->pe_static.insert(m_PE.GetDefinition()->m_Name);
		if (m_PE.GetDefinition()->m_TextureName&&m_PE.GetDefinition()->m_TextureName[0]) inf->textures.insert(ChangeFileExt(m_PE.GetDefinition()->m_TextureName,"").LowerCase());
    }
	inf->pe_static_cnt++;
	return true;
}
//----------------------------------------------------

