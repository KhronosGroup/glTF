# KHR\_glsl\_view\_projection\_buffer

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF draft 1.0 spec plus KHR_glsl_shader_versions.

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

Uniform buffers were introduced in OpenGL version 3.1 and OpenGL ES version 3.0 and associated shader versions.
The `#version` keyword is used to specify the shader version.
To specify a version 3.1 OpenGL shader, the following is added at the top of the shader.

```C
#version 310
```

To specify a version 3.0 OpenGL ES shader, the following is added at the top of the shader.

```C
#version 300 es
```

The `KHR_glsl_shader_versions` extension is used to specify these different shader versions for OpenGL and OpenGL ES.

Note that compared to GLSL 1.00 glTF shaders, these newer vertex shaders replace the following keywords:

pre 3.0     | 3.0 or later
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

Note that on modern scalar GPUs using `transpose( mat3( u_modelInverse ) )` is no more computationally expensive in
a shader than using an explicit `MODELINVERSETRANSPOSE` uniform. Instead, using an explicit uniform
only introduces overhead to pass the uniform value to the shader.

The extra multiplications are typically not full matrix multiplications but instead matrix-vector multiplications.

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
multiplying the matrices separately in the vertex shader results in noticeably better precision.

```
	"Tightening the Precision of Perspective Rendering"
	Paul Upchurch, Mathieu Desbrun
	Journal of Graphics Tools, Volume 16, Issue 1, 2012
```

## Fragment Shader Requirements

The `#version` keyword is used to specify the shader version.
To specify a version 3.1 OpenGL shader, the following is added at the top of the shader.

```C
#version 310
```

To specify a version 3.0 OpenGL ES shader, the following is added at the top of the shader.

```C
#version 300 es
```

The `KHR_glsl_shader_versions` extension is used to specify these different shader versions for OpenGL and OpenGL ES.

Note that compared to GLSL 1.00 glTF shaders, these newer fragment shaders replace the following keywords and functions:

pre 3.0             | 3.0 or later
--------------------|---------------------
`varying`           | `in`
`texture1D`         | `texture`
`texture2D`		    | `texture`
`texture3D`		    | `texture`
`textureCube`	    | `texture`
`shadow1D`		    | `texture`
`shadow2D`		    | `texture`
`texture1DProj`     | `textureProj`
`texture2DProj`	    | `textureProj`
`texture3DProj`	    | `textureProj`
`shadow1DProj`	    | `textureProj`
`shadow2DProj`	    | `textureProj`
`texture1DLod`      | `textureLod`
`texture2DLod`	    | `textureLod`
`texture3DLod`	    | `textureLod`
`textureCubeLod`    | `textureLod`
`shadow1DLod`	    | `textureLod`
`shadow2DLod`	    | `textureLod`
`texture1DProjLod`  | `textureProjLod`
`texture2DProjLod`	| `textureProjLod`
`texture3DProjLod`	| `textureProjLod`
`shadow1DProjLod`	| `textureProjLod`
`shadow2DProjLod`	| `textureProjLod`

The `gl_fragColor` built-in variable is also no longer available in version 3.0 or later fragment shaders.
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

## Known Implementations

https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_opengl.c
https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_vulkan.c

## Conformance

If this extension is supported, then the application must update a uniform buffer with
the view and projection matrices.
