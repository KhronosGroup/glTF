# KHR\_glsl\_joint\_buffer

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the [glTF 1.0 specification](#references) plus [KHR_glsl_shader_versions](#references).

## Overview

This extension defines an new uniform semantic to enable joint uniform buffers.

The [glTF 1.0 specification](#references) only allows arrays with joint matrices which are stored as regular uniform vectors.
The [GLSL 1.0 ES specification](#references) requires a minimimum of 128 uniform vectors,
but there is no guarantee a GPU will support more than 128 uniform vectors.
Even if no other uniforms are used at all, a limit of 128 uniform vectors would limit the
number of 4x4 joint matrices to just 32.
Not only does this significantly limit the number of joints per skin,
updating a joint array is also not as efficient as updating a uniform buffer on modern GPUs.
So even when using a GPU that supports many more uniform vectors, it is more efficient to
updated a single uniform buffer per skin, in particular if the same skin is used for multiple meshes.

## Extension

Usage of the extension must be listed in the glTF `extensionsUsed` array.

```javascript
"extensionsUsed" : [
    "KHR_glsl_shader_versions",
    "KHR_glsl_joint_buffer"
]
```

This extension introduces a new uniform semantic `JOINTBUFFER`.

## Vertex Shader Requirements

Uniform buffers were introduced in OpenGL version 3.1 and OpenGL ES version 3.0 (GLSL 1.4 and GLSL 1.3 respectively).
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

A typical vertex shader using a joint uniform buffer looks like the following:

```C
in vec4 a_joint;
in vec4 a_weight;

uniform jointUniformBuffer   // JOINTBUFFER
{
	mat4 u_jointMat[256];
};

void main( void )
{
    mat4 skinMat = a_weight.x * u_jointMat[int( a_joint.x )] +
                   a_weight.y * u_jointMat[int( a_joint.y )] +
                   a_weight.z * u_jointMat[int( a_joint.z )] +
                   a_weight.w * u_jointMat[int( a_joint.w )];
    ...
}
```

## Conformance

If this extension is supported, then the application must update a uniform buffer to
specify joint matrices for a vertex shader.

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
- GLSL 1.0 ES used by glTF 1.0<br/>
  https://www.khronos.org/files/opengles_shading_language.pdf
