#include "stdafx.h"
#include "postprocessanimator.h"


// postprocess value LOAD method implementation
void CPostProcessValue::load (IReader &pReader)
{
    m_Value.Load_2 (pReader);
}

void CPostProcessValue::save (IWriter &pWriter)
{
    m_Value.Save (pWriter);
}
 
// postprocess color LOAD method implementation
void CPostProcessColor::load (IReader &pReader)
{
    m_fBase = pReader.r_float	();
    m_Red.Load_2				(pReader);
    m_Green.Load_2				(pReader);
    m_Blue.Load_2				(pReader);
}

void CPostProcessColor::save (IWriter &pWriter)
{
    pWriter.w_float				(m_fBase);
    m_Red.Save					(pWriter);
    m_Green.Save				(pWriter);
    m_Blue.Save					(pWriter);
}

//main PostProcessAnimator class

CPostprocessAnimator::CPostprocessAnimator(int id, bool cyclic)
:CEffectorPP((EEffectorPPType)id, 100000, true)
{
    Create				();
	m_factor			= 1.0f;
	m_bStop				= false;
	m_stop_speed		= 1.0f;
	m_bCyclic			= cyclic;
	m_start_time		= -1.0f;
}

CPostprocessAnimator::~CPostprocessAnimator           ()
{
    Clear ();
}

void        CPostprocessAnimator::Clear                           ()
{
    for (int a = 0; a < POSTPROCESS_PARAMS_COUNT; a++)
        xr_delete (m_Params[a]);
}

void        CPostprocessAnimator::Load                            (LPCSTR name)
{
    m_Name = name;
#ifndef _PP_EDITOR_
    string_path full_path;
    if (!FS.exist (full_path, "$level$", name))
       if (!FS.exist (full_path, "$game_anims$", name))
          Debug.fatal ("Can't find motion file '%s'.", name);
#else /*_PP_EDITOR_*/
    string_path full_path;
    strcpy (full_path, name);
#endif /*_PP_EDITOR_*/

    LPCSTR  ext = strext(full_path);
    if (ext)
       {
       if (!xr_strcmp (ext,POSTPROCESS_FILE_EXTENSION))
          {
          IReader* F = FS.r_open (full_path);
          u32 dwVersion = F->r_u32();
          VERIFY (dwVersion == POSTPROCESS_FILE_VERSION);
          //load base color
          VERIFY (m_Params[0]);
          m_Params[0]->load (*F);
          //load add color
          VERIFY (m_Params[1]);
          m_Params[1]->load (*F);
          //load gray color
          VERIFY (m_Params[2]);
          m_Params[2]->load (*F);
          //load gray value
          VERIFY (m_Params[3]);
          m_Params[3]->load (*F);
          //load blur value
          VERIFY (m_Params[4]);
          m_Params[4]->load (*F);
          //load duality horizontal
          VERIFY (m_Params[5]);
          m_Params[5]->load (*F);
          //load duality vertical
          VERIFY (m_Params[6]);
          m_Params[6]->load (*F);
          //load noise intensity
          VERIFY (m_Params[7]);
          m_Params[7]->load (*F);
          //load noise granularity
          VERIFY (m_Params[8]);
          m_Params[8]->load (*F);
          //load noise fps
          VERIFY (m_Params[9]);
          m_Params[9]->load (*F);
          //close reader
          FS.r_close (F);
          }
        else
           Debug.fatal("ERROR: Can't support files with many animations set. Incorrect file.");
        }

   if(!m_bCyclic) fLifeTime = GetLength	();
}

void        CPostprocessAnimator::Stop       (float sp)
{
	if(m_bStop)			return;
	m_bStop				= true;
	m_stop_speed		= sp;
}

float       CPostprocessAnimator::GetLength                       ()
{
    float v = 0.0f;
    for (int a = 0; a < POSTPROCESS_PARAMS_COUNT; a++)
        {
        float t = m_Params[a]->get_length();
        if (t > v) v = t;
        }
    return v;
}

