# MPEG_node_interactivity

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
<a name="reference-mpeg_node_interactivity-triggers-object"></a>
### MPEG_node_interactivity

MPEG node interactivity extension allows the specialization of XR interactivity features at the node level.

**`MPEG_node_interactivity.triggers object` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**triggers**|`trigger.node` `[1-*]`|Array of node triggers . Only distinct types are allowed.The minimum size of this array is 1, and the maximum size is size of trigger types of AMD2.| &#10003; Yes|
|**extensions**|`any`||No|
|**extras**|`any`||No|

Additional properties are allowed.

* **JSON schema**: [MPEG_node_interactivity.triggers.node.schema.json](./schema/MPEG_node_interactivity.triggers.node.schema.json)

#### MPEG_node_interactivity.triggers object.triggers

Array of node triggers . Only distinct types are allowed.The minimum size of this array is 0, and the maximum size is size of trigger types of AMD2.

* **Type**: `trigger.node` `[1-*]`
* **Required**:  &#10003; Yes

#### MPEG_node_interactivity.triggers object.extensions

* **Type**: `any`
* **Required**: No

#### MPEG_node_interactivity.triggers object.extras

* **Type**: `any`
* **Required**: No

---------------------------------------
<a name="reference-trigger-node"></a>
### MPEG_node_interactivity.trigger object

glTF extension to specify trigger formats at node level

**`MPEG_node_interactivity.trigger object` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**type**|`string`|the type of the trigger. Value is oneOf TRIGGER_COLLISION, TRIGGER_PROXIMITY, TRIGGER_USER_INPUT, TRIGGER_VISIBILITY and can be extended| &#10003; Yes|
|**collider**|`integer`|the index of the mesh element that provides the collider geometry for the current node. <br/>Required if type is TRIGGER_COLLISION.|No|
|**static**|`boolean`|If True, the collider is defined as a static collider. <br/>Required if type is TRIGGER_COLLISION.|No, default: `true`|
|**usePhysics**|`boolean`|Indicates if the object shall be considered by the physics simulation. <br/>Required if type is TRIGGER_COLLISION.|No, default: `false`|
|**useGravity**|`boolean`|Indicates if the gravity affects the object. <br/>Required if usePhysics is true.|No, default: `true`|
|**mass**|`number`|Mass of the object in kilogram. <br/>Required if usePhysics is true.|No, default: `1.0`|
|**bounciness**|`number`|Provides the amount that an object bounces in response to a collision. <br/>Required if usePhysics is true.|No, default: `0`|
|**roughness**|`number`|Provides the roughness of an object which affects how much the object moves in response to a collision. <br/>Required if usePhysics is true.|No, default: `0.5`|
|**use_geocoordinates**|`boolean`|Indicates if geo coordinates are used. <br/>Required if type is TRIGGER_PROXIMITY.|No|
|**allow_occlusion**|`boolean`|Indicates if occlusion by other nodes should be considered. <br/>Required if type is TRIGGER_PROXIMITY.|No, default: `true`|
|**upper_distance_weight**|`number`|The weight applied to the distanceUpperLimit parameter defined at scene level <br/>Required if type is TRIGGER_PROXIMITY. |No, default: `1`|
|**lower_distance_weight**|`number`|The weight applied to the distanceLowerLimit parameter defined at scene level <br/>Required if type is TRIGGER_PROXIMITY. |No, default: `1`|
|**geo_coordinate**|`integer`|Index to geo coordinate item in array in MPEG_GEO_COORDINATES extension <br/>Required if use_geocoordinate is true. |No|
|**userInputParameters**|`string` `[1-*]`|Provides additional information related to the user inputs <br/>Required if type is TRIGGER_USER_INPUT. |No|
|**visibilityFull**|`boolean`|Indicates if the visibility shall be full or partial to activate the trigger <br/>Required if type is TRIGGER_VISIBILITY.|No, default: `true`|
|**nodes**|`integer` `[1-*]`|Set of nodes that shall not be considered for the visibility computation <br/>Required if type is TRIGGER_VISIBILITY.|No|
|**mesh**|`integer`|Index of the mesh in the scene meshes array that will be used to compute visibility <br/>Required if type is TRIGGER_VISIBILITY.|No|
|**extensions**|`extension`|JSON object with extension-specific objects.|No|
|**extras**|`extras`|Application-specific data.|No|

Additional properties are allowed.


* **JSON schema**: [MPEG_node_interactivity.trigger.schema.json](./schema/MPEG_node_interactivity.trigger.schema.json)

#### trigger.node.type

the type of the trigger. Value is oneOf TRIGGER_COLLISION, TRIGGER_PROXIMITY, TRIGGER_USER_INPUT, TRIGGER_VISIBILITY and can be extended

* **Type**: `string`
* **Required**:  &#10003; Yes

#### trigger.node.collider

the index of the mesh element that provides the collider geometry for the current node

* **Type**: `integer`
* **Required**: No

#### trigger.node.static

If True, the collider is defined as a static collider

* **Type**: `boolean`
* **Required**: No, default: `true`

#### trigger.node.usePhysics

Indicates if the object shall be considered by the physics simulation

* **Type**: `boolean`
* **Required**: No, default: `false`

#### trigger.node.useGravity

Indicates if the gravity affects the object

* **Type**: `boolean`
* **Required**: No, default: `true`

#### trigger.node.mass

Mass of the object in kilogram

* **Type**: `number`
* **Required**: No, default: `1.0`

#### trigger.node.bounciness

Provides the amount that an object bounces in response to a collision

* **Type**: `number`
* **Required**: No, default: `0`
* **Minimum**: ` >= 0`
* **Maximum**: ` <= 1`

#### trigger.node.roughness

Provides the roughness of an object which affects how much the object moves in response to a collision

* **Type**: `number`
* **Required**: No, default: `0.5`
* **Minimum**: ` >= 0`
* **Maximum**: ` <= 1`

#### trigger.node.use_geocoordinates

Indicates if geo coordinates are used

* **Type**: `boolean`
* **Required**: No

#### trigger.node.allow_occlusion

Indicates if occlusion by other nodes should be considered

* **Type**: `boolean`
* **Required**: No, default: `true`

#### trigger.node.upper_distance_weight

The weight applied to the distanceUpperLimit parameter defined at scene level 

* **Type**: `number`
* **Required**: No, default: `1`

#### trigger.node.lower_distance_weight

The weight applied to the distanceLowerLimit parameter defined at scene level 

* **Type**: `number`
* **Required**: No, default: `1`

#### trigger.node.geo_coordinate

Index to geo coordinate item in array in MPEG_GEO_COORDINATES extension 

* **Type**: `integer`
* **Required**: No

#### trigger.node.userInputParameters

Provides additional information related to the user inputs 

* **Type**: `string` `[1-*]`
* **Required**: No

#### trigger.node.visibilityFull

Indicates if the visibility shall be full or partial to activate the trigger

* **Type**: `boolean`
* **Required**: No, default: `true`

#### trigger.node.nodes

Set of nodes that shall not be considered for the visibility computation

* **Type**: `integer` `[1-*]`
* **Required**: No

#### trigger.node.mesh

Index of the mesh in the scene meshes array that will be used to compute visibility

* **Type**: `integer`
* **Required**: No

#### trigger.node.extensions

JSON object with extension-specific objects.

* **Type**: `extension`
* **Required**: No
* **Type of each property**: Extension

#### trigger.node.extras

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
