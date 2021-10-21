# KHR_outputmapping_pq

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
Output pixel values from a rendered 3D model are generally in a range that is larger than that of a display device.  
This may not be a problem if the output is a high definition image format or some other target that has the same range and precision as the internal calculations.  
However, a typical usecase for realtime rasterizer implementations is that the output is a light emitting display.  
Such a display rarely has the range and precision of internal calculations making it necessary to map internal pixel values to match the characteristics of the output.  
This mapping is generally referred to as tone-mapping, a term that we will use here.  

Here the term rasterizer means a rendering engine that consits of a system wherein a buffer containing the pixel values for each frame is prepared. 
This buffer will be referred to as the framebuffer.  
The framebuffer can be of varying range, precision and colorspace. This has an impact on the color gamut that can be displayed.  
  
After completion of one framebuffer it is output to the display, this is usually done by means of a swap-chain. The details of how the swap works is outside the scope of this extension.  
KHR_outputmapping_pq specifies one method of mapping internal pixel values to that of the framebuffer.  

## Internal range

When the KHR_outputmapping_pq extension is used all lighting and pixel calculations shall be done using the value 10000 (cd / m2) as the maximum ouput brightness.  
This does not have an impact on color texture sources since they define values as contribution factor.  
The value 10000 for an output pixel with full brightness is chosen to be compatible with the Perceptual Quantizer (PQ) used in the SMPTE ST 2084 transfer function.  

When using this extension any light contribution values shall be aligned to account for 10000 cd / m2 as max luminance. This means that content creators shall be aware of 10000 cd/m2 as the maximum brightness value.  
Values above 10000 shall be clamped.  


## Outputmapping

If the framebuffer format and colorspace is known to the implementation then a format and colorspace shall be chosen to preserve the range and precision of the SMPTE ST 2084 transfer function.  
If the framebuffer format or colorspace is not known, or none is available that preserves range, precision and color gamut then lower range framebuffer with sRGB colorspace may be used.  
This is to allow for compatibility with displays that does not support higher range and/or compatible colorspaces.  
It also allows for implementations where the details of the framebuffer is not known or available.  



## Schema


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