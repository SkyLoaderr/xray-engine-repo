#pragma once


enum ETaskType{
	eTaskUnknown		=1,
	eTaskRoot,
	eTaskCopyFiles,
	eTaskCopyFolder,
	eTaskDelete,
	eTaskRunExecutable,
	eTaskBatchExecute
};
ETaskType strToType(LPCSTR str);
shared_str typeToStr(ETaskType t);


class CTask;

typedef xr_vector<CTask*>			CTaskArray;
typedef xr_vector<std::string>		CFileNamesArray;

class CTask{
protected:
	CTask*			m_parent_task;
	ETaskType		m_type;
	shared_str		m_name;
	BOOL			m_enabled;
	CTaskArray		m_sub_tasks;
	virtual void	copy_to				(CTask*);
	virtual void	run					()                                     =0;
public:
	u32				m_priority;
	HTREEITEM		m_tree_itm;
	void			sort_sub_tasks		();
	CTask*			parent				()										{return m_parent_task;}
	LPCSTR			name				()										{return *m_name;}
	void			set_name			(LPCSTR n);
	ETaskType		type				()										{return m_type;};
					CTask				();
	virtual			~CTask				();
	void			reparent			(CTask*t);
	u32				sub_task_count		()										{return m_sub_tasks.size();};
	CTask*			get_sub_task		(u32 idx);
	void			add_sub_task		(CTask* t);
	void			remove_sub_task		(CTask* t);
	BOOL			is_enabled			()										{return m_enabled;}
	void			set_enabled			(BOOL b);
	virtual BOOL	load				(CInifile& ini, LPCSTR section);
	virtual BOOL	save				(CInifile& ini, LPCSTR section);
	virtual void	exec				();
	CTask*			copy				(BOOL bRoot);
	BOOL			section_exist		(LPCSTR s);
};

class CTaskRoot :public CTask
{
	virtual void	run					()										{CTask::exec();};
public:
					CTaskRoot			()										{m_type=eTaskRoot;};
	virtual			~CTaskRoot			()										{};
};

class CTaskCopyFiles :public CTask
{
	shared_str		m_target_folder;
	CFileNamesArray m_file_names;
	virtual void	copy_to				(CTask*);
	virtual void	run					();
public:
					CTaskCopyFiles		()										{m_type=eTaskCopyFiles;};
	virtual			~CTaskCopyFiles		()										{};
	virtual BOOL	load				(CInifile& ini, LPCSTR section);
	virtual BOOL	save				(CInifile& ini, LPCSTR section);
	LPCSTR			target_folder		()										{return *m_target_folder;}
	void			set_target_folder	(LPCSTR s)								{m_target_folder = s;}
	CFileNamesArray* file_list			()										{return &m_file_names;}
};

class CTaskCopyFolder :public CTask
{
	shared_str		m_source_folder;
	shared_str		m_target_folder;
	virtual void	copy_to				(CTask*);
	virtual void	run					();
public:
					CTaskCopyFolder		()										{m_type=eTaskCopyFolder;};
	virtual			~CTaskCopyFolder	()										{};
	virtual BOOL	load				(CInifile& ini, LPCSTR section);
	virtual BOOL	save				(CInifile& ini, LPCSTR section);
	LPCSTR			target_folder		()										{return *m_target_folder;}
	LPCSTR			source_folder		()										{return *m_source_folder;}
	void			set_target_folder	(LPCSTR s)								{m_target_folder = s;}
	void			set_source_folder	(LPCSTR s)								{m_source_folder = s;}
};


class CTaskDelete :public CTask
{
	virtual void	run					(){};
public:
					CTaskDelete			()										{m_type=eTaskDelete;};
	virtual			~CTaskDelete		()										{};
	virtual BOOL	load				(CInifile& ini, LPCSTR section)			{return TRUE;};
};

class CTaskExecute :public CTask
{
	shared_str		m_app_name;
	shared_str		m_params;
	shared_str		m_working_folder;
	virtual void	copy_to				(CTask*);
	virtual void	run					();
public:
					CTaskExecute		()										{m_type=eTaskRunExecutable;};
	virtual			~CTaskExecute		()										{};
	virtual BOOL	load				(CInifile& ini, LPCSTR section);
	virtual BOOL	save				(CInifile& ini, LPCSTR section);
	LPCSTR			get_app_name		()										{return *m_app_name;}
	void			set_app_name		(LPCSTR n)								{m_app_name=n;}
	LPCSTR			get_params			()										{return *m_params;}
	void			set_params			(LPCSTR n)								{m_params=n;}
	LPCSTR			get_wrk_folder		()										{return *m_working_folder;}
	void			set_wrk_folder		(LPCSTR n)								{m_working_folder=n;}

};

class CTaskBatchExecute :public CTask
{
	CFileNamesArray m_file_names;
	shared_str		m_params;
	shared_str		m_working_folder;
	virtual void	copy_to				(CTask*);
	virtual void	run					();
public:
					CTaskBatchExecute	()										{m_type=eTaskBatchExecute;};
	virtual			~CTaskBatchExecute	()										{};
	virtual BOOL	load				(CInifile& ini, LPCSTR section);
	virtual BOOL	save				(CInifile& ini, LPCSTR section);
	CFileNamesArray* file_list			()										{return &m_file_names;}
	LPCSTR			get_params			()										{return *m_params;}
	void			set_params			(LPCSTR n)								{m_params=n;}
	LPCSTR			get_wrk_folder		()										{return *m_working_folder;}
	void			set_wrk_folder		(LPCSTR n)								{m_working_folder=n;}

};


class CTaskFacrory
{
public:
	static CTask*	create_task			(CInifile& ini, LPCSTR section);
	static CTask*	create_task			(ETaskType eTaskCopyFiles);
};

extern void updateTreeItemName(HTREEITEM itm, CTask* t);
void _VerifyPath(LPCSTR path);