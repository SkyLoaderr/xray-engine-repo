void CPHSimpleCharacter::UpdateStaticDamage(dContact* c,SGameMtl* tri_material,bool bo1)
{
	const	dReal	*v			=	dBodyGetLinearVel(m_body);
			dReal	norm_prg	=	dFabs(dDOT(v,c->geom.normal));
			dReal	smag		=	dDOT(v,v);
			dReal	plane_pgr	=	_sqrt(smag-norm_prg*norm_prg);
			dReal	mag			=	0.f;
				if(tri_material->Flags.test(SGameMtl::flPassable))
				{
					mag					=	_sqrt(smag)*tri_material->fBounceDamageFactor;
				}
				else
				{
					float				vel_prg;vel_prg=_max(plane_pgr*tri_material->fPHFriction,norm_prg);
					mag					=	(vel_prg)*tri_material->fBounceDamageFactor;
				}
				if(mag>m_contact_velocity)
				{
  					m_contact_velocity	=	mag;
					m_dmc_signum		=	bo1 ? 1.f : -1.f;
					m_dmc_type			=	ctStatic;
					m_damege_contact	=	*c;
				}
}

void CPHSimpleCharacter::UpdateDynamicDamage(dContact* c,SGameMtl* obj_material,dBodyID b,bool bo1)
{
	const dReal* vel=dBodyGetLinearVel(m_body);
	dReal c_vel;
	dMass m;
	dBodyGetMass(b,&m);
	const dReal* obj_vel=dBodyGetLinearVel(b);
	dVector3 obj_vel_effective={obj_vel[0]*object_damage_factor,obj_vel[1]*object_damage_factor,obj_vel[2]*object_damage_factor};
	dVector3 obj_impuls={obj_vel_effective[0]*m.mass,obj_vel_effective[1]*m.mass,obj_vel_effective[2]*m.mass};
	dVector3 impuls={vel[0]*m_mass,vel[1]*m_mass,vel[2]*m_mass};
	dVector3 c_mas_impuls={obj_impuls[0]+impuls[0],obj_impuls[1]+impuls[1],obj_impuls[2]+impuls[2]};
	dReal cmass=m_mass+m.mass;
	dVector3 c_mass_vel={c_mas_impuls[0]/cmass,c_mas_impuls[1]/cmass,c_mas_impuls[2]/cmass};
	//dVector3 rel_impuls={obj_impuls[0]-impuls[0],obj_impuls[1]-impuls[1],obj_impuls[2]-impuls[2]};
	//c_vel=dFabs(dDOT(obj_vel,c->geom.normal)*_sqrt(m.mass/m_mass));
	dReal vel_prg=dDOT(vel,c->geom.normal);
	dReal obj_vel_prg=dDOT(obj_vel_effective,c->geom.normal);
	dReal c_mass_vel_prg=dDOT(c_mass_vel,c->geom.normal);

	//dReal kin_energy_start=dDOT(vel,vel)*m_mass/2.f+dDOT(obj_vel,obj_vel)*m.mass/2.f*object_damage_factor;
	//dReal kin_energy_end=dDOT(c_mass_vel,c_mass_vel)*cmass/2.f;

	dReal kin_energy_start=vel_prg*vel_prg*m_mass/2.f+obj_vel_prg*obj_vel_prg*m.mass/2.f;
	dReal kin_energy_end=c_mass_vel_prg*c_mass_vel_prg*cmass/2.f;

	dReal accepted_energy=(kin_energy_start-kin_energy_end);
	if(accepted_energy>0.f)
		c_vel=dSqrt(accepted_energy/m_mass*2.f)*obj_material->fBounceDamageFactor;
	else c_vel=0.f;

	if(c_vel>m_contact_velocity) 
	{
		m_contact_velocity=c_vel;
		m_dmc_signum=bo1 ? 1.f : -1.f;
		m_dmc_type=ctObject;
		m_damege_contact=*c;
	}
}