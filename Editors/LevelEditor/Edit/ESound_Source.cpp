//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESound_Source.h"
#include "d3dutils.h"
#include "PropertiesListHelper.h"
//----------------------------------------------------

#define VIS_RADIUS 0.25f
#define SOUND_SEL0_COLOR 	0x00A0A0F0
#define SOUND_SEL1_COLOR 	0x00FFFFFF
#define SOUND_NORM_COLOR 	0x000000FF
#define SOUND_LOCK_COLOR 	0x00FF0000
//----------------------------------------------------

#define SOUND_SOURCE_VERSION   			0x0012
//----------------------------------------------------
#define SOUND_CHUNK_VERSION				0x1001
#define SOUND_CHUNK_TYPE				0x1002
#define SOUND_CHUNK_SOURCE_NAME			0x1003
#define SOUND_CHUNK_SOURCE_PARAMS		0x1004
#define SOUND_CHUNK_SOURCE_FLAGS		0x1005
//----------------------------------------------------

ESoundSource::ESoundSource(LPVOID data, LPCSTR name)
	:CCustomObject(data,name)
{
	Construct(data);
}

void ESoundSource::Construct(LPVOID data)
{
	ClassID					= OBJCLASS_SOUND_SRC;

    m_Type					= stStaticSource;

    m_WAVName				= "";
    m_Params.volume 		= 1.f;
    m_Params.freq			= 1.f;
    m_Params.min_distance   = 1.f;
    m_Params.max_distance   = 300.f;

    m_Flags.set				(flLooped,TRUE);
	m_Command				= stNothing; 
}

ESoundSource::~ESoundSource()
{
	m_Source.destroy		();
}

//----------------------------------------------------

bool ESoundSource::GetBox( Fbox& box )
{
	box.set( m_Params.position, m_Params.position );
	box.min.sub(m_Params.max_distance);
	box.max.add(m_Params.max_distance);
	return true;
}

void ESoundSource::Render(int priority, bool strictB2F)
{
    if((1==priority)&&(false==strictB2F)){
	 	RCache.set_xform_world(Fidentity);
	 	Device.SetShader(Device.m_WireShader);
        u32 clr0=Locked()?SOUND_LOCK_COLOR:(Selected()?SOUND_SEL0_COLOR:SOUND_NORM_COLOR);
        u32 clr1=Locked()?SOUND_LOCK_COLOR:(Selected()?SOUND_SEL1_COLOR:SOUND_NORM_COLOR);
        if (Selected()){ 
        	DU::DrawLineSphere( m_Params.position, m_Params.max_distance, clr1, true );
        	DU::DrawLineSphere( m_Params.position, m_Params.min_distance, clr0, false );
        }else{
			DU::DrawSound(m_Params.position,VIS_RADIUS, clr1);
        }
    }
}

bool ESoundSource::FrustumPick(const CFrustum& frustum)
{
    return (frustum.testSphere_dirty(m_Params.position,VIS_RADIUS))?true:false;
}

bool ESoundSource::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	Fvector ray2;
	ray2.sub( m_Params.position, start );

	float d = ray2.dotproduct(direction);
	if( d > 0  ){
		float d2 = ray2.magnitude();
		if( ((d2*d2-d*d) < (VIS_RADIUS*VIS_RADIUS)) && (d>VIS_RADIUS)){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
		}
	}

	return false;
}
//----------------------------------------------------

bool ESoundSource::Load(IReader& F)
{
	u32 version 	= 0;

    R_ASSERT(F.r_chunk(SOUND_CHUNK_VERSION,&version));
    if(version!=SOUND_SOURCE_VERSION){
        ELog.DlgMsg( mtError, "ESoundSource: Unsupported version.");
        return false;
    }

	inherited::Load			(F);

    R_ASSERT(F.find_chunk(SOUND_CHUNK_TYPE));
	m_Type					= ESoundType(F.r_u32());

    R_ASSERT(F.find_chunk(SOUND_CHUNK_SOURCE_NAME));
    F.r_stringZ		(m_WAVName);

    R_ASSERT(F.find_chunk(SOUND_CHUNK_SOURCE_PARAMS));
    F.r				(&m_Params,sizeof(m_Params));

    if(F.find_chunk(SOUND_CHUNK_SOURCE_FLAGS))
		F.r			(&m_Flags,sizeof(m_Flags));
    
    ResetSource		();

    switch (m_Type){
    case stStaticSource: if (m_Flags.is(flPlaying)) Play(); break;
    default: THROW;
    }
    return true;
}

