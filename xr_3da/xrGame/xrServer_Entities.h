#ifndef xrServer_EntitiesH
#define xrServer_EntitiesH

#include "xrMessages.h"
#include "net_utils.h"
#ifdef _EDITOR
	#include "PropertiesListHelper.h"
#endif
#include "xrServer_Entity_Custom.h"
#include "ai_alife_server_objects.h"

// refs
//class xrServerEntity;
//class xrClientData;

// t-defs
struct	SRotation
{
	float  yaw, pitch;
	SRotation() { yaw=pitch=0; }
};

// 
//***** CFormed (Base)
class xrSE_CFormed
{
public:
	enum{
    	cfSphere=0,
        cfBox
    };
	union shape_data
	{
		Fsphere		sphere;
		Fmatrix		box;
	};
	struct shape_def
	{
		u8			type;
		shape_data	data;
	};
    DEFINE_VECTOR	(shape_def,ShapeVec,ShapeIt);
	ShapeVec		shapes;
public:
	void			cform_read			(NET_Packet& P);
	void			cform_write			(NET_Packet& P);
};

// Some preprocessor help
#ifdef _EDITOR
#define xrSE_EDITOR_METHODS	virtual void FillProp(LPCSTR pref, PropItemVec& values);
#else
#define xrSE_EDITOR_METHODS	
#endif

//

//
#define xrSE_DECLARE_BEGIN(__A,__B)	class __A : public __B	{ typedef __B inherited; public:

//
#define	xrSE_DECLARE_END \
public:\
virtual void UPDATE_Read	(NET_Packet& P); \
virtual void UPDATE_Write	(NET_Packet& P); \
virtual void STATE_Read		(NET_Packet& P, u16 size); \
virtual void STATE_Write	(NET_Packet& P); \
xrSE_EDITOR_METHODS\
};

//***** Weapon
xrSE_DECLARE_BEGIN(xrSE_Weapon,CALifeObject)
	u32						timestamp;
	u8						flags;
	u8						state;

	u16						a_current;
	u16						a_elapsed;

	xrSE_Weapon				();

	virtual void			OnEvent			(NET_Packet& P, u16 type, u32 time, u32 sender );

	u8						get_slot		();
	u16						get_ammo_limit	();
	u16						get_ammo_total	();
	u16						get_ammo_elapsed();
	u16						get_ammo_magsize();
xrSE_DECLARE_END

//***** Teamed
xrSE_DECLARE_BEGIN(xrSE_Teamed,CALifeObject)
	u8						s_team;
	u8						s_squad;
	u8						s_group;

	virtual u8				g_team()			{ return s_team;	}
	virtual u8				g_squad()			{ return s_squad;	}
	virtual u8				g_group()			{ return s_group;	}
    xrSE_Teamed				();
xrSE_DECLARE_END

//***** Dummy
xrSE_DECLARE_BEGIN(xrSE_Dummy,xrServerEntity)
	enum SStyle{
		esAnimated			=1<<0,	
		esModel				=1<<1, 
		esParticles			=1<<2, 
		esSound				=1<<3,
		esRelativePosition	=1<<4
	};
	u8						s_style;
	char*					s_Animation;
	char*					s_Model;
	char*					s_Particles;
	char*					s_Sound;
    xrSE_Dummy				();
    virtual ~xrSE_Dummy		();
xrSE_DECLARE_END

//***** MercuryBall
xrSE_DECLARE_BEGIN(xrSE_MercuryBall,CALifeItem)
	string64				s_Model;
    xrSE_MercuryBall		();
xrSE_DECLARE_END

//***** Car
xrSE_DECLARE_BEGIN(xrSE_Car,xrSE_Teamed)
xrSE_DECLARE_END

//***** Crow
xrSE_DECLARE_BEGIN(xrSE_Crow,xrServerEntity)
xrSE_DECLARE_END

//***** Health
xrSE_DECLARE_BEGIN(xrSE_Health,CALifeObject)
	u8						amount;
xrSE_DECLARE_END

//***** Target
xrSE_DECLARE_BEGIN(xrSE_Target,xrServerEntity)
xrSE_DECLARE_END

//***** Target Assault
xrSE_DECLARE_BEGIN(xrSE_Target_Assault,xrSE_Target)
xrSE_DECLARE_END

//***** Target CS Base
xrSE_DECLARE_BEGIN(xrSE_Target_CSBase,xrSE_Target)
	float					radius;
	u8						s_team;
	xrSE_Target_CSBase		();
xrSE_DECLARE_END

//***** Target CS Cask
xrSE_DECLARE_BEGIN(xrSE_Target_CSCask,xrSE_Target)
	string64				s_Model;
	xrSE_Target_CSCask();
xrSE_DECLARE_END

