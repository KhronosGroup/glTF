# MPEG_scene_interactivity

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
<a name="reference-mpeg_scene_interactivity"></a>
### MPEG_scene_interactivity

MPEG scene interactivity extension allows the support of XR interactivity feautures at the scene level.

**`MPEG_scene_interactivity` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**triggers**|`trigger` `[1-*]`|An array of triggers descriptions.| &#10003; Yes|
|**actions**|`action` `[1-*]`|An array of actions descriptions.| &#10003; Yes|
|**behaviors**|`behavior` `[1-*]`|An array of behaviors descriptions.| &#10003; Yes|
|**extensions**|`extension`|JSON object with extension-specific objects.|No|
|**extras**|`extras`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_scene_interactivity.schema.json](./schema/MPEG_scene_interactivity.schema.json)

#### MPEG_scene_interactivity.triggers

An array of triggers descriptions.

* **Type**: `trigger` `[1-*]`
* **Required**:  &#10003; Yes

#### MPEG_scene_interactivity.actions

An array of actions descriptions.

* **Type**: `action` `[1-*]`
* **Required**:  &#10003; Yes

#### MPEG_scene_interactivity.behaviors

An array of behaviors descriptions.

* **Type**: `behavior` `[1-*]`
* **Required**:  &#10003; Yes

#### MPEG_scene_interactivity.extensions

JSON object with extension-specific objects.

* **Type**: `extension`
* **Required**: No
* **Type of each property**: Extension

#### MPEG_scene_interactivity.extras

Application-specific data.

* **Type**: `extras`
* **Required**: No




---------------------------------------
<a name="reference-action"></a>
### MPEG_scene_interactivity.action object

glTF extension to specify action formats

**`MPEG_scene_interactivity.action object` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**type**|`string`|the type of the action.Value is oneOf ACTION_ACTIVATE, ACTION_TRANSFORM, ACTION_BLOCK, ACTION_ANIMATION, ACTION_MEDIA, ACTION_MANIPULATE, ACTION_SET_MATERIAL, ACTION_SET_HAPTIC and can be extended| &#10003; Yes|
|**delay**|`number`|Duration of delay in second before executing the action|No|
|**activationStatus**|`any`|ENABLED=0: the node shall be processed or skipped by the application. <br/>Required if type is ACTION_ACTIVATE.|No|
|**nodes**|`integer` `[1-*]`|Indices of the nodes in the nodes array to be considered. <br/>Required if type is ACTION_ACTIVATE, ACTION_TRANSFORM, ACTION_BLOCK, ACTION_MANIPULATE, ACTION_SET_MATERIAL and ACTION_SET_HAPTIC.|No|
|**transform**|`number` `[16-*]`|transformation matrix to apply to the nodes. <br/>Required if type is ACTION_TRANSFORM.|No|
|**animation**|`integer`|index of the animation in the animations array to be considered. <br/>Required if type is ACTION_ANIMATION.  |No|
|**animationControl**|`string`|defines the control of the animation. <br/>Required if type is ACTION_ANIMATION.<br/>Value is oneOf ANIMATION_PLAY, ANIMATION_PAUSE, ANIMATION_STOP, ANIMATION_RESUME and can be extended|No|
|**media**|`integer`|Index of the media in the MPEG media array to be considered.<br/>Required if type is ACTION_MEDIA.|No|
|**mediaControl**|`string`|defines the control of the media.<br/>Required if type is ACTION_MEDIA. <br/>Value is oneOf MEDIA_PLAY, MEDIA_PAUSE, MEDIA_STOP, MEDIA_RESUME and can be extended|No|
|**manipulate_action_type**|`string`|defines the manipulation.<br/>Required if type is ACTION_MANIPULATE.<br/>Value is oneOf ACTION_MANIPULATE_FREE, ACTION_MANIPULATE_SLIDE, ACTION_MANIPULATE_TRANSLATE, ACTION_MANIPULATE_ROTATE,ACTION_MANIPULATE_SCALE and can be extended|No|
|**axis**|`number` `[3]`|coordinates of the axis used for rotation and sliding. <br/>Required if type is ACTION_MANIPULATE.|No|
|**material**|`integer`|Index of the material in the materials array to apply to the nodes.<br/>Required if type is ACTION_SET_MATERIAL.|No|
|**extensions**|`extension`|JSON object with extension-specific objects.|No|
|**extras**|`extras`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_scene_interactivity.action.schema.json](./schema/MPEG_scene_interactivity.action.schema.json)


