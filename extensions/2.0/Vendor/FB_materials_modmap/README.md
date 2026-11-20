# FB\_materials\_modmap

## Contributors

* Alfred Young, Facebook, [@idorurez](https://github.com/idorurez)
* Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds an additional annotation to a material, providing a means to introduce a texture map to be applied suggestively as multiplicative operation.

Some proven real-world applications for this information:
1. Having multiple variations of lighting scenarios without having to bake lighting into a single, original base color/texture.
2. Specify a separate UV channel (texCoord), to vary the new texture texel resolution with respect to the texture it's being applied to.
3. Modulate the intensity of a modmap, dimming or increasing it's effect.

## Color

Modmap texture values use the sRGB color primaries and the sRGB transfer function.

## Specifying Material extension

The data is defined as an optional extension to a glTF `materials`, by adding an `extensions` property and defining a `FB_materials_modmap` property with a `modmapTexture` node as a child:

```javascript
"materials" : [
	{
		"name" : "materials_test",
		"pbrMetallicRoughness" : {
			"baseColorTexture" : {
				"index" : 0,
				"texCoord" : 0
			}
		},
		"alphaMode" : "OPAQUE",
		"extensions" : {
			"FB_materials_modmap" : {
				"modmapTexture" : {
					"index" : 1,
					"texCoord" : 1
				},
				"modmapFactor" : [
					1,
					1,
					1
				]
			}
		}
	}
],
```

## Sample Implementation

In it's simplest form, Modmap implementation predominantly involes a base, texture or color in a material, and the supplied modmap texture is sampled and multiplied against that sampled value:

```javascript
    lowp vec4 modmapColor = texture2D(ModmapTexture, oModmapTexCoord);
    color.rgb *= (ModmapFactor.rgb * modmapColor.rgb);

```

## Limitations
This does not define how the modmap will be applied, just that one exists.

## modmapTexture properties
| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `index` | Texture descriptor index. | :white_check_mark: Yes |
| `texCoord` | Texture coordinate index, aka UV set | No, Default: `0` |
| `modmapFactor` | A 3-component vector describing the attenuation of the modmap before baseColor application. | No, Default: `[1.0, 1.0, 1.0]` |


