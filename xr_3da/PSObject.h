#ifndef _INCDEF_PSObject_H_
#define _INCDEF_PSObject_H_

#include "TempObject.h"

class ENGINE_API CSector;

class ENGINE_API CPSObject: public CTempObject{
	CSector*			m_pCurSector;
public:
	PS::SEmitter		m_Emitter;
public:
						CPSObject		(LPCSTR ps_name, CSector* S=0, BOOL bAutoRemove=true);
	virtual				~CPSObject		();
	virtual void		Update			(DWORD dt);
	virtual Fvector&	Position		(){return m_Emitter.m_Position;}
	void				UpdateSector	(CSector* sect);
	void				PlayAtPos		(const Fvector& pos);
	void				Stop			();
};
#endif /*_INCDEF_PSObject_H_*/

