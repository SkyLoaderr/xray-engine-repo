#ifndef MXFITFRAME_INCLUDED // -*- C++ -*-
#define MXFITFRAME_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxFitFrame

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxFitFrame.h,v 1.9 1999/05/12 17:58:57 garland Exp $

 ************************************************************************/

#include "MxFrame.h"
#include "MxQMetric3.h"

#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif

class MxFitFrame : public MxFrame
{
private:
    double vmin[3], vmax[3];	// Bounding box in local frame
    double axis_evals[3];	// Eigenvalues from prin. component analysis

    double normal_accum[3], avg_normal[3];

public:
    MxFitFrame();
    MxFitFrame(double *c, double *u0, double *u1, double *u2);
    MxFitFrame(const MxQuadric3&, uint nverts);

    double       *min()       { return vmin; }
    const double *min() const { return vmin; }
    double       *max()       { return vmax; }
    const double *max() const { return vmax; }

    double       *normal()       { return avg_normal; }
    const double *normal() const { return avg_normal; }
    double       *total_normal()       { return normal_accum; }
    const double *total_normal() const { return normal_accum; }

    void set_normal(const double *);
    void add_normal(const double *);
    void clear_normal();
    void finalize_normal();

    bool compute_frame(const MxQuadric3&, uint nverts);
    bool compute_frame(const float *, const float *, const float *);
    void reset_bounds();
    void accumulate_bounds(const MxFitFrame&);
    void accumulate_bounds(const float *, uint npoint=1);
    void accumulate_bounds(const double *, uint npoint=1);

    bool check_intersection(const MxFitFrame&) const;
    void worldspace_corners(Vec3 *) const;

    double compute_closest_distance(const double *v, double *w=NULL) const;
    double compute_closest_sqrdistance(const double *v, double *w=NULL) const;
    double compute_max_distance(const double *v) const;
    double compute_max_sqrdistance(const double *v) const;
    void compute_distance_bounds(const MxFitFrame&,double *,double *) const;
};

extern void mx_draw_fitframe(const MxFitFrame&, float rgbscale=1.0f);

// MXFITFRAME_INCLUDED
#endif
