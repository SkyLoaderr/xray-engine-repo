#ifndef PH_SYNCHRONIZE_H
#define PH_SYNCHRONIZE_H

#ifdef AI_COMPILER
	#include "net_utils.h"
#endif

struct SPHNetState
{
	Fvector		linear_vel;
	Fvector		angular_vel;
	Fvector		force;
	Fvector		torque;
	Fvector		position;
	Fvector		previous_position;
	union{
		Fquaternion quaternion;
		struct{
			Fvector	accel;
			float	max_velocity;
		};
	};
	Fquaternion	previous_quaternion;
	bool		enabled;
	void								net_Export			(		NET_Packet&		P);					
	void								net_Import			(		NET_Packet&		P);
	void								net_Save			(		NET_Packet&		P);					
	void								net_Load			(		NET_Packet&		P);
};
class CPHSynchronize
{
public:
	virtual void						net_Export			(		NET_Packet&		P)							{};					// export to server
	virtual void						net_Import			(		NET_Packet&		P)							{};
	virtual void						get_State			(		SPHNetState&	state)								=0;
	virtual void						set_State			(const	SPHNetState&	state)								=0;
	virtual void						cv2obj_Xfrom		(const Fquaternion& q,const Fvector& pos, Fmatrix& xform)	=0;
	virtual void						cv2bone_Xfrom		(const Fquaternion& q,const Fvector& pos, Fmatrix& xform)	=0;
protected:
private:
};
DEFINE_VECTOR(SPHNetState,PHNETSTATE_VECTOR,PHNETSTATE_I);
#endif