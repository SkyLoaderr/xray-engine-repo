#ifndef I_COLLISION_DAMAGE_INFO_H
#define I_COLLISION_DAMAGE_INFO_H
class ICollisionDamageInfo
{
public:		
		virtual float					ContactVelocity				()				const						=0;
		virtual void					HitDir						(Fvector &dir)	const						=0;
		virtual const	Fvector&		HitPos						()				const						=0;
		virtual u16						DamageInitiatorID			()				const						=0;
		virtual CObject*				DamageInitiator				()				const						=0;
};
#endif