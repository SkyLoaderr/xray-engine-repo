//---------------------------------------------------------------------------
#ifndef ParticleGroupH
#define ParticleGroupH

class CParticleGroup
{
	int			m_HandleGroup;
	int			m_HandleActionList;
public: 
				CParticleGroup	();
	virtual 	~CParticleGroup	();
	void	 	OnFrame			();
	void	 	Render			();
    
	void 		ParseCommandList(LPCSTR txt);
};

//---------------------------------------------------------------------------
#endif
