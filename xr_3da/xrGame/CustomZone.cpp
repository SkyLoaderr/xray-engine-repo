#include "stdafx.h"
#include "../xr_ioconsole.h"
#include "customzone.h"
#include "actor.h"
#include "hudmanager.h"
#include "ParticlesObject.h"

#define SMALL_OBJECT_RADIUS 0.6f


CCustomZone::CCustomZone(void) 
{
	m_fMaxPower = 100.f;
	m_fAttenuation = 1.f;
	m_dwPeriod = 1100;
	m_bZoneReady = false;
	m_pLocalActor = NULL;


	m_pIdleParticles = NULL;

	m_sIdleParticles = NULL;
	m_sBlowoutParticles = NULL;

	m_sHitParticlesSmall = NULL;
	m_sHitParticlesBig = NULL;
	m_sIdleObjectParticlesSmall = NULL;
	m_sIdleObjectParticlesBig = NULL;
}

CCustomZone::~CCustomZone(void) 
{
	SoundDestroy(m_idle_sound);
	SoundDestroy(m_blowout_sound);
	SoundDestroy(m_hit_sound);
}

void CCustomZone::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	m_fHitImpulseScale = pSettings->r_float(section,"hit_impulse_scale");	
	
	
//////////////////////////////////////////////////////////////////////////
	ISpatial*		self				=	dynamic_cast<ISpatial*> (this);
	if (self)		self->spatial.type	|=	STYPE_COLLIDEABLE;
//////////////////////////////////////////////////////////////////////////

	LPCSTR sound_str = NULL;
	sound_str = pSettings->r_string(section,"idle_sound");
	SoundCreate		(m_idle_sound, sound_str);
	sound_str = pSettings->r_string(section,"blowout_sound");
	SoundCreate		(m_blowout_sound, sound_str);
	sound_str = pSettings->r_string(section,"hit_sound");
	SoundCreate		(m_hit_sound, sound_str);

/*	string512		m_effectsSTR;
	strcpy			(m_effectsSTR,pSettings->r_string(section,"effects"));
	
	char* l_effectsSTR	= m_effectsSTR; 
	
	R_ASSERT		(l_effectsSTR);
	

	//no particles, that distinguish zone
	if(l_effectsSTR[0] == 'n' &&
		l_effectsSTR[1] == 'o' &&
		l_effectsSTR[2] == 'n' &&
		l_effectsSTR[3] == 'e')
	{
		m_effects.clear();
		return;
	}
	
	m_effects.clear		(); 
	m_effects.push_back	(l_effectsSTR);

	//parse the string
	while(*l_effectsSTR)
	{
		if(*l_effectsSTR == ',')
		{
			*l_effectsSTR = 0;
			++l_effectsSTR;

			while(*l_effectsSTR == ' ' || *l_effectsSTR == '\t')
				++l_effectsSTR;

			m_effects.push_back(l_effectsSTR);
		}
		++l_effectsSTR;
	}*/


	m_sIdleParticles	= pSettings->r_string(section,"idle_particles");
	m_sBlowoutParticles = pSettings->r_string(section,"blowout_particles");
}

BOOL CCustomZone::net_Spawn(LPVOID DC) 
{
	CCF_Shape *l_pShape			= xr_new<CCF_Shape>(this);
	collidable.model			= l_pShape;
	CSE_Abstract				*e = (CSE_Abstract*)(DC);
	CSE_ALifeAnomalousZone		*Z = dynamic_cast<CSE_ALifeAnomalousZone*>(e);
	
	for (u32 i=0; i < Z->shapes.size(); ++i) 
	{
		CSE_Shape::shape_def	&S = Z->shapes[i];
		switch (S.type) 
		{
			case 0 : l_pShape->add_sphere(S.data.sphere); break;
			case 1 : l_pShape->add_box(S.data.box); break;
		}
	}

	BOOL bOk = inherited::net_Spawn(DC);
	
	if (bOk)
	{
		l_pShape->ComputeBounds		();
		m_fMaxPower					= Z->m_maxPower;
		m_fAttenuation				= Z->m_attn;
		m_dwPeriod					= Z->m_period;

		Fvector						P;
		XFORM().transform_tiny		(P,CFORM()->getSphere().P);
		
		//	setVisible(true);
		setEnabled(true);

		PlayIdleParticles();	
		/*CParticlesObject* pStaticPG; s32 l_c = (int)m_effects.size();
		Fmatrix l_m; l_m.set(renderable.xform);
		Fvector zero_vel = {0.f,0.f,0.f};
		for(s32 i = 0; i < l_c; ++i) {
			Fvector c; c.set(l_m.c.x,l_m.c.y+EPS,l_m.c.z);
			IRender_Sector *l_pRS = ::Render->detectSector(c);
			pStaticPG = xr_new<CParticlesObject>(*m_effects[i],l_pRS,false);
			pStaticPG->UpdateParent(l_m,zero_vel);
			pStaticPG->Play();
			m_effectsPSs.push_back(pStaticPG);
		}*/
	}

	return bOk;
}

