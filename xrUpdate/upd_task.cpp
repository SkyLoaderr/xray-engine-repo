#include "stdafx.h"
#include "upd_task.h"
#include "FileOperations.h"

void SaveStringWithBrackets(CInifile& ini, LPCSTR section, LPCSTR name, shared_str& str)
{
	string1024 buff;
	if(!xr_strlen(str)) str="";
	strconcat(buff,"\"",*str,"\"");
	ini.w_string(section, name, buff);
}

ETaskType strToType(LPCSTR str)
{
	string32 s;
	strcpy(s,str);
	if(0==strcmp(xr_strlwr(s),"copy_files"))
		return eTaskCopyFiles;
	if(0==strcmp(xr_strlwr(s),"copy_folder"))
		return eTaskCopyFolder;
	if(0==strcmp(xr_strlwr(s),"delete"))
		return eTaskDelete;
	if(0==strcmp(xr_strlwr(s),"execute"))
		return eTaskRunExecutable;
	if(0==strcmp(xr_strlwr(s),"batch_execute"))
		return eTaskBatchExecute;

	return eTaskUnknown;
}
shared_str typeToStr(ETaskType t)
{
	if(t==eTaskCopyFiles)
		return "copy_files";
	if(t==eTaskCopyFolder)
		return "copy_folder";
	if(t==eTaskDelete)
		return "delete";
	if(t==eTaskRunExecutable)
		return "execute";
	if(t==eTaskBatchExecute)
		return "batch_execute";
	return "unknown";
}

CTask::~CTask()
{
	CTaskArray::iterator sub_task_it =	m_sub_tasks.begin();
	for(;sub_task_it !=m_sub_tasks.end();++sub_task_it)
		xr_delete(*sub_task_it);

	m_sub_tasks.clear();
}

CTask::CTask ()
{
	m_priority		= 0;//highest
	m_tree_itm		= NULL;
	m_parent_task	= NULL;
	m_type			= eTaskUnknown;
}

void CTask::set_name			(LPCSTR n)
{
	m_name=n;
	if(m_tree_itm){
		updateTreeItemName(m_tree_itm, this);
	}
}

CTask* CTask::get_sub_task		(u32 idx)
{
	return m_sub_tasks[idx];
}
void CTask::remove_sub_task		(CTask* t)								
{
	CTaskArray::iterator it = std::find(m_sub_tasks.begin(),m_sub_tasks.end(),t);
	if(it!=m_sub_tasks.end())
		m_sub_tasks.erase(it);
}

void CTask::add_sub_task (CTask* t)	
{
	m_sub_tasks.push_back(t); 
	t->m_parent_task=this;
}

CTask*	CTask::copy()
{
	string128 new_name;
	CTask* t = CTaskFacrory::create_task( type() );
	
	strconcat(new_name,*m_name,"_copy");
	t->set_name(new_name);

	copy_to(t);
	return t;
}

void CTask::copy_to	(CTask* t)
{
	t->m_type		= m_type;
	t->m_enabled	= m_enabled;
	//name&section already tuned
	CTaskArray::iterator sub_task_it =	m_sub_tasks.begin();
	for(;sub_task_it !=m_sub_tasks.end();++sub_task_it){
		CTask* t_sub = (*sub_task_it)->copy();
		add_sub_task(t_sub);
	}

}
bool task_sorter(const CTask* t1, const CTask* t2)
{
	return t1->m_priority < t2->m_priority;
}
void CTask::sort_sub_tasks()
{
	std::sort(m_sub_tasks.begin(),m_sub_tasks.end(),task_sorter);
}

BOOL CTask::load(CInifile& ini, LPCSTR section)
{
	m_type				= strToType(ini.r_string(section,	"type") );
	m_name				= ini.r_string_wb(section,				"task_name");
	m_enabled			= ini.r_bool(section,				"enabled");
	m_priority			= ini.r_u32(section,				"priority");

	if(ini.line_exist(section,"sub_task_count") ){
		u32 cnt = ini.r_u32(section,				"sub_task_count");
		string512 sname;
		for (u32 i=0; i<cnt; ++i){
			sprintf(sname,"%s.%04d",section,i);
			CTask* new_task = CTaskFacrory::create_task(ini, sname);
			add_sub_task(new_task);
			BOOL res = new_task->load(ini, sname);//recurse
			if(!res)
				return FALSE;
			sort_sub_tasks();
		}
	}
	return TRUE;
}

