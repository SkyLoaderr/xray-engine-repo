/************************************************************************

  Specialized QSlim output routines.  The bulk of the code here is to
  support multiresolution output formats.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: output.cxx,v 1.4 1999/10/18 15:55:10 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include "qslim.h"

static
void output_ivrml(ostream& out, bool vrml=false)
{
    unsigned int i;

    if( vrml )
	out << "#VRML V1.0 ascii" << endl;
    else
	out << "#Inventor V2.0 ascii" << endl;

    out << "Separator {" << endl
         << "Coordinate3 {" << endl
         << "point [" << endl;

    for(i=0; i<m->vert_count(); i++)
	if( m->vertex_is_valid(i) )
	    out << "   " << m->vertex(i)[0] << " "
		<< m->vertex(i)[1] << " "
		<< m->vertex(i)[2] << "," << endl;

    out << "]"<< endl << "}" << endl;
    out << "IndexedFaceSet {" << endl
         << "coordIndex [" << endl;

    for(i=0; i<m->face_count(); i++)
	if( m->face_is_valid(i) )
	    out << "   "
		<< m->face(i)[0] << ", "
		<< m->face(i)[1] << ", "
		<< m->face(i)[2] << ", "
		<< "-1," << endl;

    out << "]}}" << endl;
}

void output_iv(ostream& out) { output_ivrml(out, false); }
void output_vrml(ostream& out) { output_ivrml(out, true); }

void output_regressive_mmf(ostream& out)
{
    if( !history ) return;

    out << "set delta_encoding 1" << endl;

    for(unsigned int i=0; i<history->length(); i++)
    {
        MxPairContraction& conx = (*history)[i];

	// Output the basic contraction record
        out << "v% " << conx.v1+1 << " " << conx.v2+1 << " "
            << conx.dv1[X] << " " << conx.dv1[Y] << " " << conx.dv1[Z]
            << endl;

        // Output the faces that are being removed
        for(unsigned int j=0; j<conx.dead_faces.length(); j++)
            out << "f- " << conx.dead_faces(j)+1 << endl;
    }
}

void output_regressive_log(ostream& out)
{
    if( !history ) return;

    for(unsigned int i=0; i<history->length(); i++)
    {
        MxPairContraction& conx = (*history)[i];

	// Output the basic contraction record
        out << "v% " << conx.v1+1 << " " << conx.v2+1 << " "
            << conx.dv1[X] << " " << conx.dv1[Y] << " " << conx.dv1[Z];

        // Output the faces that are being removed
        for(unsigned int j=0; j<conx.dead_faces.length(); j++)
            out << " " << conx.dead_faces(j)+1;

        // Output the faces that are being reshaped
        out << " &";
        for(unsigned int k=0; k<conx.delta_faces.length(); k++)
            out << " " << conx.delta_faces(k)+1;

        out << endl;
    }
}

void output_progressive_pm(ostream& out)
{
    if( !history ) return;

    MxBlock<MxVertexID> vmap(m->vert_count());  // Maps old VIDs to new VIDs
    MxBlock<MxFaceID> fmap(m->face_count());    // Maps old FIDs to new FIDs
    unsigned int i,k;

    MxVertexID next_vert = 0;
    MxFaceID   next_face = 0;

    ////////////////////////////////////////////////////////////////////////
    //
    // Output base mesh
    //
    for(i=0; i<m->vert_count(); i++)
	if( m->vertex_is_valid(i) )
	{
	    vmap(i) = next_vert++;
	    out << m->vertex(i) << endl;
	}
    
    for(i=0; i<m->face_count(); i++)
	if( m->face_is_valid(i) )
	{
	    fmap(i) = next_face++;
	    VID v1 = vmap(m->face(i)(0));
	    VID v2 = vmap(m->face(i)(1));
	    VID v3 = vmap(m->face(i)(2));

	    out << "f " << v1+1 << " " << v2+1 << " " << v3+1 << endl;
	}

    ////////////////////////////////////////////////////////////////////////
    //
    // Output mesh expansion
    //
    for(i=history->length()-1; i<=history->length(); i--)
    {	
	const MxPairContraction& conx = (*history)[i];
	SanityCheck( m->vertex_is_valid(conx.v1) );
	SanityCheck( !m->vertex_is_valid(conx.v2) );

	out << "v* " << vmap(conx.v1) + 1;
	out << "  "<<conx.dv1[X]<<" "<<conx.dv1[Y]<<" "<<conx.dv1[Z];
	out << "  "<<conx.dv2[X]<<" "<<conx.dv2[Y]<<" "<<conx.dv2[Z];
	out << " ";

	// Output new faces
	for(k=0; k<conx.dead_faces.length(); k++)
	{
	    FID fk = conx.dead_faces(k);
	    VID vk = m->face(fk).opposite_vertex(conx.v1, conx.v2);
 	    SanityCheck( m->vertex_is_valid(vk) );

 	    out << " ";
	    if( !m->face(fk).is_inorder(vk, conx.v1) ) out << "-";
 	    out << vmap(vk)+1;

	    fmap(conx.dead_faces(k)) = next_face++;
	    m->face_mark_valid(conx.dead_faces(k));
	}

	// Output delta faces
	out << " &";
	for(k=0; k<conx.delta_faces.length(); k++)
	{
	    out << " ";
	    FID fk = conx.delta_faces(k);
	    assert(m->face_is_valid(fk));
	    out << " " << fmap(fk)+1;
	}

	vmap(conx.v2) = next_vert++;
	m->vertex_mark_valid(conx.v2);
	out << endl;
    }
}
