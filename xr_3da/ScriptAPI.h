// ScriptAPI.h: interface for the CScriptAPI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTAPI_H__CDDBB1C1_9294_4C91_BDB1_A698B6A02F50__INCLUDED_)
#define AFX_SCRIPTAPI_H__CDDBB1C1_9294_4C91_BDB1_A698B6A02F50__INCLUDED_
#pragma once

typedef char* scScript;

struct ENGINE_API SScriptDef {
	scScript	S;
	char		name[64];
};
class ENGINE_API CScript;

class ENGINE_API CScriptAPI
{
	vector<SScriptDef>	Scripts;
public:
	void				Initialize	(void);
	void				Destroy		(void);

	CScript*			CreateInstance(const char *script);

	CScriptAPI			();
	~CScriptAPI			();
};

#endif // !defined(AFX_SCRIPTAPI_H__CDDBB1C1_9294_4C91_BDB1_A698B6A02F50__INCLUDED_)
