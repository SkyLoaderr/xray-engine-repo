//---------------------------------------------------------------------------
#ifndef ParticleGroupH
#define ParticleGroupH

// refs
namespace PAPI{
class Particle;
class ParticleGroup;
}

class CParticleGroup
{
    struct SAnimation
    {
        Fvector2		m_TexSize;
        Fvector2		m_FrameSize;
        int     		m_iFrameDimX;
        int 			m_iFrameCount;
        float			m_fSpeed;

        IC void 		InitDefault()
        {
            m_FrameSize.set(32.f,64.f);
            m_TexSize.set(32.f/256.f,64.f/128.f);
            m_iFrameDimX = 8;
            m_iFrameCount = 16;
            m_fSpeed	= 24.f;
        }
        IC void 		Set(int frame_count, float w, float h, float fw, float fh)
        {
			m_iFrameCount 	= frame_count;
            m_FrameSize.set	(fw,fh);
            m_TexSize.set	(fw/w,fh/h);
            m_iFrameDimX 	= iFloor(1.f/m_TexSize.x);
        }
        IC void       	CalculateTC(int frame, Fvector2& lt, Fvector2& rb)
        {
            lt.x        = (frame%m_iFrameDimX)*m_TexSize.x;
            lt.y        = (frame/m_iFrameDimX)*m_TexSize.y;
            rb.x        = lt.x+m_TexSize.x;
            rb.y        = lt.y+m_TexSize.y;
        }
    };
    enum{
    	flSprite		= (1<<0),
    	flModel			= (1<<1),
        
    	flFramed		= (1<<10),
    	flAnimated		= (1<<11),
        flRandomFrame   = (1<<12),
        flRandomPlayback= (1<<13),
    };
    
    string64	m_Name;                   
    Flags32		m_Flags;
    
	int			m_HandleGroup;
	int			m_HandleActionList;

    LPSTR		m_ShaderName;
    LPSTR		m_TextureName;
    Shader*		m_Shader;
    
    SAnimation	m_Animation;
public:
#ifdef _PARTICLE_EDITOR
	AnsiString	m_SourceText;
#endif
protected:    
	BOOL 		SaveActionList		(CFS_Base& F);
	BOOL 		LoadActionList		(CStream& F);

    void		RefreshShader		();

    // action 
    void		pFrameInitExecute	(PAPI::ParticleGroup *group);
    void		pAnimateExecute		(PAPI::ParticleGroup *group);
public: 
    // api function
	// state api
    void		pSprite			(LPCSTR sh_name, LPCSTR tex_name);
    void		pFrame			(BOOL random_frame=TRUE, u32 frame_count=16, u32 texture_width=128, u32 texture_height=128, u32 frame_width=32, u32 frame_height=32);
    // action api
	void 		pAnimate		(float speed=24.f, BOOL random_playback=FALSE);
public: 
				CParticleGroup	();
	virtual 	~CParticleGroup	();
	void	 	OnFrame			();

    void		SetName			(LPCSTR name);
    
	void	 	Render			();
	void 		RenderEditor	();
    
	void 		ParseCommandList(LPCSTR txt);

    void 		OnDeviceCreate	();
    void 		OnDeviceDestroy	();

    void		UpdateParent	(const Fmatrix& m, const Fvector& velocity);

   	void 		Save			(CFS_Base& F); 
   	BOOL 		Load			(CStream& F); 
};

#define PG_VERSION				0x0001
//----------------------------------------------------
#define PG_CHUNK_VERSION		0x0001
#define PG_CHUNK_NAME			0x0002
#define PG_CHUNK_GROUPDATA		0x0003
#define PG_CHUNK_ACTIONLIST		0x0004
#define PG_CHUNK_FLAGS			0x0005
#define PG_CHUNK_FRAME			0x0006
#define PG_CHUNK_SPRITE			0x0007
#define PG_CHUNK_SOURCETEXT		0x0020

#define ACTION_LIST_VERSION		0x0001
//----------------------------------------------------
#define AL_CHUNK_VERSION		0x0001
#define AL_CHUNK_ACTIONS		0x0002

//---------------------------------------------------------------------------
#endif
