# KHR_environment_sky

## Contributors

- Aaron Franke, Godot Engine.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

Optionally depends on `KHR_lights_punctual`, which is used to define suns.

## Overview

This extension allows defining the skybox of a glTF asset, and how the sky provides environment lighting. The sky can be a gradient, panorama, physical sky, or plain sky. The sky can be used to provide a background, and optionally provide lighting to the scene.

To add suns to the sky, use the `KHR_lights_punctual` extension to define directional lights. For sky types that display suns, every visible directional light defined with `KHR_lights_punctual` on a node is treated as a sun. Since `KHR_lights_punctual` directional lights point along their node's local -Z axis, the sun appears in the sky in the direction of the node's local +Z axis.

The exact appearance of the sky **MAY** vary between applications.

### Example:

This example defines a panorama sky with an equirectangular texture, and defines a sun by having a directional light aligned with the texture. It is a subset of `epic_blue_sunset_equirectangular.gltf`.

```json
{
    "extensions": {
        "KHR_environment_sky": {
            "skies": [
                {
                    "panorama": {
                        "equirectangular": 0
                    },
                    "type": "panorama"
                }
            ]
        },
        "KHR_lights_punctual": {
            "lights": [{ "type": "directional" }]
        }
    },
    "extensionsUsed": ["KHR_environment_sky", "KHR_lights_punctual"],
    "images": [
        {
            "mimeType": "image/jpeg",
            "name": "all_sky_free_epic_blue_sunset",
            "uri": "epic_blue_sunset/all_sky_free_epic_blue_sunset.jpg"
        }
    ],
    "nodes": [
        { "children": [1], "name": "EpicBlueSunsetEquirectangular" },
        {
            "extensions": { "KHR_lights_punctual": { "light": 0 } },
            "name": "Sun",
            "rotation": [-0.0348995115423885, 0.0, 0.0, 0.999390826500875]
        }
    ],
    "scene": 0,
    "scenes": [{ "nodes": [0] }],
    "textures": [{ "source": 0 }]
}
```

This example defines a gradient sky, black all around, with a red dwarf star huge in the sky thanks to the sun curve and a red directional light. It is a subset of `space_black_gradient.gltf`.

```json
{
    "extensions": {
        "KHR_environment_sky": {
            "skies": [
                {
                    "gradient": {
                        "bottomColor": [0.0001, 0.0001, 0.0001],
                        "horizonColor": [0.0001, 0.0001, 0.0001],
                        "sunAngleMax": 0.25,
                        "sunCurve": 4,
                        "topColor": [0.0001, 0.0001, 0.0001]
                    },
                    "type": "gradient"
                }
            ]
        },
        "KHR_lights_punctual": {
            "lights": [
                {
                    "color": [1, 0.05, 0.05],
                    "intensity": 0.5,
                    "type": "directional"
                }
            ]
        }
    },
    "extensionsUsed": ["KHR_environment_sky", "KHR_lights_punctual"],
    "nodes": [
        { "children": [1], "name": "SpaceBlackGradient" },
        {
            "extensions": { "KHR_lights_punctual": { "light": 0 } },
            "name": "SunDimRed",
            "rotation": [-0.12941067816757, 0.836515365063497, 0.482963555466784, 0.224145320004181]
        }
    ],
    "scene": 0,
    "scenes": [{ "nodes": [0] }]
}
```

## glTF Schema Updates

This extension consists of one new data structure for defining the environment sky, `"KHR_environment_sky"`. The key `"KHR_environment_sky"` can be added to the document-level `"extensions"` to define the environment sky of the glTF asset.

The extension **MUST** also be added to the glTF's `extensionsUsed` array and because it is optional, it does not need to be added to the `extensionsRequired` array.

### Sky Property Summary

These properties are defined on each sky. Multiple skies may be specified in the `"skies"` array.

