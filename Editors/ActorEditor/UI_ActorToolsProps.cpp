//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_ActorTools.h"
#include "UI_Main.h"
#include "EditObject.h"
#include "PropertiesList.h"               
#include "motion.h"
#include "bone.h"
#include "leftbar.h"
#include "SkeletonAnimated.h"
#include "ItemList.h"
//------------------------------------------------------------------------------

void CActorTools::OnObjectItemFocused(ListItemsVec& items)
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
            if (prop){
                m_EditMode = EEditMode(prop->Type());
                switch(m_EditMode){
                case emObject:
                    FillObjectProperties(props,MOTIONS_PREFIX,prop);
                break;
                case emMotion:{ 	
                	LPCSTR m_name		= prop->Key()+xr_strlen(MOTIONS_PREFIX);
                    if (m_name[0]=='\\')m_name++;
                    FillMotionProperties(props,MOTIONS_PREFIX,prop);
                    SetCurrentMotion	(m_name);
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

void CActorTools::PMMotionItemClick(TObject *Sender)
{
	R_ASSERT(m_pEditObject);
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
		PropItem* prop = (PropItem*)mi->Tag;
        if (prop){
        }
    }
}

void CActorTools::OnChangeTransform(PropValue* sender)
{
//    OnMotionKeysModified();
	UI->RedrawScene();
}
//------------------------------------------------------------------------------

void CActorTools::OnMotionEditClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	R_ASSERT(m_pEditObject);
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    std::string fn;
    switch (V->btn_num){
    case 0:{ // append
        AnsiString folder,nm,full_name;
        std::string fnames;
        if (EFS.GetOpenName(_smotion_,fnames,true)){
            AStringVec lst;
            _SequenceToList(lst,fnames.c_str());
            bool bRes = false;
            for (AStringIt it=lst.begin(); it!=lst.end(); it++)
                if (AppendMotion(it->c_str())) bRes=true;
            ExecCommand	(COMMAND_UPDATE_PROPERTIES);
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
                ExecCommand(COMMAND_UPDATE_PROPERTIES);
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
        LHelper().CreateItem				(items,OBJECT_PREFIX,0,emObject);
        m_pEditObject->FillSurfaceList		(SURFACES_PREFIX,items,emSurface);
        // skin
        if (m_pEditObject->IsSkeleton()){
            if (m_pEditObject->m_SMotionRefs.size()){
		        m_RenderObject.FillMotionList(MOTIONS_PREFIX,items,emMotion);
            }else{ 
            	m_pEditObject->FillMotionList(MOTIONS_PREFIX,items,emMotion);
            }
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
                        
void CActorTools::OnMotionNameChange(PropValue* V)
{
    OnMotionKeysModified();
}
//------------------------------------------------------------------------------

void CActorTools::OnMotionControlClick(PropValue* sender, bool& bModif, bool& bSafe)
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

void  CActorTools::OnMotionRefsChange(PropValue* sender)
{
    OnMotionKeysModified	();
	ExecCommand				(COMMAND_UPDATE_PROPERTIES);
}

void CActorTools::FillMotionProperties(PropItemVec& items, LPCSTR pref, ListItem* sender)
{
	R_ASSERT(m_pEditObject);
	CSMotion* SM = m_pEditObject->m_SMotionRefs.size()?0:(CSMotion*)sender->m_Object;
    PropValue* V;

    AnsiString m_cnt;
    if (m_pEditObject->m_SMotionRefs.size()){ 
	    if (fraLeftBar->ebRenderEngineStyle->Down){
            CSkeletonAnimated* V	= dynamic_cast<CSkeletonAnimated*>(m_RenderObject.m_pVisual);
            if (V) m_cnt	= V->LL_CycleCount()+V->LL_FXCount();
        }else{
        	m_cnt	 		= AnsiString(m_pEditObject->SMotionCount())+" (Inaccessible)";
        }
    }else{
    	m_cnt 				= m_pEditObject->SMotionCount();
    }
                                            
    PHelper().CreateCaption			(items, PrepareKey(pref,"Global\\Motion count"),	m_cnt.c_str());
    V=PHelper().CreateChoose		(items, PrepareKey(pref,"Global\\Motion reference"),&m_pEditObject->m_SMotionRefs, smGameSMotions);
    V->OnChangeEvent.bind			(this,&CActorTools::OnMotionRefsChange);
    ButtonValue* B;                         
    B=PHelper().CreateButton		(items, PrepareKey(pref,"Global\\Edit"),			"Append,Delete,Save",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind			(this,&CActorTools::OnMotionEditClick); 
    if (SM){                                                                     
        B=PHelper().CreateButton	(items, PrepareKey(pref,"Motion\\Control"),	"Play,Stop,Pause",ButtonValue::flFirstOnly);
        B->OnBtnClickEvent.bind		(this,&CActorTools::OnMotionControlClick);
	    PHelper().CreateCaption		(items, PrepareKey(pref,"Motion\\Frame\\Start"),	shared_str().sprintf("%d",SM->FrameStart()));
	    PHelper().CreateCaption		(items, PrepareKey(pref,"Motion\\Frame\\End"),	shared_str().sprintf("%d",SM->FrameEnd()));
	    PHelper().CreateCaption		(items, PrepareKey(pref,"Motion\\Frame\\Length"),	shared_str().sprintf("%d",SM->Length()));
        PropValue* P=0;                                              
        P=PHelper().CreateName		(items,PrepareKey(pref,"Motion\\Name"),		&SM->name, sender);
        P->OnChangeEvent.bind		(this,&CActorTools::OnMotionNameChange);
        PHelper().CreateFloat		(items,PrepareKey(pref,"Motion\\Speed"),	&SM->fSpeed,  0.f,10.f,0.01f,2);
        PHelper().CreateFloat		(items,PrepareKey(pref,"Motion\\Accrue"),	&SM->fAccrue, 0.f,10.f,0.01f,2);
        PHelper().CreateFloat		(items,PrepareKey(pref,"Motion\\Falloff"), 	&SM->fFalloff,0.f,10.f,0.01f,2);

        PropValue /**C=0,*/*TV=0;
        TV = PHelper().CreateFlag8(items,PrepareKey(pref,"Motion\\Type FX"),	&SM->m_Flags, esmFX);
        TV->OnChangeEvent.bind	(this,&CActorTools::OnMotionTypeChange);
        m_BoneParts.clear		();
        if (SM->m_Flags.is(esmFX)){
            for (BoneIt it=m_pEditObject->FirstBone(); it!=m_pEditObject->LastBone(); it++)
				m_BoneParts.push_back	(xr_rtoken((*it)->Name(),(*it)->index));
			PHelper().CreateRToken16 	(items,PrepareKey(pref,"Motion\\FX\\Start bone"),	(u16*)&SM->m_BoneOrPart,	&*m_BoneParts.begin(), m_BoneParts.size());

            PHelper().CreateFloat		(items,PrepareKey(pref,"Motion\\FX\\Power"),	 	&SM->fPower,   	0.f,10.f,0.01f,2);
        }else{
            m_BoneParts.push_back(xr_rtoken("--all bones--",BI_NONE));
            for (BPIt it=m_pEditObject->FirstBonePart(); it!=m_pEditObject->LastBonePart(); it++)
				m_BoneParts.push_back	(xr_rtoken(it->alias.c_str(),it-m_pEditObject->FirstBonePart()));
			PHelper().CreateRToken16  	(items,PrepareKey(pref,"Motion\\Cycle\\Bone part"),		&SM->m_BoneOrPart,	&*m_BoneParts.begin(), m_BoneParts.size());
            PHelper().CreateFlag8	  	(items,PrepareKey(pref,"Motion\\Cycle\\Stop at end"),	&SM->m_Flags,		esmStopAtEnd);
            PHelper().CreateFlag8	  	(items,PrepareKey(pref,"Motion\\Cycle\\No mix"),	  	&SM->m_Flags,		esmNoMix);
            PHelper().CreateFlag8	  	(items,PrepareKey(pref,"Motion\\Cycle\\Sync part"),		&SM->m_Flags,		esmSyncPart);
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

void  CActorTools::OnJointTypeChange(PropValue* V)
{
	ExecCommand(COMMAND_UPDATE_PROPERTIES);
}
void  CActorTools::OnShapeTypeChange(PropValue* V)
{
	UI->RedrawScene();
	ExecCommand(COMMAND_UPDATE_PROPERTIES);
}
void  CActorTools::OnBindTransformChange(PropValue* V)
{
	R_ASSERT(m_pEditObject);
    m_pEditObject->OnBindTransformChange();
	UI->RedrawScene();
}

void  CActorTools::OnBoneShapeClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	R_ASSERT(m_pEditObject);
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: m_pEditObject->GenerateBoneShape(false); break;
    case 1: m_pEditObject->GenerateBoneShape(true); break;
	}
}

void  CActorTools::OnBoneEditClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	R_ASSERT(m_pEditObject);
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: 
    	m_pEditObject->GotoBindPose(); 
		ExecCommand(COMMAND_UPDATE_PROPERTIES);
	    bModif = false;
    break;
    case 1:
    	if (ELog.DlgMsg(mtConfirmation,"Are you sure to reset IK data?")==mrYes)
			m_pEditObject->ResetBones(); 
	    bModif = true;
    break;
    case 2:
		m_pEditObject->ClampByLimits(false); 
		ExecCommand(COMMAND_UPDATE_PROPERTIES);
	    bModif = false;
    break;
	}
}

void  CActorTools::OnBoneFileClick(PropValue* sender, bool& bModif, bool& bSafe)
{              
	R_ASSERT(m_pEditObject);
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0:{ 
    	std::string fn;
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
    	std::string fn;
    	if (EFS.GetSaveName("$sbones$",fn)){
        	IWriter* W = FS.w_open(fn.c_str());
	    	m_pEditObject->SaveBoneData(*W);
            FS.w_close(W);
            bModif = false;
        }
    }break;
	}
}

void  CActorTools::OnBoneLimitsChange(PropValue* sender)
{
	m_pEditObject->ClampByLimits(true);
}

void CActorTools::FillBoneProperties(PropItemVec& items, LPCSTR pref, ListItem* sender)
{
	R_ASSERT(m_pEditObject);
	CBone* BONE = (CBone*)sender->m_Object;

    PHelper().CreateCaption	(items, PrepareKey(pref,"Global\\Bone count"),	shared_str().sprintf("%d",m_pEditObject->BoneCount()));
    ButtonValue* B;
    B=PHelper().CreateButton	(items, PrepareKey(pref,"Global\\File"),"Load,Save",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind		(this,&CActorTools::OnBoneFileClick);
    B=PHelper().CreateButton	(items, PrepareKey(pref,"Global\\Edit"),"Bind pose,Reset IK,Clamp limits",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind		(this,&CActorTools::OnBoneEditClick);
    B=PHelper().CreateButton	(items, PrepareKey(pref,"Global\\Generate Shape"),"All, Selected",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind		(this,&CActorTools::OnBoneShapeClick);
    if (BONE){
    	PropValue* V;
        PHelper().CreateCaption		(items, PrepareKey(pref,"Bone\\Name"),						BONE->Name());
//.		PHelper().CreateCaption		(items, PrepareKey(pref,"Bone\\Influence"),					shared_str().sprintf("%d vertices",0));
		PHelper().CreateChoose		(items,	PrepareKey(pref,"Bone\\Game Material"),				&BONE->game_mtl, smGameMaterial);
        PHelper().CreateFloat	 	(items, PrepareKey(pref,"Bone\\Mass"),						&BONE->mass, 			0.f, 10000.f);
        PHelper().CreateVector		(items, PrepareKey(pref,"Bone\\Center Of Mass"),			&BONE->center_of_mass, 	-10000.f, 10000.f);
        V=PHelper().CreateVector 	(items, PrepareKey(pref,"Bone\\Bind Position"),				&BONE->_RestOffset(),	-10000.f, 10000.f);	V->OnChangeEvent.bind	(this,&CActorTools::OnBindTransformChange);
        V=PHelper().CreateAngle3 	(items, PrepareKey(pref,"Bone\\Bind Rotation"),				&BONE->_RestRotate());						V->OnChangeEvent.bind	(this,&CActorTools::OnBindTransformChange);
        PHelper().CreateFlag16		(items, PrepareKey(pref,"Bone\\Flags\\No Pickable"),		&BONE->shape.flags, SBoneShape::sfNoPickable);
        PHelper().CreateFlag16		(items, PrepareKey(pref,"Bone\\Flags\\Remove After Break"),	&BONE->shape.flags,SBoneShape::sfRemoveAfterBreak);
		V=PHelper().CreateToken16	(items,	PrepareKey(pref,"Bone\\Shape\\Type"),				&BONE->shape.type, shape_types);			V->OnChangeEvent.bind	(this,&CActorTools::OnShapeTypeChange);
        switch (BONE->shape.type){
        case SBoneShape::stBox:
	        PHelper().CreateVector	(items, PrepareKey(pref,"Bone\\Shape\\Box\\Center"),		&BONE->shape.box.m_translate, -10000.f, 10000.f);
	        PHelper().CreateVector	(items, PrepareKey(pref,"Bone\\Shape\\Box\\Half Size"),  	&BONE->shape.box.m_halfsize, 0.f, 1000.f);
        break;
        case SBoneShape::stSphere:
	        PHelper().CreateVector	(items, PrepareKey(pref,"Bone\\Shape\\Sphere\\Position"),	&BONE->shape.sphere.P, -10000.f, 10000.f);
	        PHelper().CreateFloat  	(items, PrepareKey(pref,"Bone\\Shape\\Sphere\\Radius"),  	&BONE->shape.sphere.R, 0.f, 1000.f);
        break;
        case SBoneShape::stCylinder:
	        PHelper().CreateVector	(items, PrepareKey(pref,"Bone\\Shape\\Cylinder\\Center"),	&BONE->shape.cylinder.m_center, -10000.f, 10000.f);
	        PHelper().CreateVector	(items, PrepareKey(pref,"Bone\\Shape\\Cylinder\\Direction"),&BONE->shape.cylinder.m_direction, 0.f, 10000.f);
	        PHelper().CreateFloat  	(items, PrepareKey(pref,"Bone\\Shape\\Cylinder\\Height"),	&BONE->shape.cylinder.m_height, 0.f, 1000.f);
	        PHelper().CreateFloat  	(items, PrepareKey(pref,"Bone\\Shape\\Cylinder\\Radius"),	&BONE->shape.cylinder.m_radius, 0.f, 1000.f);
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
        
        PHelper().CreateCaption		(items, PrepareKey(pref,"Bone\\Joint\\Current Rotation"),	shared_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(lim_rot)));
    	SJointIKData& data			= BONE->IK_data;
        V=PHelper().CreateFlag32 	(items, PrepareKey(pref,"Bone\\Joint\\Breakable"),		&data.ik_flags, SJointIKData::flBreakable);
        V->OnChangeEvent.bind		(this,&CActorTools::OnJointTypeChange);
        if (data.ik_flags.is(SJointIKData::flBreakable)){
	        PHelper().CreateFloat 	(items, PrepareKey(pref,"Bone\\Joint\\Break Force"),		&data.break_force, 	0.f, 1000000000.f);
	        PHelper().CreateFloat 	(items, PrepareKey(pref,"Bone\\Joint\\Break Torque"),		&data.break_torque, 0.f, 1000000000.f);
        }
		V=PHelper().CreateToken32	(items,	PrepareKey(pref,"Bone\\Joint\\Type"),				(u32*)&data.type,	joint_types);  	V->OnChangeEvent.bind	(this,&CActorTools::OnJointTypeChange);
        switch (data.type){
        case jtRigid: 
        break; 
        case jtCloth:{ 
	        PHelper().CreateFloat 	(items, PrepareKey(pref,"Bone\\Joint\\Friction"),	 			&data.friction, 	0.f, 1000000000.f);
	        PHelper().CreateFloat 	(items, PrepareKey(pref,"Bone\\Joint\\Spring Factor"),		&data.spring_factor, 	0.f, 1000.f);
	        PHelper().CreateFloat 	(items, PrepareKey(pref,"Bone\\Joint\\Damping Factor"),		&data.damping_factor, 	0.f, 1000.f);
        }break; 
        case jtJoint:{
	        PHelper().CreateFloat	(items, PrepareKey(pref,"Bone\\Joint\\Friction"),	 			&data.friction, 	0.f, 1000000000.f);
	        PHelper().CreateFloat 	(items, PrepareKey(pref,"Bone\\Joint\\Spring Factor"),		&data.spring_factor, 	0.f, 1000.f);
	        PHelper().CreateFloat	(items, PrepareKey(pref,"Bone\\Joint\\Damping Factor"),		&data.damping_factor, 	0.f, 1000.f);
            for (int k=0; k<3; k++){
		        V=PHelper().CreateAngle	(items,PrepareKey(pref,"Bone\\Joint\\Limits",axis[k],"Min"),				&data.limits[k].limit.x, 		-M_PI, 0.f);
                V->OnChangeEvent.bind	(this,&CActorTools::OnBoneLimitsChange);
		        V=PHelper().CreateAngle	(items,PrepareKey(pref,"Bone\\Joint\\Limits",axis[k],"Max"),				&data.limits[k].limit.y, 		0.f, M_PI);
                V->OnChangeEvent.bind	(this,&CActorTools::OnBoneLimitsChange);
		        V=PHelper().CreateFloat	(items,PrepareKey(pref,"Bone\\Joint\\Limits",axis[k],"Spring Factor"),	&data.limits[k].spring_factor, 	0.f, 1000.f);
                V->OnChangeEvent.bind	(this,&CActorTools::OnBoneLimitsChange);
		        V=PHelper().CreateFloat	(items,PrepareKey(pref,"Bone\\Joint\\Limits",axis[k],"Damping Factor"),	&data.limits[k].damping_factor, 0.f, 1000.f);
                V->OnChangeEvent.bind	(this,&CActorTools::OnBoneLimitsChange);
            }
        }break; 
		case jtWheel:
        {
//	        int idx = (data.type-jtWheelXZ)/2;
	        int idx = (data.type-jtWheel)/2;
	        PHelper().CreateFloat  	(items,PrepareKey(pref,"Bone\\Joint\\Friction"),	 			&data.friction, 	0.f, 1000000000.f);
	        PHelper().CreateFloat  	(items,PrepareKey(pref,"Bone\\Joint\\Spring Factor"),			&data.spring_factor, 0.f, 1000.f);
	        PHelper().CreateFloat  	(items,PrepareKey(pref,"Bone\\Joint\\Damping Factor"),	 	&data.damping_factor, 0.f, 1000.f);
            V=PHelper().CreateAngle	(items,PrepareKey(pref,"Bone\\Joint\\Steer\\Limits Min"),		&data.limits[idx].limit.x, -PI_DIV_2, 0.f);
			V->OnChangeEvent.bind	(this,&CActorTools::OnBoneLimitsChange);
            V=PHelper().CreateAngle	(items,PrepareKey(pref,"Bone\\Joint\\Steer\\Limits Max"),		&data.limits[idx].limit.y, 0, PI_DIV_2);
            V->OnChangeEvent.bind	(this,&CActorTools::OnBoneLimitsChange);
        }break;
        }
    }
}
//------------------------------------------------------------------------------

void CActorTools::FillSurfaceProperties(PropItemVec& items, LPCSTR pref, ListItem* sender)
{
	R_ASSERT(m_pEditObject);
	CSurface* SURF = (CSurface*)sender->m_Object;
    PHelper().CreateCaption			(items, PrepareKey(pref,"Statistic\\Count"),	shared_str().sprintf("%d",m_pEditObject->SurfaceCount()));
    if (SURF){
        PHelper().CreateCaption		(items,PrepareKey(pref,"Surface\\Name"),		SURF->_Name());
        AnsiString _pref			= PrepareKey(pref,"Surface").c_str();
	    m_pEditObject->FillSurfaceProps(SURF,_pref.c_str(),items);
    }
}
//------------------------------------------------------------------------------

void CActorTools::FillObjectProperties(PropItemVec& items, LPCSTR pref, ListItem* sender)
{
	R_ASSERT(m_pEditObject);
    PropValue* V=0;
    PHelper().CreateFlag32			(items, "Object\\Flags\\Make Progressive",	&m_pEditObject->m_Flags,		CEditableObject::eoProgressive);
    V=PHelper().CreateVector		(items, "Object\\Transform\\Position",		&m_pEditObject->a_vPosition, 	-10000,	10000,0.01,2);
    V->OnChangeEvent.bind			(this,&CActorTools::OnChangeTransform);
    V=PHelper().CreateAngle3		(items, "Object\\Transform\\Rotation",		&m_pEditObject->a_vRotate, 		-10000,	10000,0.1,1);
    V->OnChangeEvent.bind			(this,&CActorTools::OnChangeTransform);
    V=PHelper().CreateCaption		(items, "Object\\Transform\\BBox Min",		shared_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(m_pEditObject->GetBox().min)));
    V=PHelper().CreateCaption		(items, "Object\\Transform\\BBox Max",		shared_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(m_pEditObject->GetBox().max)));
    m_pEditObject->FillSummaryProps	("Object\\Summary",items);
}
//------------------------------------------------------------------------------

void CActorTools::SelectListItem(LPCSTR pref, LPCSTR name, bool bVal, bool bLeaveSel, bool bExpand)
{
	AnsiString nm = (name&&name[0])?PrepareKey(pref,name).c_str():AnsiString(pref).c_str();
	m_ObjectItems->SelectItem(nm.c_str(),bVal,bLeaveSel,bExpand);
	if (pref){
    	m_ObjectItems->SelectItem(pref,true,true,bExpand);
    }
}
//------------------------------------------------------------------------------