void        CPostprocessAnimator::Update                          (float tm)
{
    for (int a = 0; a < POSTPROCESS_PARAMS_COUNT; a++)
        m_Params[a]->update (tm);
}

BOOL CPostprocessAnimator::Process(SPPInfo &PPInfo)
{
	if(m_bCyclic)
		fLifeTime				= 100000;

	if(m_start_time<0.0f)m_start_time=Device.fTimeGlobal;

	Update					(Device.fTimeGlobal-m_start_time);
	
	if(m_bStop)
		m_factor			-=	Device.fTimeDelta*m_stop_speed;

	clamp					(m_factor, 0.001f, 1.0f);

	m_EffectorParams.color_base		+= pp_identity.color_base;
	m_EffectorParams.color_gray		+= pp_identity.color_gray;
	m_EffectorParams.color_add		+= pp_identity.color_add;

	PPInfo.lerp				(pp_identity, m_EffectorParams, m_factor);
	
	if(fsimilar(m_factor,0.001f,EPS_S))
		return FALSE;

	return TRUE;
}

void        CPostprocessAnimator::Create                          ()
{
    m_Params[0] = xr_new<CPostProcessColor> (&m_EffectorParams.color_base);			//base color
    VERIFY (m_Params[0]);
    m_Params[1] = xr_new<CPostProcessColor> (&m_EffectorParams.color_add);          //add color
    VERIFY (m_Params[1]);
    m_Params[2] = xr_new<CPostProcessColor> (&m_EffectorParams.color_gray);         //gray color
    VERIFY (m_Params[2]);
    m_Params[3] = xr_new<CPostProcessValue> (&m_EffectorParams.gray);              //gray value
    VERIFY (m_Params[3]);
    m_Params[4] = xr_new<CPostProcessValue> (&m_EffectorParams.blur);              //blur value
    VERIFY (m_Params[4]);
    m_Params[5] = xr_new<CPostProcessValue> (&m_EffectorParams.duality.h);          //duality horizontal
    VERIFY (m_Params[5]);
    m_Params[6] = xr_new<CPostProcessValue> (&m_EffectorParams.duality.v);          //duality vertical
    VERIFY (m_Params[6]);
    m_Params[7] = xr_new<CPostProcessValue> (&m_EffectorParams.noise.intensity);    //noise intensity
    VERIFY (m_Params[7]);
    m_Params[8] = xr_new<CPostProcessValue> (&m_EffectorParams.noise.grain);        //noise granularity
    VERIFY (m_Params[8]);
    m_Params[9] = xr_new<CPostProcessValue> (&m_EffectorParams.noise.fps);          //noise fps
    VERIFY (m_Params[9]);
}