|                            | Type        | Description                                                                        | Default value          |
| -------------------------- | ----------- | ---------------------------------------------------------------------------------- | ---------------------- |
| **ambientLightColor**      | `number[3]` | The color of the ambient light, if sky contribution is less than 1.0.              | `[0.0, 0.0, 0.0]`      |
| **ambientSkyContribution** | `number`    | The ratio of contribution the sky makes to the ambient light.                      | 1.0                    |
| **rotation**               | `number[4]` | The unit quaternion representing the sky's rotation.                               | `[0.0, 0.0, 0.0, 1.0]` |
| **type**                   | `string`    | The type of sky as a string (`"gradient"`, `"panorama"`, `"physical"`, `"plain"`). | Required, no default   |
| **gradient**               | `object`    | If the type is `"gradient"`, the gradient sky properties.                          | `null`                 |
| **panorama**               | `object`    | If the type is `"panorama"`, the panorama sky properties.                          | `null`                 |
| **physical**               | `object`    | If the type is `"physical"`, the physical sky properties.                          | `null`                 |
| **plain**                  | `object`    | If the type is `"plain"`, the plain color sky properties.                          | `null`                 |

#### Ambient Light Color

The `"ambientLightColor"` property is an array of three numbers that defines the color of the ambient light. The default value is `[0.0, 0.0, 0.0]`, which is black.

The ambient light color applies to all sky types. It is a sky-level property, outside of each type's sub-object.

This only has an effect if the `"ambientSkyContribution"` is less than 1.0. As such, aside from just simple dimming behavior, to set a custom ambient light color separate from the sky's color, both properties would need to be specified.

Like all colors in glTF, this is defined in linear color space. The ambient light color may have values greater than 1.0, which indicate high energy, HDR, or overbright ambient light color, but usually, the values are between 0.0 and 1.0.

#### Ambient Sky Contribution

The `"ambientSkyContribution"` property is a number that defines the ratio of ambient light contribution between the ambient color and the sky color. The default value is 1.0, which means that the ambient color is the same as the sky color.

The ambient sky contribution applies to all sky types. It is a sky-level property, outside of each type's sub-object.

The value **MUST** be between 0.0 and 1.0, inclusive. A value of 0.0 means that all ambient light is the ambient color, and no sky color is added. A value of 1.0 means that all ambient light is the sky color, and the `"ambientLightColor"` property is ignored. A value between 0.0 and 1.0 is a mix of the two colors. If `"ambientLightColor"` is not specified, it is black by default, which effectively means that `"ambientSkyContribution"` acts as a dimming multiplier.

> Informative note: Unity only supports 0.0 and 1.0, while Godot supports the full range of values in-between.

#### Rotation

The `"rotation"` property is an array of four numbers representing the unit quaternion for the sky's rotation. The default value is `[0.0, 0.0, 0.0, 1.0]`, which corresponds to no rotation.

The value **MUST** be a unit quaternion within the margin of floating point precision. The rotation follows the standard quaternion convention used by glTF: `[x, y, z, w]`, where `w` is the scalar component.

Not all skies are visually affected by the rotation. For example, a plain color sky will look the same regardless of its rotation, a panorama sky will rotate according to the quaternion, and a gradient sky will not be affected by rotation around its local vertical axis.

#### Type

The `"type"` property is a string enum that defines the type of sky. It **MUST** be defined and has no default value. The possible values are `"gradient"`, `"panorama"`, `"physical"`, and `"plain"`.

Here is a comparison of the types of sky in `KHR_environment_sky` and the corresponding sky types in various engines. This table is informative, not normative:

| Sky Type    | Unity                     | Unreal        | Godot                   | Blender |
| ----------- | ------------------------- | ------------- | ----------------------- | ------- |
| Gradient    | Skybox/Procedural         | Skydome Mesh  | ProceduralSkyMaterial   |         |
| Panorama    | Skybox/Panoramic          | Skydome Mesh  | PanoramaSkyMaterial     |         |
| Physical    | HDRP Physically Based Sky | SkyAtmosphere | PhysicalSkyMaterial     |         |
| Plain Color |                           |               | Background Custom Color |         |

In Unity, only Panorama and Gradient skies are supported in all pipelines. Physical sky requires the High Definition Render Pipeline (HDRP).

In Unreal, only Physical skies are directly supported. Panorama and Gradient skies can be implemented with a skydome mesh; the material needs the "Shading Model" set to "Unlit" and "Is Sky" set to true.

### Gradient Sky Properties

