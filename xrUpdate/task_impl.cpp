#include "stdafx.h"
#include "upd_task.h"
#include "FileOperations.h"
#include <direct.h>

//for batch task
CString parseParams(LPCSTR fn, LPCSTR params);
BOOL copy_file(LPCSTR src, LPCSTR dst_fldr, LPCSTR dst_fn);
BOOL rename_file(LPCSTR src, LPCSTR dst);
BOOL check_RO(LPCSTR src, BOOL& old);

BOOL file_exist(LPCSTR src);
BOOL mk_bk_rename(LPCSTR src, int start_from=0);
void copy_folder(LPCSTR src, LPCSTR dst);
void checkRightFolderName(LPSTR fn);

CFileOperation fo;      // create object


void CTask::exec()
{
	if( !is_enabled() ) return;
	sort_sub_tasks();
	CTaskArray::iterator sub_task_it =	m_sub_tasks.begin();
	for(;sub_task_it !=m_sub_tasks.end();++sub_task_it)
		(*sub_task_it)->run();
//	CTaskArray::reverse_iterator sub_task_it =	m_sub_tasks.rbegin();
//	for(;sub_task_it !=m_sub_tasks.rend();++sub_task_it)
//		(*sub_task_it)->run();

}


void CTaskCopyFiles::run()
{
	if( !is_enabled() ) return;
	CTask::exec();

	CFileNamesArray::iterator it = m_file_names.begin();
	string_path file_name;
	string16	drive;
	string_path dir;
	string16	ext;


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
	CTask::exec();

	Msg("[%s]:Copying folder %s to %s ",name(),*m_source_folder, *m_target_folder);
	check_RO(*m_target_folder,attr_save);
	copy_folder(*m_source_folder, *m_target_folder);
}

void CTaskExecute::run	()
{
//	проверить : cmd.exe attrib.... copy,,, sds
	if( !is_enabled() ) return;
	CTask::exec();

	string_path cur_dir;
	if( xr_strlen(m_working_folder) ){
		GetCurrentDirectoryA(_MAX_PATH,cur_dir);
		SetCurrentDirectoryA(*m_working_folder);
	};
	Msg("[%s]:Running %s %s", name(), *m_app_name, (xr_strlen(m_params))?" ":*m_params);
	spawnl(_P_WAIT, *m_app_name, (xr_strlen(m_params))?*m_params:" ");
	Msg("[%s]:Done.",name());

	if( xr_strlen(m_working_folder) )
		SetCurrentDirectoryA(cur_dir);
}

void CTaskBatchExecute::run					()
{
	if( !is_enabled() ) return;
	CTask::exec();

	// если надо установить раб каталог
	string_path cur_dir;
	if( xr_strlen(m_working_folder) ){
		GetCurrentDirectoryA(_MAX_PATH,cur_dir);
		SetCurrentDirectoryA(*m_working_folder);
	};

	CFileNamesArray::iterator it = m_file_names.begin();
	CString		params;
//	CString		command;

	if( 0==strstr(*m_params,"$") ){
		Msg("[%s]:Executing %s", name(), params.GetBuffer() );
		system(*m_params);
		Msg("[%s]:Done.",name());
	}else{
	

		for(;it!=m_file_names.end();++it){
			params = parseParams(*(*it), *m_params);
	//		spawnl(_P_WAIT, *m_app_name, (params.IsEmpty())?" ":params.GetBuffer() );
	//		command.Format( "%s %s",*m_app_name,params.GetBuffer() );
			Msg("[%s]:Executing %s", name(), params.GetBuffer() );
			system(params.GetBuffer());
			Msg("[%s]:Done.",name());
		}
	}

	if( xr_strlen(m_working_folder) )
		SetCurrentDirectoryA(cur_dir);

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
	_VerifyPath(dst_fn);
	fo.SetOverwriteMode(true); // reset OverwriteMode flag (optional)
	fo.SetAskIfReadOnly(true);
	BOOL res;
	if(FALSE==res)
		return FALSE;


	Msg("Copying...");
	if ( fo.Copy(src, dst_fldr) ){
		Msg("Done.");
		return TRUE;
	}else{
		Msg("Warning :%s", fo.m_sError.GetBuffer());
		
		if( (fo.GetErrorCode() == 32) ){
			res = mk_bk_rename(dst_fn);
			if(FALSE==res)
				return FALSE;
			return copy_file(src, dst_fldr, dst_fn);
		}
		return FALSE;
	}
}

