<!--
Copyright 2015-2021 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# KHR\_materials\_parallax\_mapping

## Contributors


## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Many materials provide a height-map that can either be used for real geometry displacement or for parallax-mapping.
This extension defines a way to specify a texture as a height map.

## glTF Schema Updates

The height at a given position is calculated by sampling the given texture and using the red channel. The sampled value is then multiplied with the `heightFactor`.
The result is used in an implementation defined way to offset all other texture lookups. (See Resources)

This extension does _not_ require the geometry of the object this material is applied to to be modified.

```json
{
    "materials":[
        {
            "extension": {
                "KHR_material_parallax_mapping": {
                    "heightTexture": 0,
                    "heightFactor": 0.5,
                }
            }
        }
    ]
}
```

### JSON Schema

- [parallaxMapping.schema.json](schema/parallaxMapping.schema.json)

## Known Implementations

None.

## Resources
* [Introduction](https://en.wikipedia.org/wiki/Parallax_mapping)
* [Discussion about different implementations](https://learnopengl.com/Advanced-Lighting/Parallax-Mapping)
