//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------
#ifndef ESceneAIMapToolsH
#define ESceneAIMapToolsH

#include "SceneCustomMTools.H"
#include "xrLevel.H"
#include "AIMapExport.H"

// refs
class ESceneAIMapTools;

const DWORD InvalidNode		= (1<<24)-1;

#pragma pack(push,1)
struct SAINode					// definition of "patch" or "node"
{
	union	{
		struct {
			SAINode* 	n1;		// Left
            SAINode* 	n2;		// Forward
            SAINode* 	n3;		// Right
            SAINode* 	n4;		// Backward
		};
		SAINode*		n[4];
	};
	Fplane		Plane;			// plane of patch										
	Fvector		Pos;			// position of patch center								

    enum{
    	flSelected 	= (1<<0),
    	flHLSelected= (1<<1),

    	flN1		= (1<<4),
    	flN2		= (1<<5),
    	flN3		= (1<<6),
    	flN4		= (1<<7),
    };
	Flags8		flags;
    u32			idx;

	SAINode				() 	{n1=n2=n3=n4=0;idx=0;flags.zero();}
	SAINode*	nLeft	()	{return n1;}
	SAINode*	nForward()	{return n2;}
	SAINode*	nRight	()	{return n3;}
	SAINode*	nBack	()	{return n4;}
	
    int			Links	()	{int cnt=0; for (int k=0; k<4; k++) if(n[k]) cnt++; return cnt;}
	void		PointLF	(Fvector& D, float patch_size);
	void		PointFR	(Fvector& D, float patch_size);
	void		PointRB	(Fvector& D, float patch_size);
	void		PointBL	(Fvector& D, float patch_size);

    void   		Load	(IReader&, ESceneAIMapTools*);
    void   		Save	(IWriter&, ESceneAIMapTools*);
};
#pragma pack(pop)
DEFINE_VECTOR(SAINode*,AINodeVec,AINodeIt);

struct SAIEmitter{
    enum{
        flSelected = (1<<0),
    };
    Fvector			pos;
    Flags32			flags;
    SAIEmitter		(){pos.set(0,0,0); flags.zero();}
    SAIEmitter		(const Fvector& p, u32 fl){pos.set(p); flags.set(fl);}
};
DEFINE_VECTOR(SAIEmitter,AIEmitterVec,AIEmitterIt);

const int				HDIM_X	= 128;
const int				HDIM_Z	= 128;
class ESceneAIMapTools: public ESceneCustomMTools
{
	friend class SAINode;
	typedef ESceneCustomMTools inherited;
    ObjectList			m_SnapObjects;
	// hash
	AINodeVec			m_HASH[HDIM_X+1][HDIM_Z+1];
    AINodeVec			m_Nodes;

	AIEmitterVec 		m_Emitters;

    SAIParams			m_Params;
    Fbox				m_BBox;

	SGeometry*			m_RGeom;
    Shader*				m_Shader;
protected:    
    void 				hash_FillFromNodes		();
    void 				hash_Initialize			();
	void 				hash_Clear				();
    void				HashRect				(const Fvector& v, float radius, Irect& result);
	AINodeVec*			HashMap					(int ix, int iz);
	AINodeVec*			HashMap					(Fvector& V);
	SAINode*			FindNode				(Fvector& vAt, float eps=0.05f);
	SAINode* 			FindNeighbor			(SAINode* N, int side);

    void				RemoveNode				(AINodeIt N);
	SAINode* 			BuildNode				(Fvector& vFrom, Fvector& vAt, bool bIgnoreConstraints, bool bSuperIgnoreConstraints=false);
	int	 				BuildNodes				(const Fvector& pos, int sz, bool bIgnoreConstraints);
	void 				BuildNodes				();
	BOOL 				CreateNode				(Fvector& vAt, SAINode& N, bool bIgnoreConstraints);
	BOOL 				CanTravel				(Fvector _from, Fvector _at);

	SAINode* 			GetNode					(Fvector vAt, bool bIgnoreConstraints);
	void				UpdateLinks				(SAINode* N, bool bIgnoreConstraints);

	void 				UnpackPosition			(Fvector& Pdest, const NodePosition& Psrc, Fbox& bb, SAIParams& params);
	u32 				UnpackLink				(u32& L);
	void				PackPosition			(NodePosition& Dest, Fvector& Src, Fbox& bb, SAIParams& params);
    
    void				EnumerateNodes			();
    void				DenumerateNodes			();
public:
	enum EMode{
    	mdAppend,
    	mdRemove,
    	mdInvert,
    };
	enum{
    	flDrawSnapObjects 	= (1<<0),
    	flUpdateHL 			= (1<<15),
    };
    Flags32				m_Flags;

    float 				m_VisRadius;
public:
						ESceneAIMapTools 	   	();
	virtual        	 	~ESceneAIMapTools 		();

    virtual	void		RemoveFromSnapList		(CCustomObject* object);
	// selection manipulate
    SAINode*			PickNode				(const Fvector& start, const Fvector& dir, float dist);
    SAIEmitter*			PickEmitter				(const Fvector& start, const Fvector& dir, float dist);
    virtual bool		PickGround				(Fvector& dest, const Fvector& start, const Fvector& dir, float dist);
	virtual int			RaySelect				(bool flag, float& distance, const Fvector& start, const Fvector& direction);
	virtual int	   		FrustumSelect			(bool flag);
	virtual int    		SelectObjects           (bool flag);
	virtual int    		InvertSelection         ();
	virtual int 		RemoveSelection         ();
	virtual int    		SelectionCount          (bool testflag);

    virtual void		Clear					(bool bOnlyNodes=false);

    // validation
    virtual bool   		Valid					();
    virtual bool   		IsNeedSave				();

    // events
	virtual void   		OnFrame					();
    virtual void   		OnRender				(int priority, bool strictB2F);

    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool   		Export          		(LPCSTR fn);

	// device dependent funcs    
	virtual void		OnDeviceCreate			();
	virtual void		OnDeviceDestroy			();

	virtual void		OnSynchronize			();

    // utils
    bool				GenerateMap				();

    void				AddEmitter				(const Fvector& pos);
    int					AddNode					(const Fvector& pos, bool bIgnoreConstraints, bool bAutoLink, int cnt);

    ObjectList&			SnapObjects				(){return m_SnapObjects;}
    bool				FillSnapList			(ObjectList* lst);

    AIEmitterVec&		Emitters				(){return m_Emitters;}
    AINodeVec&			Nodes					(){return m_Nodes;}
    
    void				MakeLinks				(u8 side_flag, EMode mode);
    void				RemoveLinks				();
    void				InvertLinks				();

	void 				UpdateHLSelected		(){m_Flags.set(flUpdateHL,TRUE);}

    void 				SmoothNodes				();
    void				RemoveInvalidNodes		(int link);
};
#endif // ESceneAIMapToolsH

