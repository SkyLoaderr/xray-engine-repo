#include "stdafx.h"
#include "contextmenu.h"
#include "..\xr_ini.h"
  
const float fade_speed = 8.0f;

CContextMenu::~CContextMenu(){
	for (vector<MenuItem>::iterator I=Items.begin(); I!=Items.end(); I++){
		Engine.Event.Destroy(I->Event);
		_FREE(I->Name);
		_FREE(I->Param);
	}
	_FREE(Name);
}
void CContextMenu::Load(CInifile* INI, LPCSTR SECT){
	CInifile::Sect& S = INI->ReadSection(SECT);
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++){
		char	Event[128],Param[128];
		Event[0]=0; Param[0]=0;
		sscanf	(I->second,"%[^,],%s",Event,Param);
		MenuItem	Item;
		Item.Name	= xr_strdup(I->first);
		Item.Event	= Engine.Event.Create(Event);
		Item.Param	= xr_strdup(Param);
		Items.push_back(Item);
	}
}
void CContextMenu::Render(CGameFont* F, u32 cT, u32 cI, float s)
{
	F->SetSize	(0.05f);
	F->SetColor	(cT);
	F->OutNext	("%s",Name);
	F->SetColor	(cI);
	F->SetSize	(0.03f);
	for (u32 i=0; i<Items.size(); i++)
		F->OutNext("%d. %s", i, (char*)Items[i].Name);
}
void CContextMenu::Select(int I){
	if (I>=0 && I<(int)(Items.size())){
		MenuItem& M = Items[I];
		Engine.Event.Signal(M.Event, u32(M.Param));
	}
}