#### action.type

the type of the action.Value is oneOf ACTION_ACTIVATE, ACTION_TRANSFORM, ACTION_BLOCK, ACTION_ANIMATION, ACTION_MEDIA, ACTION_MANIPULATE, ACTION_SET_MATERIAL, ACTION_SET_HAPTIC and can be extended

* **Type**: `string`
* **Required**:  &#10003; Yes

#### action.delay

Duration of delay in second before executing the action

* **Type**: `number`
* **Required**: No

#### action.activationStatus

ENABLED=0: the node shall be processed or skipped by the application

* **Type**: `any`
* **Required**: No
* **Allowed values**:
    * `ENABLED`
    * `DISABLED `

#### action.nodes

Indices of the nodes in the nodes array to be considered

* **Type**: `integer` `[1-*]`
* **Required**: No

#### action.transform

transformation matrix to apply to the nodes

* **Type**: `number` `[16-*]`
* **Required**: No

#### action.animation

index of the animation in the animations array to be considered 

* **Type**: `integer`
* **Required**: No

#### action.animationControl

defines the control of the animation.Value is oneOf ANIMATION_PLAY, ANIMATION_PAUSE, ANIMATION_STOP, ANIMATION_RESUME and can be extended

* **Type**: `string`
* **Required**: No

#### action.media

Index of the media in the MPEG media array to be considered

* **Type**: `integer`
* **Required**: No

#### action.mediaControl

defines the control of the animation.Value is oneOf MEDIA_PLAY, MEDIA_PAUSE, MEDIA_STOP, MEDIA_RESUME and can be extended

* **Type**: `string`
* **Required**: No

#### action.manipulate_action_type

defines the control of the animation.Value is oneOf ACTION_MANIPULATE_FREE, ACTION_MANIPULATE_SLIDE, ACTION_MANIPULATE_TRANSLATE, ACTION_MANIPULATE_ROTATE,ACTION_MANIPULATE_SCALE and can be extended

* **Type**: `string`
* **Required**: No

#### action.axis

coordinates of the axis used for rotation and sliding

* **Type**: `number` `[3]`
* **Required**: No

#### action.material

Index of the material in the materials array to apply to the nodes

* **Type**: `integer`
* **Required**: No

#### action.extensions

JSON object with extension-specific objects.

* **Type**: `extension`
* **Required**: No
* **Type of each property**: Extension

#### action.extras

Application-specific data.

* **Type**: `extras`
* **Required**: No




---------------------------------------
<a name="reference-behavior"></a>
### MPEG_scene_interactivity.behavior object

glTF extension to specify behavior formats

**`MPEG_scene_interactivity.behavior object` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**triggers**|`integer` `[1-*]`|Indices of the triggers in the triggers array considered for this behavior| &#10003; Yes|
|**actions**|`integer` `[1-*]`|Indices of the actions in the actions array considered for this behavior| &#10003; Yes|
|**triggersCombinationControl**|`string`|Set of logical operations to apply to the triggers| &#10003; Yes|
|**triggersActivationControl**|`string`|Value is oneOf TRIGGER_ACTIVATE_FIRST_ENTER, TRIGGER_ACTIVATE_EACH_ENTER, TRIGGER_ACTIVATE_ON, TRIGGER_ACTIVATE_FIRST_EXIT, TRIGGER_ACTIVATE_EACH_EXIT, TRIGGER_ACTIVATE_OFF and can be extended| &#10003; Yes|
|**actionsControl**|`any`|Defines the way to execute the defined actions.| &#10003; Yes|
|**interruptAction**|`integer`|Index of the action in the actions array to be executed if the behavior is still on-going and is no more defined in a newly received scene update|No|
|**priority**|`number`|Weight associated to the behavior. Used to select a behavior when several behaviors are active at same time for one node| &#10003; Yes|
|**extensions**|`any`||No|
|**extras**|`any`||No|

Additional properties are allowed.

* **JSON schema**: [MPEG_scene_interactivity.behavior.schema.json](./schema/MPEG_scene_interactivity.behavior.schema.json)

#### behavior.triggers

Indices of the triggers in the triggers array considered for this behavior

* **Type**: `integer` `[1-*]`
* **Required**:  &#10003; Yes

#### behavior.actions

