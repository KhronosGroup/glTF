# KHR\_glsl\_view\_projection\_buffer

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the [glTF 1.0 specification](#references) plus [KHR_glsl_shader_versions](#references).

## Overview

This extension deprecates a number of uniform semantics and defines an new uniform semantic
to enable a uniform buffer with the view and projection matrices and their inverses.

This extension can significantly improve the efficiency of draw call submissions.
The view-projection uniform buffer has to be updated only once per frame and requires only
a single bind point per draw call, as opposed to updating a number of matrix uniforms per
draw call which may come with significant overhead.

## Extension

Usage of the extension must be listed in the `extensionsUsed` array.

```javascript
"extensionsUsed" : [
    "KHR_glsl_shader_versions",
    "KHR_glsl_view_projection_buffer"
]
```

This extension introduces a new uniform semantic `VIEWPROJECTIONBUFFER`.

## Vertex Shader Requirements

Uniform buffers were introduced in OpenGL version 3.1 and OpenGL ES version 3.0  (GLSL 1.4 and GLSL 1.3 respectively).
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

Note that compared to GLSL 1.0 ES glTF shaders, these newer vertex shaders replace the following keywords:

GLSL 1.0 ES | GLSL 1.3 or later
------------|---------------------
`attribute` | `in`
`varying`   | `out`

The `VIEW`, `VIEWINVERSE`, `PROJECTION` and `PROJECTIONINVERSE` uniform semantics are placed in a uniform buffer:

```C
uniform viewProjectionUniformBuffer     // VIEWPROJECTIONBUFFER
{
	mat4 u_view;
	mat4 u_viewInverse;
	mat4 u_projection;
	mat4 u_projectionInverse;
};
```

A new uniform semantic `VIEWPROJECTIONBUFFER` is introduced to identify this uniform buffer.

The `LOCAL`, `MODEL` and `MODELINVERSE` semantic uniforms are left unchanged as regular uniforms.

```C
uniform mat4 u_local;         // LOCAL
uniform mat4 u_model;         // MODEL
uniform mat4 u_modelInverse;  // MODELINVERSE
```

The following uniform semantics are not allowed inside the vertex shader code and are replaced as follows:

Semantic uniform             | Replacement
-----------------------------|-------------------------------------
`MODELINVERSETRANSPOSE`      | `transpose( mat3( u_modelInverse ) )`
`MODELVIEW`                  | `u_view * u_model`
`MODELVIEWINVERSE`           | `u_viewInverse * u_modelInverse`
`MODELVIEWINVERSETRANSPOSE`  | `transpose( mat3( u_viewInverse ) ) * transpose( mat3( u_modelInverse ) )`
`MODELVIEWPROJECTION`        | `u_projection * u_view * u_model`
`MODELVIEWPROJECTIONINVERSE` | `u_projectionInverse * u_viewInverse * u_modelInverse`

Note that on modern scalar GPUs, using `transpose( mat3( u_modelInverse ) )` is no more computationally expensive in
a shader than using an explicit `MODELINVERSETRANSPOSE` uniform. Using an explicit uniform only introduces overhead
to pass the uniform value to the shader.

The extra multiplications are typically not full matrix-matrix multiplications but instead matrix-vector multiplications.

```C
in vec3 a_position;

void main( void )
{
    gl_Position = ( u_projection * ( u_view * ( u_model * vec4( a_position, 1.0 ) ) ) );
}
```

Instead of two matrix-matrix multiplications and one matrix-vector multiplication (144 multiplications, 108 additions),
the above code uses three matrix-vector multiplications (48 multiplications, 36 additions). The extra math still takes
time but on modern scalar GPUs this is not a bottleneck especially considering the typical ratio between vertices and fragments.

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

The `gl_fragColor` built-in variable is also no longer available in GLSL 1.3 or later fragment shaders.
Instead an explicit `out` parameter must be added.
The color for the first render target can then be assigned to this explicit `out` parameter.

```C
out vec4 fragColor;

void main( void )
{
    fragColor = vec4( 1, 0, 0, 1 );
}
```

None of the following semantic uniforms are allowed in the fragment shader:

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

If this extension is supported, then the application must update a uniform buffer with
the view and projection matrices.

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