BOOL CTask::save(CInifile& ini, LPCSTR section)
{
	ini.w_string(section,"type", *(typeToStr(m_type)) );
	ini.w_string(section,"task_name",*m_name);
	
	SaveStringWithBrackets(ini, section, "task_name", m_name);
	
	ini.w_bool(section,"enabled",is_enabled());
	ini.w_u32(section,"priority",m_priority);

	u32 sub_cnt = m_sub_tasks.size();
	if(sub_cnt){
		ini.w_u32(section,"sub_task_count",sub_cnt);
		string512 sname;
		for(u32 idx=0; idx<sub_cnt;++idx){
			sprintf(sname,"%s.%04d",section,idx);
			get_sub_task(idx)->save(ini,sname);
		}
	}
	return TRUE;
}

void CTask::set_enabled(BOOL b)
{
	m_enabled = b;
	CTaskArray::iterator it = m_sub_tasks.begin();
	for(;it != m_sub_tasks.end(); ++it){
		(*it)->set_enabled(b);
	}
}
void CTask::run()
{
	if( !is_enabled() ) return;

	CTaskArray::reverse_iterator sub_task_it =	m_sub_tasks.rbegin();
	for(;sub_task_it !=m_sub_tasks.rend();++sub_task_it)
		(*sub_task_it)->run();

}


BOOL CTaskCopyFiles::load				(CInifile& ini, LPCSTR section)
{
	CTask::load(ini, section);

	m_target_folder		= ini.r_string_wb(section,	"target_folder");

	u32 file_count		= ini.r_u32(section,		"file_count");
	for(u32 i=0; i<file_count; ++i){
		string_path s;
		string16 idx;
		itoa(i,idx,10);
		strconcat(s,"file",idx);
		if(ini.line_exist(section,s) )
			m_file_names.push_back(ini.r_string(section,s));
	}
	return TRUE;
}

BOOL CTaskCopyFiles::save				(CInifile& ini, LPCSTR section)
{
	CTask::save(ini, section);

	SaveStringWithBrackets(ini, section, "target_folder", m_target_folder);

	ini.w_u32(section,"file_count",m_file_names.size());
	for(u32 i=0; i<m_file_names.size(); ++i){
		string_path s;
		string16 idx;
		itoa(i,idx,10);
		strconcat(s,"file",idx);
		ini.w_string(section,s,*m_file_names[i]);
	}
	return TRUE;
}

void CTaskCopyFiles::run()
{
	if( !is_enabled() ) return;
	CTask::run();

	CFileNamesArray::iterator it = m_file_names.begin();
	string_path file_name;
	string16	drive;
	string_path dir;
	string16	ext;
	string_path new_path;

	if( !is_enabled() )
		return;

	CFileOperation fo;      // create object
	fo.SetOverwriteMode(true); // reset OverwriteMode flag (optional)
	fo.SetAskIfReadOnly(true);
	for(;it!=m_file_names.end();++it){
		_splitpath(*(*it),drive,dir,file_name,ext);
//		strconcat(new_path,target_folder(),"\\",file_name,ext);
		strcpy(new_path,target_folder());
//		BOOL res = CopyFile(*(*it),new_path,FALSE);
		if (!fo.Copy(*(*it), new_path)) // do Copy
		{
			CString msg;
			msg.Format("Copying file %s to %s\n",*(*it),new_path);
			fo.ShowError_( msg.GetBuffer() ); // if copy fails show error message
		}

	}
}

void CTaskCopyFiles::copy_to(CTask*t)
{
	CTask::copy_to(t);
	CTaskCopyFiles*tt = dynamic_cast<CTaskCopyFiles*>(t);

	tt->m_target_folder	= m_target_folder;
	tt->m_file_names		= m_file_names;
}


