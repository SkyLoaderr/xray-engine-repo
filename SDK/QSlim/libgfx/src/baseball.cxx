/************************************************************************

  Common code for ball-based rotation controllers.

  $Id: baseball.cxx,v 1.2 2003/08/13 15:40:32 garland Exp $

 ************************************************************************/

#include <gfx/gfx.h>
#include <gfx/gl.h>
#include <gfx/baseball.h>

Baseball::Baseball()
{
    curquat = Quat::ident();

    trans=0.0;
    ctr=0.0;
    radius=1;
}

void Baseball::apply_transform()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslated(trans[0], trans[1], trans[2]);
    glTranslated(ctr[0], ctr[1], ctr[2]);

    const Mat4 M=unit_quat_to_matrix(curquat);
    glMultMatrixd(M);

    glTranslated(-ctr[0], -ctr[1], -ctr[2]);
}

void Baseball::unapply_transform()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Baseball::write(std::ostream& out)
{
    out << "baseball ";
    out << curquat << " " << trans << " " << ctr << " " << radius << std::endl;
}

void Baseball::read(std::istream& in)
{
    std::string name;

//.    in >> name;
//.    in >> curquat >> trans >> ctr >> radius;
}
