//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_ActorTools.h"
#include "UI_Main.h"
#include "EditObject.h"
#include "PropertiesList.h"               
#include "ItemList.h"
#include "motion.h"
#include "bone.h"
//------------------------------------------------------------------------------

void __fastcall CActorTools::OnObjectItemFocused(ListItemsVec& items)
{
	PropItemVec props;
	m_EditMode	= emObject;
    
    // unselect
    if (m_pEditObject){
    	m_pEditObject->ResetSAnimation(false);
//.	    StopMotion					();     // убрал из-за того что не миксятся анимации в режиме енжине
    	m_pEditObject->SelectBones	(false);
    }
                            
	if (!items.empty()){
	    for (ListItemsIt it=items.begin(); it!=items.end(); it++){
            ListItem* prop = *it;
            m_ObjectItems->pmItems->Items->Clear();
            if (prop){
                m_EditMode = EEditMode(prop->Type());
                switch(m_EditMode){
                case emObject:
                    FillObjectProperties(props,MOTIONS_PREFIX,prop);
                break;
                case emMotion:{ 	
                    FillMotionProperties(props,MOTIONS_PREFIX,prop);
                    CSMotion* MOT		= ((CSMotion*)prop->m_Object);
                    SetCurrentMotion	(MOT?MOT->Name():"");
                }break;
                case emBone:{
                    FillBoneProperties	(props,BONES_PREFIX,prop);
                    CBone* BONE 		= (CBone*)prop->m_Object;
                    if (BONE) 			BONE->Select(TRUE);
                }break;
                case emSurface: 
                    FillSurfaceProperties(props,SURFACES_PREFIX,prop);
                break;
                case emMesh: 
                break;
                }
            }
        }
    }
	m_Props->AssignItems(props);
    UI->RedrawScene();
}
//------------------------------------------------------------------------------

void __fastcall CActorTools::PMMotionItemClick(TObject *Sender)
{
	R_ASSERT(m_pEditObject);
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
		PropItem* prop = (PropItem*)mi->Tag;
        if (prop){
        }
    }
}

void __fastcall CActorTools::OnChangeTransform(PropValue* sender)
{
//    OnMotionKeysModified();
	UI->RedrawScene();
}
//------------------------------------------------------------------------------

void __fastcall CActorTools::OnMotionEditClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	R_ASSERT(m_pEditObject);
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    AnsiString fn;
    switch (V->btn_num){
    case 0:{ // append
        AnsiString folder,nm,fnames,full_name;
        if (EFS.GetOpenName(_smotion_,fnames,true)){
            AStringVec lst;
            _SequenceToList(lst,fnames.c_str());
            bool bRes = false;
            for (AStringIt it=lst.begin(); it!=lst.end(); it++)
                if (AppendMotion(it->c_str())) bRes=true;
            UI->Command	(COMMAND_UPDATE_PROPERTIES);
			if (bRes)	OnMotionKeysModified();
            else 		ELog.DlgMsg(mtError,"Append not completed.");
            bModif = false;
        }else
        	bModif = false;
    }break;
    case 1:{ // delete
    	ListItemsVec items;
        if (m_ObjectItems->GetSelected(MOTIONS_PREFIX,items,true)){
            if (ELog.DlgMsg(mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, "Delete selected %d item(s)?",items.size()) == mrYes){
            	for (ListItemsIt it=items.begin(); it!=items.end(); it++){
                	VERIFY((*it)->m_Object);
	                RemoveMotion(((CSMotion*)(*it)->m_Object)->Name()); 
                }
		    	m_ObjectItems->LockUpdating();
				SelectListItem(MOTIONS_PREFIX,0,true,false,false);
		    	m_ObjectItems->UnlockUpdating();
                UI->Command(COMMAND_UPDATE_PROPERTIES);
				OnMotionKeysModified();
                bModif = false;
            }else{
		        bModif = false;
            }
        }else
        	ELog.DlgMsg(mtInformation,"Select at least one motion.");
    }break;
    case 2:{ // save
        int mr=ELog.DlgMsg(mtConfirmation, "Save selected motions only?");
        if (mr!=mrCancel){
            if (EFS.GetSaveName(_smotion_,fn,0,1)){
                switch (mr){
                case mrYes: SaveMotions(fn.c_str(),true); break;
                case mrNo: 	SaveMotions(fn.c_str(),false);break;
                }
            }
        }
        bModif = false;
    }break;
	}
}

