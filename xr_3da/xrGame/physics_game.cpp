#include "stdafx.h"
#include "ParticlesObject.h"
#include "gamemtllib.h"
#include "level.h"
#include "Extendedgeom.h"
#include "PhysicsGamePars.h"
#include "PhysicsCommon.h"
#include "PhSoundPlayer.h"
#include "PhysicsShellHolder.h"
#include "PHCommander.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

class CPHParticlesPlayCall :
		public CPHAction
{
LPCSTR ps_name;
dContactGeom c;
public:
	CPHParticlesPlayCall(const dContactGeom &contact,bool invert_n,LPCSTR psn)
	{
		ps_name=psn;
		c=contact;
		if(invert_n)
		{
			c.normal[0]=-c.normal[0];c.normal[1]=-c.normal[1];c.normal[2]=-c.normal[2];
		}
	}
	virtual void 			run								()
	{
		CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

		Fmatrix pos; 
		Fvector zero_vel = {0.f,0.f,0.f};
		pos.k.set(*((Fvector*)c.normal));
		Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
		pos.c.set(*((Fvector*)c.pos));

		ps->UpdateParent(pos,zero_vel);
		Level().ps_needtoplay.push_back(ps);
	};
	virtual bool 			obsolete						()const{return false;}
};


class CPHWallMarksCall :
	public CPHAction
{
	ref_shader pWallmarkShader;
	Fvector pos;
	CDB::TRI* T;
public:
	CPHWallMarksCall(const Fvector &p,CDB::TRI* Tri,ref_shader s)
	{
		pWallmarkShader=s;
		pos.set(p);
		T=Tri;
	}
	virtual void 			run								()
	{
		//добавить отметку на материале
		::Render->add_StaticWallmark(pWallmarkShader,pos, 
			0.09f, T,
			Level().ObjectSpace.GetStaticVerts());
	};
	virtual bool 			obsolete						()const{return false;}
};




template<class Pars>
void __stdcall TContactShotMark(CDB::TRI* T,dContactGeom* c)
{
	dBodyID b=dGeomGetBody(c->g1);
	dxGeomUserData* data;
	bool b_invert_normal=false;
	if(!b) 
	{
		b=dGeomGetBody(c->g2);
		data=dGeomGetUserData(c->g2);
		b_invert_normal=true;
	}
	else
	{
		data=dGeomGetUserData(c->g1);
	}
	if(!b) return;
	dVector3 vel;
	dMass m;
	dBodyGetMass(b,&m);
	dBodyGetPointVel(b,c->pos[0],c->pos[1],c->pos[2],vel);
	dReal vel_cret=dFabs(dDOT(vel,c->normal))* _sqrt(m.mass);

	if(data)
	{
		SGameMtlPair* mtl_pair		= GMLib.GetMaterialPair(T->material,data->material);
		if(mtl_pair)
		{
			if(vel_cret>Pars.vel_cret_wallmark && !mtl_pair->CollideMarks.empty())
			{

				ref_shader pWallmarkShader = mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];

				Level().ph_commander().add_call(xr_new<CPHOnesCondition>(),xr_new<CPHWallMarksCall>( *((Fvector*)c->pos),T,pWallmarkShader));
			}
			SGameMtl* static_mtl =  GMLib.GetMaterialByIdx(T->material);
			if(!static_mtl->Flags.test(SGameMtl::flPassable))
			{
				if(vel_cret>Pars.vel_cret_sound)
				{
					if(!mtl_pair->CollideSounds.empty())
					{
						ref_sound& sound= SELECT_RANDOM1(mtl_pair->CollideSounds);
						float volume=collide_volume_min+vel_cret*(collide_volume_max-collide_volume_min)/(_sqrt(mass_limit)*default_l_limit-Pars.vel_cret_sound);
						::Sound->play_at_pos_unlimited(
							sound,0,*((Fvector*)c->pos)
							);
						sound.set_volume(volume);
					}
				}
			}
			else
			{
				if(data->ph_ref_object&&!mtl_pair->CollideSounds.empty())
				{
					CPHSoundPlayer* sp=NULL;
					sp=data->ph_ref_object->ph_sound_player();
					if(sp) sp->Play(mtl_pair,*(Fvector*)c->pos);
				}
			}
			if(vel_cret>Pars.vel_cret_particles && !mtl_pair->CollideParticles.empty())
			{
				LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];
				//отыграть партиклы столкновения материалов
				Level().ph_commander().add_call(xr_new<CPHOnesCondition>(),xr_new<CPHParticlesPlayCall>(*c,b_invert_normal,ps_name));
			}
		}
	}
 }


ContactCallbackFun *ContactShotMark = &TContactShotMark<EffectPars>;
ContactCallbackFun *CharacterContactShotMark = &TContactShotMark<CharacterEffectPars>;