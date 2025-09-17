# EXT_animation_map

## Contributors

* Michael Feldstein, Facebook, [@msfeldstein](https://twitter.com/msfeldstein)
* Ocean Quigley, Facebook, [@oceanquigley](https://twitter.com/oceanquigley)

## Status

Draft

## Dependencies

Written against the glTF draft 2.0 spec.

## Overview

This extensions maps optional semantic meaning to animations so that client applications can attach interactive and reactive behaviors to the 3d models and scenes. This can be useful to let a client know to play a certain animation when the model first enters a scene, or when a model is about to leave.

Facebook uses it to play animations when a model enters/leaves the feed, or scrubs an animation as the user scrolls the model through the screen.
It is also used to respond to user events, such as tapping, or horizontal panning

It maps a semantic (which can be any of the string-named events listed below), to an array of animation indices which point to a list of gltf Animations.  A semantic can have a list of glTF scene nodes indicating which nodes this trigger acts upon.  For example in the list below, a TRIGGER semantic can have a list of nodes indicating which nodes should be the hit-box, or GAZE can trigger only when certain nodes are gazed at. If this list of nodes is omitted, the semantic will act on all the nodes in the scene.

##### Formal Semantics

* **ENTER**: Trigger a single shot animation when the object first enters the scene or viewport
* **LEAVE**: Trigger a single shot animation as the object is leaving the scene or viewport
* **ALWAYS**: Constantly loop an animation

The following semantics can include a list of glTF Node objects to specify interaction targets.  The event is triggered by acting on any of the nodes listed.  If the nodes are omitted, it can be assumed that the entire glTF Scene is the target.

* **TRIGGER**: A user triggering a single shot animation on Touch, Tap, Click, or any other way of activating the nodes specified.
* **GRAB**: Triggered when an object is picked up, for example grabbed in vr, or dragged in WebGL.
* **HELD**: Looped during the entire duration an object is grabbed.
* **RELEASED**: Triggered when an object is released from a grab.
* **GAZE_ENTER**: Triggered when an object is first looked at
* **GAZE**: Looped  while an object is being looked at
* **GAZE_LEAVE**: Triggered when an object stops being looked at
* **PROXIMITY_ENTER**: Triggered when a viewer gets close to an object
* **PROXIMITY**: Looped while a viewer is near to an object
* **PROXIMITY_LEAVE**: Triggered when a viewer gets further from an object
* **POINTER_ENTER**: Triggered when pointer intersects an object
* **POINTING**: Looped while a user is pointing at an object
* **POINTER_LEAVE**: Triggered when a pointer exits an object

## glTF Schema Updates

The new EXT_animation_map property is a root glTF level optional extension object.  The **bindings** array is a list of objects containing a semantic, an array of animations mapped to that semantic, and an optional list of nodes that this animation targets.  The node parameter is used for interactive animations like GRAB or GAZE.  Animations should be played all at once, though future additions could add parallel (default), sequential, or random play modes.

**Listing 1**: An animation map that binds animations 0 and 3 to the ENTER event, 1 to the LEAVE event, and 2 to loop continuously for the life of the object.  There is a GAZE_ENTER event that is triggered when the specified node 3 is looked at.

```javascript
{
    "extensionsUsed": {
        "EXT_animation_map"
    },
    "extensions" : {
        "EXT_animation_map" : {
              "bindings": [
                  {
                      "semantic": "ENTER",
                      "animations": [0, 3]
                  },
                  {
                      "semantic": "LEAVE",
                      "animations": [1]
                  },
                  {
                      "semantic": "ALWAYS",
                      "animations": [2]
                  },
                  {
                      "semantic": "GAZE_ENTER",
                      "animations": [4],
                      "nodes": [3]
                  }
              ]
        }
    }
}
```

### JSON Schema

TODO: Links to the JSON schema for the new extension properties.

## Known Implementations

* None yet, will be implemented in Facebook 3d Posts

## Resources

*
