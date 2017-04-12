# KHR\_materials\_pbrSpecularGlossiness

## Contributors

* Max Limper, Fraunhofer IGD, [@mlimper_cg](https://twitter.com/mlimper_cg)
* Timo Sturm, Fraunhofer IGD, [@\_tsturm\_](https://twitter.com/\_tsturm\_)
* Miguel Sousa, Fraunhofer IGD, [@mfportela](https://twitter.com/mfportela)
* Maik Th&ouml;ner, Fraunhofer IGD, [@mthoener](https://twitter.com/mthoener)
* Eric Haines, Autodesk, [@pointinpolygon](https://twitter.com/pointinpolygon)
* Cedric Pinson, Sketchfab, [@trigrou](https://twitter.com/trigrou)
* Saurabh Bhatia, Microsoft [@iamsbtron](https://twitter.com/iamsbtron)
* Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
* Patrick Cozzi, Cesium, [@pjcozzi](https://twitter.com/pjcozzi)

## Status

Draft 

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines the specular-glossiness material model from Physically-Based Rendering (PBR). This extensions allows glTF to support this additional workflow. 

The [conformance](#conformance) section specifies what an implementation must to do when encountering this extension, and how the extension interacts with the materials defined in the base specification.

## Extending Materials

The PBR specular-glossiness materials are defined by adding the `KHR_materials_pbrSpecularGlossiness` extension to any glTF material. 
For example, the following defines a material like gold using specular-glossiness parameters.:

```
{
    "materials": [
        {
            "name": "gold",
            "extensions": {
                "KHR_materials_pbrSpecularGlossiness": {
                    "diffuseFactor": [ 0.5, 0.5, 0.5, 1 ],
                    "specularFactor": [ 0.0, 0.0, 0.0 ],
                    "glossinessFactor": 0.8
                }
            }

        }
    ]
}
```

### Specular - Glossiness

<p>
<img src="./figures/specular-glossiness material.png"/>
</p>

The specular-glossiness material model is defined by the following properties:
* `diffuse` - Reflected diffuse color of the material
* `specular` - Specular color of the material
* `glossiness` - Glossiness of the material

The diffuse value represents the reflected diffuse color of the material. The specular value defines the specific measured reflectance value at normal incidence (F0). The glossiness property is a factor between `0.0` (rough surface) and `1.0` (smooth surface) and represents the surface irregularities that cause light diffusion. 

The specular property from specular-glossiness material model is the same as the base color value from the metallic-roughness material model for metals. The glossiness property from specular-glossiness material model is related with the roughness property from the metallic-roughness material model and is defined as `glossiness = 1 - roughness`. See [appendix](#appendix) for more details on how you can convert between these two material models.

The value for each property (`diffuse`, `specular`, `glossiness`) can be defined using factors or textures. The `specular` and `glossiness` properties are packed together in a single texture called `specularGlossinessTexture`.  If a texture is not given, all respective texture components within this material model are assumed to have a value of `1.0`. The factors (`diffuseFactor`, `specularFactor`, `glossinessFactor`) scale, in linear space, the components given in the respective textures (`diffuseTexture`, `specularGlossinessTexture`). Texture content must be converted to linear space before it is used for any lighting computations.

The following equations show how to calculate bidirectional reflectance distribution function (BRDF) inputs (*c<sub>diff</sub>*, *F<sub>0</sub>*, *&alpha;*) from the metallic-roughness material properties. In addition to the material properties, if a primitive specifies a vertex color using the attribute semantic property `COLOR_0`, then this value acts as an additional linear multiplier to *c<sub>diff</sub>*.

*c<sub>diff</sub>* = `diffuse.rgb * (1 - max(specular.r, specular.g, specular.b)) * vertexColor`
<br>
*F<sub>0</sub>* = `specular`
<br>
*&alpha;* = `max((1 - glossiness) ^ 2, epsilon)`

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](/specification/2.0/README.md#appendix-a) for more details on the BRDF calculations.

The following table lists the allowed types and ranges for the specular-glossiness model:

|   |Type|Description|Required|
|---|----|-----------|--------|
|**diffuseFactor** | `number[4]` | The reflected diffuse factor of the material.|No, default:`[1.0,1.0,1.0,1.0]`|
|**diffuseTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)  | The diffuse texture.|No|
|**specularFactor** | `number[3]` | The specular RGB color of the material. |No, default:`[1.0,1.0,1.0]`|
|**glossinessFactor** | `number` | The glossiness or smoothness of the material. |No, default:`1.0`|
|**specularGlossinessTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The specular-glossiness texture.|No|

Additional properties are not allowed.

* **JSON schema**: [glTF.KHR_materials_pbrSpecularGlossiness.schema.json](schema/glTF.KHR_materials_pbrSpecularGlossiness.schema.json)

### diffuseFactor

The RGBA components of the reflected diffuse color of the material. Metals have a diffuse value of `[0.0, 0.0, 0.0]`. The fourth component (A) is the opacity of the material. The values are linear.

* **Type**: `number[4]`
* **Required**: No, default:`[1.0,1.0,1.0,1.0]`
* **Range**: [0,1] for all components

### diffuseTexture

The diffuse texture. This texture contains RGB(A) components of the reflected diffuse color of the material in sRGB color space. If the fourth component (A) is present, it represents the alpha coverage of the material. Otherwise, an alpha of 1.0 is assumed. The `alphaMode` property specifies how alpha is interpreted. The stored texels must not be premultiplied.

* **Type**: [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)  
* **Required**: No

### specularFactor

The specular RGB color of the material. This value is linear.

* **Type**: `number[3]`
* **Required**: No, default:`[1.0,1.0,1.0]`
* **Range**: [0,1] for all components


### glossinessFactor

The glossiness or smoothness of the material. A value of 1.0 means the material has full glossiness or is perfectly smooth. A value of 0.0 means the material has no glossiness or is perfectly rough. This value is linear.

* **Type**: `number`
* **Required**: No, default:`1.0`
* **Range**: [0,1]

### specularGlossinessTexture

The specular-glossiness texture is a RGBA texture, containing the specular color (RGB) in sRGB space and the glossiness value (A) in linear space.

* **Type**: [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) 
* **Required**: No


#### Additional Maps

The [additional maps](/specification/2.0/README.md#additional-maps) defined in glTF materials node can also be used with the PBR specular-glossiness material model parameters defined in this extension.

```
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_pbrSpecularGlossiness": {
                    "diffuseTexture": {
                        "index": 0
                    },
                    "glossinessFactor": 0.5,
                    "specularGlossinessTexture": {
                        "index": 0,
                        "texCoord": 0
                    }
                }
            },
            "normalTexture": {
                "scale": 2,
                "index": 3,
                "texCoord": 1
            },
            "emissiveFactor": [ 0.2, 0.1, 0.0 ]
        }
    ]
}
```

<a name="bestpractices"></a>
## Best practices

The PBR specular-glossiness extension can be used along with PBR metallic-roughness material model in glTF to enable support for both PBR workflows. Specular-glossiness can represent a broader range of materials compared to metallic-roughness. However, supporting specular-glossiness on low-resource devices may not be possible as it is more resource heavy than the metallic-roughness model. To get the best of both worlds a glTF asset can include both metallic-roughness and specular-glossiness in a single glTF asset. This allows the asset to take advantage of richer specular-glossiness materials where possible and still have a fall back with metallic-roughness to ensure that the asset can be rendered everywhere. Since such an approach requires including both material models it is best suited for a web scenario where a client can choose to download the appropriate material model from a server hosting the glTF asset. 

The following example shows how the same material can be defined using both metallic-roughness as well as specular-glossiness material models:

```
{
    "materials": [
        {
            "name": "gold",
            "pbrMetallicRoughness": {
                "baseColorfactor": [ 1.000, 0.766, 0.336, 1.0 ],
                "metallicFactor": 1.0,
                "roughnessFactor": 0.0
            },
            "extensions": {
                "KHR_materials_pbrSpecularGlossiness": {
                    "diffuseFactor": [ 0.5, 0.5, 0.5, 1 ],
                    "specularFactor": [ 0.0, 0.0, 0.0 ],
                    "glossinessFactor": 0.8
                }
            }
        }
    ]
}
```
If the specular-glossiness extension is included in an asset, then any client implementation that supports the extension should always render the asset using the specular-glossiness material properties.

A `material` may specify the specular-glossiness extension without including any metallic-roughness properties. For such an asset, the best practice is to include the specular-glossiness extension using the `extensionRequired` property so that the asset is only loaded by clients that can render the specular-glossiness material. This approach trades asset portability for a smaller file size. If `extensionsUsed` property is specified instead of `extensionsRequired`, then a client that does not support the specular-glossiness extension will render the asset as if no material was specified. 

The following table describes the expected rendering behavior based on the material definitions included in the asset:

| | Client supports metallic-roughness | Client supports metallic-roughness and specular-glossiness | 
|----|:----:|:----:|
|Asset has metallic-roughness | Render metallic-roughness | Render metallic-roughness |  
|Asset has metallic-roughness and specular-glossiness | Render metallic-roughness | Render specular-glossiness | 
|Asset has specular-glossiness with `extensionsRequired`| Fail to load | Render specular-glossiness | 
|Asset has specular-glossiness with `extensionsUsed` | Render as if no material | Render specular-glossiness | 

## Appendix

Conversion between the two PBR material models:
- [Convert between metallic-roughness and specular-glossiness using BabylonJS](examples/convert-between-workflows-bjs/) 
- [Convert between metallic-roughness and specular-glossiness using WebGL and threeJS](examples/convert-between-workflows/) 





