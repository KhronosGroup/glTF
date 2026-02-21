# EXT_animation_quantization

## Contributors

* Lukas Cone

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

In a current standard implementation, animation tracks must be stored as `FLOAT` component type with exception of rotation. Translation and scale must be stored as `VEC3` and rotation as `VEC4`.
Such implementation can be heavy on memory and transmition sizes.

This extension expands the set of allowed component types for animation sampler storage to provide a memory/precision tradeoff - depending on the application needs, 16-bit or 8-bit storage can be sufficient. It also adds ability to use sampler per axis for translations as well as uniform scale sampling.

In order to keep simplicity, this extension doesn't introduce new component types, but new schema.

Translation tracks can use sampler per axis, this allows more freedom of reducing frames.

Scale tracks as well can use sampler per axis, but also a single `SCALAR` accesor can be used for uniform scaling.

Rotation tracks can be now processed in multiple ways. Quaternion doesn't need to store W component, such component can be recomputed from X, Y, Z values. If rotation is applied only for single axis, there is no need to store other axes, since they should be 0 values. In this case `SCALAR` or `VEC2` can be used istead of `VEC4`.

This extension also reintroduces schema from `KHR_texture_transform` to increase quantized precision.

Time values can now be quantized as well.

This extension is NOT optional, files that use it must specify it in `extensionsRequired` array.

## glTF Schema Updates

```json
 "channels": [
    {
        "sampler": 0,
        "target": {
            "node": 1,
            "path": "translation.x"
        },
        "extensions": {
            "EXT_animation_quantization": {
                "offset": [-0.5],
                "scale": [1],
                "timeScale": 2.3333,
            }
        }
    },
]
```

### Properties

| |Type|Description|Required|
|-|-|-|-|
|**offset**|`number[1-4]`|Applies offset to values from sampler.|No|
|**scale**|`number[1-4]`|Applies scale to values from sampler. Scale must be applied before offset!|No|
|**timeScale**|`number`|Applies scale to time value.|No|

#### offset

Applies offset to values from sampler. Number of elements in array must match number of components in accessor type!

- Type: `number[1-4]`
- Required: No
- Default value: [0, 0, 0, 0]

#### scale

Applies scale to values from sampler. Scale must be applied before offset! Number of elements in array must match number of components in accessor type!

- Type: `number[1-4]`
- Required: No
- Default value: [1, 1, 1, 1]

#### timeScale

Applies scale to time values.

- Type: `number`
- Required: No
- Minimum: `>0`
- Default value: 1

### Extending channel.path

|`channel.path`|Accessor Type|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`"translation"`|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|XYZ translation vector|
|`"translation.x"`<br>`"translation.y"`<br>`"translation.z"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|Single axis translation|
|`"rotation"`|`"VEC3"`<br>`"VEC4"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|XYZW rotation quaternion|
|`"rotation.x"`|`"SCALAR"`<br>`"VEC2"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|XW rotation quaternion|
|`"rotation.y"`|`"SCALAR"`<br>`"VEC2"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|YW rotation quaternion|
|`"rotation.z"`|`"SCALAR"`<br>`"VEC2"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|ZW rotation quaternion|
|`"scale"`|`"VEC3"`<br>`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|XYZ scale vector or uniform scale|
|`"scale.x"`<br>`"scale.y"`<br>`"scale.z"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|Single axis scale|

> **Implementation Note:** It is forbidden to use `rotation`, `rotation.x`, `rotation.y`, `rotation.z` in any combination on a single target! It is forbidden to use `translation` with `translation.x` or `translation.y` or `translation.z` on a single target, same rule goes for `scale`.

### Extending rotation.\<axis\> accessors

|Accessor type|Component Type(s)|
|-|-|
|`"SCALAR"`<br>`"VEC2"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized

#### Rotation as SCALAR

Such value represents single X, Y or Z axis, this mode should be used for rotations around single axis, all other axes (except W) in quaternion must be 0.

W component should be calculated as `W = sqrt(1.0 - <axis> * <axis>)`

#### Rotation as VEC2

First component must represent X, Y or Z. Second component always represents W.

This mode is same as `rotation as scalar` but without the need of recomputing W component.

### Extending rotation accessors

|Accessor type|Component Type(s)|
|-|-|
|`"VEC3"`<br>`"VEC4"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized

#### Rotation as VEC3

All components represents X, Y and Z axes.

W component should be calculated as `W = sqrt(1 - X * X + Y * Y + Z * Z)`

### Extending scale accessors

|Accessor type|Component Type(s)|
|-|-|
|`"SCALAR"`<br>`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized

#### Scale as SCALAR

This mode can represent uniform scale across all axes or scale along single axis.

### Extending time accessors

|Accessor type|Component Type(s)|
|-|-|
|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized

Restiction for time values still apply, values must strictly increase.

### Decoding/encoding quantized values

Implementations should assume following equations are used to get corresponding floating-point value `f` from a normalized integer `c` and should use the specified equations to encode floating-point values to integers after range normalization:

|`accessor.componentType`|int-to-float|float-to-int|
|-----------------------------|--------|----------------|
| `5120`&nbsp;(BYTE)          |`f = max(c / 127.0, -1.0)`|`c = round(f * 127.0)`|
| `5121`&nbsp;(UNSIGNED_BYTE) |`f = c / 255.0`|`c = round(f * 255.0)`|
| `5122`&nbsp;(SHORT)         |`f = max(c / 32767.0, -1.0)`|`c = round(f * 32767.0)`|
| `5123`&nbsp;(UNSIGNED_SHORT)|`f = c / 65535.0`|`c = round(f * 65535.0)`|

> **Implementation Note:** Due to OpenGL ES 2.0 / WebGL 1.0 platform differences, some implementations may decode signed normalized integers to floating-point values differently.

> **Implementation Note:** Un-normalized values can increase performance. Quantization decode fraction can be stored inside `scale` property, hence removing need for normalization.

### JSON Schema

[animation.EXT_animation_quantization.schema](schema/animation.EXT_animation_quantization.schema.json)