Gradient skies use color gradients to define the sky colors. The `"gradient"` property is an object that contains the gradient sky properties:

|                  | Type        | Description                                                     | Default value        |
| ---------------- | ----------- | --------------------------------------------------------------- | -------------------- |
| **bottomColor**  | `number[3]` | The color of the sky at the bottom, also known as the nadir.    | Required, no default |
| **bottomCurve**  | `number`    | The transition curve between the bottom and the horizon colors. | 0.02                 |
| **horizonColor** | `number[3]` | The color of the sky at the horizon.                            | Required, no default |
| **topColor**     | `number[3]` | The color of the sky at the top, also known as the zenith.      | Required, no default |
| **topCurve**     | `number`    | The transition curve between the top and the horizon colors.    | 0.15                 |
| **sunAngleMax**  | `number`    | The maximum radius angle of the sun(s) in radians.              | 0.5                  |
| **sunCurve**     | `number`    | The transition curve between the sun(s) and the sky colors.     | 0.15                 |

#### Bottom Color

The `"bottomColor"` property is an array of three numbers that defines the color of the sky at the bottom, also known as the nadir. This property **MUST** be defined for gradient skies and has no default value.

Like all colors in glTF, this is defined in linear color space. The bottom color may have values greater than 1.0, which indicate high energy, HDR, or overbright bottom color, but usually, the values are between 0.0 and 1.0.

#### Bottom Curve

The `"bottomCurve"` property is a number that defines the transition curve between the bottom and the horizon colors. It is a unitless number. If not specified, the default value is 0.02.

