# KHR\_lights\_shadows

## Contributors

* Takahiro Aoyagi, Mozilla, [@takahirox](https://github.com/takahirox)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec and
[`KHR_lights_punctual`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_lights_punctual).

## Overview

This `KHR_lights_shadows` extension defines what meshes cast or receive shadows
and what lights cast shadows against
[`KHR_lights_punctual`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_lights_punctual).

Rendering with shadows needs a lot of calculation. It is a common technique to
select objects which cast or receive shadows in a scene.
`KHR_lights_punctual` extension has the capability of defining lights but doesn't
have the capability of defining shadows.

This `KHR_lights_shadows` extension provides a way to let applications or
engines know what meshes should cast or receive shadows and what lights should
cast shadows.

## Cast/Receive shadows for Meshes

Whether a mesh casts shadows and whether it receives shadows are defined by
adding `extensions.KHR_lights_shadows` property to a `mesh.primitive` definition
with `castShadows` and `receiveShadows` boolean properties inside it.

```
"meshes": [{
    "primitives": [{
        ...
        "extensions": {
            "KHR_lights_shadows": {
                "castShadows": false,
                "receiveShadows": true
            }
        }
    }, {
        ...
        "extensions": {
            "KHR_lights_shadows": {
                "castShadows": true,
                "receiveShadows": false
            }
        }
    }]
}]
```

## Turn on/off shadows for Lights

Whether a light casts shadows is defined by adding
`extensions.KHR_lights_shadows` property to a top-level
`extensions.KHR_lights_punctual` definition with `castShadows` boolean property
inside it.

```
"extensions": {
    "KHR_lights_punctual": {
        "lights": [{
            ...,
            "extensions": {
                "KHR_lights_shadows": {
                    "castShadows": true
                }
            }
        }]
    }
}]
```

## Cast/Receive Shadows properties for Mesh primitive

| Property | Type | Description | Requires |
|:------|:------|:------|:------|
| `castShadows` | `boolean` | Whether the mesh primitive casts shadows | No, default: `true` |
| `receiveShadows` | `boolean` | Whether the mesh primitive receives shadows | No, default: `true` |

## Cast shadows property for Light

| Property | Type | Description | Requires |
|:------|:------|:------|:------|
| `castShadows` | `boolean` | Whether the light casts shadows | No, default: `true` |

## glTF Schema Updates

* **JSON schema**:
  * [mesh.primitive.KHR_lights_shadows.schema.json](schema/mesh.primitive.KHR_lights_shadows.schema.json)
  * [glTF.KHR_lights_punctual.KHR_lights_shadows.schema.json](schema/glTF.KHR_lights_punctual.KHR_lights_shadows.schema.json)

## Implementation Note

This extension doesn't define shadow rendering techniques (ie. Shadow Map) or
shadow types (ie. PCF). They should be application or engine specific.

The `castShadows` and `receiveShadows` properties for Mesh primitives don't
define whether the mesh primitives are visible. Similarly the `castShadows`
property for Lights doesn't define whether the lights are active.
