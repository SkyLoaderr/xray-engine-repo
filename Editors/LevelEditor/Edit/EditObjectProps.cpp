#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#include "ui_main.h"
#include "PropertiesListHelper.h"
#include "ItemListHelper.h"
#include "motion.h"
#include "bone.h"

void __fastcall	CEditableObject::OnChangeShader(PropValue* prop)
{
    OnDeviceDestroy	();
    UI.RedrawScene	();
}
//---------------------------------------------------------------------------

void CEditableObject::FillSurfaceProps(CSurface* SURF, LPCSTR pref, PropItemVec& items)
{
    PropValue* V;
    V=PHelper.CreateChoose		(items, FHelper.PrepareKey(pref,"Texture"), 	&SURF->m_Texture, 	smTexture);		V->OnChangeEvent=OnChangeShader;
    V=PHelper.CreateChoose		(items, FHelper.PrepareKey(pref,"Shader"), 		&SURF->m_ShaderName,smEShader);		V->OnChangeEvent=OnChangeShader;
	if (IsStatic()){
	    V=PHelper.CreateChoose	(items, FHelper.PrepareKey(pref,"Compile"), 	&SURF->m_ShaderXRLCName,smCShader);
    	PHelper.CreateChoose	(items, FHelper.PrepareKey(pref,"Game Mtl"),	&SURF->m_GameMtlName, smGameMaterial);
    }
    V=PHelper.CreateFlag<Flags32>(items, FHelper.PrepareKey(pref,"2 Sided"), 	&SURF->m_Flags, CSurface::sf2Sided);V->OnChangeEvent=OnChangeShader;
}
//---------------------------------------------------------------------------

void CEditableObject::FillBasicProps(LPCSTR pref, PropItemVec& items)
{
	PropValue* V=0;
	PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Reference Name"),		m_LibName.c_str());
	PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Version\\Owner Name"),	m_CreateName.c_str());
	PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Version\\Modif Name"),	m_ModifName.c_str());
	PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Version\\Creation Time"),Trim(AnsiString(ctime(&m_CreateTime))));
	PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Version\\Modified Time"),Trim(AnsiString(ctime(&m_ModifTime))));
    PHelper.CreateFlag<Flags32>	(items,	FHelper.PrepareKey(pref,"Flags\\Dynamic"),		&m_Flags,		CEditableObject::eoDynamic);
    PHelper.CreateFlag<Flags32>	(items,	FHelper.PrepareKey(pref,"Flags\\HOM"),			&m_Flags,		CEditableObject::eoHOM);
    V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,"Flags\\Use LOD"),		&m_Flags,		CEditableObject::eoUsingLOD); V->OnChangeEvent=OnChangeShader;
    PHelper.CreateFlag<Flags32>	(items,	FHelper.PrepareKey(pref,"Flags\\Multiple Usage"),&m_Flags,		CEditableObject::eoMultipleUsage);
    V=PHelper.CreateVector		(items, FHelper.PrepareKey(pref,"Transform\\Position"),	&t_vPosition,	-10000,	10000,0.01,2); 	V->OnChangeEvent=OnChangeTransform;
    V=PHelper.CreateAngle3		(items, FHelper.PrepareKey(pref,"Transform\\Rotation"),	&t_vRotate, 	-10000,	10000,0.1,1);	V->OnChangeEvent=OnChangeTransform;
    V=PHelper.CreateVector		(items, FHelper.PrepareKey(pref,"Transform\\Scale"),	&t_vScale, 		0.01,	10000,0.01,2);	V->OnChangeEvent=OnChangeTransform;
    V=PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Transform\\BBox Min"),	AnsiString().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(GetBox().min)));
    V=PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Transform\\BBox Max"),	AnsiString().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(GetBox().max)));

    FillSummaryProps			(pref,items);
}
//---------------------------------------------------------------------------

void CEditableObject::FillSummaryProps(LPCSTR pref, PropItemVec& items)
{
    AnsiString t; t.sprintf("V: %d, F: %d",		GetVertexCount(),GetFaceCount());
    PHelper.CreateCaption(items,FHelper.PrepareKey(pref,"Geometry\\Object"),t.c_str());
    for (EditMeshIt m_it=FirstMesh(); m_it!=LastMesh(); m_it++){
        CEditableMesh* MESH=*m_it;
        t.sprintf("V: %d, F: %d",MESH->GetVertexCount(),MESH->GetFaceCount());
	    PHelper.CreateCaption(items,FHelper.PrepareKey(pref,AnsiString(AnsiString("Geometry\\Meshes\\")+MESH->GetName()).c_str()),t.c_str());
    }
    PHelper.CreateAText(items,FHelper.PrepareKey(pref, "Game options\\User Data"),&m_ClassScript);
}
//---------------------------------------------------------------------------

AnsiString MakeFullBoneName(CBone* bone)
{
	if (bone->Parent()){
    	return MakeFullBoneName(bone->Parent())+"\\"+bone->Name();
    }else{
    	return bone->Name();
    }
}

AnsiString MakeFullBonePath(CBone* bone)
{
	if (bone->Parent()){
    	return MakeFullBoneName(bone->Parent());
    }else{
    	return "";
    }
}

void CEditableObject::FillSurfaceList(LPCSTR pref, ListItemsVec& items, int modeID)
{
    SurfaceVec& s_lst 	= Surfaces();
	if (pref) LHelper.CreateItem(items, pref, modeID, ListItem::flSorted);
    for (SurfaceIt s_it=s_lst.begin(); s_it!=s_lst.end(); s_it++)
        LHelper.CreateItem(items, FHelper.PrepareKey(pref, (*s_it)->_Name()).c_str(), modeID, 0, *s_it);
}
//---------------------------------------------------------------------------

void CEditableObject::FillBoneList(LPCSTR pref, ListItemsVec& items, int modeID)
{
    BoneVec& b_lst 		= Bones();
	if (pref) LHelper.CreateItem(items, pref, modeID, ListItem::flSorted);
    for(BoneIt b_it=b_lst.begin(); b_it!=b_lst.end(); b_it++){
    	AnsiString pt	= MakeFullBonePath(*b_it);
    	AnsiString path	= pt.IsEmpty()?pref:FHelper.PrepareKey(pref, pt.c_str()).c_str();
        LHelper.CreateItem(items, FHelper.PrepareKey(path.c_str(), (*b_it)->Name()).c_str(), modeID, 0, *b_it);
    }
}

void CEditableObject::FillMotionList(LPCSTR pref, ListItemsVec& items, int modeID)
{
    SMotionVec&	m_lst	= SMotions();
	if (pref) LHelper.CreateItem(items, pref,  modeID, ListItem::flSorted);
    for (SMotionIt m_it=m_lst.begin(); m_it!=m_lst.end(); m_it++)
        LHelper.CreateItem(items, FHelper.PrepareKey(pref, (*m_it)->Name()).c_str(), modeID, 0, *m_it);
}



 