void CActorTools::RealUpdateProperties()
{
	m_Flags.set(flRefreshProps,FALSE);
	ListItemsVec items;
	if (m_pEditObject){
        LHelper.CreateItem					(items,OBJECT_PREFIX,0,emObject);
        m_pEditObject->FillSurfaceList		(SURFACES_PREFIX,items,emSurface);
        // skin
        if (m_pEditObject->IsSkeleton()){
	        m_pEditObject->FillMotionList	(MOTIONS_PREFIX,items,emMotion);
            m_pEditObject->FillBoneList		(BONES_PREFIX,items,emBone);
        }
	}
	m_ObjectItems->AssignItems(items,false);//,"",true);
    // if appended motions exist - select it
    if (!appended_motions.empty()){
		SelectListItem(MOTIONS_PREFIX,0,true,false,true);
    	m_ObjectItems->LockUpdating();
		for (SMotionIt m_it=appended_motions.begin(); m_it!=appended_motions.end(); m_it++)
			SelectListItem(MOTIONS_PREFIX,(*m_it)->Name(),true,true,false);
    	m_ObjectItems->UnlockUpdating();
		appended_motions.clear();
    }
}
//------------------------------------------------------------------------------
                               
void CActorTools::OnMotionTypeChange(PropValue* sender)
{
	RefreshSubProperties();
}
//------------------------------------------------------------------------------
                        
void __fastcall CActorTools::OnMotionNameChange(PropValue* V)
{
    OnMotionKeysModified();
}
//------------------------------------------------------------------------------

void __fastcall CActorTools::OnMotionControlClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: PlayMotion();	break;
    case 1: StopMotion();	break;
    case 2: PauseMotion();	break;
	}
    bModif = false;
}
//------------------------------------------------------------------------------
static TokenValue3Custom::ItemVec bone_parts;

void CActorTools::FillMotionProperties(PropItemVec& items, LPCSTR pref, ListItem* sender)
{
	R_ASSERT(m_pEditObject);
	CSMotion* SM = (CSMotion*)sender->m_Object;

    PHelper.CreateCaption	(items, FHelper.PrepareKey(pref,"Global\\Motion count"),	m_pEditObject->SMotionCount());
    ButtonValue* B;                         
    B=PHelper.CreateButton	(items, FHelper.PrepareKey(pref,"Global\\Edit"),			"Append,Delete,Save",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnMotionEditClick; 
    if (SM){                                                                     
        B=PHelper.CreateButton	(items, FHelper.PrepareKey(pref,"Motion\\Control"),		"Play,Stop,Pause",ButtonValue::flFirstOnly);
        B->OnBtnClickEvent		= OnMotionControlClick;
	    PHelper.CreateCaption	(items, FHelper.PrepareKey(pref,"Motion\\Frame\\Start"),	SM->FrameStart());
	    PHelper.CreateCaption	(items, FHelper.PrepareKey(pref,"Motion\\Frame\\End"),		SM->FrameEnd());
	    PHelper.CreateCaption	(items, FHelper.PrepareKey(pref,"Motion\\Frame\\Length"),	SM->Length());
        PropValue* P=0;                                              
        P=PHelper.CreateName	(items,FHelper.PrepareKey(pref,"Motion\\Name"),		&SM->Name(), sizeof(SM->Name()), sender);
        P->OnChangeEvent		= OnMotionNameChange;
        PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\Speed"),	&SM->fSpeed,  0.f,10.f,0.01f,2);
        PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\Accrue"),	&SM->fAccrue, 0.f,10.f,0.01f,2);
        PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\Falloff"), 	&SM->fFalloff,0.f,10.f,0.01f,2);

        PropValue /**C=0,*/*TV=0;
        TV = PHelper.CreateFlag<Flags8>(items,FHelper.PrepareKey(pref,"Motion\\Type FX"),	&SM->m_Flags, esmFX);
        TV->OnChangeEvent		= OnMotionTypeChange;
        if (SM->m_Flags.is(esmFX)){
        	bone_parts.clear	();
            for (BoneIt it=m_pEditObject->FirstBone(); it!=m_pEditObject->LastBone(); it++)
				bone_parts.push_back	(TokenValue3Custom::Item((*it)->index,(*it)->Name()));
			PHelper.CreateToken3<u16>	(items,FHelper.PrepareKey(pref,"Motion\\FX\\Start bone"),	(u16*)&SM->m_BoneOrPart,	&bone_parts);

            PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,"Motion\\FX\\Power"),	 	&SM->fPower,   	0.f,10.f,0.01f,2);
        }else{
        	bone_parts.clear	();
            bone_parts.push_back(TokenValue3Custom::Item(BI_NONE,"--all bones--"));
            for (BPIt it=m_pEditObject->FirstBonePart(); it!=m_pEditObject->LastBonePart(); it++)
				bone_parts.push_back	(TokenValue3Custom::Item(it-m_pEditObject->FirstBonePart(),it->alias));
			PHelper.CreateToken3<u16>	(items,FHelper.PrepareKey(pref,"Motion\\Cycle\\Bone part"),		&SM->m_BoneOrPart,	&bone_parts);
            PHelper.CreateFlag<Flags8>	(items,FHelper.PrepareKey(pref,"Motion\\Cycle\\Stop at end"),	&SM->m_Flags,	esmStopAtEnd);
            PHelper.CreateFlag<Flags8>	(items,FHelper.PrepareKey(pref,"Motion\\Cycle\\No mix"),	  	&SM->m_Flags,	esmNoMix);
            PHelper.CreateFlag<Flags8>	(items,FHelper.PrepareKey(pref,"Motion\\Cycle\\Sync part"),		&SM->m_Flags,	esmSyncPart);
        }
    }
}
//------------------------------------------------------------------------------

