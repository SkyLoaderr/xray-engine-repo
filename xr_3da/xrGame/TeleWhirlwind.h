#ifndef TELE_WHIRLWIND
#define TELE_WHIRLWIND
#include "ai/telekinesis.h"
#include "ai/telekinetic_object.h"


class CTeleWhirlwind;

class CTeleWhirlwindObject : public CTelekineticObject
{
	typedef			CTelekineticObject	inherited;
	CTeleWhirlwind		*m_telekinesis;
public:
	virtual							~CTeleWhirlwindObject	(){};
	CTeleWhirlwindObject	();
	virtual		bool				init					(CTelekinesis* tele,CPhysicsShellHolder *obj, float s, float h, u32 ttk); 
	virtual		void				raise					(float power);
	virtual		void				raise_update			();
	virtual		void				keep					();
	virtual		void				release					();
	virtual		void				fire					(const Fvector &target);
	virtual		void				fire					(const Fvector &target, float power);
	virtual		void				switch_state			(ETelekineticState new_state);
};

class CTeleWhirlwind : public CTelekinesis
{
typedef	CTelekinesis inherited;
		Fvector			m_center;

public: 

				const Fvector&		Center				()const						{return m_center;}
				void				SetCenter			(const Fvector center)		{m_center.set(center);}
		virtual CTelekineticObject*	alloc_tele_object()	{return static_cast<CTelekineticObject*>(xr_new<CTeleWhirlwindObject>());}
};


#endif