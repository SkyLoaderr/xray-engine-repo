#ifndef SceneCustomH
#define SceneCustomH
//---------------------------------------------------------------------------

class ECORE_API ISceneCustom
{
public:
    						ISceneCustom		(){}
	virtual					~ISceneCustom		(){}
	virtual CCustomObject*	RayPickObject 		(float dist, const Fvector& start, const Fvector& dir, ObjClassID classfilter, SRayPickInfo* pinf, ObjectList* from_list)=0;
	virtual ObjectList* 	GetSnapList			(bool bIgnoreUse)=0;
	virtual void 			GenObjectName		(ObjClassID cls_id, char *buffer, const char* prefix=NULL)=0;
	virtual void 			AppendObject		(CCustomObject* object, bool bExecUndo=true)=0;
	virtual bool 			RemoveObject		(CCustomObject* object, bool bExecUndo=true)=0;
	virtual CCustomObject* 	FindObjectByName	(LPCSTR name, ObjClassID classfilter)=0;
    virtual CCustomObject* 	FindObjectByName	(LPCSTR name, CCustomObject* pass_object)=0;
};

extern ECORE_API ISceneCustom* IScene;

#endif
