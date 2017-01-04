# KHR\_glsl\_joint\_buffer

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF draft 1.0 spec plus KHR_glsl_shader_versions.

## Overview

This extension defines an new uniform semantic to enable joint uniform buffers.

The glTF 1.0 spec only allows joint arrays.
Not only does this significantly limit the number of joints per skin,
updating a joint array is also not as efficient as updating a uniform buffer on modern GPUs.

## Extension

Usage of the extension must be listed in the `extensionsUsed` array.

```javascript
"extensionsUsed" : [
    "KHR_glsl_shader_versions",
    "KHR_glsl_joint_buffer"
]
```

This extension introduces a new uniform semantic `JOINTBUFFER`.

## Joint Buffer Vertex Shader Requirements

Uniform buffers were introduced in OpenGL version 3.1 and OpenGL ES version 3.0 and associated shader versions.
The `#version` keyword is used to specify the shader version.
To specify a version 3.1 OpenGL shader, the following is added to the top of the shader.

```C
#version 310
```

To specify a version 3.0 OpenGL ES shader, the following is added to the top of the shader.

```C
#version 300 es
```

The `KHR_glsl_shader_versions` extension is used to specify these different shader versions for OpenGL and OpenGL ES.

Note that compared to GLSL 1.00 glTF shaders, these newer vertex shaders replace the following keywords:

pre 3.0     | 3.0 or later
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

## Known Implementations

https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_opengl.c
https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_vulkan.c

## Conformance

If this extension is supported, then the application must update a uniform buffer to
specify joint matrices for a vertex shader.
