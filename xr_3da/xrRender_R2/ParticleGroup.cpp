//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "..\psystem.h"
#include "ParticleGroup.h"
#include "PSLibrary.h"

using namespace PS;

//------------------------------------------------------------------------------
CPGDef::CPGDef()
{                             
    m_Flags.zero	();
    m_fTimeLimit	= 0.f;
}

CPGDef::~CPGDef()
{
}

void CPGDef::SetName(LPCSTR name)
{
    m_Name			= name;
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPGDef::Load(IReader& F)
{
	R_ASSERT		(F.find_chunk(PGD_CHUNK_VERSION));
	u16 version		= F.r_u16();

    if (version!=PGD_VERSION){
		Log			("!Unsupported PG version. Load failed.");
    	return FALSE;
    }

	R_ASSERT		(F.find_chunk(PGD_CHUNK_NAME));
	F.r_stringZ		(m_Name);

	F.r_chunk		(PGD_CHUNK_FLAGS,&m_Flags);

	R_ASSERT		(F.find_chunk(PGD_CHUNK_EFFECTS2));
    m_Effects.resize(F.r_u32());
    for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
    	F.r_stringZ		(it->m_EffectName);
    	F.r_stringZ		(it->m_ChildEffectName);
    	it->m_Time0 	= F.r_float();
    	it->m_Time1 	= F.r_float();
    	it->m_Flags.set	(F.r_u32());
    }
    
    if (F.find_chunk(PGD_CHUNK_TIME_LIMIT)){
   		m_fTimeLimit= F.r_float();
    }

#ifdef _EDITOR
    if (F.find_chunk(PGD_CHUNK_OWNER)){
	    F.r_stringZ	(m_OwnerName);
	    F.r_stringZ	(m_ModifName);
        F.r			(&m_CreateTime,sizeof(m_CreateTime));
        F.r			(&m_ModifTime,sizeof(m_ModifTime));
    }
#endif
    
    return TRUE;
}                   

void CPGDef::Save(IWriter& F)
{
	F.open_chunk	(PGD_CHUNK_VERSION);
	F.w_u16			(PGD_VERSION);
    F.close_chunk	();

	F.open_chunk	(PGD_CHUNK_NAME);
    F.w_stringZ		(m_Name);
    F.close_chunk	();

	F.w_chunk		(PGD_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

	F.open_chunk	(PGD_CHUNK_EFFECTS2);
    F.w_u32			(m_Effects.size());
    for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
    	F.w_stringZ	(it->m_EffectName);
    	F.w_stringZ	(it->m_ChildEffectName);
    	F.w_float	(it->m_Time0);
    	F.w_float	(it->m_Time1);
    	F.w_u32		(it->m_Flags.get());
    }
    F.close_chunk	();

    F.open_chunk	(PGD_CHUNK_TIME_LIMIT);
   	F.w_float		(m_fTimeLimit);
    F.close_chunk	();

#ifdef _EDITOR
	F.open_chunk	(PGD_CHUNK_OWNER);
    F.w_stringZ		(m_OwnerName);
    F.w_stringZ		(m_ModifName);
    F.w				(&m_CreateTime,sizeof(m_CreateTime));
    F.w				(&m_ModifTime,sizeof(m_ModifTime));
	F.close_chunk	();
#endif
}