BOOL mk_bk_rename(LPCSTR src, int start_from)
{
	string_path file_name;
	string16	drive;
	string_path dir;
	string16	ext;
	
	Msg("Creating backup copy for %s", src);
	_splitpath(src, drive, dir, file_name, ext);
	
	string16 num;
	string16 new_ext;
	itoa(start_from,num,10);
	string_path new_file_name;

	char* f = 0;
	if( f = strstr(ext,"_old") ){ //its a bk_copy !!!
		string16 ext_orig;
		ZeroMemory(ext_orig,sizeof(ext_orig));
		strncpy(ext_orig,ext, f-ext);
		strconcat(new_ext,ext_orig,"_old",num);
		strconcat(new_file_name,drive,"\\",dir,file_name,new_ext);
	}else{
		strconcat(new_ext,"_old",num);
		strconcat(new_file_name,drive,"\\",dir,file_name,ext,new_ext);
	}


	if( file_exist(new_file_name) )
		if( !mk_bk_rename(new_file_name, ++start_from) )
			return FALSE;

	return rename_file(src, new_file_name);
}

BOOL rename_file(LPCSTR src, LPCSTR dst)
{
	fo.SetOverwriteMode(true); // reset OverwriteMode flag (optional)
	fo.SetAskIfReadOnly(true);
	
	check_RO(dst);
	if( fo.Rename(src, dst) ){
		Msg("Done.");
		return TRUE;
	}else
		Msg("Error :%s", fo.m_sError.GetBuffer() );

	return TRUE;
}

BOOL check_RO(LPCSTR src, BOOL& old)
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

BOOL file_exist(LPCSTR src)
{
	WIN32_FIND_DATA fd;
	HANDLE h = ::FindFirstFile(src, &fd);
	 BOOL res = h!=INVALID_HANDLE_VALUE;

	 FindClose(h);
	 return res;
}

void copy_folder(LPCSTR src, LPCSTR dst)
{
	WIN32_FIND_DATA fd;
	string_path fldr_all_files;
	strcpy(fldr_all_files, src);
	checkRightFolderName(fldr_all_files);
	strconcat(fldr_all_files, fldr_all_files,"*.*");

	HANDLE h = ::FindFirstFile(fldr_all_files, &fd);
	while( h != INVALID_HANDLE_VALUE ){
		
		if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){
			//make new dst...
			if( (0==strcmp(fd.cFileName,"."))|| (0==strcmp(fd.cFileName,"..")) ){
				::FindNextFile(h, &fd);
				continue;
			}

			string_path new_dst_folder;
			strcpy(new_dst_folder,dst);
			checkRightFolderName(new_dst_folder);
			strconcat(new_dst_folder,new_dst_folder,fd.cFileName);

			string_path new_src_folder;
			strcpy(new_src_folder,src);
			checkRightFolderName(new_src_folder);
			strconcat(new_src_folder,new_src_folder,fd.cFileName);

			copy_folder(new_src_folder, new_dst_folder);

		}else{
			string_path new_dist_filename;
			string_path new_src_filename;
			string16	drive;
			string_path dir;
			string_path file_name;
			string16	ext;
	
			strcpy(new_src_filename,src);
			checkRightFolderName(new_src_filename);
			strconcat(new_src_filename,new_src_filename,fd.cFileName);

			_splitpath(new_src_filename, drive, dir, file_name, ext);

			strcpy(new_dist_filename,dst);
			checkRightFolderName(new_dist_filename);

			strconcat(new_dist_filename,new_dist_filename, file_name, ext );


			copy_file(new_src_filename, dst, new_dist_filename);
		}

		if(!::FindNextFile(h, &fd))
			break;
	}
	FindClose(h);

}

void checkRightFolderName(LPSTR fn)
{
	if( (fn+xr_strlen(fn)) != "\\")
		strcat(fn,"\\");
}

void _VerifyPath(LPCSTR path)
{
	string1024 tmp;
	for(int i=0;path[i];i++){
		if( path[i]!='\\' || i==0 )
			continue;
		Memory.mem_copy( tmp, path, i );
		tmp[i] = 0;
        _mkdir(tmp);
	}
}
