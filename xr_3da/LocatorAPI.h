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

class ENGINE_API CStream;
class ENGINE_API CVirtualFileStream;

class ENGINE_API CLocatorAPI  
{
public:
	struct	file
	{
		LPCSTR	name;			// low-case name
		DWORD	vfs;			// 0xffffffff - standart file
		DWORD	ptr;			// pointer inside vfs
		DWORD	size;
		BOOL	bCompressed;
	};
	struct	file_pred		: public std::binary_function<file&, file&, bool> 
	{	
		IC bool operator()	(const file& x, const file& y) const
		{	return strcmp(x.name,y.name)<0;	}
	};
	struct	archive
	{
		CVirtualFileStream*		vfs;
	};
private:
	typedef set<file,file_pred>		set_files;
	typedef set_files::iterator		set_files_it;
	typedef vector<archive>			vec_archives;
	typedef vec_archives::iterator	vec_archives_it;

	set_files					files;
	vec_archives				archives;

	void						Register		(const char* name, DWORD vfs, DWORD ptr, DWORD size, BOOL bCompressed);
	void						ProcessArchive	(const char* path);
	void						ProcessOne		(const char* path, LPVOID F);
	void						Recurse			(const char* path);
public:
	void						Initialize		();
	void						Destroy			();

	CStream*					Open			(const char* N);
	void						Close			(CStream* &S);

	BOOL						Exist			(const char* N);
	BOOL						Exist			(char* fn, const char* path, const char* name);
	BOOL						Exist			(char* fn, const char* path, const char* name, const char* ext);

	void						List			(vector<char*>& dest, const char* path, DWORD flags=FS_ListFiles);

	CLocatorAPI();
	~CLocatorAPI();
};

#endif // !defined(AFX_LOCATORAPI_H__C37C9279_84E9_462B_9B58_F080971F8AED__INCLUDED_)
