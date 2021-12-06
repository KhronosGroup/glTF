# KHR_displaymapping_pq

## Contributors

Copyright (C) 2021 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions


## Overview

The goal of this extension is to provide the means to map internal, usually floating point, light contribution values that may be in an unknown range to that of an attached display.  
This is so that the hue of the source materials is retained under the light conditions declared within the glTF scene.  
Correct representation of hue is important in order to keep artistic intent, or to achieve a physically correct visualization of products for instance in e-commerce.  

It also provides the specification for using HDR compatible display outputs while at the same time retaining compatibility with SDR display outputs.  
The intended usecases for this extension is any usecase where the light contribution values will go above 1.0, for instance by using KHR_lights_punctual, KHR_emissive_strength or KHR_environment_lights.  

Output pixel values from a rendered 3D model are generally in a range that is larger than that of a display device.  
This may not be a problem if the output is a high definition image format or some other target that has the same range and precision as the internal calculations.  
However, a typical usecase for realtime rasterizer implementations is that the output is a light emitting display.  
Such a display rarely has the range and precision of internal calculations making it necessary to map internal pixel values to match the characteristics of the output.  
This mapping is generally referred to as tone-mapping, however the exact meaning of tone-mapping varies and can also mean the process of applying artistic intent to the output.  
For that reason this document will use the term displaymapping.    

Here the term rasterizer means a rendering engine that consits of a system wherein a buffer containing the pixel values for each frame is prepared. 
This buffer will be referred to as the framebuffer.  
The framebuffer can be of varying range, precision and colorspace. This has an impact on the color gamut that can be displayed.  
  
After completion of one framebuffer it is output to the display, this is usually done by means of a swap-chain. The details of how the swap works is outside the scope of this extension.  
KHR_displaymapping_pq specifies one method of mapping internal pixel values to that of the framebuffer.  

This extension does not take the viewing environment of the display, or eye light adaptation, into consideration.  
It is assumed that the content is viewed in an environment that is dimly lit (~5 cd / m2) without direct light on the display.  


## Internal range

When the KHR_displaymapping_pq extension is used all lighting and pixel calculations shall be done using the value 10000 (cd / m2) as the maximum ouput brightness.  
This does not have an impact on color texture sources since they define values as contribution factor.  
The value 10000 cd / m2 for an output pixel with full brightness is chosen to be compatible with the Perceptual Quantizer (PQ) used in the SMPTE ST 2084 transfer function.  

When using this extension light contribution values shall be aligned to account for 10000 cd / m2 as max luminance.  
This means that content creators shall be aware of 10000 cd/m2 as the maximum brightness value range, it does not mean that the display will be capable of outputing at this light luminance.  

## Scene light value

A content creation tool supporting this extension shall sum upp light contribution for a scene before exporting to glTF, this can be a naive addition of all lights included in the scene that adds max values together.  
If scene max light contribution is above 10000 cd / m2 there is a choice to either downscale light values before export or to set `sceneAperture` to `AUTO`  
This means that implementations will calculate max light intensity for the scene and use as a scale factor to keep all light contribution within 0 - 10000 cd / m2.  

Light contribution values above 10000 cd / m2 is strongly discouraged and will be clamped by implementations if not adjusted by scene aperture.    

The internal precision shall at a minimum match the equivalent of 12 bits unsigned integer, which maps to the PQ.  


## Displaymapping

To convert from internal values (linear scene light) to the non-linear output value in the range 0.0 - 1.0 the PQ EOTF shall be used.  
This is specified in Rec. 2100:
https://www.itu.int/rec/R-REC-BT.2100/en  

If the framebuffer format and colorspace is known to the implementation then a format and colorspace shall be chosen to preserve the range and precision of the SMPTE ST 2084 transfer function.  
If available, a framebuffer colorspace that is compatible with the color primaries of Rec. 2020 shall be used.  
If this colorspace is used the source images must be converted from Rec. 709 linear values to Rec. 2020 linear values, after conversion has been done from sRGB to linear values.
Color conversion from Rec. 709 to Rec. 2020 is specified in Rec. 2087  
https://www.itu.int/rec/R-REC-BT.2087/en

The M2 linear color conversion matrix is defined as:  
0.6274 0.3293 0.0433  
0.0691 0.9195 0.0114  
0.0164 0.0880 0.8956  

If the framebuffer format or colorspace is not known, or none is available that preserves range, precision and color gamut then lower range framebuffer with sRGB colorspace may be used.  
This is to allow for compatibility with displays that does not support higher range and/or compatible colorspaces.  
It also allows for implementations where the details of the framebuffer is not known or available.  

Resulting linear scene light values shall be display mapped according to Rec. 2100:  
The opto-optical reference transform shall be applied, the reference transform compatible with both SDR and HDR displays is described in Rec. 2390 with the use of range extension and gamma values.  

E′ = G709[E] = pow(1.099 (rangeExtension E), 0.45) – 0.099 for 1 > E > 0.0003024
               267.84 E for 0.0003024 ≥ E ≥ 0
FD = G1886[E'] = pow(100 E′, gamma

Where the rangeExtension and gamma values can be set by this extension for HDR and SDR display outputs.  
* For HDR output,  a range extension value of 59.5208 and gamma of 2.4 is suggested, these values can be changed according to artistic intent.  

* For SDR output,  a range extension value of 46.42 and gamma of 2.4 is suggested, these values can be changed according to artistic intent.  


After the OOTF is applied the OETF shall be applied, this will yield a non linear output-signal in the range [0.0 - 1.0] that is stored in the display buffer.  

### Parameters

The following parameters are added by the `KHR_displaymapping_pq` extension:

| Name                   | Type       | Description                                    | Required           |
|------------------------|------------|------------------------------------------------|--------------------|
| **ootfHDR**         | `object`   | Opto-optical transfer function values for HDR display output, if no value is specified then the default parameters rangeExtension=59.5208 and gamma=2.4 is used  | No |
| **ootfSDR**         | `object`   | Opto-optical transfer function values for SDR display output, if no value is specified then the default parameters rangeExtension=46.42 and gamma=2.4 is used  | No |
| **sceneAperture**   | `object`   | Scene light adjustment setting, if no value supplied the default value of OFF will be used and scene light values will be kept unmodified.  OFF = No scene aperture control. Scene light values will be kept before applying display mapping. Any value above 10000 will be clamped.  AUTO = Auto scene aperture, light values are scaled according to `lightScale`, this is calculated using  `lightScale` = 10000 / `maxSceneLight`. Where `maxSceneLight` is updated for each frame.  | No |

## Schema

[glTF.KHR_displaymapping_pq.json](schema/glTF.KHR_displaymapping_pq.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2021 The Khronos Group Inc.

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
