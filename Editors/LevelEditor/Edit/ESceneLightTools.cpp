#include "stdafx.h"
#pragma hdrstop

#include "ESceneLightTools.h"
#include "PropertiesListHelper.h"
#include "IGame_Persistent.h"
#include "d3dutils.h"
#include "communicate.h"
#include "ui_main.h"
#include "TextForm.h"
#include "ELight.h"

ESceneLightTools::ESceneLightTools():ESceneCustomOTools(OBJCLASS_LIGHT)
{
	Clear				();
}
//------------------------------------------------------------------------------

ESceneLightTools::~ESceneLightTools()
{
}
//------------------------------------------------------------------------------

void ESceneLightTools::Clear(bool bSpecific)
{
	inherited::Clear(bSpecific);

    m_LFlags.zero		();
    
    m_HemiQuality		= 1;
    m_SunShadowQuality	= 1;
    m_SunShadowDir.set	(-PI_DIV_6,PI_MUL_2-PI_DIV_4);
    
	lcontrol_last_idx	= 0;
    lcontrols.clear		();
    AppendLightControl	(LCONTROL_STATIC);
    AppendLightControl	(LCONTROL_HEMI);
    AppendLightControl	(LCONTROL_SUN);
}
//------------------------------------------------------------------------------

void ESceneLightTools::SelectLightsForObject(CCustomObject* obj)
{
    for (u32 i=0; i<frame_light.size(); i++){
        CLight* l = frame_light[i];
//        if (obj->IsDynamic()&&!l->m_Flags.is(CLight::flAffectDynamic)) continue;
//        if (!obj->IsDynamic()&&!l->m_Flags.is(CLight::flAffectStatic)) continue;
        Fbox bb; 	obj->GetBox(bb);
        Fvector C; 	float R; bb.getsphere(C,R);
        float d 	= C.distance_to(*((Fvector*)&l->m_D3D.position)) - l->m_D3D.range - R;
        Device.LightEnable(i,(d<0));
    }
}

void ESceneLightTools::AppendFrameLight(CLight* src)
{
    Flight L			= src->m_D3D;
    L.diffuse.mul_rgb	(src->m_Brightness);
    L.ambient.mul_rgb	(src->m_Brightness);
    L.specular.mul_rgb	(src->m_Brightness);
    Device.SetLight		(frame_light.size(),L);
    frame_light.push_back(src);
}

void ESceneLightTools::BeforeRender()
{
    if (psDeviceFlags.is(rsLighting)){
        int l_cnt		= 0;
        // set scene lights
        for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++){
            CLight* l 		= (CLight*)(*_F);
            l_cnt++;
            if (l->Visible()&&l->m_UseInD3D&&l->m_Flags.is_any(CLight::flAffectDynamic|CLight::flAffectStatic))
                if (::Render->ViewBase.testSphere_dirty(l->m_D3D.position,l->m_D3D.range))
                	AppendFrameLight(l);
        }
    	// set sun
		if (m_LFlags.is(flShowSun)){
            Flight L;
            Fvector C;
            if (psDeviceFlags.is(rsEnvironment)){
	            C			= g_pGamePersistent->Environment.Current.sun_color;
            }else{
            	C.set		(1.f,1.f,1.f);
            }
            L.direction.setHP(m_SunShadowDir.y,m_SunShadowDir.x);
            L.diffuse.set	(C.x,C.y,C.z,1.f);
            L.ambient.set	(0.f,0.f,0.f,0.f);
            L.specular.set	(C.x,C.y,C.z,1.f);
            L.type			= D3DLIGHT_DIRECTIONAL;
            Device.SetLight	(frame_light.size(),L);
            Device.LightEnable(frame_light.size(),TRUE);
        }
		// ambient
        if (psDeviceFlags.is(rsEnvironment)){
	        Fvector& V		= g_pGamePersistent->Environment.Current.ambient;
            Fcolor C;		C.set(V.x,V.y,V.z,1.f);
            Device.SetRS	(D3DRS_AMBIENT,C.get());
        }else				Device.SetRS(D3DRS_AMBIENT,0x00000000);
        
        Device.Statistic.dwTotalLight 	= l_cnt;
        Device.Statistic.dwLightInScene = frame_light.size();
    }
}
//------------------------------------------------------------------------------

void ESceneLightTools::AfterRender()
{
    if (m_LFlags.is(flShowSun))
        Device.LightEnable(frame_light.size(),FALSE); // sun - last light!
    for (u32 i=0; i<frame_light.size(); i++)
		Device.LightEnable(i,FALSE);
    frame_light.clear();
}
//------------------------------------------------------------------------------

