//---------------------------------------------------------------------------
#ifndef ParticleGroupH
#define ParticleGroupH

// refs
class Particle;
class ParticleGroup;

class CParticleGroup
{
    struct SAnimation
    {
        Fvector2		m_TexSize;
        Fvector2		m_FrameSize;
        int     		m_iFrameDimX;
        int 			m_iFrameCount;
        float			m_fSpeed;

        void 			InitDefault()
        {
            m_FrameSize.set(32.f,64.f);
            m_TexSize.set(32.f/256.f,64.f/128.f);
            m_iFrameDimX = 8;
            m_iFrameCount = 16;
            m_fSpeed	= 24.f;
        }
        void 			Set(int frame_count, float w, float h, float fw, float fh)
        {
			m_iFrameCount 	= frame_count;
            m_FrameSize.set	(fw,fh);
            m_TexSize.set	(fw/w,fh/h);
            m_iFrameDimX 	= iFloor(1.f/m_TexSize.x);
        }
        void            CalculateTC(int frame, Fvector2& lt, Fvector2& rb)
        {
            lt.x        = (frame%m_iFrameDimX)*m_TexSize.x;
            lt.y        = (frame/m_iFrameDimX)*m_TexSize.y;
            rb.x        = lt.x+m_TexSize.x;
            rb.y        = lt.y+m_TexSize.y;
        }
    };
    enum{
    	flFramed		= (1<<0),
    	flAnimated		= (1<<1),
        flRandomFrame   = (1<<2),
        flRandomPlayback= (1<<3),
    };
    
    Flags32		m_Flags;
    
	int			m_HandleGroup;
	int			m_HandleActionList;

    LPSTR		m_ShaderName;
    LPSTR		m_TextureName;
    Shader*		m_Shader;
    
    SAnimation	m_Animation;
    
    void		pFrameExec		(Particle *particle);
    void		pAnimateExec	(ParticleGroup *group);
public: 
	// state
    void		pSprite			(LPCSTR sh_name, LPCSTR tex_name);
    void		pFrame			(BOOL random_frame=TRUE, u32 frame_count=16, u32 texture_width=128, u32 texture_height=128, u32 frame_width=32, u32 frame_height=32);
    // action
	void 		pAnimate		(float speed=24.f, BOOL random_playback=FALSE);
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