xr_token joint_types[]={
	{ "Rigid",					jtRigid		},
	{ "Cloth",					jtCloth		},
	{ "Joint",					jtJoint	 	},
	{ "Wheel [Steer-X/Roll-Z]", jtWheel		},
//	{ "Wheel [Steer-X/Roll-Z]", jtWheelXZ	},
//	{ "Wheel [Steer-X/Roll-Y]", jtWheelXY	},
//	{ "Wheel [Steer-Y/Roll-X]", jtWheelYX	},
//	{ "Wheel [Steer-Y/Roll-Z]", jtWheelYZ	},
//	{ "Wheel [Steer-Z/Roll-X]", jtWheelZX	},
//	{ "Wheel [Steer-Z/Roll-Y]", jtWheelZY	},
	{ 0,						0			}
};

xr_token shape_types[]={
	{ "None",		SBoneShape::stNone		},
	{ "Box",		SBoneShape::stBox		},
	{ "Sphere",		SBoneShape::stSphere	},
	{ "Cylinder",	SBoneShape::stCylinder	},
	{ 0,			0						}
};

static const LPCSTR axis[3]=
{ "Axis X", "Axis Y", "Axis Z" };

void __fastcall CActorTools::OnJointTypeChange(PropValue* V)
{
	UI->Command(COMMAND_UPDATE_PROPERTIES);
}
void __fastcall CActorTools::OnShapeTypeChange(PropValue* V)
{
	UI->RedrawScene();
	UI->Command(COMMAND_UPDATE_PROPERTIES);
}
void __fastcall CActorTools::OnBindTransformChange(PropValue* V)
{
	R_ASSERT(m_pEditObject);
    m_pEditObject->OnBindTransformChange();
	UI->RedrawScene();
}

void __fastcall CActorTools::OnBoneShapeClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	R_ASSERT(m_pEditObject);
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: m_pEditObject->GenerateBoneShape(false); break;
    case 1: m_pEditObject->GenerateBoneShape(true); break;
	}
}

void __fastcall CActorTools::OnBoneEditClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	R_ASSERT(m_pEditObject);
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: 
    	m_pEditObject->GotoBindPose(); 
		UI->Command(COMMAND_UPDATE_PROPERTIES);
	    bModif = false;
    break;
    case 1:
    	if (ELog.DlgMsg(mtConfirmation,"Are you sure to reset IK data?")==mrYes)
			m_pEditObject->ResetBones(); 
	    bModif = true;
    break;
    case 2:
		m_pEditObject->ClampByLimits(false); 
		UI->Command(COMMAND_UPDATE_PROPERTIES);
	    bModif = false;
    break;
	}
}

