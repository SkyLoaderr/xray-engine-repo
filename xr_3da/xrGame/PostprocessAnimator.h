#pragma once

#include "../envelope.h"
#include "../EffectorPP.h"

#define POSTPROCESS_PARAMS_COUNT    10
#define POSTPROCESS_FILE_VERSION    0x0001

#define POSTPROCESS_FILE_EXTENSION  ".ppe"


typedef enum _pp_params
{
    pp_unknown              =  -1,
    pp_base_color           =   0,
    pp_add_color            =   1,
    pp_gray_color           =   2,
    pp_gray_value           =   3,
    pp_blur                 =   4,
    pp_dual_h               =   5,
    pp_dual_v               =   6,
    pp_noise_i              =   7,
    pp_noise_g              =   8,
    pp_noise_f              =   9,
    pp_force_dword          =   0x7fffffff
} pp_params;


class CPostProcessParam
{
protected:
public:
    virtual void    update                          (float dt) = 0;
    virtual void    load                            (IReader &pReader) = 0;
    virtual void    save                            (IWriter &pWriter) = 0;
    virtual float   get_length                      () = 0;
#ifdef _PP_EDITOR_
    virtual void    add_value                       (float time, float value, float t, float c, float b, int index = 0) = 0;
    virtual void    update_value                    (float time, float value, float t, float c, float b, int index = 0) = 0;
    virtual void    get_value                       (float time, float &value, float &t, float &c, float &b, int index = 0) = 0;
    virtual size_t  get_keys_count                  () = 0;
    virtual float   get_key_time                    (size_t index) = 0;
#endif /*_PP_EDITOR_*/
};

class CPostProcessValue : public CPostProcessParam
{
protected:
    CEnvelope       m_Value;
    float          *m_pfParam;
public:
                    CPostProcessValue               (float *pfparam) { m_pfParam = pfparam; }
    virtual void    update                          (float dt)
                    {
                    *m_pfParam = m_Value.Evaluate (dt);
                    }
    virtual void    load                            (IReader &pReader);
    virtual void    save                            (IWriter &pWriter);
    virtual float   get_length                      ()
                    {
                    float mn, mx;
                    return m_Value.GetLength (&mn, &mx);
                    }
#ifdef _PP_EDITOR_
    virtual void    add_value                       (float time, float value, float t, float c, float b, int index = 0);
    virtual void    update_value                    (float time, float value, float t, float c, float b, int index = 0);
    virtual void    get_value                       (float time, float &value, float &t, float &c, float &b, int index = 0);
    virtual size_t  get_keys_count                  ()
                    {
                    return m_Value.keys.size ();
                    }
    virtual float   get_key_time                    (size_t index)
                    {
                    VERIFY (index < get_keys_count ());
                    return m_Value.keys[index]->time;
                    }
#endif /*_PP_EDITOR_*/
};


class CPostProcessColor : public CPostProcessParam
{
protected:
    float           m_fBase;
    CEnvelope       m_Red;
    CEnvelope       m_Green;
    CEnvelope       m_Blue;
	SPPInfo::SColor *m_pColor;
public:
                    CPostProcessColor               (SPPInfo::SColor *pcolor) { m_pColor = pcolor; }
    virtual void    update                          (float dt)
                    {
                    m_pColor->r = m_Red.Evaluate (dt);
                    m_pColor->g = m_Green.Evaluate (dt);
                    m_pColor->b = m_Blue.Evaluate (dt);
                    }
    virtual void    load                            (IReader &pReader);
    virtual void    save                            (IWriter &pWriter);
    virtual float   get_length                      ()
                    {
                    float mn, mx,
                    r = m_Red.GetLength (&mn, &mx),
                    g = m_Green.GetLength (&mn, &mx),
                    b = m_Blue.GetLength (&mn, &mx);
                    mn = (r > g ? r : g);
                    return mn > b ? mn : b;
                    }
#ifdef _PP_EDITOR_
    virtual void    add_value                       (float time, float value, float t, float c, float b, int index = 0);
    virtual void    update_value                    (float time, float value, float t, float c, float b, int index = 0);
    virtual void    get_value                       (float time, float &value, float &t, float &c, float &b, int index = 0);
    virtual size_t  get_keys_count                  ()
                    {
                    return m_Red.keys.size ();
                    }
    virtual float   get_key_time                    (size_t index)
                    {
                    VERIFY (index < get_keys_count ());
                    return m_Red.keys[index]->time;
                    }
#endif /*_PP_EDITOR_*/
};


class CPostprocessAnimator :public CEffectorPP
{
protected:
    CPostProcessParam                               *m_Params[POSTPROCESS_PARAMS_COUNT];
    shared_str										m_Name;
    SPPInfo											m_EffectorParams;
	float											m_factor;
	bool											m_bStop;
	float											m_stop_speed;
	bool											m_bCyclic;
	float											m_start_time;
		void		Update							(float tm);
public:
                    CPostprocessAnimator            (int id, bool cyclic);
        virtual    ~CPostprocessAnimator            ();
        void        Clear                           ();
        void        Load                            (LPCSTR name);
    IC  LPCSTR      Name                            (){return *m_Name;}
        void        Stop                            (float speed);
        float       GetLength                       ();
virtual	BOOL		Process							(SPPInfo &PPInfo);
        void        Create                          ();
#ifdef _PP_EDITOR_
        CPostProcessParam*  GetParam                (pp_params param);
        void        ResetParam                      (pp_params param);
        void        Save                            (LPCSTR name);
#endif /*_PP_EDITOR_*/
};
