#ifndef TELE_WHIRLWIND
#define TELE_WHIRLWIND
#include "ai/monsters/telekinesis.h"
#include "ai/monsters/telekinetic_object.h"
#include "PHImpact.h"

class CTeleWhirlwind;
class CGameObject;
class CTeleWhirlwindObject : public CTelekineticObject
{
	typedef			CTelekineticObject	inherited;
	CTeleWhirlwind		*m_telekinesis;

	float				throw_power;
	
public:
	virtual							~CTeleWhirlwindObject	(){};
									CTeleWhirlwindObject	();
	virtual		bool				init					(CTelekinesis* tele,CPhysicsShellHolder *obj, float s, float h, u32 ttk); 
	virtual		void				raise					(float step);
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
		Fvector				m_center;
		CGameObject*		m_owner_object;
		PH_IMPACT_STORAGE	m_saved_impacts;
public: 
						CTeleWhirlwind		();
		CGameObject*    OwnerObject			()const							{return m_owner_object;}
  const	Fvector&		Center				()const							{return m_center;}
		void			SetCenter			(const Fvector center)			{m_center.set(center);}
		void			SetOwnerObject		(CGameObject* owner_object)		{m_owner_object=owner_object;}
		void			add_impact			(const Fvector& dir,float val)	;
		void			draw_out_impact		(Fvector& dir,float& val)		;
				void	clear_impacts		()								;
		virtual void	clear				()								;
		virtual CTelekineticObject*	alloc_tele_object()	{return static_cast<CTelekineticObject*>(xr_new<CTeleWhirlwindObject>());}
};


#endif