void __fastcall CActorTools::OnBoneFileClick(PropValue* sender, bool& bModif, bool& bSafe)
{              
	R_ASSERT(m_pEditObject);
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0:{ 
    	AnsiString fn;
    	if (EFS.GetOpenName("$sbones$",fn)){
        	IReader* R = FS.r_open(fn.c_str());
	    	if (m_pEditObject->LoadBoneData(*R))	ELog.DlgMsg(mtInformation,"Bone data succesfully loaded.");
            else                                    ELog.DlgMsg(mtError,"Failed to load bone data.");
            FS.r_close(R);
        }else{
            bModif = false;
        }
    }break;
    case 1:{ 
    	AnsiString fn;
    	if (EFS.GetSaveName("$sbones$",fn)){
        	IWriter* W = FS.w_open(fn.c_str());
	    	m_pEditObject->SaveBoneData(*W);
            FS.w_close(W);
            bModif = false;
        }
    }break;
	}
}

void __fastcall CActorTools::OnBoneLimitsChange(PropValue* sender)
{
	m_pEditObject->ClampByLimits(true);
}

void CActorTools::FillBoneProperties(PropItemVec& items, LPCSTR pref, ListItem* sender)
{
	R_ASSERT(m_pEditObject);
	CBone* BONE = (CBone*)sender->m_Object;

    PHelper.CreateCaption	(items, FHelper.PrepareKey(pref,"Global\\Bone count"),	m_pEditObject->BoneCount());
    ButtonValue* B;
    B=PHelper.CreateButton	(items, FHelper.PrepareKey(pref,"Global\\File"),"Load,Save",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnBoneFileClick;
    B=PHelper.CreateButton	(items, FHelper.PrepareKey(pref,"Global\\Edit"),"Bind pose,Reset IK,Clamp limits",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnBoneEditClick;
    B=PHelper.CreateButton	(items, FHelper.PrepareKey(pref,"Global\\Generate Shape"),"All, Selected",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnBoneShapeClick;
    if (BONE){
    	PropValue* V;
        PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Bone\\Name"),						BONE->Name());
		PHelper.CreateChoose		(items,	FHelper.PrepareKey(pref,"Bone\\Game Material"),				BONE->game_mtl, sizeof(BONE->game_mtl), smGameMaterial);
        PHelper.CreateFloat			(items, FHelper.PrepareKey(pref,"Bone\\Mass"),						&BONE->mass, 			0.f, 10000.f);
        PHelper.CreateVector		(items, FHelper.PrepareKey(pref,"Bone\\Center Of Mass"),			&BONE->center_of_mass, 	-10000.f, 10000.f);
        V=PHelper.CreateVector		(items, FHelper.PrepareKey(pref,"Bone\\Bind Position"),				&BONE->_RestOffset(),	-10000.f, 10000.f);	V->OnChangeEvent = OnBindTransformChange;
        V=PHelper.CreateAngle3		(items, FHelper.PrepareKey(pref,"Bone\\Bind Rotation"),				&BONE->_RestRotate());						V->OnChangeEvent = OnBindTransformChange;
        PHelper.CreateFlag<Flags16>	(items, FHelper.PrepareKey(pref,"Bone\\Flags\\No Pickable"),		&BONE->shape.flags, SBoneShape::sfNoPickable);
        PHelper.CreateFlag<Flags16>	(items, FHelper.PrepareKey(pref,"Bone\\Flags\\Remove After Break"),	&BONE->shape.flags, SBoneShape::sfRemoveAfterBreak);
		V=PHelper.CreateToken<u16>	(items,	FHelper.PrepareKey(pref,"Bone\\Shape\\Type"),				&BONE->shape.type, shape_types);			V->OnChangeEvent = OnShapeTypeChange;
        switch (BONE->shape.type){
        case SBoneShape::stBox:
	        PHelper.CreateVector	(items, FHelper.PrepareKey(pref,"Bone\\Shape\\Box\\Center"),		&BONE->shape.box.m_translate, -10000.f, 10000.f);
	        PHelper.CreateVector	(items, FHelper.PrepareKey(pref,"Bone\\Shape\\Box\\Half Size"),  	&BONE->shape.box.m_halfsize, 0.f, 1000.f);
        break;
        case SBoneShape::stSphere:
	        PHelper.CreateVector	(items, FHelper.PrepareKey(pref,"Bone\\Shape\\Sphere\\Position"),	&BONE->shape.sphere.P, -10000.f, 10000.f);
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Shape\\Sphere\\Radius"),  	&BONE->shape.sphere.R, 0.f, 1000.f);
        break;
        case SBoneShape::stCylinder:
	        PHelper.CreateVector	(items, FHelper.PrepareKey(pref,"Bone\\Shape\\Cylinder\\Center"),	&BONE->shape.cylinder.m_center, -10000.f, 10000.f);
	        PHelper.CreateVector	(items, FHelper.PrepareKey(pref,"Bone\\Shape\\Cylinder\\Direction"),&BONE->shape.cylinder.m_direction, 0.f, 10000.f);
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Shape\\Cylinder\\Height"),	&BONE->shape.cylinder.m_height, 0.f, 1000.f);
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Shape\\Cylinder\\Radius"),	&BONE->shape.cylinder.m_radius, 0.f, 1000.f);
        break;
        }

        Fvector lim_rot;
        Fmatrix mLocal,mBind,mBindI;
        mBind.setXYZi				(BONE->_RestRotate().x,BONE->_RestRotate().y,BONE->_RestRotate().z);
        mBindI.invert				(mBind);
        mLocal.setXYZi				(BONE->_Rotate().x,BONE->_Rotate().y,BONE->_Rotate().z);
        mLocal.mulA					(mBindI);
        mLocal.getXYZi				(lim_rot);
        lim_rot.x 					= rad2deg(lim_rot.x);
        lim_rot.y 					= rad2deg(lim_rot.y);
        lim_rot.z 				 	= rad2deg(lim_rot.z);
        
        PHelper.CreateCaption		(items, FHelper.PrepareKey(pref,"Bone\\Joint\\Current Rotation"),	AnsiString().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(lim_rot)));
    	SJointIKData& data			= BONE->IK_data;
        V=PHelper.CreateFlag<Flags32>(items, FHelper.PrepareKey(pref,"Bone\\Joint\\Breakable"),			&data.ik_flags, SJointIKData::flBreakable);
        V->OnChangeEvent			= OnJointTypeChange;
        if (data.ik_flags.is(SJointIKData::flBreakable)){
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Joint\\Break Force"),		&data.break_force, 	0.f, 1000000000.f);
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Joint\\Break Torque"),		&data.break_torque, 0.f, 1000000000.f);
        }
		V=PHelper.CreateToken<u32>	(items,	FHelper.PrepareKey(pref,"Bone\\Joint\\Type"),				(u32*)&data.type,	joint_types);  	V->OnChangeEvent = OnJointTypeChange;
        switch (data.type){
        case jtRigid: 
        break; 
        case jtCloth:{ 
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Joint\\Spring Factor"),	&data.spring_factor, 	0.f, 1000.f);
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Joint\\Damping Factor"),	&data.damping_factor, 	0.f, 1000.f);
        }break; 
        case jtJoint:{
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Joint\\Spring Factor"),	&data.spring_factor, 	0.f, 1000.f);
	        PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Bone\\Joint\\Damping Factor"),	&data.damping_factor, 	0.f, 1000.f);
            for (int k=0; k<3; k++){
		        V=PHelper.CreateAngle	(items,FHelper.PrepareKey(pref,"Bone\\Joint\\Limits",axis[k],"Min"),			&data.limits[k].limit.x, 		-M_PI, 0.f);
                V->OnChangeEvent	= OnBoneLimitsChange;
		        V=PHelper.CreateAngle	(items,FHelper.PrepareKey(pref,"Bone\\Joint\\Limits",axis[k],"Max"),			&data.limits[k].limit.y, 		0.f, M_PI);
                V->OnChangeEvent	= OnBoneLimitsChange;
		        V=PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,"Bone\\Joint\\Limits",axis[k],"Spring Factor"),	&data.limits[k].spring_factor, 	0.f, 1000.f);
                V->OnChangeEvent	= OnBoneLimitsChange;
		        V=PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,"Bone\\Joint\\Limits",axis[k],"Damping Factor"),	&data.limits[k].damping_factor, 0.f, 1000.f);
                V->OnChangeEvent	= OnBoneLimitsChange;
            }
        }break; 
		case jtWheel:
//		case jtWheelXZ:
//		case jtWheelXY:
//		case jtWheelYX:
//		case jtWheelYZ:
//		case jtWheelZX:
//		case jtWheelZY:
        {
//	        int idx = (data.type-jtWheelXZ)/2;
	        int idx = (data.type-jtWheel)/2;
	        PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Bone\\Joint\\Spring Factor"),			&data.spring_factor, 0.f, 1000.f);
	        PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Bone\\Joint\\Damping Factor"),			&data.damping_factor, 0.f, 1000.f);
            V=PHelper.CreateAngle	(items,FHelper.PrepareKey(pref,"Bone\\Joint\\Steer\\Limits Min"),		&data.limits[idx].limit.x, -PI_DIV_2, 0.f);
			V->OnChangeEvent		= OnBoneLimitsChange;
            V=PHelper.CreateAngle	(items,FHelper.PrepareKey(pref,"Bone\\Joint\\Steer\\Limits Max"),		&data.limits[idx].limit.y, 0, PI_DIV_2);
            V->OnChangeEvent		= OnBoneLimitsChange;
        }break;
        }
    }
}
//------------------------------------------------------------------------------

