//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "..\psystem.h"
#include "ParticleEffect.h"

#ifdef _EDITOR
	#include "UI_Tools.h"
#endif
#ifdef _PARTICLE_EDITOR
	#include "ParticleEffectActions.h"
#endif

using namespace PAPI;
using namespace PS;

static int action_list_handle=-1;

//------------------------------------------------------------------------------
// class CParticleEffectDef
//------------------------------------------------------------------------------
CPEDef::CPEDef()
{                                         
    strcpy				(m_Name,"unknown");
    m_ShaderName		= 0;
    m_TextureName		= 0;
    m_Frame.InitDefault	();
    m_MaxParticles		= 0;
	m_CachedShader		= 0;
	m_fTimeLimit		= 0.f;
    // collision
    m_fCollideOneMinusFriction 	= 1.f;
    m_fCollideResilience		= 0.f;
    m_fCollideSqrCutoff			= 0.f;
    // velocity scale
    m_VelocityScale.set			(0.f,0.f,0.f);
    // align to path
    m_APDefaultRotation.set		(-PI_DIV_2,0.f,0.f);
	// flags
    m_Flags.zero		();
}

CPEDef::~CPEDef()
{
    xr_free				(m_ShaderName);
    xr_free				(m_TextureName);
    for (PAVecIt it=m_ActionList.begin(); it!=m_ActionList.end(); it++) xr_delete(*it);
}
void CPEDef::SetName(LPCSTR name)
{
    strcpy				(m_Name,name);
}
void CPEDef::pAlignToPath(float rot_x, float rot_y, float rot_z)
{
	m_Flags.set			(dfAlignToPath,TRUE);
    m_APDefaultRotation.set(rot_x,rot_y,rot_z);
}
void CPEDef::pVelocityScale(float scale_x, float scale_y, float scale_z)
{
	m_Flags.set			(dfVelocityScale,TRUE);
    m_VelocityScale.set	(scale_x, scale_y, scale_z);
}
void CPEDef::pCollision(float friction, float resilience, float cutoff, BOOL destroy_on_contact)
{
    m_fCollideOneMinusFriction 	= 1.f-friction;
    m_fCollideResilience		= resilience;
    m_fCollideSqrCutoff			= cutoff*cutoff;
	m_Flags.set					(dfCollision,TRUE);
	m_Flags.set					(dfCollisionDel,destroy_on_contact);
}

void CPEDef::pSprite(string128& sh_name, string128& tex_name)
{
    xr_free(m_ShaderName);	m_ShaderName	= xr_strdup(sh_name);
    xr_free(m_TextureName);	m_TextureName	= xr_strdup(tex_name);
	m_Flags.set	(dfSprite,TRUE);
}
void CPEDef::pFrame(BOOL random_frame, u32 frame_count, u32 tex_width, u32 tex_height, u32 frame_width, u32 frame_height)
{
	m_Flags.set			(dfFramed,TRUE);
	m_Flags.set			(dfRandomFrame,random_frame);
	m_Frame.Set			(frame_count, (float)tex_width, (float)tex_height, (float)frame_width, (float)frame_height);
}
void CPEDef::pAnimate(float speed, BOOL random_playback)
{
	m_Frame.m_fSpeed	= speed;
	m_Flags.set			(dfAnimated,TRUE);
	m_Flags.set			(dfRandomPlayback,random_playback);
}
void CPEDef::pTimeLimit(float time_limit)
{
	m_Flags.set			(dfTimeLimit,TRUE);
	m_fTimeLimit		= time_limit;
}
void CPEDef::pFrameInitExecute(ParticleEffect *effect)
{
	for(int i = 0; i < effect->p_count; i++){
		Particle &m = effect->list[i];
        if (m.flags.is(Particle::BIRTH)){
            if (m_Flags.is(dfRandomFrame))
                m.frame	= (u16)iFloor(Random.randI(m_Frame.m_iFrameCount)*255.f);
            if (m_Flags.is(dfAnimated)&&m_Flags.is(dfRandomPlayback)&&Random.randI(2))
                m.flags.set(Particle::ANIMATE_CCW,TRUE);
        }
    }
}
void CPEDef::pAnimateExecute(ParticleEffect *effect, float dt)
{
	float speedFac = m_Frame.m_fSpeed * dt;
	for(int i = 0; i < effect->p_count; i++){
		Particle &m = effect->list[i];
        float f						= (float(m.frame)/255.f+((m.flags.is(Particle::ANIMATE_CCW))?-1.f:1.f)*speedFac);
		if (f>m_Frame.m_iFrameCount)f-=m_Frame.m_iFrameCount;
		if (f<0.f)					f+=m_Frame.m_iFrameCount;
        m.frame						= (u16)iFloor(f*255.f);
	}
}

