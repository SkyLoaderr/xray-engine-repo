//
//
#ifndef PH_FRACTURE_H
#define PH_FRACTURE_H
#include "PHDefs.h"
class CPHFracture;
class CPHElement;

DEFINE_VECTOR(dJointFeedback,CFEEDBACK_STORAGE,CFEEDBACK_I)

struct SPHImpact 
{
	Fvector force;
	Fvector point;
	SPHImpact(const Fvector& aforce,const Fvector& apoint){force.set(aforce);point.set(apoint);}
};

DEFINE_VECTOR(SPHImpact,PH_IMPACT_STORAGE,PH_IMPACT_I)

class CPHFracturesHolder 			//stored in CPHElement
{
friend class CPHElement;
friend class CPHShellSplitterHolder;
bool			 m_has_breaks;

FRACTURE_STORAGE m_fractures;
PH_IMPACT_STORAGE m_impacts;		//filled in anytime from CPHElement applyImpulseTrace cleared in PhDataUpdate
CFEEDBACK_STORAGE m_feedbacks;		//this store feedbacks for non contact joints 
public:
CPHFracturesHolder			();
#pragma todo("Dima to Kostia : check if you need here a non-virtual destructor!")
~CPHFracturesHolder			();
void				AddImpact		(const Fvector& force,const Fvector& point);
protected:
private:

u16 				CheckFractured	();										//returns first breaked fracture

element_fracture	SplitFromEnd	(CPHElement* element,u16 geom_num);
void				InitNewElement	(CPHElement* element);
void				PassEndFractures(u16 from,CPHElement* dest);
public:
void				SplitProcess	(CPHElement* element,ELEMENT_PAIR_VECTOR &new_elements);
CPHFracture&		AddFracture		(const CPHFracture& fracture);
void				PhTune			(dBodyID body);										//set feedback for joints called from PhTune of ShellSplitterHolder
bool				PhDataUpdate	(dBodyID body);										//collect joints and external impacts in fractures Update which set m_fractured; called from PhDataUpdate of ShellSplitterHolder returns true if has breaks
};

class CShellSplitInfo 
{
friend class  CPHFracturesHolder;
friend class  CPHShellSplitterHolder;
friend class CPHElement;
friend	class SplitInfRec;
protected:
	u16				m_start_el_num;
	u16				m_end_el_num;
	u16				m_start_jt_num;
	u16				m_end_jt_num;
	u16				m_start_geom_num;
	u16				m_end_geom_num;
	u16				m_bone_id;
};
class CPHFracture : public CShellSplitInfo
{
friend class  CPHFracturesHolder;
friend class CPHElement;
friend class CPHShell;
bool			m_breaked;
Fvector			m_position;							//vs body//when fractured is additional linear vel for seccond body
Fvector			m_direction;						//norm to fracture plane vs body//when fractured is additional angular vel for seccond body
dMass			m_firstM;
dMass			m_seccondM;
float			m_break_force;
float			m_break_torque;
				CPHFracture();
public:
bool			Update(PH_IMPACT_STORAGE& impacts,dBodyID body);
IC bool			Breaked(){return m_breaked;}
void			SetMassParts(const dMass& first,const dMass& second);
};


#endif  PH_FRACTURE_H