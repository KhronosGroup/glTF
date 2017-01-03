# KHR\_multi\_view

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF draft 1.0 spec.

## Overview

This extension defines an additional material property that points to a technique with shaders specifically
written for the OpenGL multi-view extensions GL_OVR_multiview and GL_OVR_multiview2.

The 1.0 glTF spec defines a plethora of semantic uniforms.
Some of these semantic uniforms provide no benefit on modern scalar GPUs and are only time consuming to update as uniforms.
Other semantic uniforms make it hard and/or computationally time consuming to support the OpenGL GL_OVR_multiview and GL_OVR_multiview2 extensions.

This extension defines how to construct valid multi-view shaders with various restrictions on the semantic uniforms that are used.

The [conformance](#conformance) section specifies what an implementation must to do when encountering this extension.

## Material Extension

A special technique used for multi-view is defined by adding an `extensions` property to a glTF material, and defining its `KHR_multi_view` property.

For example, the following defines a material with a technique that can be used for multi-view:

```javascript
"materials": {
    "material1": {
        "extensions": {
            "KHR_multi_view" : {
                "technique" : "technique1"
            }
        }
    }
}
```

## Multi-View Vertex Shader Requirements

The GL_OVR_multiview and GL_OVR_multiview2 extensions require OpenGL 3.0 or OpenGL ES 3.0.
The `#version` keyword is used to specify the shader version.
To specify a version 3.0 OpenGL shader, the following is added to the top of the shader.

```C
#version 300
```

To specify a version 3.0 OpenGL ES shader, the following is added to the top of the shader.

```C
#version 300 es
```

Note that compared to GLSL 1.00 glTF shaders, these newer vertex shaders replace the following keywords:

pre 3.0     | 3.0 or later
------------|---------------------
`attribute` | `in`
`varying`   | `out`

The following is added close to the top of the vertex shader, typically right after specifying the shader version and/or the default precisions.

```C
define NUM_VIEWS 2
define VIEW_ID gl_ViewID_OVR
extension GL_OVR_multiview2 : require
layout( num_views = NUM_VIEWS ) in;
```

The `VIEW`, `VIEWINVERSE`, `PROJECTION` and `PROJECTIONINVERSE` semantic uniforms for all views are placed in a uniform buffer:

```C
uniform viewProjectionUniformBuffer
{
	mat4 VIEW[NUM_VIEWS];
	mat4 VIEWINVERSE[NUM_VIEWS];
	mat4 PROJECTION[NUM_VIEWS];
	mat4 PROJECTIONINVERSE[NUM_VIEWS];
};
```

A new semantic uniform `VIEWPROJECTIONBUFFER` is introduced to identify this uniform buffer.

The following semantic uniforms are left unchanged as regular uniforms.

```C
uniform mat4 LOCAL;
uniform mat4 MODEL;
uniform mat4 MODELINVERSE;
```

The following semantic uniforms are not allowed inside the vertex shader code and are replaced as follows:

Semantic uniform             | Replacement
-----------------------------|-------------------------------------
`MODELINVERSETRANSPOSE`      | `transpose( mat3( MODELINVERSE ) )`
`MODELVIEW`                  | `VIEW[VIEW_ID] * MODEL`
`MODELVIEWINVERSE`           | `VIEWINVERSE[VIEW_ID] * MODELINVERSE`
`MODELVIEWINVERSETRANSPOSE`  | `transpose( mat3( VIEWINVERSE[VIEW_ID] ) ) * transpose( mat3( MODELINVERSE ) )`
`MODELVIEWPROJECTION`        | `PROJECTION[VIEW_ID] * VIEW[VIEW_ID] * MODEL`
`MODELVIEWPROJECTIONINVERSE` | `PROJECTIONINVERSE[VIEW_ID] * VIEWINVERSE[VIEW_ID] * MODELINVERSE`

Note that on modern scalar GPUs using `transpose( mat3( MODELINVERSE ) )` is no more computationally expensive in
a shader than using an explicit `MODELINVERSETRANSPOSE` uniform. Instead, using an explicit uniform
only introduces overhead to pass the uniform value to the shader.

The extra multiplications are typically not full matrix multiplications but instead matrix-vector multiplications.

```C
in vec3 a_position;

void main( void )
{
    gl_Position = ( PROJECTION[VIEW_ID] * ( VIEW[VIEW_ID] * ( MODEL * vec4( a_position, 1.0 ) ) ) );
}
```

Instead of two matrix-matrix multiplications and one matrix-vector multiplication (144 multiplications, 108 additions),
the above code uses three matrix-vector multiplications (48 multiplications, 36 additions). The extra math still takes
time but on modern scalar GPUs this is not a bottleneck especially considering the typical ratio between vertices and fragments.

All the different pre-multiplied matrix variants could be provided as uniforms to the vertex shader.
However, this would result in 5 * NUM_VIEWS different matrices that would have to be updated per model.
The overhead of updating all these uniforms is far greater than multiplying in the vertex shader.

The view-projection and view-projection-inverse matrices are not pre-multiplied and provided in the uniform buffer because
multiplying the matrices separetely in the vertex shader results in noticeably better precision.

```
	"Tightening the Precision of Perspective Rendering"
	Paul Upchurch, Mathieu Desbrun
	Journal of Graphics Tools, Volume 16, Issue 1, 2012
```

## Multi-View Fragment Shader Requirements

The `#version` keyword is used to specify the shader version.
To specify a version 3.0 OpenGL shader, the following is added to the top of the shader.

```C
#version 300
```

To specify a version 3.0 OpenGL ES shader, the following is added to the top of the shader.

```C
#version 300 es
```

Note that compared to GLSL 1.00 glTF shaders, these newer fragment shaders replace the following keyword:

pre 3.0     | 3.0 or later
------------|---------------------
`varying`   | `in`

The `gl_fragColor` keyword is also no longer available in version 3.0 or later fragment shaders.
Instead an explicit `out` parameter must be added and the color for the first render target can then be assigned to this explicit `out` parameter.

```C
out vec4 fragColor;

void main( void )
{
    fragColor = vec4( 1, 0, 0, 1 );
}
```

None of these semantic uniforms are allowed in a multi-view fragment shader:

- `VIEW`
- `VIEWINVERSE`
- `PROJECTION`
- `PROJECTIONINVERSE`
- `MODELVIEW`
- `MODELVIEWINVERSE`
- `MODELVIEWINVERSETRANSPOSE`
- `MODELVIEWPROJECTION`
- `MODELVIEWPROJECTIONINVERSE`

## Known Implementations

https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_opengl.c
https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_vulkan.c

<a name="conformance"></a>
## Conformance

If this extension is supported, and an application wants to use one of the multi-view extensions,
then the application should use the special multi-view technique.
