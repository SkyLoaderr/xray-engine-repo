//----------------------------------------------------
// file: SObject2Editor.h
//----------------------------------------------------

#ifndef _INCDEF_SObject2Editor_H_
#define _INCDEF_SObject2Editor_H_

#include "SceneClassEditor.h"
#include "SObject2.h"
#include "MultiObjCheck.h"

//----------------------------------------------------

class SObject2Editor : public SceneClassEditor {
protected:

	bool m_Reference;
	char m_RefName[256];
	bool m_MultiSelection;
	char m_ObjectScript[4096];
	char m_ObjectName[256];
	char m_ObjectClass[256];
	MultiObjCheck m_MakeUnique;
	MultiObjCheck m_DynamicList;

	SObject2 *m_EObject;

	virtual void GetObjectsInfo();
	virtual void ApplyObjectsInfo();
	virtual void DlgExtract();
	virtual void DlgSet();

protected:

	virtual void DlgInit( HWND hw );
	virtual void Command( WPARAM wp, LPARAM lp );

public:

	SObject2Editor();
	virtual ~SObject2Editor();
};


//----------------------------------------------------
#endif /*_INCDEF_SObject2Editor_H_*/