//------------------------------------------------------------------------------
// Particle Group item
//------------------------------------------------------------------------------
void CParticleGroup::SItem::Set(IRender_Visual* e)
{
	_effect=e;
}
void CParticleGroup::SItem::Clear()
{
	VisualVec 		visuals;
    GetVisuals		(visuals);
    for (VisualVecIt it=visuals.begin(); it!=visuals.end(); it++)
	    ::Render->model_Delete(*it);
}
IRender_Visual* CParticleGroup::SItem::AppendChild(LPCSTR eff_name)
{
	IRender_Visual* V = RImplementation.model_CreatePE(eff_name);
    _children.push_back(V);
    return V;
}
void CParticleGroup::SItem::RemoveChild(u32 idx)
{
	VERIFY(idx<_children.size());
    IRender_Visual*& V 			= _children[idx];
    ((CParticleEffect*)V)->Stop	(TRUE);
    _children_stopped.push_back	(V);
    V 							= _children.back();
    _children.pop_back			();
}
void CParticleGroup::SItem::Play()
{
    CParticleEffect* E	= static_cast<CParticleEffect*>(_effect);
    if (E) E->Play();
}
void CParticleGroup::SItem::Stop(BOOL def_stop)
{
	// stop all effects
    CParticleEffect* E	= static_cast<CParticleEffect*>(_effect);
    if (E) E->Stop(def_stop);
    VisualVecIt it;
    for (it=_children.begin(); it!=_children.end(); it++)
        static_cast<CParticleEffect*>(*it)->Stop(def_stop);
    for (it=_children_stopped.begin(); it!=_children_stopped.end(); it++)
        static_cast<CParticleEffect*>(*it)->Stop(def_stop);
    // and delete if !deffered
    if (!def_stop){
        for (it=_children.begin(); it!=_children.end(); it++)            		::Render->model_Delete(*it);
        for (it=_children_stopped.begin(); it!=_children_stopped.end(); it++)   ::Render->model_Delete(*it);
        _children.clear			();
        _children_stopped.clear	();
    }
}
BOOL CParticleGroup::SItem::IsPlaying()
{
    CParticleEffect* E	= static_cast<CParticleEffect*>(_effect);
    return E?E->IsPlaying():FALSE;
}
void CParticleGroup::SItem::UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM)
{
    CParticleEffect* E	= static_cast<CParticleEffect*>(_effect);
    if (E) E->UpdateParent(m,velocity,bXFORM);
}
//------------------------------------------------------------------------------
static const u32	uDT_STEP = 33;
static const float	fDT_STEP = float(uDT_STEP)/1000.f;
void OnGroupParticleBirth(void* owner, u32 param, PAPI::Particle& m, u32 idx)
{
	CParticleGroup* PG 	= static_cast<CParticleGroup*>(owner); 	VERIFY(PG);
    CParticleEffect*PE	= static_cast<CParticleEffect*>(PG->items[param]._effect);
	PS::OnEffectParticleBirth(PE, param, m, idx);
    // if have child
    const CPGDef* PGD			= PG->GetDefinition();					VERIFY(PGD);
    const CPGDef::SEffect& eff	= PGD->m_Effects[param];
    if (eff.m_Flags.is(CPGDef::SEffect::flHaveChild)){
	    CParticleEffect*C		= static_cast<CParticleEffect*>(PG->items[param].AppendChild(*eff.m_ChildEffectName));
        Fmatrix M; 				M.translate(m.pos);
        Fvector vel; 			vel.sub(m.pos,m.posB); vel.div(fDT_STEP);
        C->UpdateParent	(M,vel,FALSE);
    }
}
void OnGroupParticleDead(void* owner, u32 param, PAPI::Particle& m, u32 idx)
{
	CParticleGroup* PG 	= static_cast<CParticleGroup*>(owner); VERIFY(PG);
    CParticleEffect*PE	= static_cast<CParticleEffect*>(PG->items[param]._effect);
	PS::OnEffectParticleDead(PE, param, m, idx);
    // if have child
    const CPGDef* PGD			= PG->GetDefinition();					VERIFY(PGD);
    const CPGDef::SEffect& eff	= PGD->m_Effects[param];
    if (eff.m_Flags.is(CPGDef::SEffect::flHaveChild))
    	PG->items[param].RemoveChild(idx);
}
//------------------------------------------------------------------------------
struct zero_vis_pred : public std::unary_function<IRender_Visual*, bool>
{
	bool operator()(const IRender_Visual* x){ return x==0; }
};
void CParticleGroup::SItem::OnFrame(u32 u_dt, const CPGDef::SEffect& def, Fbox& box, bool& bPlaying)
{
    CParticleEffect* E		= static_cast<CParticleEffect*>(_effect);
    if (E){
        E->OnFrame			(u_dt);
        if (E->IsPlaying()){
            bPlaying		= true;
            if (E->vis.box.is_valid())     box.merge	(E->vis.box);
            if (def.m_Flags.is(CPGDef::SEffect::flHaveChild)&&*def.m_ChildEffectName){
                PAPI::Particle* particles;
                u32 p_cnt;
                PAPI::ParticleManager()->GetParticles(E->GetHandleEffect(),particles,p_cnt);
                VERIFY(p_cnt==_children.size());
                if (p_cnt){
                    for(u32 i = 0; i < p_cnt; i++){
                        PAPI::Particle &m	= particles[i]; 
                        CParticleEffect* C 	= static_cast<CParticleEffect*>(_children[i]);
                        C->Play				();
                        Fmatrix M; 			M.translate(m.pos);
				        Fvector vel; 		vel.sub(m.pos,m.posB); vel.div(fDT_STEP);
                        C->UpdateParent	(M,vel,FALSE);
                    }
                }
            }
        }
    }
    VisualVecIt it;
    if (!_children.empty()){
        for (it=_children.begin(); it!=_children.end(); it++){
            CParticleEffect* E	= static_cast<CParticleEffect*>(*it);
            if (E){
                E->OnFrame		(u_dt);
                if (E->IsPlaying()){
                    bPlaying	= true;
                    if (E->vis.box.is_valid())     box.merge	(E->vis.box);
                }else
                    ::Render->model_Delete(*it);
            }
        }
    }
    if (!_children_stopped.empty()){
    	u32 rem_cnt				= 0;
        for (it=_children_stopped.begin(); it!=_children_stopped.end(); it++){
            CParticleEffect* E	= static_cast<CParticleEffect*>(*it);
            if (E){
                E->OnFrame		(u_dt);
                if (E->IsPlaying()){ 
                    bPlaying	= true;
                    if (E->vis.box.is_valid()) box.merge	(E->vis.box);
                }else{
                	rem_cnt++;
                    ::Render->model_Delete(*it);
                }
            }
        }
        // remove if stopped
        if (rem_cnt){
            VisualVecIt new_end=std::remove_if(_children_stopped.begin(),_children_stopped.end(),zero_vis_pred());
            _children_stopped.erase(new_end,_children_stopped.end());
        }
    }
//	Msg("C: %d CS: %d",_children.size(),_children_stopped.size());
}
void CParticleGroup::SItem::OnDeviceCreate()
{
	VisualVec 		visuals;
    GetVisuals		(visuals);
    for (VisualVecIt it=visuals.begin(); it!=visuals.end(); it++)
	    static_cast<CParticleEffect*>(*it)->OnDeviceCreate();
}
void CParticleGroup::SItem::OnDeviceDestroy()
{
	VisualVec 		visuals;
    GetVisuals		(visuals);
    for (VisualVecIt it=visuals.begin(); it!=visuals.end(); it++)
	    static_cast<CParticleEffect*>(*it)->OnDeviceDestroy();
}
u32	CParticleGroup::SItem::ParticlesCount()
{
	u32 p_count=0;
	VisualVec 		visuals;
    GetVisuals		(visuals);
    for (VisualVecIt it=visuals.begin(); it!=visuals.end(); it++)
	    p_count		+= static_cast<CParticleEffect*>(*it)->ParticlesCount();
    return p_count;
}


