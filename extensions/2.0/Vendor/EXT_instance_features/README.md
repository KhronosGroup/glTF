# EXT_instance_features

## Contributors

* Peter Gagliardi, Cesium
* Sean Lilley, Cesium
* Sam Suhag, Cesium
* Don McCurdy, Independent
* Marco Hutter, Cesium
* Bao Tran, Cesium
* Samuel Vargas, Cesium
* Patrick Cozzi, Cesium

## Status

Draft

## Dependencies

Written against the glTF 2.0 specification. It depends on the [`EXT_mesh_gpu_instancing`](../EXT_mesh_gpu_instancing) and [`EXT_mesh_features`](../EXT_mesh_features) extensions, to define Feature IDs for GPU instances.

## Overview

In most realtime 3D contexts, performance requirements demand minimizing the number of nodes and meshes in an asset. These requirements compete with interactivity, as applications may wish to merge static objects while still supporting interaction or inspection on those objects. A common performance optimizations is GPU instancing, using the `EXT_mesh_gpu_instancing` extension. But this destroys references to distinct objects, their attributes, and their behaviors.

This extension defines a means of associating GPU instances that are created using the `EXT_mesh_gpu_instancing` extension with unique identifiers. These allow identifying the GPU instances as individual _features_, and are therefore referred to as _feature IDs_. The precise definition of the structure of these identifiers can be found in the `EXT_mesh_features` extension specification. 

### Feature ID by GPU Instance

*Defined in [node.EXT_instance_features.schema.json](./schema/node.EXT_instance_features.schema.json).*

Feature IDs may be assigned to individual GPU instances using an instance attribute, or generated implicitly by instance index. Nodes with `EXT_instance_features` must also define an `EXT_mesh_gpu_instancing` extension, and are invalid without this dependency.

> **Example:** A node defining instances of mesh `0`, with each instance having a feature ID in the `_FEATURE_ID_0` instance attribute.
>
> ```jsonc
> {
>   "nodes": [
>     {
>       "mesh": 0,
>       "extensions": {
>         "EXT_mesh_gpu_instancing": {
>           "attributes": {
>             "TRANSLATION": 0,
>             "ROTATION": 1,
>             "SCALE": 2,
>             "_FEATURE_ID_0": 3
>           },
>         },
>         "EXT_instance_features": {
>           "featureIds": {
>             "exampleFeatureId": {
>               "featureCount": 10,
>               "attribute": 0
>             }
>           }
>         }
>       }
>     }
>   ]
> }
> ```

## Optional vs. Required

This extension is optional, meaning it should be placed in the `extensionsUsed` list, but not in the `extensionsRequired` list.

## Schema

* [node.EXT_instance_features.schema.json](./schema/node.EXT_instance_features.schema.json)


## Revision History

This extension was originally part of a the `EXT_mesh_features` proposal extension. The revision history of this extension can be found in the [common revision history of the 3D Tiles Next extensions](https://github.com/CesiumGS/3d-tiles/blob/extension-revisions/next/REVISION_HISTORY.md).