BOOL CTaskCopyFolder::load				(CInifile& ini, LPCSTR section)
{
	CTask::load(ini, section);

	m_target_folder		= ini.r_string_wb(section,	"target_folder");
	m_source_folder		= ini.r_string_wb(section,	"source_folder");

	return TRUE;
}

BOOL CTaskCopyFolder::save				(CInifile& ini, LPCSTR section)
{
	CTask::save(ini, section);

	SaveStringWithBrackets(ini, section, "source_folder", m_source_folder);
	SaveStringWithBrackets(ini, section, "target_folder", m_target_folder);

	return TRUE;
}

void CTaskCopyFolder::run ()
{
	if( !is_enabled() ) return;
	CTask::run();

	CFileOperation fo;      // create object
	fo.SetOverwriteMode(true); // reset OverwriteMode flag (optional)
	fo.SetAskIfReadOnly();   // set AskIfReadonly flag (optional)
	if (!fo.Copy(*m_source_folder, *m_target_folder)) // do Copy
	{
		fo.ShowError(); // if copy fails show error message
	}

/*	if (!fo.Delete("c:\\source")) // do Copy
	{
		fo.ShowError(); // if copy fails show error message
	}
*/
}

void CTaskCopyFolder::copy_to(CTask*t)
{
	CTask::copy_to(t);
	CTaskCopyFolder*tt = dynamic_cast<CTaskCopyFolder*>(t);
	tt->m_source_folder		= m_source_folder;
	tt->m_target_folder		= m_target_folder;
}

BOOL CTaskExecute::load				(CInifile& ini, LPCSTR section)
{
	CTask::load(ini, section);

	m_app_name			= ini.r_string_wb(section,	"app_name");
	m_params			= ini.r_string_wb(section,	"params");
	m_working_folder	= ini.r_string_wb(section,	"working_folder");

	return TRUE;
}
BOOL CTaskExecute::save				(CInifile& ini, LPCSTR section)
{
	CTask::save(ini, section);

	SaveStringWithBrackets(ini,section,"app_name",m_app_name);
	SaveStringWithBrackets(ini,section,"params",m_params);
	SaveStringWithBrackets(ini,section,"working_folder",m_working_folder);

	return TRUE;
}

void CTaskExecute::run					()
{
	if( !is_enabled() ) return;
	CTask::run();

	string_path cur_dir;
	if( xr_strlen(m_working_folder) ){
		GetCurrentDirectoryA(_MAX_PATH,cur_dir);
		SetCurrentDirectoryA(*m_working_folder);
	};
	spawnl(_P_WAIT, *m_app_name, (xr_strlen(m_params))?" ":*m_params);
	if( xr_strlen(m_working_folder) )
		SetCurrentDirectoryA(cur_dir);
}

void CTaskExecute::copy_to(CTask*t)
{
	CTask::copy_to(t);
	CTaskExecute*tt = dynamic_cast<CTaskExecute*>(t);
	tt->m_app_name			= m_app_name;
	tt->m_params				= m_params;
	tt->m_working_folder		= m_working_folder;
}

BOOL CTaskBatchExecute::load				(CInifile& ini, LPCSTR section)
{
	CTask::load(ini, section);

//	m_app_name		= ini.r_string(section,	"app_name");
	m_params		= ini.r_string_wb(section,	"params");
//	m_working_folder		= ini.r_string(section,	"working_folder");

	u32 file_count		= ini.r_u32(section,		"file_count");
	for(u32 i=0; i<file_count; ++i){
		string_path s;
		string16 idx;
		itoa(i,idx,10);
		strconcat(s,"file",idx);
		if(ini.line_exist(section,s) )
			m_file_names.push_back(ini.r_string(section,s));
	}
	return TRUE;
}