void CCustomZone::net_Destroy() 
{
	inherited::net_Destroy();
	
/*	while(m_effectsPSs.size()) 
	{ 
		xr_delete(*(m_effectsPSs.begin())); 
		m_effectsPSs.pop_front(); 
	}*/
	StopIdleParticles();
}

//void CCustomZone::Update(u32 dt) {
	//inherited::Update	(dt);
void CCustomZone::UpdateCL() 
{
	//u32 dt = Device.dwTimeDelta;
	inherited::UpdateCL();

	const Fsphere& s		= CFORM()->getSphere();
	Fvector					P;
	XFORM().transform_tiny(P,s.P);
	//feel_touch.clear(); m_inZone.clear();
	feel_touch_update		(P,s.R);

	if(m_bZoneReady) 
	{
		xr_set<CObject*>::iterator l_it;
		for(l_it = m_inZone.begin(); m_inZone.end() != l_it; ++l_it) 
		{
			Affect(*l_it);
		}
		m_bZoneReady = false;
	}
}

void CCustomZone::feel_touch_new(CObject* O) 
{
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"entering a zone.");
	m_inZone.insert(O);
	if(dynamic_cast<CActor*>(O) && O == Level().CurrentEntity()) 
					m_pLocalActor = dynamic_cast<CActor*>(O);

	PlayObjectIdleParticles(O);
}

void CCustomZone::feel_touch_delete(CObject* O) 
{
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"leaving a zone.");
	
	m_inZone.erase(O);

	if(dynamic_cast<CActor*>(O)) m_pLocalActor = NULL;

	StopObjectIdleParticles(O);
}

BOOL CCustomZone::feel_touch_contact(CObject* O) 
{
	if(O == this) return false;
	if(!O->Local() || !((CGameObject*)O)->IsVisibleForZones()) return false;
	
	return ((CCF_Shape*)CFORM())->Contact(O);
}

float CCustomZone::Power(float dist) 
{
	float radius = CFORM()->getRadius()*3/4.f;

//	f32 l_r = Visual()->vis.sphere.R;
//	return l_r < dist ? 0 : m_fMaxPower * (1.f - m_fAttenuation*dist/l_r);
	float power = radius < dist ? 0 : m_fMaxPower * (1.f - m_fAttenuation*(dist/radius)*(dist/radius));
	return power < 0 ? 0 : power;
}

void CCustomZone::SoundCreate(ref_sound& dest, LPCSTR s_name, int iType, BOOL /**bCtrlFreq/**/) 
{
	string256	temp;
	if (FS.exist(temp,"$game_sounds$",s_name,".ogg"))
	{
		Sound->create(dest,TRUE,s_name,iType);
		return;
	}
	Debug.fatal	("Can't find ref_sound '%s'",s_name,cName());
}

void CCustomZone::SoundDestroy(ref_sound& dest) 
{
	Sound->destroy(dest);
}

void CCustomZone::spatial_register()
{
	R_ASSERT2				(CFORM(),"Invalid or no CForm!");
	Fvector					P;
	XFORM().transform_tiny	(P,CFORM()->getSphere().P);
	spatial.center.set		(P);
	spatial.radius			= CFORM()->getRadius();
	ISpatial::spatial_register();
}

void CCustomZone::spatial_unregister()
{
	ISpatial::spatial_unregister();
}

void CCustomZone::spatial_move()
{
	R_ASSERT2				(CFORM(),"Invalid or no CForm!");
	Fvector					P;
	XFORM().transform_tiny	(P,CFORM()->getSphere().P);
	spatial.center.set		(P);
	spatial.radius			= CFORM()->getRadius();
	ISpatial::spatial_move	();
}

void CCustomZone::OnRender() 
{
	if(!bDebug) return;
	RCache.OnFrameEnd();
	Fvector l_half; l_half.set(.5f, .5f, .5f);
	Fmatrix l_ball, l_box;
	xr_vector<CCF_Shape::shape_def> &l_shapes = ((CCF_Shape*)CFORM())->Shapes();
	xr_vector<CCF_Shape::shape_def>::iterator l_pShape;
	
	for(l_pShape = l_shapes.begin(); l_shapes.end() != l_pShape; ++l_pShape) 
	{
		switch(l_pShape->type)
		{
		case 0:
			{
                Fsphere &l_sphere = l_pShape->data.sphere;
				l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R);
				//l_ball.scale(1.f, 1.f, 1.f);
				Fvector l_p; XFORM().transform(l_p, l_sphere.P);
				l_ball.translate_add(l_p);
				//l_ball.mul(XFORM(), l_ball);
				//l_ball.mul(l_ball, XFORM());
				RCache.dbg_DrawEllipse(l_ball, D3DCOLOR_XRGB(0,255,255));
			}
			break;
		case 1:
			{
				l_box.mul(XFORM(), l_pShape->data.box);
				RCache.dbg_DrawOBB(l_box, l_half, D3DCOLOR_XRGB(0,255,255));
			}
			break;
		}
	}
}