A curve value of 1.0 is a linear transition. Smaller values between 0.0 and 1.0 make the bottom color more dominant, while larger values between 1.0 and infinity make the horizon color more dominant. The value **MUST** be a positive number. See [Gradient Sky Formulas](#gradient-sky-formulas) for an example of how this value may be used.

#### Horizon Color

The `"horizonColor"` property is an array of three numbers that defines the color of the sky at the horizon. This property **MUST** be defined for gradient skies and has no default value.

Like all colors in glTF, this is defined in linear color space. The horizon color may have values greater than 1.0, which indicate high energy, HDR, or overbright horizon color, but usually, the values are between 0.0 and 1.0.

#### Top Color

The `"topColor"` property is an array of three numbers that defines the color of the sky at the top, also known as the zenith. This property **MUST** be defined for gradient skies and has no default value.

Like all colors in glTF, this is defined in linear color space. The top color may have values greater than 1.0, which indicate high energy, HDR, or overbright top color, but usually, the values are between 0.0 and 1.0.

#### Top Curve

The `"topCurve"` property is a number that defines the transition curve between the top and the horizon colors. It is a unitless number. If not specified, the default value is 0.15.

A curve value of 1.0 is a linear transition. Smaller values between 0.0 and 1.0 make the top color more dominant, while larger values between 1.0 and infinity make the horizon color more dominant. The value **MUST** be a positive number. See [Gradient Sky Formulas](#gradient-sky-formulas) for an example of how this value may be used.

#### Sun Angle Max

The `"sunAngleMax"` property is a number that defines the maximum radius angle of the sun in radians. If not specified, the default value is 0.5, or about 28.6479 degrees.

Note that this defines the outermost maximum angle that the sun would have if the sun curve was infinity.

#### Sun Curve

The `"sunCurve"` property is a number that defines the transition curve between the sun and the sky colors. It is a unitless number. If not specified, the default value is 0.15.

A curve value of 1.0 is a linear transition. Smaller values between 0.0 and 1.0 make the sky color more dominant, while larger values between 1.0 and infinity make the sun color more dominant. The value **MUST** be a positive number. See [Gradient Sky Formulas](#gradient-sky-formulas) for an example of how this value may be used.

#### Gradient Sky Formulas

This section is informative. Implementations are not required to use these formulas, and the exact appearance of gradient skies **MAY** vary between applications.

In the formulas below, `d` is the normalized view direction in the sky's local space, `y` is the Y component of `d`, `mix(a, b, t)` is `a + (b - a) * t`, and each `t` value is clamped to the range 0.0 to 1.0.

Above the horizon, where `y >= 0`, the sky color is a mix between the top color and the horizon color:

```
t = pow(1 - y, 0.6 / topCurve)
color = mix(topColor, horizonColor, t)
```

Below the horizon, where `y < 0`, the sky color is a mix between the bottom color and the horizon color:

```
t = pow(1 + y, 0.6 / bottomCurve)
color = mix(bottomColor, horizonColor, t)
```

Then, above the horizon, each sun is mixed in. In the formulas below, `s` is the normalized direction toward the sun in the same space as `d`, and `sunColor` is the light's color scaled by its intensity. Where `dot(d, s) > cos(sunAngleMax)`:

```
c = (1 - dot(d, s)) / (1 - cos(sunAngleMax))
t = pow(1 - c, 1.6 / pow(sunCurve, 1.4))
color = mix(color, sunColor, t)
```

With these formulas, larger curve values move the transition toward the horizon color or the sun color. A top or bottom curve of 0.6 gives a linear transition with respect to `y`, and a sun curve of about 1.4 gives a linear transition with respect to `c`. As the sun curve approaches infinity, the sun fills the entire `sunAngleMax` radius.

### Panorama Sky Properties

Panorama skies use a texture or set of textures to represent the sky. The `"panorama"` property is an object that contains the panorama sky properties:

|                     | Type         | Description                                                               |
| ------------------- | ------------ | ------------------------------------------------------------------------- |
| **cubemap**         | `integer[6]` | The indices of 6 textures making up a cubemap in the `textures` array.    |
| **equirectangular** | `integer`    | The index of an equirectangular panorama texture in the `textures` array. |

#### Cubemap

The `"cubemap"` property is an array of 6 integer indices that reference textures in the glTF document-level textures array. If not specified, this sky does not have a cubemap texture.

If present, the value **MUST** be a 6-element array of integers which refer to 6 textures by index. The face order and the orientation of each face image **MUST** follow the cubemap conventions of the [KTX 2.0 specification](https://registry.khronos.org/KTX/specs/2.0/ktxspec.v2.html) (section 3.6 "faceCount" and Appendix A "Cubemap Orientation"): the faces are in the order +X, -X, +Y, -Y, +Z, -Z, and each face image has a top-left origin, in KTX's left-handed cubemap coordinate system. KTX's cubemap coordinate system is converted to glTF's right-handed coordinate system by negating the X axis, which is one of the conversions described in KTX 2.0 Appendix A. The Y and Z axes are unchanged. These directions are in the sky's local space, before the sky's `"rotation"` is applied.

The KTX2 naming makes the X labels unintuitive for glTF's right-handed coordinate system: the face labeled "+X" (index 0) is seen when looking toward glTF's -X direction, and the face labeled "-X" (index 1) is seen when looking toward glTF's +X direction. This is because the labels refer to KTX's left-handed X axis, which is negated compared to glTF. The Y and Z faces are seen in the directions their labels suggest. In exchange, the face images are not mirrored when viewed from inside the skybox, consistent with equirectangular skies, so text on the sky is readable. KTX 2.0 cubemap files and GPU cubemap textures can also be used directly, by sampling with the X component of the sampling direction negated.

The following table restates the resulting orientation of each face image in glTF's coordinate system, as seen by a viewer inside the skybox:

| Index | KTX label | Seen when looking toward | Image right edge toward | Image top edge toward |
| ----- | --------- | ------------------------ | ----------------------- | --------------------- |
| 0     | +X        | -X                       | -Z                      | +Y                    |
| 1     | -X        | +X                       | +Z                      | +Y                    |
| 2     | +Y        | +Y                       | -X                      | -Z                    |
| 3     | -Y        | -Y                       | -X                      | +Z                    |
| 4     | +Z        | +Z                       | -X                      | +Y                    |
| 5     | -Z        | -Z                       | +X                      | +Y                    |

> Informative note: Tools that render cubemap faces with a right-handed camera, such as by looking along each glTF axis with +Y up, produce a face for glTF's +X direction and a face for glTF's -X direction. These must be stored at index 1 and index 0, respectively. Some engines name their cubemap faces differently; for example, Unity's 6 sided skybox labels its +X slot as "Left".

Cubemap skybox textures are not recommended because not all game engines support them natively. Unity supports them with its built-in skybox shaders, while Godot and Unreal require a custom sky shader or sky material that samples the cubemap faces. If an engine does not support cubemap skyboxes, an importer may convert the cubemap to an equirectangular texture, use a custom sky shader, or use a mesh to represent the skybox. Additionally, if the glTF file includes both a cubemap and equirectangular texture, the equirectangular texture may be loaded as a fallback.

#### Equirectangular

The `"equirectangular"` property is an integer index that references a texture in the glTF document-level textures array. If not specified, this sky does not have an equirectangular texture.

The equirectangular skybox texture is projected with the top of the texture in the +Y direction, the middle of the texture in the +Z direction, and the left of the texture in the +X direction. It may seem odd to have the left be +X, but since this is a skybox, it is viewed from the inside; this ensures that the texture is not flipped when viewed from the inside. For example, if the skybox image has text on it, the text would be readable on the skybox. This is consistent with what most major 3D engines do.

### Physical Sky Properties

Physical skies use a physically-based model to simulate the scattering of light in the atmosphere. The `"physical"` property is an object that contains the physical sky properties:

|                         | Type        | Description                                                                 | Default value        |
| ----------------------- | ----------- | --------------------------------------------------------------------------- | -------------------- |
| **groundColor**         | `number[3]` | The tint of the ground below the horizon, which is lit by the sky.          | `[0.04, 0.02, 0.01]` |
| **mieAnisotropy**       | `number`    | The anisotropy of Mie scattering (the thing that makes clouds white).       | 0.8                  |
| **mieCoefficient**      | `number`    | The coefficient of Mie scattering (the thing that makes clouds white).      | 0.000005             |
| **mieColor**            | `number[3]` | The color of Mie scattering (the thing that makes clouds white).            | `[1.0, 1.0, 1.0]`    |
| **rayleighCoefficient** | `number`    | The coefficient of Rayleigh scattering (the thing that makes the sky blue). | 0.00003              |
| **rayleighColor**       | `number[3]` | The color of Rayleigh scattering (the thing that makes the sky blue).       | `[0.3, 0.5, 1.0]`    |

Scattering coefficients are defined as inverse meters (m⁻¹), where larger values indicate stronger scattering.

#### Ground Color

The `"groundColor"` property is an array of three numbers that defines the color tint of the ground below the horizon. The default value is `[0.04, 0.02, 0.01]`, which is a sane default for Earth-like skies.

The ground is not part of the atmospheric simulation, but it is lit by the sky. The ground color acts like the albedo of the ground: it is multiplied by the light that the ground receives from the sun and atmosphere. As such, the displayed color of the bottom part of the sky depends on the lighting, and it is usually darker and less saturated than the ground color itself.

Like all colors in glTF, this is defined in linear color space. The ground color may have values greater than 1.0, which indicate high energy, HDR, or overbright ground color, but usually, the values are between 0.0 and 1.0.

> Informative note: Typically, the bottom of the sky is obscured by a planet's terrain, but there may be cases where the bottom of the sky is visible, or there is a need for the scene to have ambient light from below.

#### Mie Anisotropy

The `"mieAnisotropy"` property is a number that defines the anisotropy of Mie scattering, also known as eccentricity. It is a unitless number. The value **MUST** be between -1.0 and 1.0, inclusive. If not specified, the default value is 0.8, which is a sane default for Earth-like skies.

A value of 0.0 is isotropic scattering, all light scatters equally in all directions. A value of 1.0 is forward scattering, where light scatters more in the forward direction. A value of -1.0 is backward scattering, where light scatters more in the backward direction. Realistic skies usually have a value between 0.6 and 0.9. Large particle sizes will have a higher value, such as rain droplets reflecting light to make a rainbow. Unreal does not support negative anisotropy values, while Godot does.

#### Mie Coefficient

The `"mieCoefficient"` property is a number that defines the coefficient of Mie scattering, also known as the scale of Mie scattering. It has a unit of inverse meters (m⁻¹). If not specified, the default value is 0.000005, which is a sane default for Earth-like skies.

The Mie scattering settings affect the color of the sky itself. In clear skies, Mie scattering is usually quite weak. In cases where there is a lot of dust, smoke, haze, fog, pollution, or other particles in the air, the Mie scattering can be stronger.

> Informative note: Game engines may use different scales for the units. glTF uses only base metric units, so it is in inverse meters. However, Unreal and Godot use inverse kilometers, so a glTF value of 0.000005 needs to be multiplied by 1000 on import to 0.005. Unreal defaults to 0.003996, and Godot defaults to 0.05 (as a combination of 0.005 `mie_coefficient` and 10.0 `turbidity`).

#### Mie Color

The `"mieColor"` property is an array of three numbers that defines the color of Mie scattering, which is the thing that makes clouds white. The default value is `[1.0, 1.0, 1.0]`, which is a sane default for Earth-like skies.

Like all colors in glTF, this is defined in linear color space. The Mie color may have values greater than 1.0, which indicate high energy, HDR, or overbright Mie color, but usually, the values are between 0.0 and 1.0.

#### Rayleigh Coefficient

The `"rayleighCoefficient"` property is a number that defines the coefficient of Rayleigh scattering, also known as the scale of Rayleigh scattering. It has a unit of inverse meters (m⁻¹). If not specified, the default value is 0.00003, which is a sane default for Earth-like skies.

> Informative note: Game engines may use different scales for the units. glTF uses only base metric units, so it is in inverse meters. However, Unreal uses inverse kilometers, so a glTF value of 0.00003 needs to be multiplied by 1000 on import to 0.03. Godot uses inverse hundred kilometers, so a glTF value of 0.00003 needs to be multiplied by 100000 on import to 3.0. Unreal defaults to 0.0331, and Godot defaults to 2.0.

#### Rayleigh Color

The `"rayleighColor"` property is an array of three numbers that defines the color of Rayleigh scattering, which is the thing that makes the sky blue. The default value is `[0.3, 0.5, 1.0]`, which is a sane default for Earth-like skies.

Like all colors in glTF, this is defined in linear color space. The Rayleigh color may have values greater than 1.0, which indicate high energy, HDR, or overbright Rayleigh color, but usually, the values are between 0.0 and 1.0.

#### Plain Sky Properties

Plain color skies use a single color to represent the sky. Plain color skies are the simplest type, and are useful for making a single neutral color sky for previewing a scene, among other uses. The `"plain"` property is an object that contains the plain color sky properties:

|           | Type        | Description           | Default value     |
| --------- | ----------- | --------------------- | ----------------- |
| **color** | `number[3]` | The color of the sky. | `[0.0, 0.0, 0.0]` |

#### Color

The `"color"` property is an array of three numbers that defines the color of the sky. If not specified, the default value is `[0.0, 0.0, 0.0]`, which is black.

Like all colors in glTF, this is defined in linear color space. The plain color may have values greater than 1.0, which indicate high energy, HDR, or overbright plain color, but usually, the values are between 0.0 and 1.0.

### Document-level Properties

The `"KHR_environment_sky"` extension is defined at the root of the glTF document, in the `"extensions"` object.

|           | Type       | Description                                            | Default value |
| --------- | ---------- | ------------------------------------------------------ | ------------- |
| **skies** | `object[]` | The skies available to use.                            | Required.     |
| **sky**   | `integer`  | The index of the sky to use for this glTF, if defined. | 0             |

The sky index specifies which sky is used as the sky. If `"sky"` is not set, and `"skies"` contains skies, use the sky at index 0 as the sky. Only one sky may be active at a time. In most situations, glTF files will only contain one sky. However, a glTF file may contain multiple skies to support changing the sky at runtime, such as by animation or script.

It is expected that only one glTF file with a sky should be instantiated at a time. This may be the main "map" glTF scene, or a glTF dedicated to holding a sky. The behavior of multiple glTF files trying to control the sky is undefined.

> Informative note: glTF files intended to be used as individual objects that get composed into a larger scene should not use the `"KHR_environment_sky"` extension. The `"KHR_environment_sky"` extension should not be used as a "preview environment" within a glTF representing an object, to avoid conflicts with the sky when that object is instantiated in a larger scene.

### glTF Object Model

The following JSON pointers are defined representing mutable properties defined by this extension, for use with the glTF Object Model including extensions such as `KHR_animation_pointer` and `KHR_interactivity`.

| JSON Pointer                                                            | Object Model Type |
| ----------------------------------------------------------------------- | ----------------- |
| `/extensions/KHR_environment_sky/skies/{}/ambientLightColor`            | `float3`          |
| `/extensions/KHR_environment_sky/skies/{}/ambientSkyContribution`       | `float`           |
| `/extensions/KHR_environment_sky/skies/{}/rotation`                     | `float4`          |
| `/extensions/KHR_environment_sky/skies/{}/gradient/bottomColor`         | `float3`          |
| `/extensions/KHR_environment_sky/skies/{}/gradient/bottomCurve`         | `float`           |
| `/extensions/KHR_environment_sky/skies/{}/gradient/horizonColor`        | `float3`          |
| `/extensions/KHR_environment_sky/skies/{}/gradient/topColor`            | `float3`          |
| `/extensions/KHR_environment_sky/skies/{}/gradient/topCurve`            | `float`           |
| `/extensions/KHR_environment_sky/skies/{}/gradient/sunAngleMax`         | `float`           |
| `/extensions/KHR_environment_sky/skies/{}/gradient/sunCurve`            | `float`           |
| `/extensions/KHR_environment_sky/skies/{}/physical/groundColor`         | `float3`          |
| `/extensions/KHR_environment_sky/skies/{}/physical/mieAnisotropy`       | `float`           |
| `/extensions/KHR_environment_sky/skies/{}/physical/mieCoefficient`      | `float`           |
| `/extensions/KHR_environment_sky/skies/{}/physical/mieColor`            | `float3`          |
| `/extensions/KHR_environment_sky/skies/{}/physical/rayleighCoefficient` | `float`           |
| `/extensions/KHR_environment_sky/skies/{}/physical/rayleighColor`       | `float3`          |
| `/extensions/KHR_environment_sky/skies/{}/plain/color`                  | `float3`          |
| `/extensions/KHR_environment_sky/sky`                                   | `int`             |

Additionally, the following JSON pointers are defined for read-only properties:

| JSON Pointer                                   | Object Model Type |
| ---------------------------------------------- | ----------------- |
| `/extensions/KHR_environment_sky/skies.length` | `int`             |

### JSON Schema

See [schema/glTF.KHR_environment_sky.sky.schema.json](schema/glTF.KHR_environment_sky.sky.schema.json) for the main schema, and the following for the sub-schemas:

- Gradient: [schema/glTF.KHR_environment_sky.sky.gradient.schema.json](schema/glTF.KHR_environment_sky.sky.gradient.schema.json)
- Panorama: [schema/glTF.KHR_environment_sky.sky.panorama.schema.json](schema/glTF.KHR_environment_sky.sky.panorama.schema.json)
- Physical: [schema/glTF.KHR_environment_sky.sky.physical.schema.json](schema/glTF.KHR_environment_sky.sky.physical.schema.json)
- Plain: [schema/glTF.KHR_environment_sky.sky.plain.schema.json](schema/glTF.KHR_environment_sky.sky.plain.schema.json)

See also [schema/glTF.KHR_environment_sky.schema.json](schema/glTF.KHR_environment_sky.schema.json) for the document-level schema.

## Known Implementations

- Godot Engine add-on https://github.com/omigroup/omi-godot/pull/15

## Resources:

- Unity Procedural Skybox: https://docs.unity3d.com/2022.3/Documentation/Manual/shader-skybox-procedural.html
- Unity Physically Based Sky HDRP: https://docs.unity3d.com/Packages/com.unity.render-pipelines.high-definition@12.1/manual/Override-Physically-Based-Sky.html
- Unreal Sky Atmosphere: https://dev.epicgames.com/documentation/en-us/unreal-engine/sky-atmosphere-component-in-unreal-engine#renderingtheskyusingaskydomemesh
- Godot Sky and Sky Material: https://docs.godotengine.org/en/stable/classes/class_sky.html#class-sky-property-sky-material
