// LocatorAPI.h: interface for the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCATORAPI_H__C37C9279_84E9_462B_9B58_F080971F8AED__INCLUDED_)
#define AFX_LOCATORAPI_H__C37C9279_84E9_462B_9B58_F080971F8AED__INCLUDED_
#pragma once

enum FS_List
{
	FS_ListFiles	=(1<<0),
	FS_ListFolders	=(1<<1),
	FS_forcedword	=DWORD(-1)
};

class ENGINE_API CLocatorAPI  
{
private:
	struct str_pred : public binary_function<char*, char*, bool> 
	{	
		IC bool operator()(const char* x, const char* y) const
		{	return strcmp(x,y)<0;	}
	};
	
	typedef set<char*,str_pred>	set_cstr;
	typedef set_cstr::iterator	set_cstr_it;

	set_cstr			files;

	void				ProcessOne	(_finddata_t& F, const char* path);
	void				Recurse		(const char* path);
public:
	void				Initialize	();
	void				Destroy		();

	BOOL				Exist		(const char* N);
	BOOL				Exist		(char* fn, const char* path, const char* name);
	BOOL				Exist		(char* fn, const char* path, const char* name, const char* ext);

	void				List		(vector<char*>& dest, const char* path, DWORD flags=FS_ListFiles);

	CLocatorAPI();
	~CLocatorAPI();
};

#endif // !defined(AFX_LOCATORAPI_H__C37C9279_84E9_462B_9B58_F080971F8AED__INCLUDED_)