//***** Target CS
xrSE_DECLARE_BEGIN(xrSE_Target_CS,xrSE_Target)
	string64				s_Model;
	xrSE_Target_CS();
xrSE_DECLARE_END

//***** Spectator
xrSE_DECLARE_BEGIN(xrSE_Spectator,xrServerEntity)
xrSE_DECLARE_END

//***** Actor
xrSE_DECLARE_BEGIN(xrSE_Actor,xrSE_Teamed)
	u32						timestamp;
	u8						flags;
	u16						mstate;
	float					model_yaw;
	SRotation				torso;
	Fvector					accel;
	Fvector					velocity;
	float					fHealth;
	float					fArmor;
	u8						weapon;
xrSE_DECLARE_END

//***** Enemy
xrSE_DECLARE_BEGIN(xrSE_Enemy,xrSE_Teamed)
	u32						timestamp;				// server(game) timestamp
	u8						flags;
	float					o_model;				// model yaw
	SRotation				o_torso;				// torso in world coords
xrSE_DECLARE_END

xrSE_DECLARE_BEGIN(xrSE_Rat,xrSE_Enemy)
	// model
	string64				caModel;
	
	// Personal characteristics:
	float					fEyeFov;
	float					fEyeRange;
	float					fHealth;
	float					fMinSpeed;
	float					fMaxSpeed;
	float					fAttackSpeed;
	float					fMaxPursuitRadius;
	float					fMaxHomeRadius;
	// morale
	float					fMoraleSuccessAttackQuant;
	float					fMoraleDeathQuant;
	float					fMoraleFearQuant;
	float					fMoraleRestoreQuant;
	u16						u16MoraleRestoreTimeInterval;
	float					fMoraleMinValue;
	float					fMoraleMaxValue;
	float					fMoraleNormalValue;
	// attack
	float					fHitPower;
	u16						u16HitInterval;
	float					fAttackDistance;
	float					fAttackAngle;
	float					fAttackSuccessProbability;

							xrSE_Rat();				// constructor for variable initialization
xrSE_DECLARE_END

xrSE_DECLARE_BEGIN(xrSE_Zombie,xrSE_Enemy)
	// model
	string64				caModel;
	
	// Personal characteristics:
	float					fEyeFov;
	float					fEyeRange;
	float					fHealth;
	float					fMinSpeed;
	float					fMaxSpeed;
	float					fAttackSpeed;
	float					fMaxPursuitRadius;
	float					fMaxHomeRadius;
	// attack
	float					fHitPower;
	u16						u16HitInterval;
	float					fAttackDistance;
	float					fAttackAngle;

							xrSE_Zombie();				// constructor for variable initialization
xrSE_DECLARE_END

xrSE_DECLARE_BEGIN(xrSE_Dog,xrSE_Enemy)
	// model
	string64				caModel;
	
	// Personal characteristics:
	float					fEyeFov;
	float					fEyeRange;
	float					fHealth;
	float					fMinSpeed;
	float					fMaxSpeed;
	float					fAttackSpeed;
	float					fMaxPursuitRadius;
	float					fMaxHomeRadius;
	// morale
	float					fMoraleSuccessAttackQuant;
	float					fMoraleDeathQuant;
	float					fMoraleFearQuant;
	float					fMoraleRestoreQuant;
	u16						u16MoraleRestoreTimeInterval;
	float					fMoraleMinValue;
	float					fMoraleMaxValue;
	float					fMoraleNormalValue;
	// attack
	float					fHitPower;
	u16						u16HitInterval;
	float					fAttackDistance;
	float					fAttackAngle;
	float					fAttackSuccessProbability;

							xrSE_Dog();				// constructor for variable initialization
xrSE_DECLARE_END

//***** Zone
//xrSE_DECLARE_BEGIN(xrSE_Zone,CALifeObject)
class xrSE_Zone : public CALifeObject, public xrSE_CFormed { typedef CALifeObject inherited; public:
	xrSE_Zone();
	f32 m_maxPower, m_attn;
	u32 m_period;
xrSE_DECLARE_END

//***** Detector
xrSE_DECLARE_BEGIN(xrSE_Detector,CALifeObject)
xrSE_DECLARE_END

xrSE_DECLARE_BEGIN(xrGraphPoint,xrServerEntity)
public:
	xrGraphPoint();
	string32				m_caConnectionPointName;
	u32						m_tLevelID;
	u32						m_tLocBaseID;
	u32						m_tLocAuxID;
xrSE_DECLARE_END

// 
#undef xrSE_EDITOR_METHODS
#undef xrSE_DECLARE_BEGIN
#undef xrSE_DECLARE_END

//
xrServerEntity*	F_entity_Create		(LPCSTR name);
void			F_entity_Destroy	(xrServerEntity* P);

#endif