void CActorTools::FillSurfaceProperties(PropItemVec& items, LPCSTR pref, ListItem* sender)
{
	R_ASSERT(m_pEditObject);
	CSurface* SURF = (CSurface*)sender->m_Object;
    PHelper.CreateCaption			(items, FHelper.PrepareKey(pref,"Statistic\\Count"),	m_pEditObject->SurfaceCount());
    if (SURF){
        PHelper.CreateCaption		(items,FHelper.PrepareKey(pref,"Surface\\Name"),		SURF->_Name());
        AnsiString _pref			= FHelper.PrepareKey(pref,"Surface");
	    m_pEditObject->FillSurfaceProps(SURF,_pref.c_str(),items);
    }
}
//------------------------------------------------------------------------------

void CActorTools::FillObjectProperties(PropItemVec& items, LPCSTR pref, ListItem* sender)
{
	R_ASSERT(m_pEditObject);
    PropValue* V=0;
    PHelper.CreateFlag<Flags32>(items, "Object\\Flags\\Make Progressive",	&m_pEditObject->m_Flags,		CEditableObject::eoProgressive);
    V=PHelper.CreateVector	(items, "Object\\Transform\\Position",		&m_pEditObject->a_vPosition, 	-10000,	10000,0.01,2);
    V->OnChangeEvent 		= OnChangeTransform;
    V=PHelper.CreateAngle3	(items, "Object\\Transform\\Rotation",		&m_pEditObject->a_vRotate, 		-10000,	10000,0.1,1);
    V->OnChangeEvent 		= OnChangeTransform;
    V=PHelper.CreateCaption	(items, "Object\\Transform\\BBox Min",		AnsiString().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(m_pEditObject->GetBox().min)));
    V=PHelper.CreateCaption	(items, "Object\\Transform\\BBox Max",		AnsiString().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(m_pEditObject->GetBox().max)));
    m_pEditObject->FillSummaryProps	("Object\\Summary",items);
}
//------------------------------------------------------------------------------

void CActorTools::SelectListItem(LPCSTR pref, LPCSTR name, bool bVal, bool bLeaveSel, bool bExpand)
{
	AnsiString nm = (name&&name[0])?FHelper.PrepareKey(pref,name):AnsiString(pref);
	m_ObjectItems->SelectItem(nm,bVal,bLeaveSel,bExpand);
	if (pref){
    	m_ObjectItems->SelectItem(pref,true,true,bExpand);
    }
}
//------------------------------------------------------------------------------

