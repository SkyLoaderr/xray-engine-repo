////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_processor.cpp
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script processor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_processor.h"
#include "ai_script.h"

DEFINE_VECTOR(LPSTR,LPSTR_VECTOR,LPSTR_IT);

// I need this because we have to exclude option /EHsc (exception handling) from the project
namespace boost {
	void __stdcall throw_exception(const exception &A)
	{
		Debug.fatal("Boost exception raised %s",A.what());
	}
};

CScriptProcessor::CScriptProcessor(LPCSTR caCaption, LPCSTR caScriptString)
{
	Msg				("* Initializing %s script processor",caCaption);
	u32				N = _GetItemCount(caScriptString);
	string16		I;
	string256		S,S1;
	for (u32 i=0; i<N; i++) {
		FS.update_path(S,"$game_scripts$",strconcat(S1,_GetItem(caScriptString,i,I),".script"));
		R_ASSERT3	(FS.exist(S),"Script file not found!",S);
		m_tpScripts.push_back(xr_new<CScript>(S));
	}
}

CScriptProcessor::~CScriptProcessor()
{
	SCRIPT_IT		I = m_tpScripts.begin();
	SCRIPT_IT		E = m_tpScripts.end();
	for ( ; I != E; I++)
		xr_delete	(*I);
}

void CScriptProcessor::Update()
{
	SCRIPT_IT		I = m_tpScripts.begin();
	SCRIPT_IT		E = m_tpScripts.end();
	for ( ; I != E; I++)
		(*I)->Update();
}