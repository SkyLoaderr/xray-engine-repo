//---------------------------------------------------------------------------
#ifndef ParticleGroupH
#define ParticleGroupH

class CParticleGroup
{
	int			m_HandleGroup;
	int			m_HandleActionList;

    string128	m_ShaderName;
    string128	m_TextureName;
    Shader*		m_Shader;
    
    void		pSprite			(LPCSTR sh_name, LPCSTR tex_name);
public: 
				CParticleGroup	();
	virtual 	~CParticleGroup	();
	void	 	OnFrame			();
	void	 	Render			();
    
	void 		ParseCommandList(LPCSTR txt);
};

//---------------------------------------------------------------------------
#endif
