// CustomItem.h: interface for the CCustomItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMITEM_H__50C86D07_B38B_4155_B724_7904136B0166__INCLUDED_)
#define AFX_CUSTOMITEM_H__50C86D07_B38B_4155_B724_7904136B0166__INCLUDED_
#pragma once

#include "..\xr_object.h"

enum EItemState {
	esShowing,
	esHidding,
	esShow,
	esHide,
	es_forcedword = DWORD(-1)
};

class ENGINE_API CCustomItem : public CObject  
{
private:
	float				fTime;
	float				fStateTime;

	Fvector				start_pos;
	EItemState			state;

	int					hSndTake;
public:
	int					iValue;
	CLASS_ID			clsid_target;
public:
						CCustomItem		();
	virtual				~CCustomItem	();

	virtual void		SetState		(EItemState e);
	virtual void		OnMove			();
	virtual void		Load			(CInifile *pIni, const char *section);
	virtual void		OnNear			(CObject* O);
};

#endif // !defined(AFX_CUSTOMITEM_H__50C86D07_B38B_4155_B724_7904136B0166__INCLUDED_)
