//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESound_Environment.h"
#include "EShape.h"
#include "ui_main.h"
//----------------------------------------------------

#define SOUND_SEL0_COLOR 	0x00A0A0A0
#define SOUND_SEL1_COLOR 	0x00FFFFFF
#define SOUND_NORM_COLOR 	0x000000FF
#define SOUND_LOCK_COLOR 	0x00FF0000
//----------------------------------------------------

#define SOUND_ENV_VERSION  				0x0012
//----------------------------------------------------
#define SOUND_CHUNK_VERSION				0x1001
#define SOUND_CHUNK_ENV_SHAPE			0x1002
#define SOUND_CHUNK_ENV_REFS			0x1003
//----------------------------------------------------

ESoundEnvironment::ESoundEnvironment(LPVOID data, LPCSTR name)
	:CCustomObject(data,name)
{
	Construct(data);
}

void ESoundEnvironment::Construct(LPVOID data)
{
	ClassID					= OBJCLASS_SOUND_ENV;
    
    m_Shape					= xr_new<CEditShape>((LPVOID)NULL,"internal");
	m_Shape->add_box		(Fidentity);
	m_Shape->SetDrawColor	(0x205050FF, 0xFF202020);
    m_EnvInner				= "";
    m_EnvOuter				= "";
}

ESoundEnvironment::~ESoundEnvironment()
{
    xr_delete				(m_Shape);
}

//----------------------------------------------------

bool ESoundEnvironment::GetBox( Fbox& box )
{
	return m_Shape->GetBox(box);
}

void ESoundEnvironment::Render(int priority, bool strictB2F)
{
	m_Shape->m_bSelected 	= Selected();
    m_Shape->m_bLocked		= Locked();
    m_Shape->Render			(priority,strictB2F);
}

bool ESoundEnvironment::FrustumPick(const CFrustum& frustum)
{
    return m_Shape->FrustumPick(frustum);
}

bool ESoundEnvironment::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
    return m_Shape->RayPick(distance, start, direction, pinf);
}
//----------------------------------------------------

bool ESoundEnvironment::Load(IReader& F)
{
	u32 version 	= 0;

    R_ASSERT(F.r_chunk(SOUND_CHUNK_VERSION,&version));
    if(version!=SOUND_ENV_VERSION){
        ELog.DlgMsg( mtError, "ESoundSource: Unsupported version.");
        return false;
    }
	inherited::Load			(F);

    IReader* OBJ			= F.open_chunk(SOUND_CHUNK_ENV_SHAPE);
    if (OBJ){
    	m_Shape->Load		(*OBJ);
        OBJ->close			();
    }

    R_ASSERT(F.find_chunk(SOUND_CHUNK_ENV_REFS));
    F.r_stringZ				(m_EnvInner);
    F.r_stringZ				(m_EnvOuter);

    return true;
}

void ESoundEnvironment::Save(IWriter& F)
{
	inherited::Save	(F);

	F.open_chunk	(SOUND_CHUNK_VERSION);
	F.w_u16			(SOUND_ENV_VERSION);
	F.close_chunk	();

    F.open_chunk	(SOUND_CHUNK_ENV_SHAPE);
    m_Shape->Save	(F);
    F.close_chunk	();
    
    F.open_chunk	(SOUND_CHUNK_ENV_REFS);
    F.w_stringZ		(m_EnvInner.c_str());
    F.w_stringZ		(m_EnvOuter.c_str());
    F.close_chunk	();
}
//----------------------------------------------------

void __fastcall ESoundEnvironment::OnChangeEnvs	(PropValue* prop)
{
	UI.Command		(COMMAND_REFRESH_SOUND_ENV_GEOMETRY);
}
//----------------------------------------------------

void ESoundEnvironment::FillProp(LPCSTR pref, PropItemVec& values)
{
	PropValue* P;
    P=PHelper.CreateASoundEnv	(values, PHelper.PrepareKey(pref,"Evironment Inner"),	&m_EnvInner);
    P->OnChangeEvent			= OnChangeEnvs;
    P=PHelper.CreateASoundEnv	(values, PHelper.PrepareKey(pref,"Evironment Outer"),	&m_EnvOuter);
    P->OnChangeEvent			= OnChangeEnvs;
}
//----------------------------------------------------

bool ESoundEnvironment::GetSummaryInfo(SSceneSummary* inf)
{
	return true;
}

void ESoundEnvironment::OnFrame()
{
	m_Shape->OnFrame();
}

void ESoundEnvironment::Scale(Fvector& amount)
{
	m_Shape->Scale(amount);
}

const Fmatrix& ESoundEnvironment::get_box()
{
	CSE_Shape::shape_def& shape = m_Shape->get_shape(0);
    R_ASSERT(shape.type==CSE_Shape::cfBox);
    Fmatrix M			= shape.data.box;
    M.mulA				(m_Shape->_Transform());
}

void ESoundEnvironment::OnSceneUpdate()
{
	inherited::OnSceneUpdate();
	UI.Command( COMMAND_REFRESH_SOUND_ENV_GEOMETRY );
}