void CPEDef::pCollisionExecute(PAPI::ParticleEffect *effect, float dt, CParticleEffect* owner, CollisionCallback cb)
{
    pVector pt,n;
	// Must traverse list in reverse order so Remove will work
	for(int i = effect->p_count-1; i >= 0; i--){
		Particle &m = effect->list[i];

        bool pick_needed;
        int pick_cnt=0;
		do{		
        	pick_needed = false;
            Fvector 	dir;
            dir.sub		(m.pos,m.posB);
            float dist 	= dir.magnitude();
            if (dist>=EPS){
            	dir.div	(dist);
#ifdef _EDITOR                
                if (Tools.RayPick(m.posB,dir,dist,&pt,&n)){
#else
                Collide::rq_result	RQ;
                if (g_pGameLevel->ObjectSpace.RayPick(m.posB,dir,dist,Collide::rqtBoth,RQ)){	
                    pt.mad	(m.posB,dir,RQ.range);
                    if (RQ.O){
                    	n.set(0.f,1.f,0.f);
                    }else{
                    	CDB::TRI*	T		=  	g_pGameLevel->ObjectSpace.GetStaticTris()+RQ.element;
						Fvector*	verts	=	g_pGameLevel->ObjectSpace.GetStaticVerts();
						n.mknormal(verts[T->verts[0]],verts[T->verts[1]],verts[T->verts[2]]);
                    }
#endif
		            pick_cnt++;
                    if (cb&&(pick_cnt==1)) if (!cb(owner,m,pt,n)) break;
                    if (m_Flags.is(dfCollisionDel)) effect->Remove(i);
                    else{
                        // Compute tangential and normal components of velocity
                        float nmag = m.vel * n;
                        pVector vn(n * nmag); 	// Normal Vn = (V.N)N
                        pVector vt(m.vel - vn);	// Tangent Vt = V - Vn

                        // Compute _new velocity heading out:
                        // Don't apply friction if tangential velocity < cutoff
                        if(vt.length2() <= m_fCollideSqrCutoff){
                            m.vel = vt - vn * m_fCollideResilience;
                        }else{
                            m.vel = vt * m_fCollideOneMinusFriction - vn * m_fCollideResilience;
                        }
                        m.pos	= m.posB + m.vel * dt; 
                        pick_needed = true;
                    }
                }
            }else{
                m.pos	= m.posB;
            }
   		}while(pick_needed&&(pick_cnt<2));
	}
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPEDef::Load(IReader& F)
{
	R_ASSERT		(F.find_chunk(PED_CHUNK_VERSION));
	u16 version		= F.r_u16();

    if (version!=PED_VERSION)
    	return FALSE;

	R_ASSERT		(F.find_chunk(PED_CHUNK_NAME));
	F.r_stringZ		(m_Name);

	R_ASSERT		(F.find_chunk(PED_CHUNK_EFFECTDATA));
    m_MaxParticles	= F.r_u32();

    R_ASSERT		(F.find_chunk(PED_CHUNK_ACTIONLIST));
    m_ActionList.resize(F.r_u32());
    for (PAVecIt it=m_ActionList.begin(); it!=m_ActionList.end(); it++){
    	*it			= PAPI::pCreateAction((PActionEnum)F.r_u32());
        (*it)->Load	(F);
    }

	F.r_chunk		(PED_CHUNK_FLAGS,&m_Flags);

    string256		buf;
    if (m_Flags.is(dfSprite)){
        R_ASSERT	(F.find_chunk(PED_CHUNK_SPRITE));
        F.r_stringZ	(buf); m_ShaderName = xr_strdup(buf);
        F.r_stringZ	(buf); m_TextureName= xr_strdup(buf);
    }

    if (m_Flags.is(dfFramed)){
        R_ASSERT	(F.find_chunk(PED_CHUNK_FRAME));
        F.r			(&m_Frame,sizeof(SFrame));
    }

    if (m_Flags.is(dfTimeLimit)){
        R_ASSERT(F.find_chunk(PED_CHUNK_TIMELIMIT));
        m_fTimeLimit= F.r_float();
    }

    if (m_Flags.is(dfCollision)){
    	R_ASSERT(F.find_chunk(PED_CHUNK_COLLISION));
        m_fCollideOneMinusFriction 	= F.r_float();
        m_fCollideResilience		= F.r_float();
        m_fCollideSqrCutoff			= F.r_float();
    }

    if (m_Flags.is(dfVelocityScale)){
    	R_ASSERT(F.find_chunk(PED_CHUNK_VEL_SCALE));
        F.r_fvector3				(m_VelocityScale);
    }

    if (m_Flags.is(dfAlignToPath)){
    	if (F.find_chunk(PED_CHUNK_ALIGN_TO_PATH)){
	        F.r_fvector3			(m_APDefaultRotation);
        }
    }
    
//    m_Flags.and(dfAllFlags);
    
#ifdef _PARTICLE_EDITOR
    if (F.find_chunk(PED_CHUNK_OWNER)){
    	AnsiString tmp;
	    F.r_stringZ	(m_OwnerName);
	    F.r_stringZ	(m_ModifName);
        F.r			(&m_CreateTime,sizeof(m_CreateTime));
        F.r			(&m_ModifTime,sizeof(m_ModifTime));
    }
    
	if (F.find_chunk(PED_CHUNK_SOURCETEXT)){
	    F.r_stringZ	(m_SourceText);
//        Compile		();
    }

    if (F.find_chunk(PED_CHUNK_EDATA)){
    }    
#endif

    return TRUE;
}

void CPEDef::Save(IWriter& F)
{
	F.open_chunk	(PED_CHUNK_VERSION);
	F.w_u16			(PED_VERSION);
    F.close_chunk	();

	F.open_chunk	(PED_CHUNK_NAME);
    F.w_stringZ		(m_Name);
    F.close_chunk	();

	F.open_chunk	(PED_CHUNK_EFFECTDATA);
    F.w_u32			(m_MaxParticles);
    F.close_chunk	();

	F.open_chunk	(PED_CHUNK_ACTIONLIST);
    F.w_u32			(m_ActionList.size());
    for (PAVecIt it=m_ActionList.begin(); it!=m_ActionList.end(); it++){
    	F.w_u32		((*it)->type);
        (*it)->Save	(F);
    }
    F.close_chunk	();

	F.w_chunk		(PED_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

    if (m_Flags.is(dfSprite)){
        F.open_chunk	(PED_CHUNK_SPRITE);
        F.w_stringZ		(m_ShaderName);
        F.w_stringZ		(m_TextureName);
        F.close_chunk	();
    }

    if (m_Flags.is(dfFramed)){
        F.open_chunk	(PED_CHUNK_FRAME);
        F.w				(&m_Frame,sizeof(SFrame));
        F.close_chunk	();
    }

    if (m_Flags.is(dfTimeLimit)){
        F.open_chunk	(PED_CHUNK_TIMELIMIT);
        F.w_float		(m_fTimeLimit);
        F.close_chunk	();
    }

    if (m_Flags.is(dfCollision)){
        F.open_chunk	(PED_CHUNK_COLLISION);
        F.w_float		(m_fCollideOneMinusFriction);
        F.w_float		(m_fCollideResilience);
        F.w_float		(m_fCollideSqrCutoff);
        F.close_chunk	();
    }
    
    if (m_Flags.is(dfVelocityScale)){
        F.open_chunk	(PED_CHUNK_VEL_SCALE);
        F.w_fvector3	(m_VelocityScale);
        F.close_chunk	();
    }

    if (m_Flags.is(dfAlignToPath)){
        F.open_chunk	(PED_CHUNK_ALIGN_TO_PATH);
        F.w_fvector3	(m_APDefaultRotation);
        F.close_chunk	();
    }
#ifdef _PARTICLE_EDITOR
	F.open_chunk	(PED_CHUNK_OWNER);
    F.w_stringZ		(m_OwnerName);
    F.w_stringZ		(m_ModifName);
    F.w				(&m_CreateTime,sizeof(m_CreateTime));
    F.w				(&m_ModifTime,sizeof(m_ModifTime));
	F.close_chunk	();

	F.open_chunk	(PED_CHUNK_SOURCETEXT);
    F.w_stringZ		(m_SourceText.c_str());
	F.close_chunk	();
#endif
}
//------------------------------------------------------------------------------
// class CParticleEffect
//------------------------------------------------------------------------------
CParticleEffect::CParticleEffect()
{
	m_HandleEffect 			= pGenParticleEffects(1, 1);
    m_HandleActionList		= pGenActionLists();
    m_RT_Flags.zero			();
    m_Def					= 0;
    m_fElapsedLimit			= 0.f;
	m_MemDT					= 0;
	m_InitialPosition.set	(0,0,0);
	m_DestroyCallback		= 0;
	m_CollisionCallback		= 0;
    m_XFORM.identity		();
}
CParticleEffect::~CParticleEffect()
{
	OnDeviceDestroy			();
	pDeleteParticleEffects	(m_HandleEffect);
	pDeleteActionLists		(m_HandleActionList);
}

void CParticleEffect::OnDeviceCreate()
{
	if (m_Def){
        if (m_Def->m_Flags.is(CPEDef::dfSprite)){
            hGeom.create	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
            if (m_Def) hShader = m_Def->m_CachedShader;
        }
    }
}

void CParticleEffect::OnDeviceDestroy()
{
	if (m_Def){
        if (m_Def->m_Flags.is(CPEDef::dfSprite)){
            hGeom.destroy		();
            hShader.destroy		();
        }    
    }
}

void CParticleEffect::Play()
{
	m_RT_Flags.zero		();
	m_RT_Flags.set		(flRT_Playing,TRUE);
   	pStartPlaying		(m_HandleActionList);
}
void CParticleEffect::Stop(BOOL bDefferedStop)
{
	if (bDefferedStop){
		m_RT_Flags.set	(flRT_DefferedStop,TRUE);
    	pStopPlaying	(m_HandleActionList);
    }else{
    	m_RT_Flags.set	(flRT_Playing,FALSE);
		ResetParticles	();
    }
}
void CParticleEffect::RefreshShader()
{
	OnDeviceDestroy();
    OnDeviceCreate();
}

void CParticleEffect::ResetParticles()
{
	if (m_Def){
        pSetMaxParticlesG	(m_HandleEffect,0);
        pSetMaxParticlesG	(m_HandleEffect,m_Def->m_MaxParticles);
    }
}

void CParticleEffect::UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM)
{
    m_RT_Flags.set			(flRT_XFORM, bXFORM);
    if (bXFORM)				m_XFORM.set	(m);
	else{
		m_InitialPosition	= m.c;
		pSetActionListParenting	(m_HandleActionList,m,velocity);
	}
}

static const u32	uDT_STEP = 33;
static const float	fDT_STEP = float(uDT_STEP)/1000.f;

void CParticleEffect::OnFrame(u32 frame_dt)
{
	if (m_Def && m_RT_Flags.is(flRT_Playing)){
		m_MemDT			+= frame_dt;
		for (;m_MemDT>=uDT_STEP; m_MemDT-=uDT_STEP){
            if (m_Def->m_Flags.is(CPEDef::dfTimeLimit)){ 
				if (!m_RT_Flags.is(flRT_DefferedStop)){
                    m_fElapsedLimit -= fDT_STEP;
                    if (m_fElapsedLimit<0.f){
                        m_fElapsedLimit = m_Def->m_fTimeLimit;
                        Stop		(true);
                    }
                }
            }
			pTimeStep			(fDT_STEP);
			pCurrentEffect		(m_HandleEffect);

			// execute action list
			pCallActionList		(m_HandleActionList);

			if (action_list_handle>-1) 
            	pCallActionList	(action_list_handle);
            
			ParticleEffect *pg 	= _GetEffectPtr(m_HandleEffect);
			// our actions
			if (m_Def->m_Flags.is(CPEDef::dfFramed))    		  		m_Def->pFrameInitExecute(pg);
			if (m_Def->m_Flags.is(CPEDef::dfFramed|CPEDef::dfAnimated))	m_Def->pAnimateExecute	(pg,fDT_STEP);
            if (m_Def->m_Flags.is(CPEDef::dfCollision)) 				m_Def->pCollisionExecute(pg,fDT_STEP,this,m_CollisionCallback);

			//-move action
			if (pg->p_count)	
			{
				vis.box.invalidate	();
				float p_size = 0.f;
				for(int i = 0; i < pg->p_count; i++){
					Particle &m 	= pg->list[i]; 
					if (m.flags.is(Particle::DYING)){if (m_DestroyCallback) m_DestroyCallback(this,m);}
					if (m.flags.is(Particle::BIRTH))m.flags.set(Particle::BIRTH,FALSE);
					vis.box.modify((Fvector&)m.pos);
					if (m.size.x>p_size) p_size = m.size.x;
					if (m.size.y>p_size) p_size = m.size.y;
					if (m.size.z>p_size) p_size = m.size.z;
				}
				vis.box.grow		(p_size);
				vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
			}
            if (m_RT_Flags.is(flRT_DefferedStop)&&(0==pg->p_count)){
				m_RT_Flags.set		(flRT_Playing|flRT_DefferedStop,FALSE);
                break;
            }
		}
	} else {
		vis.box.set			(m_InitialPosition,m_InitialPosition);
		vis.box.grow		(EPS_L);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}
}

BOOL CParticleEffect::Compile(CPEDef* def)
{
	m_Def 						= def;
    if (m_Def){
        // set current effect for action
        pCurrentEffect			(m_HandleEffect);
        // refresh shader
        RefreshShader			();
        // reset particles
        ResetParticles			();
        // load action list
        // get pointer to specified action list.
        if (!_GetListPtr(m_HandleActionList)) return FALSE;

        // append actions
        pNewActionList			(m_HandleActionList);
	    for (PAVecIt it=m_Def->m_ActionList.begin(); it!=m_Def->m_ActionList.end(); it++)
            pAddActionToList	(*it);
        pEndActionList();
        
        // time limit
		if (m_Def->m_Flags.is(CPEDef::dfTimeLimit))
			m_fElapsedLimit 	= m_Def->m_fTimeLimit;
    }
	if (def)	hShader			= def->m_CachedShader;
	return TRUE;
}

//------------------------------------------------------------------------------
// Render
//------------------------------------------------------------------------------
void CParticleEffect::Copy(IRender_Visual* pFrom)
{
	Debug.fatal("Can't duplicate particle system - NOT IMPLEMENTED");
}

//----------------------------------------------------
IC void FillSprite	(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float a, float b, u32 clr, float angle, float scale, float w_2, float h_2)
{
	FVF::TL			PT;

	PT.transform	(pos, M);
//	float sz		= scale * radius / PT.p.w;
    a				*= scale / PT.p.w;
    b				*= scale / PT.p.w;
	// 'Cause D3D clipping have to clip Four points
	// We can help him :)

	if (a<1.f)	return;
	if (b<1.f)	return;
	if (PT.p.x<-1)	return;
	if (PT.p.x> 1)	return;
	if (PT.p.y<-1)	return;
	if (PT.p.y> 1)	return;
	if (PT.p.z< 0) 	return;

	// Convert to screen coords
	Fvector2	c;
	c.x				= (PT.p.x+1)*w_2;
	c.y				= (PT.p.y+1)*h_2;

	// Rotation                         
	float	sa,ca,s_a,c_a,s_b,c_b;
	sa	= _sin(angle);  
    ca	= _cos(angle);  
    s_a	= sa*a;
    c_a	= ca*a;
    s_b	= sa*b;
    c_b	= ca*b;

	pv->set	(c.x-c_a-s_b,	c.y-s_a+c_b,	PT.p.z, PT.p.w, clr, lt.x,rb.y);	pv++;
	pv->set	(c.x-c_a+s_b,	c.y-s_a-c_b,	PT.p.z, PT.p.w, clr, lt.x,lt.y);	pv++;
	pv->set	(c.x+c_a-s_b,	c.y+s_a+c_b,	PT.p.z, PT.p.w, clr, rb.x,rb.y);	pv++;
	pv->set	(c.x+c_a+s_b,	c.y+s_a-c_b,	PT.p.z, PT.p.w, clr, rb.x,lt.y);	pv++;
}

IC void FillSprite	(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float size_x, float size_y, u32 clr, const Fvector& D, float scale, float factor, float w_2, float h_2)
{
	Fvector			P1,P2;

	P1.mad			(pos,D,-size_x*factor);
	P2.mad			(pos,D,size_x*factor);

	FVF::TL			s1,s2;
	s1.transform	(P1,M);
	s2.transform	(P2,M);

	if ((s1.p.w<=0)||(s2.p.w<=0)) return;

    float l1,l2		= 0.7071f*scale*size_y; l1 = l2;
	l1				/= s1.p.w;
	l2				/= s2.p.w;

	Fvector2		dir,R;
	R.cross			(dir.set(s2.p.x-s1.p.x,s2.p.y-s1.p.y).norm());
	s1.p.x = (s1.p.x+1)*w_2; s1.p.y	= (s1.p.y+1)*h_2;
	s2.p.x = (s2.p.x+1)*w_2; s2.p.y	= (s2.p.y+1)*h_2;
	pv->set			(s1.p.x+l1*R.x,	s1.p.y+l1*R.y,	s2.p.z, s2.p.w, clr, lt.x,rb.y);	pv++;
	pv->set			(s2.p.x+l2*R.x,	s2.p.y+l2*R.y,	s2.p.z, s2.p.w, clr, lt.x,lt.y);	pv++;
	pv->set			(s1.p.x-l1*R.x,	s1.p.y-l1*R.y,	s2.p.z, s2.p.w, clr, rb.x,rb.y);	pv++;
	pv->set			(s2.p.x-l2*R.x,	s2.p.y-l2*R.y,	s2.p.z, s2.p.w, clr, rb.x,lt.y);	pv++;
}

void CParticleEffect::Render(float LOD)
{
	u32			dwOffset,dwCount;
    // Get a pointer to the particles in gp memory
	ParticleEffect *pe 		= _GetEffectPtr(m_HandleEffect);
    if((pe!=NULL)&&(pe->p_count>0)){
        if (m_Def->m_Flags.is(CPEDef::dfSprite)){
            // build transform matrix
            Fmatrix mSpriteTransform;

            if (m_RT_Flags.is(flRT_XFORM))	mSpriteTransform.mul(Device.mFullTransform,m_XFORM);
            else							mSpriteTransform.set(Device.mFullTransform);

            float	w_2			= float(::Render->getTarget()->get_width()) / 2;
            float	h_2			= float(::Render->getTarget()->get_height()) / 2;
            float	fov_rate	= (Device.fFOV/90.f);
            float	fov_scale_w	= float(::Render->getTarget()->get_width()) / fov_rate;
            float 	factor_r	= (0.2952f*fov_rate*fov_rate - 0.0972f*fov_rate + 0.8007f) * (1.41421356f/Device.fASPECT); // � ������ ������� ��������� �����������

            FVF::TL* pv_start	= (FVF::TL*)RCache.Vertex.Lock(pe->p_count*4*4,hGeom->vb_stride,dwOffset);
            FVF::TL* pv			= pv_start;

            for(int i = 0; i < pe->p_count; i++){
                PAPI::Particle &m = pe->list[i];

                Fvector2 lt,rb;
                lt.set			(0.f,0.f);
                rb.set			(1.f,1.f);
                if (m_Def->m_Flags.is(CPEDef::dfFramed)) m_Def->m_Frame.CalculateTC(iFloor(float(m.frame)/255.f),lt,rb);
                float r_x		= m.size.x*0.5f;
                float r_y		= m.size.y*0.5f;
                if (m_Def->m_Flags.is(CPEDef::dfVelocityScale)){
                    float speed	= m.vel.magnitude();
                    r_x			+= speed*m_Def->m_VelocityScale.x;
                    r_y			+= speed*m_Def->m_VelocityScale.y;
                }
                if (m_Def->m_Flags.is(CPEDef::dfAlignToPath)){
                    Fvector 	dir;
                    float speed	= m.vel.magnitude();
                    if (speed>=EPS_S)	dir.div	(m.vel,speed);
                    else				dir.setHP(-m_Def->m_APDefaultRotation.y,-m_Def->m_APDefaultRotation.x);
                    FillSprite	(pv,mSpriteTransform,m.pos,lt,rb,r_x,r_y,m.color,dir,fov_scale_w,factor_r,w_2,h_2);
                }else{
                    FillSprite	(pv,mSpriteTransform,m.pos,lt,rb,r_x,r_y,m.color,m.rot.x,fov_scale_w,w_2,h_2);
                }
            }
            dwCount 			= u32(pv-pv_start);
            RCache.Vertex.Unlock(dwCount,hGeom->vb_stride);
            if (dwCount)    {
                RCache.set_Geometry	(hGeom);
                RCache.Render	   	(D3DPT_TRIANGLELIST,dwOffset,0,dwCount,0,dwCount/2);
            }
        }
    }
}

u32 CParticleEffect::ParticlesCount()
{
	ParticleEffect *pe 		= _GetEffectPtr(m_HandleEffect);
	return pe?pe->p_count:0;
}

void CParticleEffect::ApplyExplosion()
{
/*
//    pExplosion			(0,0,0, 1, 1, 0.1f, EPS_L, 1);
	pExplosion			(0,0,0, 1, 8, 3, 0.1, 1.0f);
*/
    pCurrentEffect		(m_HandleEffect);

    action_list_handle	= pGenActionLists();
	pNewActionList		(action_list_handle);
	pExplosion			(0,0,0, 1, 8, 3, 0.1f, 1.0f);
	pEndActionList		();
}



/*
::Load
    if (m_Flags.is(dfObject)){
        R_ASSERT	(F.find_chunk(PED_CHUNK_MODEL));
        F.r_stringZ	(buf); m_ObjectName = xr_strdup(buf.c_str());
    }
::Save
	if (m_Flags.is(dfObject)){
        F.open_chunk	(PED_CHUNK_MODEL);
        F.w_stringZ		(m_ObjectName);
        F.close_chunk	();
    }
void CPEDef::pObject(string128& obj_name)
{
	if (m_Flags.is(dfSprite)){
    	ELog.DlgMsg		(mtError,"Sprite and Object can't work jointly.");
    	return;
    }
    xr_free(m_ObjectName);	m_ObjectName	= xr_strdup(obj_name);
	m_Flags.set	(dfObject,TRUE);
}
::Render
        else if (m_Def->m_Flags.is(CPEDef::dfObject)){
            // Get a pointer to the particles in pe memory
            PAPI::ParticleEffect *pe = PAPI::_GetEffectPtr(m_HandleEffect);
            if((pe!=NULL)&&(pe->p_count>0)){
                for(int i = 0; i < pe->p_count; i++){
                    PAPI::Particle &m = pe->list[i];
                    Fmatrix M,S;
                    M.setXYZi	(m.rot.x,m.rot.y,m.rot.z);
					S.scale		(m.size);
					M.mulB		(S);
                    M.translate_over(m.pos);
					::Render->set_Transform(&M);
					::Render->add_Visual   (m_Object);
                }
            }
        }
*/
