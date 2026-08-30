<!--
Copyright 2018-2020 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_image\_formation

## Contributors

* Romain Guy, Google [@romainguy](https://twitter.com/romainguy)
* Emmett Lalish, Google [@emmettlalish](https://twitter.com/emmettlalish)

Copyright 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The scene referred values computed during the lighting pass using the glTF 2.0 spec are luminance values, expressed in cd/m<sup>2</sup> or nits. It follows that light incident to our virtual optical system can cover a large range of values when physical light units (KHR_lights_punctual) are used, from 10<sup>-5</sup> cd/m<sup>2</sup> for starlight to 10<sup>9</sup> cd/m<sup>2</sup> for the sun. However the common methods we have of storing and displaying images have limited dynamic range. Of course this problem does not just apply to rendering; the human eye also has limited dynamic range, and of course photographs printed on paper (or displayed on a screen) cannot possibly reproduce the range  of intensity that was incident on the lens. 

This extension is intended to allow the producer of a glTF to define all the parameters necessary for a consumer (renderer) to generate a consistent output image of the scene, given consistent lighting and view. Consistent does not imply pixel-identical, as the rendering is still subject to many arbitrary approximations, but simply that the output would be identical if the physics were respected exactly. What this extension defines is a function that maps [0, infinity) input light RGB to the [0, 1] output encoding color gamut (most commonly sRGB). In practice, this function has a much larger effect on the appearance of the output than the differences in renderer approximations.

Supporting this extension is only appropriate for rendering a single glTF scene; since it defines a global output mapping it is impossible to respect conflicting mappings for two glTFs in the same scene. Likewise, if a shop wanted a consistent “look” across  their portfolio, they might be better served by picking a consistent mapping function and requiring their artists to design with this mapping in mind, rather than displaying different products with unique parameters. 

This mapping function is split into two parts: the linear multiplier, exposure, and the nonlinear look-up tables (LUT). Exposure represents what the human pupil does to compensate for scenes with different average light levels, but here we will define it in terms from the photography world, as they have already created standard physical units to represent the camera’s version of the pupil. 

The non-linear LUT is much more complicated and arbitrary, but is nevertheless completely necessary. This compression step is necessary because the [0, infinity) range of physical light must be converted to the [0, 1] range of any output format. When no LUT is specified, this generally means the value is clamped, which is simply a piecewise linear compression function with sharp corners that  create unsightly artifacts. This is rarely the default mode for existing renderers, as it yields such poor results, but their  defaults are also rarely consistent with each other, hence the value of this extension.

There is no “right” way to compress outgoing scene light, but the goal is to mimic the nonlinear response of the human retina without a lot of expensive computation. Camera sensors and film are likewise nonlinear and designed also around the human eye’s response. This is the purpose of the hdr_lut. Effectively we are mapping from physical light units to unitless perception values, represented as sRGB or other wide-gamut, HDR formats. 

One key aspect of a good transform is that it does not operate solely on luminance but also affects color, especially saturation. This is because as a colored light becomes very bright, its color ceases to be perceived. This is true of CCDs as well, and can be easily seen by taking an overexposed photo of a bright, colored light and noting how it tends toward white. This is the purpose of the tone_mapping_lut.

Regarding color gamuts, since glTF defines its textures in sRGB, so long as the scene’s lighting is also represented in Rec 709 (sRGB’s color gamut), then the output light will naturally also be restricted to the Rec 709 gamut. If a wide gamut like Rec 2020 is used for output, then the values will naturally fall into the Rec 709 subset. 

## Extending Scene

The precise method of converting scene linear light into output pixels is defined by adding the `KHR_image_formation` extension to any glTF scene. For example:

```json
{
    "scene": [
        {
            "extensions": {
                "KHR_image_formation": {
                    "exposure": 1.0,
                    "hdr_lut": "hdr.cube",
                    "tone_mapping_lut": "tone.cube",
                }
            }
        }
    ]
}
```

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `exposure` | Linear multiplier on lighting. Must be a positive value. | No, Default: `1.0` |
| `hdr_lut` | Link to a 1D .cube file, defining the first post-processing step. | No |
| `tone_mapping_lut` | Link to a 3D .cube LUT file, defining the second post-processing step.  | No |

### Exposure

All incident light must be multiplied by the exposure factor to transform scene luminance into exposed luminance. This can be done in several places in a renderer:

* Multiply the result of a BRDF computation
* Multiply light intensities before computing lighting
* Multiply the result of light accumulation at the end of the lighting pass
* Multiply the input of the compression function
* Etc.

Note: all outgoing light must be multiplied by the exposure factor, including the emissive component of the materials present in the scene.

### HDR LUT

This LUT MUST be a [1D-type](https://wwwimages2.adobe.com/content/dam/acom/en/products/speedgrade/cc/pdfs/cube-lut-specification-1.0.pdf) Adobe .cube file. It is applied first, often called a shaping LUT, whose purpose is to effectively create non-linear spacing of the data points in the tone_mapping_lut. This is important because with the huge floating-point range of inputs, the mapping curve will need more detail in some regions than others. If this LUT is not provided, the second step is run directly on the scene’s linear light output. 

### Tone Mapping LUT

This LUT MUST be a [3D-type](https://wwwimages2.adobe.com/content/dam/acom/en/products/speedgrade/cc/pdfs/cube-lut-specification-1.0.pdf) Adobe .cube file. It is applied to the output of the HDR LUT in the same way as standard photographic post-processing effects. In this way, our physically-based rendering output can match the existing photography/video pipelines as closely as possible. If this LUT is not provided, the result of the HDR LUT is output directly.

### Default LUT

If neither LUT is provided, the default tone mapping is the widely-used [ACES standard](http://j.mp/TB-2014-004) (specifically ACES sRGB 120 nits dim surround), for which numerous renderers already have efficient analytical approximations, in which case both the file size and potential frame time effects are minimized. 

For situations where direct output of linear light is desired (particularly when rendering to a floating point output for further processing downstream), an identity LUT can be easily specified.


## Implementation

*This section is non-normative.*

### Exposure

Extra care must be taken in renderers wishing to perform computations in fp16 (half-floats). Half floats have limited range and precision that are not directly suited to many computations that rely on photometric light units and exposure as defined by this extension. For instance, a typical scene on a bright sunny clear day outside in the northern hemisphere would have the following characteristics:

* Sun light intensity set to 100,000 lux
* Camera exposure set to the equivalent of ƒ/16, 1/125s, ISO 100 (see Annex A)

In this example, the sunlight intensity is greater than the maximum value allowed by half floats, and the exposure computed from the camera settings (0.00002604) is smaller than the smallest normalized half float value.

Such renderers can solve this problem by using pre-exposed light intensities, multiplying light intensities at normal or double precision ahead of any lighting computations. This can be done on the CPU for instance before passing data to the GPU for rendering. In the example above, the sunlight intensity would be stored for rendering as 100_000.0 * 0.00002604 = 2.604, a value that fits both the range and precision constraints of half floats.

### Default ACES tone mapping

The ACES standard can always be implemented as an actual LUT, but it may be performance-beneficial to approximate it analytically instead. As with rendering, the approximation used and its accuracy is up to the implementer, but it should be noted that several popular approximations exhibit quite poor behavior, introducing significant color-shifts for high-intensity light rather than trending toward white properly. A better approximation that is still cheap can be found [here](https://github.com/mrdoob/three.js/blob/342946c8392639028da439b6dc0597e58209c696/examples/jsm/shaders/ACESFilmicToneMappingShader.js).

## Schema

- [glTF.KHR_image_formation.schema.json](schema/glTF.KHR_image_formation.schema.json)

## Reference

[Adobe Cube format](https://wwwimages2.adobe.com/content/dam/acom/en/products/speedgrade/cc/pdfs/cube-lut-specification-1.0.pdf)  
[ACES tone mapping](http://j.mp/TB-2014-004)  

## Annex A: Exposure from physically based parameters

Exposure can be computed from 3 physically based parameters. These parameters can be used to recreate a virtual camera akin to video or photography cameras, or to match known equipment and shooting conditions.

`aperture` controls how open or closed the optical system is at the pupil of the virtual lens. This value is expressed in f-stops, noted ƒ/. Since an f-stop indicates the ratio of the lens' focal length to the diameter of the entrance pupil, high-values (ƒ/16) indicate a small aperture (less light entering the virtual optical system) and small values (ƒ/1.4) indicate a wide aperture (more light entering the virtual optical system). This value can be used to inform other parts of the image formation pipeline, such as the depth of field to render a bokeh blur.

`shutterSpeed` controls how long the virtual sensor gathers incident light, and is expressed in seconds. The longer the shutter speed, the more incident light hits the sensor. While the shutter speed is expressed as a unit of time, it is only used to compute exposure and does not define how long a frame must take to render. This value can however be used to inform other parts of the image formation pipeline, such as the strength of a motion blur.

`sensitivity` controls how the light reaching the sensor is quantized. It is expressed in ISO, and as such this parameter is often referred to as simply the “ISO” or “ISO setting”. It is important to note that this parameter does not change how much incident light reaches the virtual sensor, it is simply a gain.

The equation to compute exposure is as follows:

`exposure = 1 / (1.2 * (aperture<sup>2</sup> / shutterSpeed) * (100 / sensitivity))`

| Property | Type | Description |
|:-----------------------|:------------| :-----------------------------------------------------|
| `aperture` | number | Aperture of the virtual lens in front of the virtual sensor, in f-stops. For instance, 1.4 for ƒ/1.4. |
| `shutterSpeed` | number | Shutter speed of the virtual camera, in seconds. For instance, 0.02 for 1/50s. |
| `sensitivity` | number  | Sensitivity (or gain) of the virtual sensor, expressed in ISO. For instance, 400 for ISO 400. |

A full derivation of this equation can be found in [Physically based rendering in Filament](https://google.github.io/filament/Filament.html#imagingpipeline/physicallybasedcamera/exposure).

Note: this equation assumes an imperfect lens with vignetting and transmittance attenuation. To model a perfect lens instead, remove the factor 1.2 from the equation.

## Appendix: Full Khronos Copyright Statement

Copyright 2018-2020 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.

This specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast
or otherwise exploited in any manner without the express prior written permission
of Khronos.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters and confirm the implementation as conformant under
the process defined by Khronos for this specification;
see https://www.khronos.org/adopters.

Khronos makes no, and expressly disclaims any, representations or warranties,
express or implied, regarding this specification, including, without limitation:
merchantability, fitness for a particular purpose, non-infringement of any
intellectual property, correctness, accuracy, completeness, timeliness, and
reliability. Under no circumstances will Khronos, or any of its Promoters,
Contributors or Members, or their respective partners, officers, directors,
employees, agents or representatives be liable for any damages, whether direct,
indirect, special or consequential damages for lost revenues, lost profits, or
otherwise, arising from or in connection with these materials.

Khronos® and Vulkan® are registered trademarks, and ANARI™, WebGL™, glTF™, NNEF™, OpenVX™,
SPIR™, SPIR-V™, SYCL™, OpenVG™ and 3D Commerce™ are trademarks of The Khronos Group Inc.
OpenXR™ is a trademark owned by The Khronos Group Inc. and is registered as a trademark in
China, the European Union, Japan and the United Kingdom. OpenCL™ is a trademark of Apple Inc.
and OpenGL® is a registered trademark and the OpenGL ES™ and OpenGL SC™ logos are trademarks
of Hewlett Packard Enterprise used under license by Khronos. ASTC is a trademark of
ARM Holdings PLC. All other product names, trademarks, and/or company names are used solely
for identification and belong to their respective owners.
