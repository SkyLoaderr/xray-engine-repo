#include "stdafx.h"
#include "upd_task.h"
#include "FileOperations.h"

//for batch task
CString parseParams(LPCSTR fn, LPCSTR params);
BOOL copy_file(LPCSTR src, LPCSTR dst_fldr, LPCSTR dst_fn);
BOOL rename_file(LPCSTR src, LPCSTR dst);
BOOL check_RO(LPCSTR src);

void CTask::run()
{
	if( !is_enabled() ) return;

	CTaskArray::reverse_iterator sub_task_it =	m_sub_tasks.rbegin();
	for(;sub_task_it !=m_sub_tasks.rend();++sub_task_it)
		(*sub_task_it)->run();

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

	if( !is_enabled() )
		return;

	for(;it!=m_file_names.end();){
		_splitpath(*(*it),drive,dir,file_name,ext);
//		strcpy(new_path,target_folder());

		Msg("[%s]:Copying %s to %s ", name(), *(*it), target_folder() );
		string_path dst_file_name;
		strconcat(dst_file_name, target_folder(), "\\", file_name, ext );
		BOOL res = copy_file( *(*it), target_folder(), dst_file_name );

		++it;
		}
}

void CTaskCopyFolder::run ()
{
	if( !is_enabled() ) return;
	CTask::run();

	CFileOperation fo;      // create object
	fo.SetOverwriteMode(true); // reset OverwriteMode flag (optional)
	fo.SetAskIfReadOnly();   // set AskIfReadonly flag (optional)

	Msg("[%s]:Copying folder %s to %s ",name(),*m_source_folder, *m_target_folder);
	check_RO(*m_target_folder);
	if (!fo.Copy(*m_source_folder, *m_target_folder)) // do Copy
	{
		Msg("[%s]:Error :%s",name(), fo.m_sError.GetBuffer());
//		fo.ShowError(); // if copy fails show error message
	}else
			Msg("[%s]:Done.",name());


/*	if (!fo.Delete("c:\\source")) // do Copy
	{
		fo.ShowError(); // if copy fails show error message
	}
*/
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
	Msg("[%s]:Running %s %s", name(), *m_app_name, (xr_strlen(m_params))?" ":*m_params);
	spawnl(_P_WAIT, *m_app_name, (xr_strlen(m_params))?" ":*m_params);
	Msg("[%s]:Done.",name());
	if( xr_strlen(m_working_folder) )
		SetCurrentDirectoryA(cur_dir);
}

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

	if( 0==strstr(*m_params,"$") ){
		Msg("[%s]:Executing %s", name(), params.GetBuffer() );
		system(*m_params);
		Msg("[%s]:Done.",name());
		return;
	}

	for(;it!=m_file_names.end();++it){
		params = parseParams(*(*it), *m_params);
//		spawnl(_P_WAIT, *m_app_name, (params.IsEmpty())?" ":params.GetBuffer() );
//		command.Format( "%s %s",*m_app_name,params.GetBuffer() );
		Msg("[%s]:Executing %s", name(), params.GetBuffer() );
		system(params.GetBuffer());
		Msg("[%s]:Done.",name());
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

BOOL copy_file(LPCSTR src, LPCSTR dst_fldr, LPCSTR dst_fn)
{
	CFileOperation fo;      // create object
	fo.SetOverwriteMode(true); // reset OverwriteMode flag (optional)
	fo.SetAskIfReadOnly(true);
	BOOL res;

	res = check_RO(dst_fn);
	if(FALSE==res)
		return FALSE;


	Msg("Copying...");
	if ( fo.Copy(src, dst_fldr) ){
		Msg("Done.");
		return TRUE;
	}else{
		Msg("Warning :%s", fo.m_sError.GetBuffer());
		
		if( (fo.GetErrorCode() == 32) ){
			string_path new_path_rename;
			strconcat(new_path_rename,dst_fn,"_old");
			Msg("Trying to rename %s to %s", dst_fn, new_path_rename);
			res = rename_file(dst_fn, new_path_rename);
			if(FALSE==res)
				return FALSE;

			return copy_file(src, dst_fldr, dst_fn);
		}
		return FALSE;
	}
}

BOOL rename_file(LPCSTR src, LPCSTR dst)
{
	CFileOperation fo;      // create object
	fo.SetOverwriteMode(true); // reset OverwriteMode flag (optional)
	fo.SetAskIfReadOnly(true);
	
	check_RO(dst);
	if( fo.Rename(src, dst) ){
		Msg("Done.");
		return TRUE;
	}

	string_path new_path_rename;
	strconcat(new_path_rename,dst,"_old");
	Msg("Trying to rename %s to %s", dst, new_path_rename);
	if( rename_file(dst, new_path_rename) )
		rename_file(src,dst);

	return TRUE;
}

BOOL check_RO(LPCSTR src)
{
	DWORD dwAttr = GetFileAttributes(src);
	//if (dwAttr == -1); //not_found
	if (  (dwAttr != -1)&& (dwAttr & FILE_ATTRIBUTE_READONLY) ){
		Msg("Warning: File %s has read-only attribute, trying to uncheck RO", src);
		dwAttr &=~ FILE_ATTRIBUTE_READONLY;
		BOOL res = SetFileAttributes(src, dwAttr);
		if(!res){
			Msg("Error: Cannot change file read-only attribute");
			return FALSE;
		}else
			Msg("Done.");
	}
	return TRUE;
}