//#define DIR_SELRANGE 	250
#define DIR_RANGE 		10
#define VIS_RADIUS 		0.25f
void  ESceneLightTools::OnRender(int priority, bool strictB2F)
{
	inherited::OnRender(priority, strictB2F);
    if (m_LFlags.is(flShowSun)){
        if ((true==strictB2F)&&(1==priority)){
            Device.SetShader		(Device.m_WireShader);
            RCache.set_xform_world	(Fidentity);
            Fvector pos				= {-5.f,5.f,0.f};
            Fvector dir;
            dir.setHP(m_SunShadowDir.y,m_SunShadowDir.x);
            DU.DrawDirectionalLight( pos, dir, VIS_RADIUS, DIR_RANGE, 0x00FFFF00 );
        }
    }
}
//------------------------------------------------------------------------------

void __fastcall ESceneLightTools::OnControlAppendClick(PropValue* sender, bool& bDataModified)
{
	AppendLightControl(GenLightControlName().c_str());
    UI.Command(COMMAND_UPDATE_PROPERTIES);
    bDataModified = true;
}
//------------------------------------------------------------------------------

void __fastcall ESceneLightTools::OnControlRenameRemoveClick(PropValue* sender, bool& bDataModified)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    AnsiString item_name = sender->Owner()->Item()->Text;
    switch (V->btn_num){
    case 0:{ 
    	AnsiString new_name=item_name;
    	if (TfrmText::RunEditor(new_name,"Control name")){
        	if (FindLightControl(new_name.c_str())){
            	ELog.DlgMsg(mtError,"Duplicate name found.");
            }else if (new_name.IsEmpty()||new_name.Pos("\\")){
            	ELog.DlgMsg(mtError,"Invalid control name.");
			}else{
            	ATokenIt it		= FindLightControlIt(item_name.c_str());
                it->rename 		(new_name.c_str());
            }
        }
    }break;
    case 1: RemoveLightControl(item_name.c_str());	break;
	}
    UI.Command(COMMAND_UPDATE_PROPERTIES);
    bDataModified = true;
}
//------------------------------------------------------------------------------
void ESceneLightTools::FillProp(LPCSTR pref, PropItemVec& items)
{
    // hemisphere
    PHelper.CreateU8	(items,	FHelper.PrepareKey(pref,"Common\\Hemisphere\\Quality"),		&m_HemiQuality,		1,2);
    // sun
    PHelper.CreateFlag32(items, FHelper.PrepareKey(pref,"Common\\Sun Shadow\\Visible"),		&m_LFlags,			flShowSun);
    PHelper.CreateU8	(items,	FHelper.PrepareKey(pref,"Common\\Sun Shadow\\Quality"),		&m_SunShadowQuality,1,2);
    PHelper.CreateAngle	(items,	FHelper.PrepareKey(pref,"Common\\Sun Shadow\\Altitude"),	&m_SunShadowDir.x,	-PI_DIV_2,0);
    PHelper.CreateAngle	(items,	FHelper.PrepareKey(pref,"Common\\Sun Shadow\\Longitude"),	&m_SunShadowDir.y,	0,PI_MUL_2);
    // light controls
    PHelper.CreateFlag32(items, FHelper.PrepareKey(pref,"Common\\Controls\\Draw Name"),		&m_LFlags,		flShowControlName);
    PHelper.CreateCaption(items,FHelper.PrepareKey(pref,"Common\\Controls\\Count"),			lcontrols.size());
    ButtonValue*	B 	= 0;
//	B=PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Common\\Controls\\Edit"),	"Append",	ButtonValue::flFirstOnly);
//	B->OnBtnClickEvent	= OnControlAppendClick;
	ATokenIt		_I 	= lcontrols.begin();
    ATokenIt		_E 	= lcontrols.end();
    for (;_I!=_E; _I++){
    	if (_I->equal(LCONTROL_HEMI)||_I->equal(LCONTROL_STATIC)||_I->equal(LCONTROL_SUN)){
		    PHelper.CreateCaption(items,	FHelper.PrepareKey(pref,"Common\\Controls\\System",_I->name.c_str()),"");
        }else{
		    B=PHelper.CreateButton(items,	FHelper.PrepareKey(pref,"Common\\Controls\\User",_I->name.c_str()),"Rename,Remove",ButtonValue::flFirstOnly);
            B->OnBtnClickEvent	= OnControlRenameRemoveClick;
        }
    }                              
	inherited::FillProp(pref, items);
}
//------------------------------------------------------------------------------