//------------------------------------------------------------------------------
// Particle Group part
//------------------------------------------------------------------------------
CParticleGroup::CParticleGroup()
{
	m_RT_Flags.zero			();
	m_InitialPosition.set	(0,0,0);
}

CParticleGroup::~CParticleGroup()
{
	for (u32 i=0; i<items.size(); i++) items[i].Clear();
	items.clear();
}

void CParticleGroup::OnFrame(u32 u_dt)
{
	if (m_Def&&m_RT_Flags.is(flRT_Playing)){
        float ct	= m_CurrentTime;
        float f_dt	= float(u_dt)/1000.f;
        for (CPGDef::EffectVec::const_iterator e_it=m_Def->m_Effects.begin(); e_it!=m_Def->m_Effects.end(); e_it++){	
            if (e_it->m_Flags.is(CPGDef::SEffect::flEnabled)){
                SItem& I			= items[e_it-m_Def->m_Effects.begin()];
                if (I.IsPlaying()){
                    if ((ct<=e_it->m_Time1)&&(ct+f_dt>=e_it->m_Time1))	
                        I.Stop(e_it->m_Flags.is(CPGDef::SEffect::flDefferedStop));
                }else{
                    if (!m_RT_Flags.is(flRT_DefferedStop))
                        if ((ct<=e_it->m_Time0)&&(ct+f_dt>=e_it->m_Time0))	
                            I.Play();
                }
            }
        }
        m_CurrentTime 	+= f_dt;
        if ((m_CurrentTime>m_Def->m_fTimeLimit)&&(m_Def->m_fTimeLimit>0.f))
            if (!m_RT_Flags.is(flRT_DefferedStop)) Stop(true);

        bool bPlaying = false;
        Fbox box; box.invalidate();
        for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) i_it->OnFrame(u_dt,m_Def->m_Effects[i_it-items.begin()],box,bPlaying);

        if (m_RT_Flags.is(flRT_DefferedStop)&&!bPlaying){
            m_RT_Flags.set		(flRT_Playing|flRT_DefferedStop,FALSE);
        }
        if (box.is_valid()){
        	vis.box.set			(box);
			vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
		}
	} else {
		vis.box.set			(m_InitialPosition,m_InitialPosition);
		vis.box.grow		(EPS_L);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}
}

