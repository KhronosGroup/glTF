# KHR\_glsl\_shader\_versions

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF draft 1.0 spec.

## Overview

This extension allows a shader to specify GLSL shaders for different graphics APIs: OpenGL, OpenGL ES and Vulkan.

## Shader Extension

Shaders specifically for OpenGL and OpenGL ES are defined by adding an `extensions` property to a glTF shader, and defining its `KHR_glsl_shader_versions` property.

For example, the following defines a shader with URIs to GLSL source for OpenGL, OpenGL ES and Vulkan:

```javascript
"shaders": {
    "shader1": {
        "extensions": {
            "KHR_glsl_shader_versions" : {
                "uriGlslOpenGL" : "uri1"
                "uriGlslOpenGLES" : "uri2"
                "uriGlslVulkan" : "uri3"
            }
        }
    }
}
```

Usage of the extension must be listed in the `extensionsUsed` array.

```javascript
"extensionsUsed" : [
    "KHR_glsl_shader_versions"
]
```

## Known Implementations

https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_opengl.c
https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_vulkan.c

## Conformance

If this extension is supported, then the application must use the shader specified
for the graphics API used by the application.
