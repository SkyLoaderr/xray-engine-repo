#ifndef _AI_COMMANDS_
#define _AI_COMMANDS_

#pragma pack(push,4)
namespace AI {
	enum	C_Target		// command target
	{
		t_None,
		t_Object,
		t_Point,
		t_Direction
	};
	enum	C_Mode
	{
		cDestroyAfterComplete	= (1<<0),
		cDestroyAfterTimeout	= (1<<1),
		cDestroyUnachievable	= (1<<2),

		cDestroyAll				= cDestroyAfterComplete|cDestroyAfterTimeout|cDestroyUnachievable
	};
	enum	C_Result
	{
		cCompleted				= (1<<0),
		cTimeout				= (1<<1),
		cUnachievable			= (1<<2),
		cLogicError				= (1<<3)
	};
	union	C_TargetObject	// the target itself
	{
		CEntity*	Object;
		Fvector		Point;
		Fvector		Direction;
	};
	class	C_Command	{
	public:
		DWORD			Command;
		WORD			Result;
		WORD			Mode;
		C_Target		Tmode;
		C_TargetObject	Tobject;
		DWORD			o_timeout;

	public:
		C_Command()		{ ZeroMemory(this,sizeof(*this)); setLogicError(); };

		IC	void		setCompleted()
		{	if (Mode&cDestroyAfterComplete) Result|=cCompleted;		}
		IC	void		setTimeout()
		{	if (Mode&cDestroyAfterTimeout)	Result|=cTimeout;		}
		IC	void		setUnachievable()
		{	if (Mode&cDestroyUnachievable)	Result|=cUnachievable;	}
		IC	void		setError()
		{	Result|=cLogicError;									}
		IC	void		setLogicError()
		{	setError(); setUnachievable();							}

		// Add message 2 queue
		IC void			setup(DWORD Cmd, C_Target T=t_None, void* Tobj=0, DWORD timeout=500, DWORD mode=cDestroyAll)
		{
			Command		= Cmd;
			Result		= 0;
			Mode		= WORD(mode);
			Tmode		= T;
			switch (T) {
			case t_Object:		Tobject.Object		= *((CEntity**)Tobj);	break;
			case t_Point:		Tobject.Point		= *((Fvector*)Tobj);	break;
			case t_Direction:	Tobject.Direction	= *((Fvector*)Tobj);	break;
			default:			ZeroMemory(&Tobject,sizeof(Tobject));		break;
			};
			o_timeout	= Device.dwTimeGlobal+timeout;
		}
	};
	class	AIC_Look	: public C_Command	
	{
	public:
		enum {
			Sleep		=0,
			Look
		};
		float			o_look_speed;
	};
	class	AIC_Move	: public C_Command 
	{
	public:
		enum {
			ms_to		= (1<<0),
			ms_from		= (1<<1),
			ms_left		= (1<<2),
			ms_right	= (1<<3),
			ms_jump		= (1<<4),
			ms_crouch	= (1<<5)
		};
		float			o_range;	// distance 2 target
		union {
			float		o_accel;
			float		o_jump_speed;
		};
	};
	class	AIC_Action	: public C_Command 
	{
	public:
		enum {
			Sleep		=0,
			FireBegin,
			FireEnd
		};
		float			o_PredictTime;	// 0.. - time of prediction
	};
};
#pragma pack(pop)

#endif