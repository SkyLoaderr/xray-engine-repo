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
	u16		geom;
	SPHImpact(const Fvector& aforce,const Fvector& apoint,u16 root_geom){force.set(aforce);point.set(apoint);geom=root_geom;}
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
void				DistributeAdditionalMass	(u16 geom_num,const dMass& m);//
void				SubFractureMass				(u16 fracture_num);
void				AddImpact		(const Fvector& force,const Fvector& point,u16 id);
PH_IMPACT_STORAGE&	Impacts			(){return m_impacts;}
protected:
private:

u16 				CheckFractured	();										//returns first breaked fracture

element_fracture	SplitFromEnd	(CPHElement* element,u16 geom_num);
void				InitNewElement	(CPHElement* element,const Fmatrix &shift_pivot,float density);
void				PassEndFractures(u16 from,CPHElement* dest);
public:
void				SplitProcess	(CPHElement* element,ELEMENT_PAIR_VECTOR &new_elements);
CPHFracture&		AddFracture		(const CPHFracture& fracture);
void				PhTune			(dBodyID body);										//set feedback for joints called from PhTune of ShellSplitterHolder
bool				PhDataUpdate	(CPHElement* element);										//collect joints and external impacts in fractures Update which set m_fractured; called from PhDataUpdate of ShellSplitterHolder returns true if has breaks

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
dMass			m_firstM;
dMass			m_seccondM;
float			m_break_force;
float			m_break_torque;
				CPHFracture();
public:
bool			Update(CPHElement* element);
IC bool			Breaked(){return m_breaked;}
void			SetMassParts(const dMass& first,const dMass& second);
void			MassSetZerro();
void			MassAddToFirst(const dMass& m);
void			MassAddToSecond(const dMass& m);
void			MassSubFromFirst(const dMass& m);
void			MassSubFromSecond(const dMass& m);
void			MassSetFirst(const dMass& m);
void			MassSetSecond(const dMass& m);
const dMass&	MassFirst(){return m_firstM;}
const dMass&	MassSecond(){return m_seccondM;}
void			MassUnsplitFromFirstToSecond(const dMass& m);
};


#endif  PH_FRACTURE_H