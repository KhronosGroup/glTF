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
                    "extensions" : [
						{
							"name" : "GL_OVR_multiview2",
							"behavior" : "require"
						}
                    ],
					"gltfExtension" : [
						"KHR_glsl_multi_view"
					]
                    "uri" : "opengles_glsl130es.vert"
                },
                {
                    "api" : "vulkan",
                    "version" : "440 core",
                    "extensions" : [
						{
							"name" : "GL_EXT_shader_io_blocks",
							"behavior" : "enable"
						},
						{
							"name" : "GL_ARB_enhanced_layouts",
							"behavior" : "enable"
						}
                    ],
					"gltfExtension" : [
						"KHR_glsl_view_projection_buffer",
						"KHR_glsl_layout_vulkan"
					]
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

## Conformance

If this extension is supported, then the application searches through the available
shaders to find the most up to date shader version for the graphics API used by the
application.

## Known Implementations

- Khronos Asynchronous Time Warp Sample for OpenGL<br/>
  https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_opengl.c
- Khronos Asynchronous Time Warp Sample for Vulkan<br/>
  https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/apps/atw/atw_vulkan.c

## References

- GLSL 1.0 ES used by glTF<br/>
  https://www.khronos.org/files/opengles_shading_language.pdf
