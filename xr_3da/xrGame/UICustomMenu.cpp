#include "stdafx.h"
#include "UICustomMenu.h"
#include "HUDManager.h"
#include "..\xr_trims.h"
#include "..\xr_ioconsole.h"

void CCustomMenuItem::Execute()			
{
	if (OnExecuteCC)		Console.Execute(OnExecuteCC);
	else if (OnExecute)		OnExecute(this);
}
//--------------------------------------------------------------------
CCustomMenuItem* UIParseMenu	(CInifile* ini, CCustomMenuItem* root, LPCSTR sect, OnExecuteEvent exec, OnItemDrawEvent draw)
{
	CCustomMenuItem* I=0;
	string256	buf,buf1;
	string64	buf2;
	int ln_cnt = ini->LineCount(sect);
	if (ln_cnt){
		if (!root)	root		= new CCustomMenuItem(0,"root",0);
		for (int i=1; i<=ln_cnt; i++){
			// append if exist menu item
			sprintf(buf,"menu_%d",i);
			if (ini->LineExists(sect,buf)){
				LPCSTR line		= ini->ReadSTRING(sect,buf);	R_ASSERT(_GetItemCount(line)==2);
				LPCSTR	name	= _GetItem(line,0,buf);
				LPCSTR	new_sect= strlwr(_GetItem(line,1,buf1));
				I				= new CCustomMenuItem(root,name,0,0,0,draw);
				UIParseMenu(ini,I,new_sect,exec,draw);
				root->AppendItem(I);
			}
			// append if exist value item
			sprintf(buf,"item_%d",i);
			if (ini->LineExists(sect,buf)){
				LPCSTR line		= ini->ReadSTRING(sect,buf);	R_ASSERT(_GetItemCount(line)>=2);
				LPCSTR	name	= _GetItem(line,0,buf);
				LPCSTR	value	= _GetItem(line,1,buf1);
				LPCSTR	execCC	= (_GetItemCount(line)>2)?_GetItem(line,2,buf2):0;
				root->AppendItem(new CCustomMenuItem(root,name,value,execCC,exec,draw));
			}
		}
	}
	return root;
}
//--------------------------------------------------------------------

CCustomMenuItem* UILoadMenu		(LPCSTR ini_name, OnExecuteEvent exec, OnItemDrawEvent draw)
{
	// check ini exist
	CCustomMenuItem* I=0;
	string256 fn;
	if (Engine.FS.Exist(fn,Path.GameData,ini_name)){
		CInifile* ini		= CInifile::Create(fn);
		I					= UIParseMenu(ini,0,"cs_buy_menu",exec,draw);
		CInifile::Destroy	(ini);
	}
	return I;
}
//--------------------------------------------------------------------

CCustomMenuItem* UIFindMenuItem	(CCustomMenuItem* root, LPCSTR name)
{
	// todo
	return 0;
}
//--------------------------------------------------------------------
