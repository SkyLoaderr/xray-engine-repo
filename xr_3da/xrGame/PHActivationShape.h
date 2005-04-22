#ifndef PH_ACTIVATION_SHAPE
#define PH_ACTIVATION_SHAPE
#endif

class CPHActivationShape : public CPHObject
{
dBodyID					m_body																																							;
dGeomID					m_geom																																							;
Flags16					m_flags																																							;
CSafeFixedRotationState m_safe_state																																					;			
public:
enum					EType
{
					etBox,
					etCylinder,
					etSphere
};

enum		{
					flFixedRotation,
					flFixedPosition,
					flGravity
			};
						CPHActivationShape					()																															;
						~CPHActivationShape					()																															;
			void		Create								(const Fvector start_pos,const Fvector start_size,CPhysicsShellHolder* ref_obj,EType type=etBox)							;
			void		Destroy								()																															;
			bool		Activate							(const Fvector need_size,u16 steps,float max_displacement,float max_rotation)												;															
const		Fvector		&Position							()																															;
			void		Size								(Fvector &size)																												;
private:
virtual		void		PhDataUpdate						(dReal step)																												;
virtual		void		PhTune								(dReal step)																												;
	virtual		void		CutVelocity						(float l_limit,float a_limit)																								;						
virtual		void		InitContact							(dContact* c,bool& do_collide,SGameMtl * ,SGameMtl * )																		;
virtual		dGeomID		dSpacedGeom							()																															;
virtual		void		get_spatial_params					()																															;
};