#ifndef SceneCustomMToolsH
#define SceneCustomMToolsH

class ESceneCustomMTools
{
	EObjClass 		FClassID;
public:
    PropertyGP(FClassID,FClassID)			EObjClass ClassID;
public:
					ESceneCustomMTools		(){;}
	virtual			~ESceneCustomMTools		(){;}

    virtual void	RemoveFromSnapList		(CCustomObject* O)=0;
	// selection manipulate
	virtual int		RaySelect				(bool flag, float& distance, const Fvector& start, const Fvector& direction)=0;
	virtual int		FrustumSelect			(bool flag)=0;
	virtual int 	SelectObjects           (bool flag)=0;
	virtual int 	InvertSelection         ()=0;
	virtual int 	RemoveSelection         ()=0;
	virtual int 	SelectionCount          (bool testflag)=0;

    virtual void	Clear					(bool bSpecific=false)=0;

    // validation
    virtual bool	Valid					()=0;

    // events
	virtual void	OnFrame					()=0;
    virtual void	OnRender				(int priority, bool strictB2F)=0;

    // IO
    virtual bool	Load            		(IReader&)=0;
    virtual void	Save            		(IWriter&)=0;
    virtual bool	Export          		(LPCSTR fn)=0;

	// device dependent funcs    
	virtual void	OnDeviceCreate			()=0;
	virtual void	OnDeviceDestroy			()=0;

	virtual void	OnSynchronize			()=0;
};

#endif //SceneCustomMToolsH
 