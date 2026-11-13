# MPEG_anchor extensions

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC DIS 23090-14 2nd Edition](https://www.iso.org/standard/90191.html)

## Dependencies

Written against the glTF 2.0 spec.

##  Overview
AR anchoring allows for anchoring of a scene or some root nodes of a scene into the user’s physical environment to provide an AR experience. AR anchoring is supported at the scene level and at the node level, as well as an extension through the definition of the MPEG_anchor extension. 
Multiple AR anchoring is then supported by adding the MPEG_anchor extension to several root nodes.

## 	Semantics
### [Semantics at glTF level](#semantics-at-gltf-level-1)
### [Semantics at scene or node level](#semantics-at-scene-or-node-level-1)


### Semantics at glTF level

The MPEG_anchor extension consists of Trackable and Anchor objects as provided in Table 1. The definition of Trackable and Anchor objects shall be provided within an MPEG_anchor extension at the glTF file level.

Table 1 – MPEG_anchor extension provided at the glTF file level
| Name | Type | Required | Default | Description |
|--|--|--|--|--|
| trackables | [`MPEG_anchor.trackable`](#reference-mpeg_gltf_anchor-trackable) `[1-*]` |  &#10003; Yes|  | Provides a list of trackables used by the anchor objects. |
| anchors | [`MPEG_anchor.anchor`](#reference-mpeg_gltf_anchor-anchor) `[1-*]`  |  &#10003; Yes|  | Provides a list of anchors for a scene or for root nodes to enable AR anchoring |

<a id="reference-mpeg_gltf_anchor-anchor"></a>
The syntax and semantics of the Anchor object is provided in Table 2. 

Table 2 – Definition of the Anchor object
| Name | Type | Required | Default | Description |
|--|--|--|--|--|
| trackable | integer |  &#10003; Yes|  | Index of the trackable in the trackables array that will be used for this anchor. |
| requiresAnchoring | boolean |  &#10003; Yes|  | Indicates if AR anchoring is required for the rendering of the associated nodes. </br>If TRUE, the application shall skip the virtual assets attached to this anchor until the pose of this anchor in the real world is known.</br>if FALSE, the application shall process the virtual assets attached to this anchor |
| minimumRequiredSpace | VEC3 | No | (0,0,0) | Space required to anchor the AR asset (width, height, depth in meters). This space corresponds to an axis-aligned bounding box, placed at the origin of the trackable space. Width is aligned with x axis, height with y axis and depth with z axis, expressed in trackable local space.  This value shall be compared to the bounding box of the real-world available space associated with the trackable as estimated by the XR runtime. |
| aligned | enumeration | No | NOT_USED | The aligned flag may take one of the following values: NOT_USED=0, ALIGNED_NOTSCALED=1, ALIGNED_SCALED=2.</br>If ALIGNED_SCALED is set, the bounding box of the virtual assets attached to that anchor is aligned and scaled to match the bounding box of the real-world available space associated with the trackable as estimated by the XR runtime. |
| actions | array(number) | No | N/A | Indices of the actions in the actions array of the interactivity extension to be executed once the pose of this anchor is determined. An example is a setTransform action to place the virtual assets attached to that anchor. |
| light | integer | No | N/A | Reference to an item in the lights array of the MPEG_lights_texture_based extension. |
|recommendedSpatialComputingConfig|[`recommendedSpatialComputingConfig`](#reference-mpeg_gltf_anchor-spatial)| No |N/A | Set of recommended parameters specifying the needed spatial description. |

<a id="reference-mpeg_gltf_anchor-spatial"></a>

Table 3: definition of the recommendedSpatialComputingConfig object
|Name |Type|Required|Default |Description|
|---|---|---|---|---|
|**scanOptions**|`integer` `[]`|No| |Array of one or more elements (enumeration) of the table 4 giving options for the scan generation|
|**scanDetails**|[`scanDetails`](#reference-mpeg_gltf_anchor-spatial-details)|No||The required level of detail for the mesh|
|**scanUpdate**|[`update`](#reference-mpeg_gltf_anchor-spatial-update)|No||Specifies the frequency of the update process.|
|**scanVolumes**|[`scanVolumes`](#reference-mpeg_gltf_anchor-spatial-volumes) `[]`|No||Array of bounding volumes that determine the spaces where scanned objects must be provided. Real scan objects that intersect one or more of the bounding volumes should be provided, and all other objects ignored. |
|**realSemantic**|`string` `[]`|No||Semantic descriptions of nodes that are needed.(“table”, “room”, “chair”, “wall”, “light”, “freespace” …)|
|**lightOptions**|`integer` `[]`|No||Array of one or more elements (enumeration) of the table 5 giving options for the light extraction.|No|
|**lightUpdate**|[`update`](#reference-mpeg_gltf_anchor-spatial-update)|No||Specifies the frequency of the update process.|No|


Table 4: Possible values of a scanOptions item
|Enumeration values   | Description|
|---|---|
|PLANE  = 0	|Request plane data for scanned objects|
|PLANAR_MESH	|Request planar meshes for scanned objects|
|VISUAL_MESH	|Request 3D visualization meshes for scanned objects|
|COLLIDER_MESH	|Request 3D collider meshes for scanned objects|
|FREE_VOLUME	|Request to get the available space around a trackable|
|BOUNDING_BOX	|Request a simplified collider mesh|
|TEXTURED_MESH	|Request mesh with a texture|


<a id="reference-mpeg_gltf_anchor-spatial-details"></a>
The definition of the scanDetails item is provided here after.

|Name   |Type|Required | Default| Description|
|---|---|---|---|---|
|**primitivesNumber**|`number`|No||The quantity of geometric primitives per m3|
|**textureOption**|`integer`|No| 0|The resolution of the texture of textured mesh. Possible values are R512x512 (0), R1024x1024 (1) or R2048x2048 (2)|

<a id="reference-mpeg_gltf_anchor-spatial-volumes"></a>
The definition of a scanVolumes item is provided here after.

|Name |Type|Required|Default| Description|
|---|---|---|---|---|
|**type**|`integer`|No | 0| The type of bounding volume. Possible values are SPHERE (0), BOX (1) or FRUSTUM (2).|
|**center**|`number` `[3]`|No | `[0,0,0]` | 3D coordinate of the center of the sphere.|
|**radius**|`number`|No | | Radius of the sphere in meters..|
|**pose**|`number` `[16-*]`|No | | 4x4 matrix representing the center position and orientation of the bounding volume.|No|
|**extents**|`number` `[3-*]`|No | | Edge-to-edge length of the box along each dimension.|
|**fov**|`number` `[4-*]`|No | |Angles of the four sides of the frustum|
|**far**|`number`|No | |Positive distance of the far plane of the frustum|
|**near**|`number`|No | |Positive distance of the near plane of the frustum|

Table 5: Possible values of a lightOptions item.
|Enumeration values   | Description|
|---|---|
|DirectionalLight = 0	|Request the extraction of directional lights|
|EnvLight	|Request the extraction of environment lights|
|PointLight	|Request the extraction of point lights|
|SpotLight	|Request the extraction of spot lights|
|AreaLight	|Request the extraction of area lights|

<a id="reference-mpeg_gltf_anchor-spatial-update"></a>
The definition of the recommendedSpatialComputingConfig.update object is providedhere after.

|Name  |Type|Required|Default| Description|
|---|---|---|---|---|
|**occurences**|`integer`|No | 0 |An occurrence value: <br> - ONCE = 0: the update is performed only once, for instance in case of a static real scene, <br>- N_FRAME: the update is performed periodically, every N rendering frames, N depending on the dynamism of the real scene. The N value is provided in the frameNumber parameter. <br>- AUTO: the update frequency is managed by the module that compute the representation. This module may perform an analysis to detect significant changes in the real world and start an update. This analysis may be performed from raw images data, like RGB images from a camera or depth images from a depth sensor.|
|**numberOfFrames**|`number`|No ||Indicate the periodicity, in number of frames, of the update, when the occurrences value is N_FRAME.|

<a id="reference-mpeg_gltf_anchor-trackable"></a>
The definition of the Trackable object is provided in the Table 6.

Table 6: Definition of the Trackable object
| Name | Type | Required | Default | Description |
|--|--|--|--|--|
| type | enumeration |  &#10003; Yes|  | The type of the trackable as defined in Table 7. |
| if (type == TRACKABLE_CONTROLLER) { |  |  |  |  |
| path | string |  &#10003; Yes|  | A path that describes the action space as specified by the OpenXR specification in clause 6.2. An example is “/user/hand/left/input”. |
| } |  |  |  |  |
| if (type== TRACKABLE_PLANE) { |  |  |  |  |
| geometricConstraint | enumeration |  &#10003; Yes|  | The geometricConstraint flag may take one of the following values:</br>HORIZONTAL_PLANE=0, VERTICAL_PLANE=1 |
| } |  |  |  |  |
| if (type== TRACKABLE_MARKER_2D OR MARKER_3D) { |  |  |  |  |
| markerNode | number |  &#10003; Yes|  | Index to the node in the nodes array in which the marker geometry and texture are described. |
| } |  |  |  |  |
| if (type== TRACKABLE_MARKER_GEO) { |  |  |  |  |
| coordinates | array |  &#10003; Yes|  | Array of three float numbers giving the longitude, the latitude, and the elevation of the geolocation of the center, that shall be as specified in [GeoJSON] for the point geometry type as follows:</br>the longitude in units of decimal degrees. The value is in range of −180.0 to 180.0, inclusive. Positive values represent eastern longitude and negative values represent western longitude.</br>the latitude in units of decimal degrees. The value is in range of −90.0 to 90.0, inclusive. Positive value represents northern latitude and negative value represents southern latitude.</br>the elevation is in units of meters above the WGS 84 reference ellipsoid. |
| } |  |  |  |  |
| if (type== TRACKABLE_APPLICATION) { |  |  |  |  |
| trackableId | string |  &#10003; Yes|  | An application-defined trackable id, that is known to the application. |
| } |  |  |  |  |

Table 7: Definition of the Trackable type
| Trackable type | Description |
|--|--|
| TRACKABLE_FLOOR = 0 | See [here](#trackable_floor)|
| TRACKABLE_VIEWER = 1 | See [here](#trackable_viewer) |
| TRACKABLE_CONTROLLER = 2 | See [here](#trackable_controller) |
| TRACKABLE_PLANE = 3 | See [here](#trackable_plane) |
| TRACKABLE_MARKER_2D = 4 | See [here](#trackable_marker_2d) |
| TRACKABLE_MARKER_3D = 5 | See [here](#trackable_marker_3d) |
| TRACKABLE_MARKER_GEO = 6 | See [here](#trackable_marker_geo) |
| TRACKABLE_APPLICATION = 7 | See [here](#trackable_application) |


### Semantics at scene or node level

Table 8: MPEG_anchor object instantiation at the node or scene level
| Name | Type | Required | Default | Description |
|--|--|--|--|--|
| anchor | integer |  &#10003; Yes|  | Reference to an item in the anchors array of the MPEG_anchor extension. |

### Trackable Definitions

#### TRACKABLE_FLOOR
A trackable of type TRACKABLE_FLOOR is an anchor with a plane that spans the xz-plane in the anchor’s local coordinate system. The origin of the local coordinate system is located at the center of the detected flat rectangular surface considered to be on the floor of the scene. The y-axis of the anchor’s local coordinate system is the plane’s normal vector and the X and Z axes are aligned with the rectangle edges.

NOTE	This corresponds to the XR_REFERENCE_SPACE_TYPE_STAGE defined in OpenXR in 7.1. Reference Spaces.
#### TRACKABLE_VIEWER
A trackable of type TRACKABLE_VIEWER is a trackable that corresponds to the viewer’s pose. 
It tracks the primary viewer with the center at the viewer’s position, with +Y up, +X to the right, and -Z forward. This trackable is viewer head locked.

NOTE	This corresponds to the XR_REFERENCE_SPACE_TYPE_VIEW defined in OpenXR in 7.1. Reference Spaces.

#### TRACKABLE_CONTROLLER
A trackable of type TRACKABLE_CONTROLLER is a trackable that corresponds to one of the active controllers. Three controller types are defined: grip, aim, and palm.

A grip pose allows applications to reliably render a virtual object held in the user’s hand, whether it is tracked directly or by a motion controller. The grip pose is defined as follows:
* The grip position:
  * For tracked hands: The user’s palm centroid when closing the fist, at the surface of the palm.
  * For handheld motion controllers: A fixed position within the controller that generally lines up with the palm centroid when held by a hand in a neutral position. This position should be adjusted left or right to center the position within the controller’s grip.
* The grip orientation
  * +X axis: When you completely open your hand to form a flat 5-finger pose, the ray that is normal to the user’s palm (away from the palm in the left hand, into the palm in the right hand).
  * -Z axis: When you close your hand partially (as if holding the controller), the ray that goes through the center of the tube formed by your non-thumb fingers, in the direction of little finger to thumb.
  * +Y axis: orthogonal to +Z and +X using the right-hand rule.

An aim pose allows applications to point in the world using the input source, according to the platform’s conventions for aiming with that kind of source. The aim pose is defined as follows:
* For tracked hands: The ray that follows platform conventions for how the user aims at objects in the world with their entire hand, with +Y up, +X to the right, and -Z forward. The ray chosen will be runtime-dependent, for example, a ray emerging from the palm parallel to the forearm.
* For handheld motion controllers: The ray that follows platform conventions for how the user targets objects in the world with the motion controller, with +Y up, +X to the right, and -Z forward. This is usually for applications that are rendering a model matching the physical controller, as an application rendering a virtual object in the user’s hand likely prefers to point based on the geometry of that virtual object. The ray chosen will be runtime-dependent, although this will often emerge from the frontmost tip of a motion controller.

A palm pose allows applications to reliably anchor visual content relative to the user’s physical hand, whether the user’s hand is tracked directly or its position and orientation is inferred by a physical controller. The palm pose is defined as follows:
* The palm position: The user’s physical palm centroid, at the surface of the palm.
* The palm orientation:
  * +X axis: When a user is holding the controller and straightens their index finger, the ray that is normal to the user’s palm (away from the palm in the left hand, into the palm in the right hand)
  * -Z axis: When a user is holding the controller and straightens their index finger, the ray that is parallel to their finger’s pointing direction.
  * +Y axis: orthogonal to +Z and +X using the right-hand rule.

NOTE	This corresponds to the Standard pose identifiers defined in OpenXR in 6.3.2. Input subpaths.

#### TRACKABLE_PLANE
The width and length of a plane span the xz-plane of the anchor instance's local coordinate system. The origin of the local coordinate system is located at the center of the detected flat rectangular surface. The y-axis of the plane anchor is the plane’s normal vector, and the X and Z axes are aligned with the rectangle edges.

#### TRACKABLE_MARKER_2D
The width and length of the marker 2D span the xz-plane of the anchor instance's local coordinate system. The origin of the local coordinate system is located at the center of the detected marker 2D surface. The y-axis of the anchor is the plane’s normal vector, and the X and Z axes are aligned with the rectangle edges.

#### TRACKABLE_MARKER_3D
For 3D models, the origin is the center of the mesh. The X, Y, and Z axes correspond to the axes of the world space.

#### TRACKABLE_MARKER_GEO
The y-axis matches the direction of gravity as detected by the device's motion sensing hardware, y points downward.
The x- and z-axes match the longitude and latitude directions. -Z points to true north -X points west. 



#### TRACKABLE_APPLICATION
The application-defined trackable object must have a right-handed coordinate space.

## 	Processing model
Each trackable provides a local reference space in which an anchor pose can be expressed. This local reference space is right-handed and depends on the type of trackable as described below.
The MPEG_anchor extension shall not be present at scene and node level at the same time in a glTF file conforming to this document. Upon activation of the XR mode, the Presentation Engine identifies the anchor points associated with the scene or with the root nodes and the related trackables. In the case of a trackable of type TRACKABLE_MARKER_GEO, it is up to the application to decide when to render the virtual object.

If the requiresAnchoring Boolean parameter of an anchor is set to TRUE, the application shall skip the virtual assets attached to this anchor until the pose of this anchor in the real world is known. Otherwise, the application shall process the virtual assets attached to this anchor.

Upon the detection of a trackable within the user’s physical environment, the application creates an XR space and possibly determines the bounding box of the real world available space in which the virtual assets can be added.

If the optional minimumRequiredSpace parameter of the anchor referencing the trackable is present and if the estimated real world available space is smaller in volume than the minimumRequiredSpace, the application shall skip the virtual assets attached to that anchor.

Otherwise, the application shall start the tracking, shall launch the actions provided in the actions parameter of the anchor and shall apply the necessary spatial transformations to the virtual assets with respect to the local space of the anchor prior to rendering.

The application could update at runtime a tracking status of each trackable defined in the trackables array at the scene level, for instance as follows:
* A tracking status set to TRUE when the trackable is detected and when its pose is well identified.
* A tracking status set to FALSE when the trackable position is lost (e.g., no more detected).

To provide the flexibility to define an anchor space different than the trackable’s space,
* The TRS of the node having the MPEG_anchor extension defines the relative transformation between the trackable’s space and the anchor space in which the child nodes containing the virtual assets are placed.
* A setTransform action may be defined to affect the scene root nodes in the case of a MPEG_anchor extension. This transformation defines the TRS between the trackable’s space and the anchor space in which all the scene root nodes are placed.

Actions are defined in MPEG_scene_interactivity extension.

If the array of action is not empty, the actions are executed once the pose of anchor is determined. If the tracking status is set to FALSE after being TRUE, actions are not canceled.


If a [`recommendedSpatialComputingConfig`](#reference-mpeg_gltf_anchor-spatial) object is present, the Presentation Engine checks if it can retrieve the recommended spatial description, specified in the  parameters such as scanOptions, scanDetails, scanUpdate, scanVolumes, scanSemantic, lightUpdate and lightOptions.
If all the recommended parameters are not satisfied, the Presentation Engine may continue the rendering of the scene with the available spatial description, but possibly with a degraded XR experience.
At runtime, the Presentation Engine may then request elements of the spatial description according to the recommendedSpatialComputingConfig parameters.


## Schemas

**JSON schemas**: [glTF level extension](./schema/glTF.MPEG_anchor.schema.json) and [scene or node level extension](./schema/MPEG_anchor.schema.json)

## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC DIS 23090-14 2nd Edition](https://www.iso.org/standard/90191.html), Information technology — Coded representation of immersive media — Part 14: Scene Description 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2025

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.

