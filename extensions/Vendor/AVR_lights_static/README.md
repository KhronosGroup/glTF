# AVR_lights_static

## Contributors

* Steven Vergenz, AltspaceVR ([steven@altvr.com](mailto:steven@altvr.com))

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec, and the draft [AVR_texture_offset_tile](../AVR_texture_offset_tile/README.md) extension.

## Overview

As a performance optimization, many game engines support computing ("baking") certain lighting information in advance, and storing it to a texture ("lightmap") for use at runtime. This has the advantage of not only reducing the computational load of rendering the scene, but it also allows the use of more accurate lighting simulations that would normally not be possible, like raytracing. In practice, this is usually used for global illumination. This is where light is virtually bounced around the scene before being recorded, producing realistic shadows and diffuse reflections that are hard to produce with standard realtime techniques.

However, this approach is not always applicable. The downside of precomputing your lighting is that changes to an object's position or material will not be reflected in the lighting on the rest of the scene. So if you move an object, the shadow it cast would stay in place. For this reason, this technique is also called "static" lighting, because it cannot change without rebaking.

This extension allows glTF materials to provide one of these precomputed textures to a consuming application, providing greater realism for a reduced computational cost. The details of how this lightmap is applied will depend on the lighting model in use by the consumer.

## glTF Schema Updates

The `AVR_lights_static` extension may be defined on `node` structures. This data is specified on the node instead of the material or texture because every node with a lightmap is going to need a separate texture from an otherwise identical but differently-placed node.

The extension object may contain the following properties:

| Name         | Type      | Default    | Description
|--------------|-----------|------------|---------------------------------
| `index`      | `glTFid`  | *required* | Texture reference to a lightmap. The referenced texture contains RGB components in sRGB color space. If a fourth component (A) is present, it is ignored.
| `texCoord`   | `integer` | `0`        | The set index of texture's TEXCOORD attribute used for texture coordinate mapping.
| `strength`   | `number`  | `1.0`      | The influence of the lightmap on the final output.
| `extensions` | `object`  |            | If desired, use the `AVR_texture_offset_tile` extension to use a texture atlas as a lightmap.

### JSON Schema

[AVR_lights_static.lightmapTextureInfo.schema.json](schema/AVR_lights_static.lightmapTextureInfo.schema.json)

## Known Implementations

* [UnityGLTF](https://github.com/AltspaceVR/UnityGLTF) (upcoming)

## Resources

* [Explanation of Unity 5's GI formula](http://www.shadercat.com/how-to-explore-unity-5s-shader-system-code-iv-global-illumination-code-overview/)
