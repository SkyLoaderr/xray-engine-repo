#ifndef PH_COLLIDE_VALIDATOR
#define PH_COLLIDE_VALIDATOR

typedef u32	CGID;
typedef u32	CLClassBits;
typedef u32 CLBits;
class CPHObject;
class CPHCollideValidator
{

	enum ETypeFlags 
	{
		cbNCGroupObject	=	1<<0,
		cbNCStatic		=	1<<1,
		cbNone			=	1<<2
	};
public:
static		CGID			RegisterGroup				()														;
static		CGID			LastGroupRegistred			()														;
////////////////////////////////////////////////////////////////////////////////
static		void			InitObject					(CPHObject& obj)										;
static		void			RegisterObjToGroup			(CGID group,CPHObject& obj)								;
static		void			RegisterObjToLastGroup		(CPHObject& obj)										;
static		void			RestoreGroupObject			(const CPHObject& obj)									;
static		bool			IsGroupObject				(const CPHObject& obj)									;
static		void			SetStaticNotCollide			(CPHObject& obj)										;
static		void			Init						()														;

static	IC	bool			DoCollide					(const CPHObject& obj1,const CPHObject& obj2)
{
	switch(CollideType(obj1.collide_class_bits().flags,obj2.collide_class_bits().flags)) {
		case cbNone:			return DoCollideNone		(obj1,obj2)			;break;
		case cbNCGroupObject:	return DoCollideGroup		(obj1,obj2)			;break;
		case 0:					return DoCollideNonMatched	(obj1,obj2)			;break;
		default: NODEFAULT;
	}
	return DoCollideNone(obj1,obj2) || DoCollideGroup(obj1,obj2);
			
}

static	IC bool				DoCollideStatic				(const CPHObject& obj)
{
	return !obj.collide_class_bits().test(cbNCStatic);
}

protected:
private:
	static IC	bool			DoCollideNonMatched			(const CPHObject& obj1,const CPHObject& obj2)
	{
		return true;
	}
	static IC	bool			DoCollideNone				(const CPHObject& obj1,const CPHObject& obj2)
	{
		return true;
			//(obj1.collide_class_bits().is(cbNone)	||
			//obj2.collide_class_bits().is(cbNone));

	}
	static IC	bool			DoCollideGroup				(const CPHObject& obj1,const CPHObject& obj2)
	{
		return !(//obj1.collide_class_bits().is(cbNCGroupObject)	&&
				 //obj2.collide_class_bits().is(cbNCGroupObject)	&&
				obj1.collide_bits()==obj2.collide_bits());
	}

	static IC	CLClassBits	CollideType(CLClassBits cb1,CLClassBits cb2)
	{
		return ((cb1&cb2)&~typeFlags.flags);//|((cb1|cb2)&onceFlags.flags);
	}

static		CGID					freeGroupID																			;
static		_flags<CLClassBits>		onceFlags;
static		_flags<CLClassBits>		typeFlags;
};


#endif