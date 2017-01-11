# KHR\_glsl\_shader\_versions

## Contributors

* Johannes van Waveren, Oculus VR, <mailto:janpaul.vanwaveren@oculus.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the [glTF 1.0 specification](#references).

## Overview

The [glTF 1.0 specification](#references) only allows [GLSL 1.0 ES](#references) shaders.
While these shaders work with both OpenGL and OpenGL ES, the GLSL 1.0 ES specification
is very limiting and does not expose many of the features that enable efficient rendering
on modern GPUs.

This extension allows a glTF shader to specify multiple GLSL shaders for different graphics APIs: OpenGL, OpenGL ES and Vulkan,
allowing the application to choose the most appropriate shader for the platform and use case.

## Shader Extension

Graphics API specific shaders are defined by adding an `extensions` property to a glTF shader, and defining its `KHR_glsl_shader_versions` property.

For example, the following defines a glTF shader with URIs to GLSL source for OpenGL, OpenGL ES and Vulkan:

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

<a name="references"></a>
## References

- The glTF 1.0 specification<br/>
  https://github.com/KhronosGroup/glTF/tree/master/specification/1.0
- The glTF 1.0 extension registsry<br/>
  https://github.com/KhronosGroup/glTF/tree/master/extensions
- GLSL 1.0 ES used by glTF<br/>
  https://www.khronos.org/files/opengles_shading_language.pdf