#ifdef _PP_EDITOR_
CPostProcessParam*  CPostprocessAnimator::GetParam                (pp_params param)
{
    VERIFY (param >= pp_base_color && param <= pp_noise_f);
    return m_Params[param];
}
void        CPostprocessAnimator::Save                            (LPCSTR name)
{
    IWriter *W = FS.w_open (name);
    VERIFY (W);
    W->w_u32(POSTPROCESS_FILE_VERSION);
    m_Params[0]->save (*W);
    m_Params[1]->save (*W);
    m_Params[2]->save (*W);
    m_Params[3]->save (*W);
    m_Params[4]->save (*W);
    m_Params[5]->save (*W);
    m_Params[6]->save (*W);
    m_Params[7]->save (*W);
    m_Params[8]->save (*W);
    m_Params[9]->save (*W);
    FS.w_close (W);

}
//-----------------------------------------------------------------------
void        CPostprocessAnimator::ResetParam                      (pp_params param)
{
    xr_delete (m_Params[param]);
    switch (param)
           {
           case pp_base_color:
                m_Params[0] = xr_new<CPostProcessColor> (&m_EffectorParams.m_BaseColor);   //base color
                break;
           case pp_add_color:
                m_Params[1] = xr_new<CPostProcessColor> (&m_EffectorParams.m_AddColor);          //add color
                break;
           case pp_gray_color:
                m_Params[2] = xr_new<CPostProcessColor> (&m_EffectorParams.m_GrayColor);         //gray color
                break;
           case pp_gray_value:
                m_Params[3] = xr_new<CPostProcessValue> (&m_EffectorParams.m_Gray);              //gray value
                break;
           case pp_blur:
                m_Params[4] = xr_new<CPostProcessValue> (&m_EffectorParams.m_Blur);              //blur value
                break;
           case pp_dual_h:
                m_Params[5] = xr_new<CPostProcessValue>  (&m_EffectorParams.m_DualityH);       //duality horizontal
                break;
           case pp_dual_v:
                m_Params[6] = xr_new<CPostProcessValue>  (&m_EffectorParams.m_DualityV);       //duality vertical
                break;
           case pp_noise_i:
                m_Params[7] = xr_new<CPostProcessValue>  (&m_EffectorParams.m_NoiseI);         //noise intensity
                break;
           case pp_noise_g:
                m_Params[8] = xr_new<CPostProcessValue>  (&m_EffectorParams.m_NoiseG);         //noise granularity
                break;
           case pp_noise_f:
                m_Params[9] = xr_new<CPostProcessValue>  (&m_EffectorParams.m_NoiseF);         //noise fps
                break;
           }
    VERIFY (m_Params[param]);
}

void        CPostProcessColor::add_value                       (float time, float value, float t, float c, float b, int index)
{
    KeyIt i;
    if (0 == index)
       {
       m_Red.InsertKey (time, value);
       i = m_Red.FindKey (time, 0.1f);
       }
    else if (1 == index)
            {
            m_Green.InsertKey (time, value);
            i = m_Green.FindKey (time, 0.1f);
            }
         else
            {
            m_Blue.InsertKey (time, value);
            i = m_Blue.FindKey (time, 0.1f);
            }
    (*i)->tension = t;
    (*i)->continuity = c;
    (*i)->bias = b;
}
void        CPostProcessColor::update_value                    (float time, float value, float t, float c, float b, int index)
{
    KeyIt i;
    if (0 == index) i = m_Red.FindKey (time, 0.1f);
    else if (1 == index) i = m_Green.FindKey (time, 0.1f);
         else i = m_Blue.FindKey (time, 0.1f);
    (*i)->value = value;
    (*i)->tension = t;
    (*i)->continuity = c;
    (*i)->bias = b;
}
void        CPostProcessColor::get_value                       (float time, float &value, float &t, float &c, float &b, int index)
{
    KeyIt i;
    if (0 == index) i = m_Red.FindKey (time, 0.1f);
    else if (1 == index) i = m_Green.FindKey (time, 0.1f);
         else i = m_Blue.FindKey (time, 0.1f);
    value = (*i)->value;
    t = (*i)->tension;
    c = (*i)->continuity;
    b = (*i)->bias;
}

void        CPostProcessValue::add_value                       (float time, float value, float t, float c, float b, int index)
{
    m_Value.InsertKey (time, value);
    KeyIt i = m_Value.FindKey (time, 0.1f);
    (*i)->tension = t;
    (*i)->continuity = c;
    (*i)->bias = b;
}
void        CPostProcessValue::update_value                    (float time, float value, float t, float c, float b, int index)
{
    KeyIt i = m_Value.FindKey (time, 0.1f);
    (*i)->value = value;
    (*i)->tension = t;
    (*i)->continuity = c;
    (*i)->bias = b;
}
void        CPostProcessValue::get_value                       (float time, float &value, float &t, float &c, float &b, int index)
{
    KeyIt i = m_Value.FindKey (time, 0.1f);
    value = (*i)->value;
    t = (*i)->tension;
    c = (*i)->continuity;
    b = (*i)->bias;
}
#endif /*_PP_EDITOR_*/
