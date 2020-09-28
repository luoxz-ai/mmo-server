#ifndef __Ray_H_
#define __Ray_H_

#include "AxisAlignedBox.h"
#include "Vector3.h"
#include "export_lua.h"
/** Representation of a ray in space, i.e. a line with an origin and direction. */
export_lua class Ray
{
protected:
    Vector3 mOrigin;
    Vector3 mDirection;

public:
    export_lua Ray()
        : mOrigin(Vector3::ZERO())
        , mDirection(Vector3::UNIT_Z())
    {
    }
    export_lua Ray(const Vector3& origin, const Vector3& direction)
        : mOrigin(origin)
        , mDirection(direction)
    {
    }

    /** Sets the origin of the ray. */
    export_lua void setOrigin(const Vector3& origin) { mOrigin = origin; }
    /** Gets the origin of the ray. */
    export_lua const Vector3& getOrigin(void) const { return mOrigin; }

    /** Sets the direction of the ray. */
    export_lua void setDirection(const Vector3& dir) { mDirection = dir; }
    /** Gets the direction of the ray. */
    export_lua const Vector3& getDirection(void) const { return mDirection; }

    /** Gets the position of a point t units along the ray. */
    export_lua Vector3 getPoint(float t) const { return Vector3(mOrigin + (mDirection * t)); }

    /** Gets the position of a point t units along the ray. */
    export_lua Vector3 operator*(float t) const { return getPoint(t); }
};

#endif