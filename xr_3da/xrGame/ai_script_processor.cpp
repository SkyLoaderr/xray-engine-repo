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

CScriptProcessor::CScriptProcessor(LPCSTR caFilePath)
{
	Msg				("* Initializing script processor for scripts in %s",caFilePath);

	LPSTR_VECTOR	*l_tpFileList = FS.file_list_open(caFilePath);

	if (l_tpFileList) {
		LPSTR_IT		I = l_tpFileList->begin();
		LPSTR_IT		E = l_tpFileList->end();
		for ( ; I != E; I++) {
			string256	l_caExtension;
			_splitpath(*I,0,0,0,l_caExtension);
			if (!strcmp(l_caExtension,".script")) {
				strconcat(l_caExtension,caFilePath,*I);
				m_tpScripts.push_back(xr_new<CScript>(l_caExtension));
			}
		}
	}

	FS.file_list_close(l_tpFileList);
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