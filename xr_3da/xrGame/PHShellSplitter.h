#ifndef PHSHELL_SPLITTER_H
#define PHSHELL_SPLITTER_H
#include "PHDefs.h"
#include "PHObject.h"
class CPHShellSplitter;
class CPHShell;
DEFINE_VECTOR(CPHShellSplitter,SPLITTER_STORAGE,SPLITTER_I)
typedef		xr_vector<CPHShellSplitter>::reverse_iterator	SPLITTER_RI;
class CPHShellSplitterHolder : public CPHObject			//call all Fractures and Breakable Joints Updates
{

bool					m_has_breaks;
CPHShell*				m_pShell;								//purpose: to extract elements and joints corresponded splitters
SPLITTER_STORAGE		m_splitters;
						CPHShellSplitterHolder();
virtual	void			PhTune(dReal step);						//call fractures PhTune for element splitters m_pShell->m_elements[m_splitters[i]->m_element]->m_pFracturesHolder->PhTune()
virtual void			PhDataUpdate(dReal step);				//call fractures PhDataUpdate for element splitters m_pShell->m_elements[m_splitters[i]->m_element]->m_pFracturesHolder->PhDataUpdate()
virtual void			InitContact(dContact* c){};
		bool			CheckSplitter(u16 aspl);		//
		CPhysicsShell*	SplitJoint(u16 aspl);		//create new shell moving into it departed elements and joints 
		void			SplitElement(u16 aspl,PHSHELL_VECTOR &out_shels);//
		void			SplitProcess(PHSHELL_VECTOR &out_shels);
		void			PassEndSplitters(u16 from,CPHShellSplitterHolder* dest,u16 position,u16 shift_elements,u16 shift_joints);
		void			InitNewShell(CPHShell* shell);			//inits new active shell
};


class CPHShellSplitter 
{
	friend	class CPHShellSplitterHolder;
public:
	enum EType				  {splElement,splJoint};
	bool					   m_breaked;
	EType					   m_type;
	u16						   m_element;
	u16						   m_joint;

	CPHShellSplitter(CPHShellSplitter::EType type,u16 element,u16 joint);

};
#endif