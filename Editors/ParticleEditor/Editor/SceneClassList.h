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
	class SPickQuery{
    	Flags32				m_Flags;
    public:
        Fvector				m_Start;
        Fvector				m_Direction;
        float				m_Dist;
        Fbox 				m_BB; 
    public:
    	class SResult{
        	Fvector			_verts[3];      
		public:            
        	Fvector*		verts[3];
            float			range;
            SResult			(const SResult& F)
            {
            	if (F.verts[0]==&F._verts[0]){
	            	verts[0]	=&_verts[0];
    	            verts[1]	=&_verts[1];
        	        verts[2]	=&_verts[2];
                    _verts[0]	= F._verts[0];
                    _verts[1]	= F._verts[1];
                    _verts[2]	= F._verts[2];
                }else{
	            	verts[0]	=F.verts[0];
    	            verts[1]	=F.verts[1];
        	        verts[2]	=F.verts[2];
                }
                range		= F.range;
            }
            SResult			(const Fmatrix& parent, CDB::TRI& t, float r)
            {
                parent.transform_tiny(_verts[0],*t.verts[0]); verts[0]	=&_verts[0];
                parent.transform_tiny(_verts[1],*t.verts[1]); verts[1]	=&_verts[1];
                parent.transform_tiny(_verts[2],*t.verts[2]); verts[2]	=&_verts[2];
                range		= r;
            }
            SResult			(CDB::TRI* t, float r)
            { 
                verts[0]	= t->verts[0];
                verts[1]	= t->verts[1];
                verts[2]	= t->verts[2];
                range		= r;
            }
        };
		DEFINE_VECTOR(SResult,ResultVec,ResultIt);
    protected:
    	ResultVec			results;
    public:
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
		IC void append_mtx	(const Fmatrix& parent, CDB::MODEL* M, CDB::RESULT* R)
        {
        	CDB::TRI* T		= M->get_tris()+R->id;
            SResult	D		(parent, *T, R->range);
            if (m_Flags.is(CDB::OPT_ONLYNEAREST)&&!results.empty()){
	            SResult& S	= results.back();
                if (D.range<S.range) S = D;
            }else			results.push_back	(D);
        }
		IC void append		(CDB::MODEL* M, CDB::RESULT* R)
        {
        	CDB::TRI* T		= M->get_tris()+R->id;
            SResult	D		(T, R->range);
            if (m_Flags.is(CDB::OPT_ONLYNEAREST)&&!results.empty()){
	            SResult& S	= results.back();
                if (D.range<S.range) S = D;
            }else			results.push_back	(D);
        }
        IC int r_count		()
        {
        	return results.size();
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


