//
//
#ifndef PH_FRACTURE_H
#define PH_FRACTURE_H
#include "PHDefs.h"
class CPHFracture;
class CPHElement;
DEFINE_VECTOR(CPHFracture,FRACTURE_STORAGE,FRACTURE_I)
DEFINE_VECTOR(dJointFeedback,CFEEDBACK_STORAGE,CFEEDBACK_I)
typedef		xr_vector<CPHFracture>::reverse_iterator	FRACTURE_RI;
struct SPHImpact 
{
	Fvector force;
	Fvector point;
	SPHImpact(const Fvector& aforce,const Fvector& apoint){force.set(aforce);point.set(apoint);}
};

DEFINE_VECTOR(SPHImpact,PH_IMPACT_STORAGE,PH_IMPACT_I)

class CPHFracturesHolder 			//stored in CPHElement
{
bool			 m_has_breaks;
FRACTURE_STORAGE m_fractures;
PH_IMPACT_STORAGE m_impacts;		//filled in anytime from CPHElement applyImpulseTrace cleared in PhDataUpdate
CFEEDBACK_STORAGE m_feedbacks;		//this store feedbacks for non contact joints 
public:
CPHFracturesHolder			();
~CPHFracturesHolder			();
void		AddImpact		(const Fvector& force,const Fvector& point);
protected:
private:

u16 		CheckFractured	();										//returns first breaked fracture

CPHElement* SplitFromEnd	(CPHElement* element,u16 geom_num);
void		InitNewElement	(CPHElement* element);
void		PassEndFractures(u16 from,CPHElement* dest,u16 shift_geoms);
public:
void		SplitProcess	(CPHElement* element,ELEMENT_STORAGE &new_elements);
void		AddFracture		(u16 geom_num,const Fvector& position,const Fvector& direction,const float& break_force,const float& break_torque);
void		PhTune			(dBodyID body);										//set feedback for joints called from PhTune of ShellSplitterHolder
bool		PhDataUpdate	(dBodyID body);										//collect joints and external impacts in fractures Update which set m_fractured; called from PhDataUpdate of ShellSplitterHolder returns true if has breaks
};

class CPHFracture
{
friend class  CPHFracturesHolder;
bool			m_breaked;
u16				m_geom_num;
Fvector			m_position;							//vs body//when fractured is additional linear vel for seccond body
Fvector			m_direction;						//norm to fracture plane vs body//when fractured is additional angular vel for seccond body
float			m_break_force;
float			m_break_torque;
				CPHFracture(u16 geom_num,const Fvector& position,const Fvector& direction,const float& break_force,const float& break_torque);
public:
bool			Update(PH_IMPACT_STORAGE& impacts,dBodyID body);
IC bool			Breaked(){return m_breaked;}
};


#endif  PH_FRACTURE_H