# KHR\_glsl\_shader\_versions

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF draft 1.0 spec.

## Overview

This extension allows a glTF shader to specify GLSL shaders for different graphics APIs: OpenGL, OpenGL ES and Vulkan.

## Shader Extension

Graphics API specific shaders are defined by adding an `extensions` property to a glTF shader, and defining its `KHR_glsl_shader_versions` property.

For example, the following defines a shader with URIs to GLSL source for OpenGL, OpenGL ES and Vulkan:

```javascript
"shaders": {
    "shader1": {
        "extensions": {
            "KHR_glsl_shader_versions" : [
                {
                    "api" : "opengl",
                    "version" : "140",
                    "uri" : "opengl_glsl140.vert"
                },
                {
                    "api" : "opengles",
                    "version" : "130 es",
                    "uri" : "opengles_glsl130es.vert"
                },
                {
                    "api" : "vulkan",
                    "version" : "450",
                    "uri" : "vulkan_glsl450.vert"
                }
            ]
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
