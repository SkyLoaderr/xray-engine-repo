#include "stdafx.h"
#include "ParticlesObject.h"
#include "gamemtllib.h"
#include "level.h"
#include "Extendedgeom.h"
#include "PhysicsGamePars.h"
#include "PhysicsCommon.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


void __stdcall ContactShotMark(CDB::TRI* T,dContactGeom* c)
{
	dBodyID b=dGeomGetBody(c->g1);
	dxGeomUserData* data;
	if(!b) 
	{
		b=dGeomGetBody(c->g2);
		data=dGeomGetUserData(c->g2);
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
	{
		if(data)
		{
			SGameMtlPair* mtl_pair		= GMLib.GetMaterialPair(T->material,data->material);
			//	char buf[40];
			//	R_ASSERT3(mtl_pair,strconcat(buf,"Undefined material pair:  # ", GMLib.GetMaterial(T->material)->name),GMLib.GetMaterial(data->material)->name);
			if(mtl_pair)
			{
				if(vel_cret>vel_cret_wallmark && !mtl_pair->CollideMarks.empty())
				{

					ref_shader pWallmarkShader = mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];

					//добавить отметку на материале
					::Render->add_Wallmark(pWallmarkShader, *((Fvector*)c->pos), 
						0.09f, T,
						Level().ObjectSpace.GetStaticVerts());

					//::Render->add_Wallmark	(
					//SELECT_RANDOM(mtl_pair->HitMarks),
					//*((Fvector*)c->pos),
					//0.09f,
					//T);

				}

				SGameMtl* mtl = NULL;
				SGameMtl* mtl1 = NULL;
				if(vel_cret>vel_cret_sound)
				{
					mtl  = GMLib.GetMaterialByID(mtl_pair->GetMtl0());
					mtl1 = GMLib.GetMaterialByID(mtl_pair->GetMtl1());

					if(!mtl_pair->CollideSounds.empty())
					{
						ref_sound& sound= SELECT_RANDOM1(mtl_pair->CollideSounds);
						float volume=collide_volume_min+vel_cret*(collide_volume_max-collide_volume_min)/(_sqrt(mass_limit)*default_l_limit-vel_cret_sound);
						::Sound->play_at_pos(
							 sound,0,*((Fvector*)c->pos)
							);
						sound.set_volume(volume);
					}
				}


				if(vel_cret>vel_cret_particlles && !mtl_pair->CollideParticles.empty())
				{
					LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];
					//отыграть партиклы столкновения материалов
					CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

					Fmatrix pos; 
					Fvector zero_vel = {0.f,0.f,0.f};
					pos.k.set(*((Fvector*)c->normal));
					Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
					pos.c.set(*((Fvector*)c->pos));

					ps->UpdateParent(pos,zero_vel);
					Level().ps_needtoplay.push_back(ps);
				}
			}
		}

		//			::Render->add_Wallmark	(
		//				CPHElement::hWallmark,
		//				*((Fvector*)c->pos),
		//				0.09f,
		//				T);

	} 
}
