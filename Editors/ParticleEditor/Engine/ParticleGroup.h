//---------------------------------------------------------------------------
#ifndef ParticleGroupH
#define ParticleGroupH

class CParticleGroup
{
    struct SAnimation
    {
        Fvector2		m_TexSize;
        Fvector2		m_FrameSize;
        int     		m_iFrameDimX;

        void 			InitDefault()
        {
            m_FrameSize.set(32.f,64.f);
            m_TexSize.set(32.f/256.f,64.f/128.f);
            m_iFrameDimX = 8;
        }
        void 			SetTextureSize(float w, float h, float fw, float fh)
        {
            m_FrameSize.set(fw,fh);
            m_TexSize.set(fw/w,fh/h);
            m_iFrameDimX = iFloor(1.f/m_TexSize.x);
        }
        void            CalculateTC(int frame, Fvector2& lt, Fvector2& rb)
        {
            lt.x        = (frame%m_iFrameDimX)*m_TexSize.x;
            lt.y        = (frame/m_iFrameDimX)*m_TexSize.y;
            rb.x        = lt.x+m_TexSize.x;
            rb.y        = lt.y+m_TexSize.y;
        }
    };
    
	int			m_HandleGroup;
	int			m_HandleActionList;

    LPSTR		m_ShaderName;
    LPSTR		m_TextureName;
    Shader*		m_Shader;
    
    SAnimation	m_Animation;
    
public: 
    void		pSprite			(LPCSTR sh_name, LPCSTR tex_name);
    void		pFrameSet		(WORD texture_width=128, WORD texture_height=128, WORD frame_width=32, WORD frame_height=32);
public: 
				CParticleGroup	();
	virtual 	~CParticleGroup	();
	void	 	OnFrame			();
	void	 	Render			();
    
	void 		ParseCommandList(LPCSTR txt);

    void 		OnDeviceCreate	();
    void 		OnDeviceDestroy	();
};

//---------------------------------------------------------------------------
#endif
