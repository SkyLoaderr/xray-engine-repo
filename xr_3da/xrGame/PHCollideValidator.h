#ifndef PH_COLLIDE_VALIDATOR
#define PH_COLLIDE_VALIDATOR

typedef u32	CGID;
typedef u32	CLClassBits;
typedef u32 CLBits;
class CPHObject;
class CPHCollideValidator
{
public:
	enum ETypeFlags 
	{
		cbNCGroupObject	=	1<<0,
		cbNCSmall		=	1<<1,
		cbSmall			=	1<<2,
		cbCharacter		=	1<<3,
		cbDeadBody		=	1<<4,
		cbNone			=	1<<5
	};
static		CGID			RegisterGroup				()														;
static		CGID			LastGroupRegistred			()														;
static		void			RegisterObjToGroup			(CGID group,CPHObject& obj)								;
static		void			RegisterObjToLastGroup		(CPHObject& obj)										;
static		void			RestoreGroupObject			(const CPHObject& obj)									;
static		void			Init						()														;
static		void			InitObject					(CPHObject& obj)										;
static	IC	bool			DoCollide					(const CPHObject& obj1,const CPHObject& obj2)
{
	return !(obj1.collide_class_bits().is(cbNCGroupObject)&&
			 obj2.collide_class_bits().is(cbNCGroupObject)&&
			 obj1.collide_bits()==obj2.collide_bits());
}

protected:
private:
static		CGID			freeGroupID																			;

};


#endif