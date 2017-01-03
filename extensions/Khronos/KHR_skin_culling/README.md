# KHR\_skin\_culling

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF draft 1.0 spec.

## Overview

The 1.0 glTF spec does not define a means to efficiently cull skinned meshes.

This extension defines additional skin properties that allow efficient culling of skinned meshes.

## Skin Extension

The data used for skin culling is defined by adding an `extensions` property to a glTF skin, and defining its `KHR_skin_culling` property.

For example, the following defines a skin with the data that is used for skin culling:

```javascript
"skins": {
    "skin1": {
        "extensions": {
            "KHR_skin_culling" : {
                "jointGeometryMins" : "accessor1"
                "jointGeometryMaxs" : "accessor2"
            }
        }
    }
}
```

Both accessors must have "componentType" : 5126 and "type" : "VEC3".

The "jointGeometryMins" and "jointGeometryMaxs" store for each joint of the skin the local space mins/maxs of all vertices that are influenced by the joint of the skin.

## Skin Culling

Using affine joint transformations, it is not particularly computationally intensive to calculate the global space mins/maxs for each joint.

```C
typedef struct ksVector3f
{
    float x;
    float y;
    float z;
} ksVector3f;

// Column-major 4x4 matrix
typedef struct ksMatrix4x4f
{
    float m[4][4];
} ksMatrix4x4f;

static bool ksMatrix4x4f_IsAffine(
                const ksMatrix4x4f * matrix,
                const float epsilon )
{
    return  fabsf( matrix->m[0][3] ) <= epsilon &&
            fabsf( matrix->m[1][3] ) <= epsilon &&
            fabsf( matrix->m[2][3] ) <= epsilon &&
            fabsf( matrix->m[3][3] - 1.0f ) <= epsilon;
}

static void ksMatrix4x4f_TransformBounds(
                ksVector3f * resultMins,
                ksVector3f * resultMaxs,
                const ksMatrix4x4f * matrix,
                const ksVector3f * mins,
                const ksVector3f * maxs )
{
    assert( ksMatrix4x4f_IsAffine( matrix, 1e-4f ) );

    const ksVector3f center =
    {
        ( mins->x + maxs->x ) * 0.5f,
        ( mins->y + maxs->y ) * 0.5f,
        ( mins->z + maxs->z ) * 0.5f
    };
    const ksVector3f extents =
    {
        maxs->x - center.x,
        maxs->y - center.y,
        maxs->z - center.z
    };
    const ksVector3f newCenter =
    {
        matrix->m[0][0] * center.x +
            matrix->m[1][0] * center.y +
                matrix->m[2][0] * center.z + matrix->m[3][0],
        matrix->m[0][1] * center.x +
            matrix->m[1][1] * center.y +
                matrix->m[2][1] * center.z + matrix->m[3][1],
        matrix->m[0][2] * center.x +
            matrix->m[1][2] * center.y +
                matrix->m[2][2] * center.z + matrix->m[3][2]
    };
    const ksVector3f newExtents =
    {
        fabsf( extents.x * matrix->m[0][0] ) +
            fabsf( extents.y * matrix->m[1][0] ) +
                fabsf( extents.z * matrix->m[2][0] ),
        fabsf( extents.x * matrix->m[0][1] ) +
            fabsf( extents.y * matrix->m[1][1] ) +
                fabsf( extents.z * matrix->m[2][1] ),
        fabsf( extents.x * matrix->m[0][2] ) +
            fabsf( extents.y * matrix->m[1][2] ) +
                fabsf( extents.z * matrix->m[2][2] )
    };

    resultMins.x = newCenter.x - newExtents.x;
    resultMins.y = newCenter.y - newExtents.y;
    resultMins.z = newCenter.z - newExtents.z;

    resultMaxs.x = newCenter.x + newExtents.x;
    resultMaxs.y = newCenter.y + newExtents.y;
    resultMaxs.z = newCenter.z + newExtents.z;
}
```

Calculating the full bounds of the skin is straight forward.

```C
ksVector3f skinMins = { FLT_MAX, FLT_MAX, FLT_MAX };
ksVector3f skinMaxs = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

for ( int jointIndex = 0; jointIndex < skin->jointCount; jointIndex++ )
{
    ksVector3f jointMins;
    ksVector3f jointMaxs;

    ksMatrix4x4f_TransformBounds( &jointMins, &jointMaxs,
        &skin->joints[jointIndex].node->globalTransform,
        &skin->jointGeometryMins[jointIndex],
        &skin->jointGeometryMaxs[jointIndex] );

    skinMins.x = ( skinMins.x < jointMins.x ) ? skinMins.x : jointMins.x;
    skinMins.y = ( skinMins.y < jointMins.y ) ? skinMins.y : jointMins.y;
    skinMins.z = ( skinMins.z < jointMins.z ) ? skinMins.z : jointMins.z;

    skinMaxs.x = ( skinMaxs.x > jointMaxs.x ) ? skinMaxs.x : jointMaxs.x;
    skinMaxs.y = ( skinMaxs.y > jointMaxs.y ) ? skinMaxs.y : jointMaxs.y;
    skinMaxs.z = ( skinMaxs.z > jointMaxs.z ) ? skinMaxs.z : jointMaxs.z;
}
```

Note that if the same skin is used for multiple meshes, then all these meshes will be culled with the same bounds.
This also means that the "jointGeometryMins" and "jointGeometryMaxs" need to contain the local space mins/maxs
of all vertices, from all meshes, that are influenced by the joint of the skin.

## Known Implementations

https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_opengl.c
https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_vulkan.c

## Conformance

If this extension is supported, it is up to the application to use the extension or not. 