void CCustomZone::Affect(CObject* O) 
{
	CGameObject *pGameObject = dynamic_cast<CGameObject*>(O);
	if(!pGameObject) return;
	
	Fvector P; 
	XFORM().transform_tiny(P,CFORM()->getSphere().P);
	
	char l_pow[255]; 
	sprintf(l_pow, "zone hit. %.1f", Power(pGameObject->Position().distance_to(P)));
	if(bDebug) HUD().outMessage(0xffffffff,pGameObject->cName(), l_pow);
	
	Fvector hit_dir; 
	hit_dir.set(::Random.randF(-.5f,.5f), 
				::Random.randF(.0f,1.f), 
				::Random.randF(-.5f,.5f)); 
	hit_dir.normalize();
	
	//Fvector l_dir; l_dir.sub(l_pO->Position(), P); l_dir.normalize();
	//l_pO->ph_Movement.ApplyImpulse(l_dir, 50.f*Power(l_pO->Position().distance_to(P)));
	
	Fvector position_in_bone_space;
	
	float power = Power(pGameObject->Position().distance_to(P));
	float impulse = m_fHitImpulseScale*power*pGameObject->GetMass();
	
	if(power > 0.01f) 
	{
		m_dwDeltaTime = 0;
		position_in_bone_space.set(0.f,0.f,0.f);
		
		NET_Packet	l_P;
		u_EventGen	(l_P,GE_HIT, pGameObject->ID());
		l_P.w_u16	(u16(pGameObject->ID()));
		l_P.w_dir	(hit_dir);
		l_P.w_float	(power);
		l_P.w_s16	((s16)0);
		l_P.w_vec3	(position_in_bone_space);
		l_P.w_float	(impulse);
		l_P.w_u16	(eHitTypeWound);
		u_EventSend	(l_P);


		PlayHitParticles(pGameObject);
		PlayBlowoutParticles();
	}
	
}


void CCustomZone::PlayIdleParticles()
{
	Sound->play_at_pos	(m_idle_sound ,this, Position(), true);

	if(!m_sIdleParticles) return;
	m_pIdleParticles = xr_new<CParticlesObject>(*m_sIdleParticles,Sector(),false);
	m_pIdleParticles->UpdateParent(XFORM(),zero_vel);
	m_pIdleParticles->Play();
}

void CCustomZone::StopIdleParticles()
{
	SoundDestroy(m_idle_sound);

	if(!m_pIdleParticles) return;
	m_pIdleParticles->Stop();
	m_pIdleParticles->PSI_destroy();
	m_pIdleParticles = NULL;
}

void CCustomZone::PlayBlowoutParticles()
{
	Sound->play_at_pos	(m_blowout_sound ,this, Position());

	if(!m_sBlowoutParticles) return;

	CParticlesObject* pParticles;
	pParticles = xr_new<CParticlesObject>(*m_sBlowoutParticles,Sector());
	pParticles->UpdateParent(XFORM(),zero_vel);
	pParticles->Play();
}

void CCustomZone::PlayHitParticles(CGameObject* pObject)
{
	Sound->play_at_pos	(m_blowout_sound ,this, pObject->Position());

	CParticlesObject* pParticles = NULL;
	ref_str particle_str = NULL;

	//разные партиклы для объектов разного размера
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sHitParticlesSmall) return;
		particle_str = m_sHitParticlesSmall;
	}
	else
	{
		if(!m_sHitParticlesBig) return;
		particle_str = m_sHitParticlesBig;
	}

	pParticles = xr_new<CParticlesObject>(*particle_str,Sector());
	pParticles->UpdateParent(pObject->XFORM(),zero_vel);
	pParticles->Play();
}


void CCustomZone::PlayObjectIdleParticles(CObject* pObject)
{
	CParticlesObject* pParticles = NULL;
	ref_str particle_str = NULL;

	//разные партиклы для объектов разного размера
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sHitParticlesSmall) return;
		particle_str = m_sHitParticlesSmall;
	}
	else
	{
		if(!m_sHitParticlesBig) return;
		particle_str = m_sHitParticlesBig;
	}

	pParticles = xr_new<CParticlesObject>(*particle_str,Sector());
	pParticles->UpdateParent(pObject->XFORM(),zero_vel);
	pParticles->Play();
}
void CCustomZone::StopObjectIdleParticles(CObject* pObject)
{
}