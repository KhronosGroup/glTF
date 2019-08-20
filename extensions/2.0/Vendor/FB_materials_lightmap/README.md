# FB\_materials\_lightmap

## Contributors

* Alfred Young, Facebook, [@idorurez](https://github.com/idorurez)
* Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds an additional annotation to a material, providing a means to introduce a lightmap to be applied in the same scope.

Some proven real-world applications for this information:
1. Having multiple variations of lighting scenarios without having to bake lighting into a single, original base color/texture.
2. Specify a separate UV channel (texCoord), to vary the new texture texel resolution with respect to the texture it's being applied to.
3. Modulate the intensity of a lightmap, dimming or increasing it's effect.

## Color

Lightmap texture values use the sRGB color primaries and the sRGB transfer function.

## Specifying Material extension

The data is defined as an optional extension to a glTF `materials`, by adding an `extensions` property and defining a `FB_materials_lightmap` property with a `lightmapTexture` node inside it:

```javascript
"materials" : [
  	{
  		"name" : "material_test",
  		"pbrMetallicRoughness" : {
  			"baseColorTexture" : {
  				"index" : 0,
  				"texCoord" : 0
  			}
  		},
  		"extensions" : {
  			"FB_materials_lightmap" : {
  				"lightmapTexture" : {
  					"index" : 1,
  					"texCoord" : 1,    
  				},
				"lightmapFactor" : [
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

In it's simplemest form, Lightmap implementation predominantely involes a base, unlit texture or color in a material, and the supplied lightmap texture is sampled and multiplied against that unlit texture/color.

```javascript
    lowp vec4 lightmapColor = texture2D(LightmapTexture, oLightmapTexCoord);
    color.rgb *= (LightmapFactor.rgb * lightmapColor.rgb);

```

## Limitations
This does not define how the lightmap will be applied, just that one exists.

## lightmapTexture properties
| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `index` | Texture descriptor index. | :white_check_mark: Yes |
| `texCoord` | Texture coordinate index, aka UV set | No, Default: `0` |
| `lightmapFactor` | A 3-component vector describing the attenuation of the lightmap before baseColor application. | No, Default: `[1.0, 1.0, 1.0]` |


