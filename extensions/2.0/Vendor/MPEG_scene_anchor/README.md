# MPEG_scene_anchor

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14/CD Amd.2](https://www.iso.org/standard/86439.html), available as [WG03_N0797](https://mpeg.expert/live/nextcloud/index.php/s/f6prqzxWMdDM3r2)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

---------------------------------------
<a name="reference-mpeg_anchors extension"></a>
### MPEG_scene_anchors extension

MPEG scene and node anchor is used to anchor a scene/node to the viewer's real-world environment.

**`MPEG_scene_anchors extension` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**trackables**|`trackable` `[1-*]`|Contains a list of trackable used in the scene| &#10003; Yes|
|**anchors**|`anchor` `[1-*]`|Provides a list of anchors for a scene or for root nodes to enable AR anchoring| &#10003; Yes|

Additional properties are allowed.

* **JSON schema**: [MPEG_scene_anchor.schema.json](./schema/MPEG_scene_anchor.schema.json)

#### MPEG_anchors_extension.trackables

Contains a list of trackable used in the scene

* **Type**: `trackable` `[1-*]`
* **Required**:  &#10003; Yes

#### MPEG_anchors_extension.anchors

Provides a list of anchors for a scene or for root nodes to enable AR anchoring

* **Type**: `anchor` `[1-*]`
* **Required**:  &#10003; Yes



---------------------------------------
<a name="reference-trackable"></a>
### MPEG_scene_anchor.trackable object

Trackable schema

**`MPEG_scene_anchor.trackables object` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**type**|`integer`|the type flag may take one of the following values: TRACKABLE_FLOOR=0, TRACKABLE_VIEWER=1, TRACKABLE_CONTROLLER=2, TRACKABLE_GEOMETRIC=3, TRACKABLE_MARKER_2D=4, TRACKABLE_MARKER_3D=5, TRACKABLE_MARKER_GEO=6, TRACKABLE_APPLICATION=7| &#10003; Yes|
|**path**|`string`|a path that describes the action space as specified by OpenXR. <br/>Required if type is 2.|No, default: |
|**geometricConstraint**|`integer`|the geometricConstraint flag may take one of the following values: HORIZONTAL_PLANE=0, VERTICAL_PLANE=1. <br/>Required if type is 3.|No|
|**markerNode**|`number`|Index to the node in the nodes array in which the marker geometry and texture are described. <br/>Required if type is 4 or 5|No|
|**geoCoordinate**|`number`|index to geo coordinate item in array in  MPEG_GEO_COORDINATES extension. <br/>Required if type is 6.|No|
|**trackable_id**|`string`|An application-defined trackable id, that is known to the application.<br/>Required if type is 7|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_scene_anchor.trackable.schema.json](./schema/MPEG_scene_anchor.trackable.schema.json)

#### trackable.type

the type flag may take one of the following values: TRACKABLE_FLOOR=0, TRACKABLE_VIEWER=1, TRACKABLE_CONTROLLER=2, TRACKABLE_GEOMETRIC=3, TRACKABLE_MARKER_2D=4, TRACKABLE_MARKER_3D=5, TRACKABLE_MARKER_GEO=6, TRACKABLE_APPLICATION=7

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Allowed values**:
    * `0`
    * `1`
    * `2`
    * `3`
    * `4`
    * `5`
    * `6`
    * `7`

#### trackable.path

a path that describes the action space as specified by OpenXR

* **Type**: `string`
* **Required**: No, default: 

#### trackable.geometricConstraint

the geometricConstraint flag may take one of the following values: HORIZONTAL_PLANE=0, VERTICAL_PLANE=1

* **Type**: `integer`
* **Required**: No
* **Allowed values**:
    * `0`
    * `1`

#### trackable.markerNode

Index to the node in the nodes array in which the marker geometry and texture are described

* **Type**: `number`
* **Required**: No

#### trackable.geoCoordinate

index to geo coordinate item in array in  MPEG_GEO_COORDINATES extension

