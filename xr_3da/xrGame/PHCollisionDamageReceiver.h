#pragma once

class CPhysicsShellHolder;
struct dContact;
struct SGameMtl;
class CPHCollisionDamageReceiver
{
xr_map<u16,float> m_controled_bones;
protected:
	virtual CPhysicsShellHolder*		PPhysicsShellHolder			()							=0;
			void						Init						()							;
			void						Hit							(u16 source_id,u16 bone_id,float power,const Fvector &dir,Fvector &pos)	;
private:
	static	void __stdcall				CollisionCallback			(bool& do_colide,dContact& c,SGameMtl* material_1,SGameMtl* material_2);
};