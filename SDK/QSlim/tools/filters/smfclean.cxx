/************************************************************************

  smfclean

  This filter tries to remove unwanted degeneracies from the input model.
  Supported cleanups:

  	(1) Remove degenerate faces
	(2) Remove unused vertices

  Copyright (C) 1998 Michael Garland, All Rights Reserved.
  
  $Id: smfclean.cxx,v 1.5 1998/08/03 13:15:04 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <MxVector.h>
#include "cmdline.h"

static
void clean(MxStdModel *m)
{
    static double zero[] = {0.0, 0.0, 0.0};
    uint degenerate_face_count = 0;
    uint unused_vertex_count = 0;

    for(MxFaceID f=0; f<m->face_count(); f++)
    {
	double n[3];

	m->compute_face_normal(f, n);
	if( m->face_is_valid(f) && mxv_equal(n, zero, 3) )
	{
	    degenerate_face_count++;
	    m->face_mark_invalid(f);
	}
    }

    for(MxVertexID v=0; v<m->vert_count(); v++)
    {
	if( m->vertex_is_valid(v) && m->neighbors(v).length() == 0 )
	{
	    unused_vertex_count++;
	    m->vertex_mark_invalid(v);
	}
    }

    cerr << "Removal totals:" << endl;
    cerr << "      " << degenerate_face_count << " degenerate faces" << endl;
    cerr << "      " << unused_vertex_count << " unused vertices" << endl;
}

#ifndef FILTER_PROCEDURE
main(int argc, char *argv[])
{
    MxStdModel *m = process_cmdline(argc, argv);

    if( !m )  return 0;

    clean(m);


    output_final_model(m, true);

    delete m;
    return 0;
}
#endif