AnsiString ESceneLightTools::GenLightControlName()
{
	AnsiString name;
    int idx=0;
    do{
    	name.sprintf("control_%02d",idx++);
    }while (FindLightControl(name.c_str()));
    return name;
}
//------------------------------------------------------------------------------

xr_a_token* ESceneLightTools::FindLightControl(int id)
{
	ATokenIt		_I 	= lcontrols.begin();
    ATokenIt		_E 	= lcontrols.end();
    for (;_I!=_E; _I++)
    	if (_I->id==id) return _I;
    return 0;
}
//------------------------------------------------------------------------------

ATokenIt ESceneLightTools::FindLightControlIt(LPCSTR name)
{
	ATokenIt		_I 	= lcontrols.begin();
    ATokenIt		_E 	= lcontrols.end();
    for (;_I!=_E; _I++)
    	if (_I->equal(name)) return _I;
    return lcontrols.end();
}
//------------------------------------------------------------------------------

void ESceneLightTools::AppendLightControl(LPCSTR name, u32* idx)
{
	if (FindLightControl(name)) return;
	lcontrols.push_back	(xr_a_token(name,idx?*idx:lcontrol_last_idx++));
}
//------------------------------------------------------------------------------

void ESceneLightTools::RemoveLightControl(LPCSTR name)
{
	ATokenIt it		= FindLightControlIt(name);
    if (it!=lcontrols.end()) lcontrols.erase(it);
}
//------------------------------------------------------------------------------

bool ESceneLightTools::Validate()
{
	if (!inherited::Validate()) return false;
	bool bRes = !m_Objects.empty();
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	CLight* L = dynamic_cast<CLight*>(*it);
    	if (!L->GetLControlName()){
        	bRes=false;
            ELog.Msg(mtError,"%s: '%s' - Invalid light control.",ClassDesc(),L->Name);
        }
    }
    return bRes;
}
//------------------------------------------------------------------------------


#include "frameLight.h"