* **Type**: `number`
* **Required**: No

#### trackable.trackable_id

An application-defined trackable id, that is known to the application.

* **Type**: `string`
* **Required**: No
---------------------------------------
<a name="reference-anchor"></a>
### MPEG_scene_anchor.anchor object

Anchor schema

**`MPEG_scene_anchor.anchor object` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**trackable**|`number`|Index of the trackable in the trackables array that will be used for this anchor| &#10003; Yes|
|**requiresAnchoring**|`boolean`|if TRUE, the application shall skip the virtual assets attached to this anchor until the pose of this anchor in the real world is known. If FALSE, the application shall process the virtual assets attached to this anchor.| &#10003; Yes|
|**minimumRequiredSpace**|`array[]`|Space required to anchor the AR asset (x, y, z in meters). This space corresponds to an axis-aligned bounding box expressed in the trackable local space. This value shall be compared to the bounding box of the real available space determined by the application.|No, default: `[0,0,0]`|
|**aligned**|`integer`|the aligned flag may take one of the following values: NOT_USED=0, ALIGNED_NOTSCALED=1, ALIGNED_SCALED=2. If ALIGNED_SCALED is set, the bounding box of the virtual assets attached to that anchor is aligned and scaled to match the bounding box of the real available space determined by the application.|No, default: `0`|
|**actions**|`array[]`|Indices of the actions in the actions array of the interactivity extension to be executed once the pose of this anchor is determined. An example is a setTransform action to place the virtual assets attached to that anchor.|No|
|**light**|`integer`|Reference to an item in the lights array of the MPEG_lights_texture_based extension.|No|
|**extensions**|`extension`|JSON object with extension-specific objects.|No|
|**extras**|`extras`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_scene_anchor.anchor.schema.json](./schema/MPEG_scene_anchor.anchor.schema.json)

### anchor.trackable

Index of the trackable in the trackables array that will be used for this anchor

* **Type**: `number`
* **Required**:  &#10003; Yes

### anchor.requiresAnchoring

if TRUE, the application shall skip the virtual assets attached to this anchor until the pose of this anchor in the real world is known. If FALSE, the application shall process the virtual assets attached to this anchor.

* **Type**: `boolean`
* **Required**:  &#10003; Yes

### anchor.minimumRequiredSpace

Space required to anchor the AR asset (x, y, z in meters). This space corresponds to an axis-aligned bounding box expressed in the trackable local space. This value shall be compared to the bounding box of the real available space determined by the application.

* **Type**: `array[]`
* **Required**: No, default: `[0,0,0]`

### anchor.aligned

the aligned flag may take one of the following values: NOT_USED=0, ALIGNED_NOTSCALED=1, ALIGNED_SCALED=2. If ALIGNED_SCALED is set, the bounding box of the virtual assets attached to that anchor is aligned and scaled to match the bounding box of the real available space determined by the application.

* **Type**: `integer`
* **Required**: No, default: `0`
* **Allowed values**:
    * `0`
    * `1`
    * `2`
    * `3`
    * `4`

### anchor.actions

Indices of the actions in the actions array of the interactivity extension to be executed once the pose of this anchor is determined. An example is a setTransform action to place the virtual assets attached to that anchor.

* **Type**: `array[]`
* **Required**: No

### anchor.light

Reference to an item in the lights array of the MPEG_lights_video_based extension.

* **Type**: `integer`
* **Required**: No

### anchor.extensions

JSON object with extension-specific objects.

* **Type**: `extension`
* **Required**: No
* **Type of each property**: Extension

### anchor.extras

Application-specific data.

* **Type**: `extras`
* **Required**: No


## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC 23090-14/CD Amd 2](https://www.iso.org/standard/86439.html), Information technology — Coded representation of immersive media — Part 14: Scene description — Amendment 2: Support for haptics, augmented reality, avatars, Interactivity, MPEG-I audio, and lighting 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2023

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.