Indices of the actions in the actions array considered for this behavior

* **Type**: `integer` `[1-*]`
* **Required**:  &#10003; Yes

#### behavior.triggersCombinationControl

Set of logical operations to apply to the triggers

* **Type**: `string`
* **Required**:  &#10003; Yes

#### behavior.triggersActivationControl

Value is oneOf TRIGGER_ACTIVATE_FIRST_ENTER, TRIGGER_ACTIVATE_EACH_ENTER, TRIGGER_ACTIVATE_ON, TRIGGER_ACTIVATE_FIRST_EXIT, TRIGGER_ACTIVATE_EACH_EXIT, TRIGGER_ACTIVATE_OFF and can be extended

* **Type**: `string`
* **Required**:  &#10003; Yes

#### behavior.actionsControl

Defines the way to execute the defined actions.

* **Type**: `any`
* **Required**:  &#10003; Yes
* **Allowed values**:
    * `SEQUENTIAL`
    * `PARALLEL `

#### behavior.interruptAction

Index of the action in the actions array to be executed if the behavior is still on-going and is no more defined in a newly received scene update

* **Type**: `integer`
* **Required**: No

#### behavior.priority

Weight associated to the behavior. Used to select a behavior when several behaviors are active at same time for one node

* **Type**: `number`
* **Required**:  &#10003; Yes

#### behavior.extensions

* **Type**: `any`
* **Required**: No

#### behavior.extras

* **Type**: `any`
* **Required**: No




---------------------------------------
<a name="reference-trigger"></a>
### MPEG_scene_interactivity.trigger object

glTF extension to specify trigger formats

**`MPEG_scene_interactivity.trigger object` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**type**|`string`|the type of the trigger. Value is oneOf TRIGGER_COLLISION, TRIGGER_PROXIMITY, TRIGGER_USER_INPUT,TRIGGER_VISIBILITY and can be extended| &#10003; Yes|
|**nodes**|`integer` `[1-*]`|Indices of the nodes in the nodes array to be considered for collision determination|No|
|**distanceLowerLimit**|`number`|Threshold min in meters for the node proximity calculation. <br/>Required if type is TRIGGER_PROXIMITY.|No|
|**distanceUpperLimit**|`number`|Threshold max in meters for the node proximity calculation. <br/>Required if type is TRIGGER_PROXIMITY.|No|
|**referenceNode**|`integer`|Index in the nodes array, of the node to consider for the proximity evaluation. <br/>Required if type is TRIGGER_PROXIMITY.<br/>Default is default Camera|No|
|**userInputDescription**|`string`|Describe the user body part and gesture related to the input. <br/>Required if type is TRIGGER_USER_INPUT.|No|
|**cameraNode**|`integer`|Index to the node containing a camera in the nodes array for which the visibilities are determined. <br/>Required if type is TRIGGER_VISIBILITY. |No|
|**extensions**|`extension`|JSON object with extension-specific objects.|No|
|**extras**|`extras`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_scene_interactivity.trigger.schema.json](./schema/MPEG_scene_interactivity.trigger.schema.json)

#### trigger.type

the type of the trigger. Value is oneOf TRIGGER_COLLISION, TRIGGER_PROXIMITY, TRIGGER_USER_INPUT,TRIGGER_VISIBILITY and can be extended

* **Type**: `string`
* **Required**:  &#10003; Yes

#### trigger.nodes

Indices of the nodes in the nodes array to be considered for collision determination

* **Type**: `integer` `[1-*]`
* **Required**: No

#### trigger.distanceLowerLimit

Threshold min in meters for the node proximity calculation

* **Type**: `number`
* **Required**: No

#### trigger.distanceUpperLimit

Threshold max in meters for the node proximity calculation

* **Type**: `number`
* **Required**: No

#### trigger.referenceNode

Index in the nodes array, of the node to consider for the proximity evaluation. Default is default Camera

* **Type**: `integer`
* **Required**: No

#### trigger.userInputDescription

Describe the user body part and gesture related to the input.

* **Type**: `string`
* **Required**: No

#### trigger.cameraNode

Index to the node containing a camera in the nodes array for which the visibilities are determined 

* **Type**: `integer`
* **Required**: No

#### trigger.extensions

JSON object with extension-specific objects.

* **Type**: `extension`
* **Required**: No
* **Type of each property**: Extension

#### trigger.extras

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
