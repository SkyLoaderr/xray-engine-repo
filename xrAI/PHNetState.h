#ifndef  PHNETSTATE_H
#define  PHNETSTATE_H

class NET_Packet;

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
	void								net_Save			(		NET_Packet&		P,const Fvector& min,const Fvector& max);					
	void								net_Load			(		NET_Packet&		P,const Fvector& min,const Fvector& max);
};

DEFINE_VECTOR(SPHNetState,PHNETSTATE_VECTOR,PHNETSTATE_I);

struct SPHBonesData 
{
	u64				  bones_mask;
	u16				  root_bone;
	PHNETSTATE_VECTOR bones;
	Fvector			  min;
	Fvector			  max;
public:
										SPHBonesData		()						  ;
	void								net_Save			(		NET_Packet&		P);					
	void								net_Load			(		NET_Packet&		P);
};
#endif