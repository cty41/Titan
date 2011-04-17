#ifndef __PlaneBoundedVolume_H_
#define __PlaneBoundedVolume_H_

// Precompiler options
#include "TiPrerequisites.h"
#include "TitanAABB.h"
#include "TitanSphere.h"
#include "TitanMath.h"
#include "TitanPlane.h"

namespace Titan 
{
	/** Represents a convex volume bounded by planes.
    */
    class _DllExport PlaneBoundedVolume
    {
    public:
        typedef vector<Plane>::type PlaneList;
        /// Publicly accessible plane list, you can modify this direct
        PlaneList planes;
        Plane::Side outside;

        PlaneBoundedVolume() :outside(Plane::NEGATIVE_SIDE) {}
        /** Constructor, determines which side is deemed to be 'outside' */
        PlaneBoundedVolume(Plane::Side theOutside) 
            : outside(theOutside) {}

        /** Intersection test with AABB
        @remarks May return false positives but will never miss an intersection.
        */
        inline bool intersects(const AABB& box) const
        {
            if (box.isNull()) return false;
            if (box.isInfinite()) return true;

            // Get centre of the box
            Vector3 centre = box.getCenter();
            // Get the half-size of the box
            Vector3 halfSize = box.getHalfSize();
            
            PlaneList::const_iterator i, iend;
            iend = planes.end();
            for (i = planes.begin(); i != iend; ++i)
            {
                const Plane& plane = *i;

                Plane::Side side = plane.getSide(centre, halfSize);
                if (side == outside)
                {
                    // Found a splitting plane therefore return not intersecting
                    return false;
                }
            }

            // couldn't find a splitting plane, assume intersecting
            return true;

        }
        /** Intersection test with Sphere
        @remarks May return false positives but will never miss an intersection.
        */
        inline bool intersects(const Sphere& sphere) const
        {
            PlaneList::const_iterator i, iend;
            iend = planes.end();
            for (i = planes.begin(); i != iend; ++i)
            {
                const Plane& plane = *i;

                // Test which side of the plane the sphere is
                float d = plane.getDistance(sphere.getCenter());
                // Negate d if planes point inwards
                if (outside == Plane::NEGATIVE_SIDE) d = -d;

                if ( (d - sphere.getRadius()) > 0)
                    return false;
            }

            return true;

        }

        /** Intersection test with a Ray
        @returns std::pair of hit (bool) and distance
        @remarks May return false positives but will never miss an intersection.
        */
        inline std::pair<bool, float> intersects(const Ray& ray)
        {
            return Math::intersects(ray, planes, outside == Plane::POSITIVE_SIDE);
        }

    };

    typedef vector<PlaneBoundedVolume>::type PlaneBoundedVolumeList;

}

#endif

