# KHR_texture_video

## Contributors

* Dwight Rodgers, Adobe
* Gary Hsu, Microsoft
* Emmett Lalish, Google

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF models to use videos as textures. A client that does not implement this extension can ignore the provided video and continue to rely on the PNG and JPG textures available in the base specification. Defining a fallback texture is optional. 

Video textures support use cases such as water or other textures that continuously repeat, coordination of video texture playback by animation (in conjunction with `KHR_animation_pointer`), or control of video texture playback by interactivity (in conjunction with `KHR_interactivity`).

## glTF Schema Updates

Videos, for use as textures, are defined in the glTF scene description file by adding an `extensions` property to the top level glTF 2.0 object and defining a `KHR_texture_video` property with a `videos` array inside it. Each object in the `videos` array provides the URI of a video texture.

The `KHR_texture_video` extension is then added in an `extensions` property of each `textures` node that will use a video texture. The `source` property of the `KHR_texture_video` object points to the index of the video texture within the aforementioned `videos` object.

The following glTF will load `texture.webm` in clients that support this extension, and otherwise fall back to `image.png`.

```
"textures": [
    {
        "source": 0,
        "extensions": {
            "KHR_texture_video": {
                "source": 0,
                "playhead": 0,
                "autoplay": true,
                "loop": true
            }
        }
    }
],
"extensions": {
    "KHR_texture_video": {
        "videos": [
            {
                "uri": "texture.webm"
            }
        ]
    }
}
"images": [
    {
        "uri": "image.png"
    }
]
```

When used in the glTF Binary (.glb) format the `videos` node that points to the texture video uses the `mimeType` value of `video/webm;codecs=vp8` or `video/webm;codecs=vp9`. 

```
"textures": [
    {
        "source": 0,
        "extensions": {
            "KHR_texture_video": {
                "source": 1,
            }
        }
    }
],
"images": [
    {
        "mimeType": "image/png",
        "bufferView": 1
    }
]
"extensions": {
    "KHR_texture_video": {
        "videos": [
            {
                "mimeType": "video/webm;codecs=vp8",
                "bufferView": 2
            }
        ]
    }
]
```

### JSON Schema

todo: update
[glTF.KHR_texture_video.schema.json](schema/glTF.KHR_texture_video.schema.json)

## Fallback

When a fallback image is defined, this extension should not be present in `extensionsRequired`. This will allow all clients to render the glTF, and those that support this extension can display the animated texture.

## Video formats

Future extensions could add support for other video container mimetypes or related animation mimetypes such as animated GIFs, dotLottie, while retaining the same structure for controlling the animations, including the path to the playhead property (/textures/#/extensions/KHR_texture_video/playhead). This extension supports only webm container with vp8 or vp9 codec unless additional extensions are also used by the content to add support for other codecs or containers.

## Control by animation

`KHR_texture_video` extension can be used on animation channels to control the playhead on an object by object basis by using `KHR_texture_video` in conjunction with `KHR_animation_pointer`:

```
{
    "animations": [
        {
            "channels": [
                {
                    "sampler": 0,
                    "target": {
                        "path": "pointer",
                        "extensions": {
                                "KHR_animation_pointer" : {
                                    "pointer" : "/textures/0/extensions/KHR_texture_video/playhead"
                                }
                            }
                    }
                },
    ...
```

In this example, the playhead for all instances of texture 0 in the scene will be controlled by sampler 0, regardless of which nodes are using the texture.

The properties that are animatable are: `playhead`, `loop`. All other properties defined in this extension are not animatable.

## Control by interactivity

Controlling video playback via interactivity in conjunction with `KHR_interactivity` can be done in two ways:
Playback can be controlled by using the interactivity extension's capabilities for starting and stopping animations, and attaching the video texture to animation as above.
Playback can be controlled by directly manipulating, via json-pointer, the parameters listed as animatable above.

### Using Without a Fallback

To use video textures without a fallback, define `KHR_texture_video` in both `extensionsUsed` and `extensionsRequired`. The `texture` node will then have only an `extensions` property as shown below.

```
"textures": [
    {
        "extensions": {
            "KHR_texture_video": {
                "source": 1,
                "autoplay": true
            }
        }
    }
]
```


## Known Implementations

Video textures of this form are currently supported in threejs and Babylon, but currently must be created using custom JavaScript in the viewer. With support of this extension, the video texture could be created automatically by the GLTF loader and the GLTF animation could control the video playback.

## Resources

