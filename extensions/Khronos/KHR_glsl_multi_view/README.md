# KHR\_glsl\_multi\_view

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the [glTF 1.0 specification](#references) plus [KHR_glsl_shader_versions](#references).

## Overview

This extension defines an additional material property that points to a technique with shaders specifically
written for the OpenGL multi-view extension GL_OVR_multiview2. The GL_OVR_multiview extension is not considered
because it is more limited and all drivers that support GL_OVR_multiview also support GL_OVR_multiview2.

The [1.0 glTF specification](#references) defines a plethora of uniform semantics.
Some of these uniform semantics provide no benefit on modern scalar GPUs and are only time consuming to update as uniforms.
Other uniform semantics make it hard and/or very inefficient to support the OpenGL GL_OVR_multiview2 extension.

Similar to the KHR_glsl_multi_view extension, this extension deprecates a number of uniform semantics and defines a
new uniform semantic to enable a uniform buffer with the view and projection matrices and their inverses for all views.
This extension also defines how to construct valid multi-view shaders.

## Material Extension

A special technique used for multi-view is defined by adding an `extensions` property to a glTF material, and defining its `KHR_glsl_multi_view` property.

For example, the following defines a material with a technique that can be used for multi-view:

```javascript
"materials": {
    "material1": {
        "extensions": {
            "KHR_glsl_multi_view" : {
                "technique" : "technique1"
            }
        }
    }
}
```

Usage of the extension must be listed in the `extensionsUsed` array.

```javascript
"extensionsUsed" : [
    "KHR_glsl_shader_versions",
    "KHR_glsl_multi_view"
]
```

This extension introduces a new uniform semantic `VIEWPROJECTIONMULTIVIEWBUFFER`.

## Vertex Shader Requirements

The GL_OVR_multiview2 extension requires OpenGL 3.0 or OpenGL ES 3.0 and
uniform buffers were introduced in OpenGL version 3.1 and OpenGL ES version 3.0 (GLSL 1.4 and GLSL 1.3 respectively).
The `#version` keyword is used to specify the shader version associated with the required OpenGL or OpenGL ES versions.
To specify a version GLSL 1.4 OpenGL shader, the following is added at the top of the shader.

```C
#version 140
```

To specify a version GLSL 1.3 OpenGL ES shader, the following is added at the top of the shader.

```C
#version 130 es
```

The `KHR_glsl_shader_versions` extension is used to specify these different shader versions for OpenGL and OpenGL ES.

Note that compared to GLSL 1.0 ES glTF shaders, these newer vertex shaders replace the following keywords:

GLSL 1.0 ES | GLSL 1.3 or later
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
uniform viewProjectionMultiViewUniformBuffer     // VIEWPROJECTIONMULTIVIEWBUFFER
{
	mat4 u_view[NUM_VIEWS];
	mat4 u_viewInverse[NUM_VIEWS];
	mat4 u_projection[NUM_VIEWS];
	mat4 u_projectionInverse[NUM_VIEWS];
};
```

A new uniform semantic `VIEWPROJECTIONMULTIVIEWBUFFER` is introduced to identify this uniform buffer.

The `LOCAL`, `MODEL` and `MODELINVERSE` uniform semantics are left unchanged as regular uniforms.

```C
uniform mat4 u_local;         // LOCAL
uniform mat4 u_model;         // MODEL
uniform mat4 u_modelInverse;  // MODELINVERSE
```

The following uniforms semantic are not allowed inside the vertex shader code and are replaced as follows:

Semantic uniform             | Replacement
-----------------------------|-------------------------------------
`MODELINVERSETRANSPOSE`      | `transpose( mat3( u_modelInverse ) )`
`MODELVIEW`                  | `u_view[VIEW_ID] * u_model`
`MODELVIEWINVERSE`           | `u_viewInverse[VIEW_ID] * u_modelInverse`
`MODELVIEWINVERSETRANSPOSE`  | `transpose( mat3( u_viewInverse[VIEW_ID] ) ) * transpose( mat3( u_modelInverse ) )`
`MODELVIEWPROJECTION`        | `u_projection[VIEW_ID] * u_view[VIEW_ID] * u_model`
`MODELVIEWPROJECTIONINVERSE` | `u_projectionInverse[VIEW_ID] * u_viewInverse[VIEW_ID] * u_modelInverse`

Note that on modern scalar GPUs using `transpose( mat3( u_modelInverse ) )` is no more computationally expensive in
a shader than using an explicit `MODELINVERSETRANSPOSE` uniform. Using an explicit uniform only introduces
overhead to pass the uniform value to the shader.

The extra multiplications are typically not full matrix-matrix multiplications but instead matrix-vector multiplications.

```C
in vec3 a_position;

void main( void )
{
    gl_Position = ( u_projection[VIEW_ID] * ( u_view[VIEW_ID] * ( u_model * vec4( a_position, 1.0 ) ) ) );
}
```

Instead of two matrix-matrix multiplications and one matrix-vector multiplication (144 multiplications, 108 additions),
the above code uses three matrix-vector multiplications (48 multiplications, 36 additions). The extra math still takes
time but on modern scalar GPUs this is not a bottleneck especially considering the typical ratio between vertices and fragments.

All the different pre-multiplied matrix variants could be provided as uniforms to the vertex shader.
However, this would result in 5 * NUM_VIEWS different matrices that would have to be updated per model.
The overhead of updating all these uniforms is far greater than multiplying in the vertex shader.

The view-projection and view-projection-inverse matrices are not pre-multiplied and provided in the uniform buffer because
multiplying the matrices separately in the vertex shader results in noticeably better precision as described by
Paul Upchurch, Mathieu Desbrun in [Tightening the Precision of Perspective Rendering](#references).

## Fragment Shader Requirements

The `#version` keyword is used to specify the shader version.
To specify a version GLSL 1.4 OpenGL shader, the following is added at the top of the shader.

```C
#version 140
```

To specify a version GLSL 1.3 OpenGL ES shader, the following is added at the top of the shader.

```C
#version 130 es
```

The `KHR_glsl_shader_versions` extension is used to specify these different shader versions for OpenGL and OpenGL ES.

Note that compared to GLSL 1.0 ES glTF shaders, these newer fragment shaders replace the following keywords and functions:

GLSL 1.0 ES         | GLSL 1.3 or later
--------------------|---------------------
`varying`           | `in`
`texture2D`		    | `texture`
`texture2DProj`	    | `textureProj`
`texture2DLod`	    | `textureLod`
`texture2DProjLod`	| `textureProjLod`
`textureCube`	    | `texture`
`textureCubeLod`    | `textureLod`

The `gl_fragColor` built-in variable is also no longer available in version GLSL 1.3 or later fragment shaders.
Instead an explicit `out` parameter must be added.
The color for the first render target can then be assigned to this explicit `out` parameter.

```C
out vec4 fragColor;

void main( void )
{
    fragColor = vec4( 1, 0, 0, 1 );
}
```

None of the following semantic uniforms are allowed in a multi-view fragment shader:

- `LOCAL`
- `MODEL`
- `MODELINVERSE`
- `MODELINVERSETRANSPOSE`
- `VIEW`
- `VIEWINVERSE`
- `PROJECTION`
- `PROJECTIONINVERSE`
- `MODELVIEW`
- `MODELVIEWINVERSE`
- `MODELVIEWINVERSETRANSPOSE`
- `MODELVIEWPROJECTION`
- `MODELVIEWPROJECTIONINVERSE`

## Conformance

If this extension is supported, and an application wants to use the GL_OVR_multiview2 OpenGL extension,
then the application should use the special multi-view technique specified by this glTF extension.

## Known Implementations

- Khronos Asynchronous Time Warp Sample for OpenGL<br/>
  https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_opengl.c
- Khronos Asynchronous Time Warp Sample for Vulkan<br/>
  https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_vulkan.c

<a name="references"></a>
## References

- The glTF 1.0 specification<br/>
  https://github.com/KhronosGroup/glTF/tree/master/specification/1.0
- The glTF 1.0 extension registsry<br/>
  https://github.com/KhronosGroup/glTF/tree/master/extensions
- GLSL 1.0 ES used by glTF<br/>
  https://www.khronos.org/files/opengles_shading_language.pdf
- Tightening the Precision of Perspective Rendering<br/>
  Paul Upchurch, Mathieu Desbrun<br/>
  Journal of Graphics Tools, Volume 16, Issue 1, 2012<br/>
  http://www.geometry.caltech.edu/pubs/UD12.pdf
