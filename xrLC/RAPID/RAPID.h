#ifndef RAPID_H
#define RAPID_H

/****************************************************************************/
// RAPID API RETURN VALUES

const int RAPID_OK = 0;
  // Used by all API routines except constructors and destructors.


const int RAPID_ERR_MODEL_OUT_OF_MEMORY = 1;
  // Returned when an API function cannot obtain enough memory to store
  // or process a RAPID_model object.


const int RAPID_ERR_COLLIDE_OUT_OF_MEMORY = 2;
  // Returned when RAPID_Collide() cannot allocate enough storage to hold
  // collision information.  In this case, there is as much collision
  // detection information available as could be allocated for.


const int RAPID_ERR_UNPROCESSED_MODEL = 3;
  // Returned when an unprocessed model is passed to a function which
  // expects only processed models, such as RAPID_Collide().


const int RAPID_ERR_BUILD_OUT_OF_SEQUENCE = 4;
  // Returned when:
  //       1. AddTri() is called before BeginModel().  The triangle will
  //          be added anyway as if BeginModel() had been previously called.
  //       2. BeginModel() is called immediately after AddTri().  The
  //          model will be placed into an empty initial state.
  // This error code is something like a warning: the invoked
  // operation takes place anyway, but the returned error code may tip
  // off the client that something out of the ordinary is happenning.


const int RAPID_ERR_BUILD_EMPTY_MODEL = 5;
  // Returned when EndModel() is called on a model to which no
  // triangles have been added.  This is similar in spirit to the
  // OUT_OF_SEQUENCE return code, except that the requested operation
  // has FAILED -- the model remains "unprocessed".


const int RAPID_UNKNOWN_OPTION = 6;
  // Returned when SetOption or UnsetOption is called with a first
  // parameter which doesn't correspond to an unknown option.  In
  // other words, if you get this error at run-time, you probably
  // typed in the wrong RAPID_ symbol.

const int RAPID_BAD_PARAMS = 7;
  // Returned when an API function accept bad params.

/****************************************************************************/

//  for SetOption

enum RAPID_option
{
  RAPID_BUILD_NUM_TRIS_PER_LEAF,
  RAPID_BUILD_MAX_LEVELS_DEEP
};

/*

  Example: Sometime before calling BeginModel(), calls to SetOption()
  will control some aspects of how the models are constructed.  For
  example,

  RAPID_model *m = new RAPID_model;
  m->SetOption(RAPID_BUILD_NUM_TRIS_PER_LEAF, 10);
  m->BeginModel();
  m->AddTri(...);
  m->AddTri(...);
  ...
  m->AddTri(...);
  m->EndModel();

  At this point, the OBB is constructed in the usual manner, except
  that during construction, bounding boxes with 10 triangles or fewer
  are made into leaf nodes.  Without the call to SetOption(), the tree
  would be built as deep as necessary until every leaf node contains
  exactly one triangle.

  Another example:

  RAPID_model *m = new RAPID_model;
  m->SetOption(RAPID_BUILD_MAX_LEVELS_DEEP, 4);
  m->BeginModel();
  m->AddTri(...);
  m->AddTri(...);
  ...
  m->AddTri(...);
  m->EndModel();

  Here, the tree is build no more than 4 levels deep.  This means the
  root node, and three levels below it.  Hence, there are 8 leaf
  nodes, at most.

  It is permissible to apply the NUM_TRIS_PER_LEAF and the
  MAX_LEVELS_DEEP options -- they are not mutually exclusive.  During
  the top-down construction of the tree, any node meeting either the
  num tris criterion OR the max levels criterion will remain unsplit,
  and be a leaf node.

  These options can also be unset, using UnsetOption().

  */

/****************************************************************************/


class	ENGINE_API box;
union	ENGINE_API tri;

class ENGINE_API RAPID_model
{
public:
  box *b;
  int num_boxes_alloced;

  tri *tris;
  int num_tris;
  int num_tris_alloced;
  int *tri_index;

  int build_opt_max_levels_deep;
  int build_opt_num_tris_per_leaf;

  int build_state;

  int build_hierarchy();

  friend RAPID_Collide(float R1[3][3], float T1[3],
		       float s1, RAPID_model *RAPID_model1,
		       float R2[3][3], float T2[3],
		       float s2, RAPID_model *RAPID_model2,
		       int flag);

public:

  // These are everything the client needs to use RAPID.

			RAPID_model();
			~RAPID_model();

  int		BeginModel		( );
  int		AddAllTris		( int count, void *p );
  int		AddTri			( const float *p1, const float *p2, const float *p3 );
  int		EndModel		( );

  int		SetOption		( RAPID_option opt, int val );
  int		UnsetOption		( RAPID_option opt );
};

/****************************************************************************/

//                These are for the client
// Find all pairwise intersecting triangles
const int RAPID_ALL_CONTACTS = 1;

// Just report one intersecting triangle pair, if there are any.
const int RAPID_FIRST_CONTACT = 2;


// This is the collision query invocation.  It assumes that the
// models are not being scaled up or down, but have their native
// dimensions.
int
RAPID_Collide(float R1[3][3], float T1[3], RAPID_model *o1,
	      float R2[3][3], float T2[3], RAPID_model *o2,
	      int flag = RAPID_ALL_CONTACTS);

// This collision query permits the models to each be scaled by
// some positive factor (must be greater than 0).
int
RAPID_Collide(float R1[3][3], float T1[3], float s1, RAPID_model *o1,
	      float R2[3][3], float T2[3], float s2, RAPID_model *o2,
	      int flag = RAPID_ALL_CONTACTS);

// this is for the client
struct ENGINE_API collision_pair
{
  int id1;
  int id2;
};

void ENGINE_API RAPID_destroy( );
/****************************************************************************/

extern	int RAPID_culling;
extern  int RAPID_num_box_tests;
extern  int RAPID_num_tri_tests;
extern  int RAPID_num_contacts;
extern  struct collision_pair *RAPID_contact;

#endif