BOOL CTaskBatchExecute::save				(CInifile& ini, LPCSTR section)
{
	CTask::save(ini, section);

//	ini.w_string(section,	"app_name", *m_app_name);
//	ini.w_string(section,	"working_folder", *m_working_folder);
	SaveStringWithBrackets(ini,section,"params",m_params);

	ini.w_u32(section,"file_count",m_file_names.size());
	for(u32 i=0; i<m_file_names.size(); ++i){
		string_path s;
		string16 idx;
		itoa(i,idx,10);
		strconcat(s,"file",idx);
		ini.w_string(section,s,*m_file_names[i]);
	}
	return TRUE;
}

void CTaskBatchExecute::copy_to(CTask*t)
{
	CTask::copy_to(t);
	CTaskBatchExecute*tt = dynamic_cast<CTaskBatchExecute*>(t);

	tt->m_file_names			= m_file_names;
	tt->m_params				= m_params;
}

CString parseParams(LPCSTR fn, LPCSTR params);

void CTaskBatchExecute::run					()
{
	if( !is_enabled() ) return;
	CTask::run();

/*
	string_path cur_dir;
	if( xr_strlen(m_working_folder) ){
		GetCurrentDirectoryA(_MAX_PATH,cur_dir);
		SetCurrentDirectoryA(*m_working_folder);
	};*/

	CFileNamesArray::iterator it = m_file_names.begin();
	CString		params;
//	CString		command;

	for(;it!=m_file_names.end();++it){
		params = parseParams(*(*it), *m_params);
//		spawnl(_P_WAIT, *m_app_name, (params.IsEmpty())?" ":params.GetBuffer() );
//		command.Format( "%s %s",*m_app_name,params.GetBuffer() );
		system(params.GetBuffer());
	}

/*
	if( xr_strlen(m_working_folder) )
		SetCurrentDirectoryA(cur_dir);
*/
}

CString parseParams(LPCSTR fn, LPCSTR params)
{
	CString res;
	// $dir$  -directory
	// $file$ -file name
	// $ext$  -file extention
	// $full_file_name$ -given full file name with path

	string16	drive;
	string_path dir;
	string_path file_name;
	string16	ext;
	_splitpath(fn,drive,dir,file_name,ext);

	const char* c = params;
	while(c){
		if(c[0]=='$'){//alias begin
			if(c==strstr(c,"$dir$")){
				res.Append(dir);
				c+=xr_strlen("$dir$");
			}else
			if(c==strstr(c,"$file$")){
				res.Append(file_name);
				c+=xr_strlen("$file$");
			}else
			if(c==strstr(c,"$ext$")){
				res.Append(ext);
				c+=xr_strlen("$ext$");
			}else
			if(c==strstr(c,"$full_file_name$")){
				res.Append(fn);
				c+=xr_strlen("$full_file_name$");
			}
		}else{
			res.AppendChar(c[0]);
			if(*(c+1))c+=1;
			else
				break;
		}
	}
	
	return res;
}


CTask*	CTaskFacrory::create_task(ETaskType t)
{
	switch (t){
		case eTaskCopyFiles:{
			return xr_new<CTaskCopyFiles>();
		}break;
		case eTaskCopyFolder:{
			return xr_new<CTaskCopyFolder>();
		}break;
		case eTaskDelete:{
			return xr_new<CTaskDelete>();
		}break;
		case eTaskRunExecutable:{
			return xr_new<CTaskExecute>();
		}break;
		case eTaskBatchExecute:{
			return xr_new<CTaskBatchExecute>();
		}break;
		case eTaskUnknown:{
			R_ASSERT("unknown task type");
			return 0;
		}break;
	}
}

CTask*	CTaskFacrory::create_task(CInifile& ini, LPCSTR section)
{
	ETaskType t		= strToType(ini.r_string(section,	"type") );
	return create_task(t);
}

/*
shared_str getBestSectionName(CTask* t)
{
	string128 sname;
	string128 s_res_name;
	int i=0;
	for(;;){
		sprintf(sname,"s%04d",i++);
		if(!t->section_exist(sname))
		{
			strconcat(s_res_name,t->section_name(),".",sname);
			return s_res_name;
		}
	}

}*/