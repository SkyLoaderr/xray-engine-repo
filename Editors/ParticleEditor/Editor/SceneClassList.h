//----------------------------------------------------
// file: SceneClassList.h
//----------------------------------------------------

#ifndef SceneClassListH
#define SceneClassListH
//----------------------------------------------------

class CEditableObject;
class CEditableMesh;
class CSceneObject;
class TUI_CustomTools;

#ifdef _EDITOR
	struct SPickQuery{
        Fvector				m_Start;
        Fvector				m_Direction;
        float				m_Dist;
        Fbox 				m_BB; 
    	Flags32				m_Flags;
    	struct SResult{
        	CDB::TRI		T;
            float			range;
            SResult			(CDB::TRI& t):T(t){;}
        };
		DEFINE_VECTOR(SResult,ResultVec,ResultIt);
    public:
    	ResultVec			results;
        IC void	prepare_rq	(const Fvector& start, const Fvector& dir, float dist, u32 flags)
        {
            m_Start.set		(start);
            m_Direction.set	(dir);
            m_Dist			= dist;
            m_Flags.set		(flags);
        	results.clear	();
        }
        IC void	prepare_bq	(const Fbox& bbox, u32 flags)
        {
        	m_BB.set		(bbox);
            m_Flags.set		(flags);
        	results.clear	();
        }
		IC void append		(const Fmatrix& parent, CDB::MODEL* M, CDB::RESULT* R)
        {
        	CDB::TRI* T		= M->get_tris()+R->id;
            SResult	D		(*T);
            D.range			= R->range;
            parent.transform_tiny(*D.T.verts[0],*T->verts[0]);
            parent.transform_tiny(*D.T.verts[1],*T->verts[1]);
            parent.transform_tiny(*D.T.verts[2],*T->verts[2]);
            if (m_Flags.is(CDB::OPT_ONLYNEAREST)&&!results.empty()){
	            SResult& S	= results.back();
                if (D.range<S.range) S = D;
            }else			results.push_back	(D);
        }
        IC int r_count		()
        {
        	results.size	();
        }
        IC SResult* r_begin	()
        {
        	return results.begin();
        }
        IC SResult* r_end	()
        {
        	return results.end();
        }
        IC void r_clear		()
        {
        	results.clear	();
        }
    };
	struct SRayPickInfo{
		CDB::RESULT 		inf;
		CSceneObject*		s_obj;
		CEditableObject*	e_obj;
		CEditableMesh*		e_mesh;
		Fvector     		pt;
		SRayPickInfo		(){Reset();}
		IC void Reset		(){ ZeroMemory(this,sizeof(SRayPickInfo));inf.range = 5000;}
		IC void SetRESULT	(CDB::MODEL* M, CDB::RESULT* R){inf=*R;inf.id=(M->get_tris()+inf.id)->dummy;}
	};
    DEFINE_VECTOR(CDB::RESULT,BPInfVec,BPInfIt);
	struct SBoxPickInfo{
    	BPInfVec			inf;
		CSceneObject*		s_obj;
		CEditableObject*	e_obj;
		CEditableMesh*		e_mesh;
		SBoxPickInfo		(){Reset();}
		IC void Reset		(){ZeroMemory(this,sizeof(SBoxPickInfo));}
		IC void AddRESULT	(CDB::MODEL* M, CDB::RESULT* R){inf.push_back(*R); inf.back().id=(M->get_tris()+inf.back().id)->dummy;}
	};
	DEFINE_VECTOR(SBoxPickInfo,SBoxPickInfoVec,SBoxPickInfoIt);
#endif

//----------------------------------------------------
class CCustomObject;

enum EObjClass{
    OBJCLASS_DUMMY     	= -1,
    OBJCLASS_GROUP		= 0,
    OBJCLASS_GLOW	   	= 1,
    OBJCLASS_SCENEOBJECT= 2,
    OBJCLASS_LIGHT	   	= 3,
    OBJCLASS_SHAPE  	= 4,
    OBJCLASS_SOUND_SRC 	= 5,
    OBJCLASS_SPAWNPOINT	= 6,
    OBJCLASS_WAY	  	= 7,
    OBJCLASS_SECTOR    	= 8,
    OBJCLASS_PORTAL    	= 9,
    OBJCLASS_SOUND_ENV 	= 10,
    OBJCLASS_PS		   	= 11,
    OBJCLASS_DO			= 12,
    OBJCLASS_AIMAP		= 13,
    OBJCLASS_COUNT,
    OBJCLASS_force_dword = -1
};

//----------------------------------------------------
TUI_CustomTools*NewToolFromTarget		(int tgt);
CCustomObject*	NewObjectFromClassID	(int _ClassID, LPVOID data, LPCSTR name);
LPSTR 			GetNameByClassID		(EObjClass cls_id);
LPSTR 			GetClassNameByClassID	(EObjClass cls_id);
bool 			IsClassID				(EObjClass cls_id);
bool 			IsObjectListClassID		(EObjClass cls_id);
bool 			IsGroupClassID			(EObjClass cls_id);
EObjClass 		ClassIDFromTargetID		(int target);
//----------------------------------------------------
#endif /*_INCDEF_SceneClassList_H_*/


