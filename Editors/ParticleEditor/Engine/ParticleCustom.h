//---------------------------------------------------------------------------
#ifndef ParticleCustomH
#define ParticleCustomH
//---------------------------------------------------------------------------
class ENGINE_API IParticleCustom{
public:
	virtual 		~IParticleCustom	(){;}

    virtual void 	OnDeviceCreate		()=0;
    virtual void 	OnDeviceDestroy		()=0;

    virtual void	UpdateParent		(const Fmatrix& m, const Fvector& velocity)=0;
	virtual void	OnFrame				(u32 dt)=0;

	virtual void	Play				()=0;
    virtual void	Stop				(BOOL bDefferedStop=TRUE)=0;
    virtual BOOL	IsPlaying			()=0;
};
//---------------------------------------------------------------------------
#endif //ParticleCustomH
 