#ifndef BOUNDINGVOLUME_H
#define BOUNDINGVOLUME_H

namespace MiniBall
{
	class ViewFrustum;
	class Plane;

	class BoundingVolume
	{
	public:
		virtual bool visible(const ViewFrustum &viewFrustum) const;
		virtual bool intersect(const Plane &plane) const;
		
		virtual bool frontSide(const Plane &plane) const = 0;
		virtual bool backSide(const Plane &plane) const;
	};
}

#endif   // BOUNDINGVOLUME_H