void ESceneLightTools::CreateControls()
{
	inherited::CreateControls();
	// frame
    pFrame 			= xr_new<TfraLight>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneLightTools::RemoveControls()
{
}
//----------------------------------------------------


/*
	m_D3D.direction.setHP(PRotation.y,PRotation.x);
	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Update(m_D3D.direction, m_D3D.diffuse);

//render
        case D3DLIGHT_DIRECTIONAL:
            if (Selected()) DU.DrawDirectionalLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, DIR_SELRANGE, clr );
            else			DU.DrawDirectionalLight( m_D3D.position, m_D3D.direction, VIS_RADIUS, DIR_RANGE, clr );
        break;
else if ((3==priority)&&(true==strictB2F)){
		if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Render();
    }
// update
	if (D3DLIGHT_DIRECTIONAL==m_D3D.type){
    	m_LensFlare.Update(m_D3D.direction, m_D3D.diffuse);
	    m_LensFlare.DeleteShaders();
    	m_LensFlare.CreateShaders();
    }
// load
	m_LensFlare.Load(F);
// save
	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.Save(F);

    
void CLight:xr_token sun_quality[]={
	{ "Low",			1	},
	{ "Normal",			2	},
	{ "Final",			3	},
	{ 0,				0	}
};

:FillSunProp(LPCSTR pref, PropItemVec& items)
{
	CEditFlare& F 			= m_LensFlare;
    PropValue* prop			= 0;
    PHelper.CreateToken		(items, FHelper.PrepareKey(pref,"Sun\\Quality"),				&m_SunQuality,			sun_quality);
    
    PHelper.CreateFlag32	(items, FHelper.PrepareKey(pref,"Sun\\Source\\Enabled"),		&F.m_Flags,				CEditFlare::flSource);
    PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Sun\\Source\\Radius"),			&F.m_Source.fRadius,	0.f,10.f);
    prop 					= PHelper.CreateTexture	(items, FHelper.PrepareKey(pref,"Sun\\Source\\Texture"),		F.m_Source.texture,		sizeof(F.m_Source.texture));
	prop->OnChangeEvent		= OnNeedUpdate;

    PHelper.CreateFlag32	(items, FHelper.PrepareKey(pref,"Sun\\Gradient\\Enabled"),		&F.m_Flags,				CEditFlare::flGradient);
    PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Sun\\Gradient\\Radius"),		&F.m_Gradient.fRadius,	0.f,100.f);
    PHelper.CreateFloat		(items, FHelper.PrepareKey(pref,"Sun\\Gradient\\Opacity"),		&F.m_Gradient.fOpacity,	0.f,1.f);
	prop					= PHelper.CreateTexture	(items, FHelper.PrepareKey(pref,"Sun\\Gradient\\Texture"),	F.m_Gradient.texture,	sizeof(F.m_Gradient.texture));
	prop->OnChangeEvent		= OnNeedUpdate;

    PHelper.CreateFlag32	(items, FHelper.PrepareKey(pref,"Sun\\Flares\\Enabled"),		&F.m_Flags,				CEditFlare::flFlare);
	for (CEditFlare::FlareIt it=F.m_Flares.begin(); it!=F.m_Flares.end(); it++){
		AnsiString nm; nm.sprintf("%s\\Sun\\Flares\\Flare %d",pref,it-F.m_Flares.begin());
		PHelper.CreateFloat	(items, FHelper.PrepareKey(nm.c_str(),"Radius"), 	&it->fRadius,  	0.f,10.f);
        PHelper.CreateFloat	(items, FHelper.PrepareKey(nm.c_str(),"Opacity"),	&it->fOpacity,	0.f,1.f);
        PHelper.CreateFloat	(items, FHelper.PrepareKey(nm.c_str(),"Position"),	&it->fPosition,	-10.f,10.f);
        prop				= PHelper.CreateTexture	(items, FHelper.PrepareKey(nm.c_str(),"Texture"),	it->texture,	sizeof(it->texture));
        prop->OnChangeEvent	= OnNeedUpdate;
	}
}
//----------------------------------------------------
void CLight::OnDeviceCreate()
{
	if (D3DLIGHT_DIRECTIONAL==m_D3D.type) m_LensFlare.DDLoad();
}
void CLight::OnDeviceDestroy()
{
//	if (D3DLIGHT_DIRECTIONAL==m_D3D.type)
    m_LensFlare.DDUnload();
}
//----------------------------------------------------

//----------------------------------------------------
// Edit Flare
//----------------------------------------------------
CEditFlare::CEditFlare()
{
    m_Flags.set(flFlare|flSource|flGradient,TRUE);
	// flares
    m_Flares.resize		(6);
    FlareIt it=m_Flares.begin();
	it->fRadius=0.08f; it->fOpacity=0.18f; it->fPosition=1.3f; strcpy(it->texture,"fx\\fx_flare1"); it++;
	it->fRadius=0.12f; it->fOpacity=0.12f; it->fPosition=1.0f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.04f; it->fOpacity=0.30f; it->fPosition=0.5f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.08f; it->fOpacity=0.24f; it->fPosition=-0.3f; strcpy(it->texture,"fx\\fx_flare2"); it++;
	it->fRadius=0.12f; it->fOpacity=0.12f; it->fPosition=-0.6f; strcpy(it->texture,"fx\\fx_flare3"); it++;
	it->fRadius=0.30f; it->fOpacity=0.12f; it->fPosition=-1.0f; strcpy(it->texture,"fx\\fx_flare1"); it++;
	// source
    strcpy(m_Source.texture,"fx\\fx_sun");
    m_Source.fRadius 	= 0.15f;
    // gradient
    strcpy(m_Gradient.texture,"fx\\fx_gradient");
    m_Gradient.fOpacity = 0.9f;
    m_Gradient.fRadius 	= 4.f;
}

void CEditFlare::Load(IReader& F){
	if (!F.find_chunk(FLARE_CHUNK_FLAG)) return;

    R_ASSERT(F.find_chunk(FLARE_CHUNK_FLAG));
    F.r				(&m_Flags.flags,sizeof(m_Flags));

    R_ASSERT(F.find_chunk(FLARE_CHUNK_SOURCE));
    F.r_stringZ		(m_Source.texture);
    m_Source.fRadius= F.r_float();

    if (F.find_chunk(FLARE_CHUNK_GRADIENT2)){
	    F.r_stringZ	(m_Gradient.texture);
	    m_Gradient.fOpacity = F.r_float();
	    m_Gradient.fRadius  = F.r_float();
    }else{
		R_ASSERT(F.find_chunk(FLARE_CHUNK_GRADIENT));
	    m_Gradient.fOpacity = F.r_float();
    }

    // flares
    if (F.find_chunk(FLARE_CHUNK_FLARES2)){
	    DeleteShaders();
	    u32 deFCnt	= F.r_u32(); VERIFY(deFCnt==6);
	   	F.r				(m_Flares.begin(),m_Flares.size()*sizeof(SFlare));
    	for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++) it->hShader._clear();
    	CreateShaders();
    }
}
//----------------------------------------------------

void CEditFlare::Save(IWriter& F)
{
	F.open_chunk	(FLARE_CHUNK_FLAG);
    F.w				(&m_Flags.flags,sizeof(m_Flags));
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_SOURCE);
    F.w_stringZ		(m_Source.texture);
    F.w_float		(m_Source.fRadius);
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_GRADIENT2);
    F.w_stringZ		(m_Gradient.texture);
    F.w_float		(m_Gradient.fOpacity);
    F.w_float		(m_Gradient.fRadius);
	F.close_chunk	();

	F.open_chunk	(FLARE_CHUNK_FLARES2);
    F.w_u32			(m_Flares.size());
    F.w				(m_Flares.begin(),m_Flares.size()*sizeof(SFlare));
	F.close_chunk	();
}
//----------------------------------------------------

void CEditFlare::Render()
{
	CLensFlare::Render(m_Flags.is(flSource),m_Flags.is(flFlare),m_Flags.is(flGradient));
}
//----------------------------------------------------

void CEditFlare::DeleteShaders()
{
    CLensFlare::DDUnload();
}

void CEditFlare::CreateShaders()
{
    CLensFlare::DDLoad();
}
//----------------------------------------------------


	AnsiString suns;
	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
		switch ((*it)->ClassID){
        case OBJCLASS_LIGHT:{
            CLight *l = (CLight *)(*it);
            if (l->m_D3D.type==D3DLIGHT_DIRECTIONAL){
	            if (suns.Length()) suns += ", ";
    	        suns += l->Name;
           	    pIni->w_fcolor	(l->Name, "sun_color", 			l->m_D3D.diffuse);
               	pIni->w_fvector3(l->Name, "sun_dir", 			l->m_D3D.direction);
               	pIni->w_string	(l->Name, "gradient", 			l->m_LensFlare.m_Flags.is(CLensFlare::flGradient)?"on":"off");
               	pIni->w_string	(l->Name, "source", 			l->m_LensFlare.m_Flags.is(CLensFlare::flSource)?"on":"off");
               	pIni->w_string	(l->Name, "flares", 			l->m_LensFlare.m_Flags.is(CLensFlare::flFlare)?"on":"off");
                pIni->w_float	(l->Name, "gradient_opacity", 	l->m_LensFlare.m_Gradient.fOpacity);
                pIni->w_string	(l->Name, "gradient_texture", 	l->m_LensFlare.m_Gradient.texture);
                pIni->w_float	(l->Name, "gradient_radius", 	l->m_LensFlare.m_Gradient.fRadius);
                pIni->w_string	(l->Name, "source_texture", 	l->m_LensFlare.m_Source.texture);
                pIni->w_float	(l->Name, "source_radius", 	l->m_LensFlare.m_Source.fRadius);
                AnsiString FT=""; AnsiString FR=""; AnsiString FO=""; AnsiString FP="";
                AnsiString T="";
                int i=l->m_LensFlare.m_Flares.size();
                for (CEditFlare::FlareIt it = l->m_LensFlare.m_Flares.begin(); it!=l->m_LensFlare.m_Flares.end(); it++,i--){
                	FT += it->texture;
                	T.sprintf("%.3f",it->fRadius); 	FR += T;
                	T.sprintf("%.3f",it->fOpacity);	FO += T;
                	T.sprintf("%.3f",it->fPosition);FP += T;
                    if (i>1){FT+=","; FR+=","; FO+=","; FP+=",";}
                }
               	pIni->w_string	(l->Name, "flare_textures",	FT.c_str());
               	pIni->w_string	(l->Name, "flare_radius",	FR.c_str());
               	pIni->w_string	(l->Name, "flare_opacity",	FO.c_str());
               	pIni->w_string	(l->Name, "flare_position",	FP.c_str());
            }
        }break;
        case OBJCLASS_GROUP:{
            CGroupObject* group = (CGroupObject*)(*it);
            bResult = ParseLTX(pIni,group->GetObjects(),group->Name);
        }break;
        }
        if (!bResult) return FALSE;
        if (suns.Length()) pIni->w_string("environment", "suns", suns.c_str());
    }
*/