void ESoundSource::Save(IWriter& F)
{
	inherited::Save	(F);

	F.open_chunk	(SOUND_CHUNK_VERSION);
	F.w_u16			(SOUND_SOURCE_VERSION);
	F.close_chunk	();

    F.w_chunk		(SOUND_CHUNK_TYPE,&m_Type,sizeof(m_Type));

    F.open_chunk	(SOUND_CHUNK_SOURCE_NAME);
    F.w_stringZ		(m_WAVName.c_str());
    F.close_chunk	();

    F.w_chunk		(SOUND_CHUNK_SOURCE_FLAGS,&m_Flags,sizeof(m_Flags));
    
    F.w_chunk		(SOUND_CHUNK_SOURCE_PARAMS,&m_Params,sizeof(m_Params));
}

//----------------------------------------------------

void __fastcall	ESoundSource::OnChangeWAV	(PropValue* prop)
{
	BOOL bPlay 		= !!m_Source.feedback;
	ResetSource		();
	if (bPlay) 		Play();
}

void __fastcall	ESoundSource::OnChangeSource	(PropValue* prop)
{
	m_Source.set_params			(&m_Params);
}

void __fastcall ESoundSource::OnControlClick(PropValue* sender)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: Play();	break;
    case 1: Stop();	break;
	}
}

void ESoundSource::FillProp(LPCSTR pref, PropItemVec& values)
{
	ButtonValue* B;
    B=PHelper.CreateButton		(values, PHelper.PrepareKey(pref,"Controls"), 	"Play,Stop");
    B->OnBtnClickEvent			= OnControlClick;
    PropValue* V;
    V=PHelper.CreateASoundSrc	(values,PHelper.PrepareKey(pref,"WAVE name"),	&m_WAVName);
    V->SetEvents				(0,0,OnChangeWAV);
	V=PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Min dist"),	&m_Params.min_distance,	0.1f,1000.f,0.1f,1);
    V->SetEvents				(0,0,OnChangeSource);
	V=PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Max dist"),	&m_Params.max_distance,	0.1f,1000.f,0.1f,1);
    V->SetEvents				(0,0,OnChangeSource);
	V=PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Frequency"),	&m_Params.freq,			0.0f,2.f);
    V->SetEvents				(0,0,OnChangeSource);
	V=PHelper.CreateFloat		(values,PHelper.PrepareKey(pref,"Volume"),		&m_Params.volume,		0.0f,2.f);
    V->SetEvents				(0,0,OnChangeSource);
}
//----------------------------------------------------

bool ESoundSource::GetSummaryInfo(SSceneSummary* inf)
{
	if (!m_WAVName.IsEmpty()) inf->waves.insert(m_WAVName);
    inf->sound_source_cnt++;
	return true;
}

void ESoundSource::OnFrame()
{
	inherited::OnFrame();
    switch (m_Command){
    case stPlay: 	
    	m_Source.play		(0,m_Flags.is(flLooped));
		m_Source.set_params	(&m_Params);
		m_Command			= stNothing; 
    break;
    case stStop: 
		m_Source.stop		();
        m_Command			= stNothing; 
    break;
    case stNothing:    		break;
    default: THROW;
    }
	m_Flags.set(flPlaying,!!m_Source.feedback);
}

void ESoundSource::ResetSource()
{
	m_Source.destroy();
	if (!m_WAVName.IsEmpty()) m_Source.create	(1,m_WAVName.c_str());
	m_Source.set_params(&m_Params);
}

void ESoundSource::SetSourceWAV(LPCSTR fname)
{
    m_WAVName		= fname;
    ResetSource		();
}

bool ESoundSource::ExportGame(SExportStreams& F)
{
	SExportStreamItem& I	= F.sound_static;
	I.stream.open_chunk		(I.chunk++);
    I.stream.w_stringZ		(m_WAVName.c_str());
    I.stream.w_fvector3		(m_Params.position);
    I.stream.w_float		(m_Params.volume);
    I.stream.w_float		(m_Params.freq);
    I.stream.w_float		(m_Params.min_distance);
    I.stream.w_float		(m_Params.max_distance);
    I.stream.close_chunk	();
    return true;
}