void CParticleGroup::UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM)
{
	m_InitialPosition		= m.c;
    for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) 
    	i_it->UpdateParent(m,velocity,bXFORM);
}

BOOL CParticleGroup::Compile(CPGDef* def)
{
	m_Def 						= def;
	// destroy existing
    for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) 
    	i_it->Clear();
    items.clear();
    // create new
    if (m_Def){
        items.resize			(m_Def->m_Effects.size());
        for (CPGDef::EffectVec::const_iterator e_it=m_Def->m_Effects.begin(); e_it!=m_Def->m_Effects.end(); e_it++){
        	CParticleEffect* eff = (CParticleEffect*)RImplementation.model_CreatePE(*e_it->m_EffectName);
            eff->SetBirthDeadCB	(OnGroupParticleBirth,OnGroupParticleDead,this,e_it-m_Def->m_Effects.begin());
			items[e_it-def->m_Effects.begin()].Set(eff);
        }
    }
    return TRUE;
}

void CParticleGroup::Play()
{
	m_CurrentTime   = 0;
	m_RT_Flags.set	(flRT_DefferedStop,FALSE);
	m_RT_Flags.set 	(flRT_Playing,TRUE);
}

void CParticleGroup::Stop(BOOL bDefferedStop)
{
	if (bDefferedStop){
		m_RT_Flags.set	(flRT_DefferedStop,TRUE);
    }else{
    	m_RT_Flags.set	(flRT_Playing,FALSE);
    }
    for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) i_it->Stop(bDefferedStop);
}

void CParticleGroup::OnDeviceCreate()
{
    for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) i_it->OnDeviceCreate();
}

void CParticleGroup::OnDeviceDestroy()
{
    for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) i_it->OnDeviceDestroy();
}

u32 CParticleGroup::ParticlesCount()
{
	int p_count=0;
    for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++)
        p_count 	+= i_it->ParticlesCount();
	return p_count;
}

