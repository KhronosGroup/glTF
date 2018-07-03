# KHR_blend

## Contributors

TODO

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines blending operations on glTF materials. The `KHR_blend`
extension may be used alongside the default PBR materials, or materials defined
in glTF extensions. When combined with other materials and techniques, blending
may be used to achieve effects like fire, steam, or holograms.

For any material with the `KHR_blend` extension, blending must use the
specified blend equation/operation and factors.

For any materials without the `KHR_blend` extension, blending is assumed to be
off unless otherwise specified.

### Properties

**blendEquation** :white_check_mark:

Determines how a new pixel is combined with a pixel already in the frame buffer.

* **Type:** `number` `[2]`
    - `0` *color* equation
    - `1` *alpha* equation
* **Required:** Yes
* **Allowed values:**

| name                     | blendEquation | description |
|--------------------------|---------------|-------------|
| FUNC_ADD                 | 0x8006        | Set an addition blend function. |
| FUNC_SUBTRACT            | 0x800A        | Specify a subtraction blend function (source - destination). |
| FUNC_REVERSE_SUBTRACT    | 0x800B        | Specify a reverse subtraction blend function (destination - source). |
| MIN                      | 0x8007        | Produces the minimum color components of the source and destination colors. |
| MAX                      | 0x8008        | Produces the maximum color components of the source and destination colors. |

**blendFactors** :white_check_mark:

Defines parameters to the blending equation.

* **Type:** `number` `[4]`
    - `0` : *source color* multiplier
    - `1` : *destination color* multiplier
    - `2` : *source alpha* multiplier
    - `3` : *destination alpha* multiplier
* **Required:** Yes
* **Allowed values:**

| name                | blendFactor | description |
|---------------------|-------------|-------------|
| ZERO                | 0           | Turn off a component. |
| ONE                 | 1           | Turn on a component. |
| SRC_COLOR           | 0x0300      | Multiply a component by the source's color. |
| ONE_MINUS_SRC_COLOR | 0x0301      | Multiply a component by one minus the source's color. |
| SRC_ALPHA           | 0x0302      | Multiply a component by the source's alpha. |
| ONE_MINUS_SRC_ALPHA | 0x0303      | Multiply a component by one minus the source's alpha. |
| DST_ALPHA           | 0x0304      | Multiply a component by the destination's alpha. |
| ONE_MINUS_DST_ALPHA | 0x0305      | Multiply a component by one minus the destination's alpha. |
| DST_COLOR           | 0x0306      | Multiply a component by the destination's color. |
| ONE_MINUS_DST_COLOR | 0x0307      | Multiply a component by one minus the destination's color. |

### Example

```json
"materials": [
    {
        "name": "My_Additive_Material",
        "pbrMetallicRoughness": {},
        "extensions": {
            "KHR_blend": {
                "blendEquation": [32778, 32778],
                "blendFactors": [1, 1, 1, 1]
            }
        }
    }
]
 ```

## glTF Schema Updates

**JSON schema**: TODO

## Known Implementations

TODO
