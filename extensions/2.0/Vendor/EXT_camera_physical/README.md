<!--
SPDX-FileCopyrightText: 2026 saultoresan
SPDX-License-Identifier: CC-BY-4.0
-->

# EXT\_camera\_physical

## Contributors

- [@saultoresan](https://github.com/saultoresan), Individual Contributor
- _Additional contributors / implementers welcome — see [Open Questions](#open-questions)._

## Status

Proposal. **Not** ratified by, or endorsed by, the Khronos Group. Published for community discussion.

## Dependencies

Written against the glTF 2.0 specification. Applies to `camera` objects whose `type` is `"perspective"`.

## Overview

The core glTF 2.0 perspective camera stores only the parameters needed to build a projection matrix at runtime: a vertical field of view (`yfov`), an optional `aspectRatio`, a required `znear` and an optional `zfar`. This is sufficient to _render_ the correct frustum, but it discards the **physical camera** that authored that frustum.

This creates a concrete interoperability problem when cameras move between tools. A camera authored in a DCC tool, game engine or web runtime may have an explicit focal length and sensor size (often called _filmback_ in DCC and cinema tools). Exporting that camera to glTF reduces those values to `yfov` and `aspectRatio`. An importer can reconstruct the same centered, ideal pinhole projection from `yfov` and `aspectRatio`, but it cannot recover the authored focal length and physical sensor size that produced it.

In particular, focal length cannot be recovered from `yfov` alone, because field of view depends on **both** focal length and sensor size:

```
yfov = 2 * atan(sensorHeight / (2 * focalLength))
```

Many different `(focalLength, sensorHeight)` pairs produce the same `yfov` — halving both the sensor height and the focal length together leaves the ratio unchanged — so the original lens is ambiguous once only `yfov` is stored.

Seen as a round-trip, the loss is concrete. Take a 50 mm lens on a full-frame 36 × 24 mm sensor in a centered, vertical-fit camera:

- **Export to glTF** → `yfov = 2 · atan(0.024 / (2 · 0.050)) ≈ 0.471 rad (about 27°)`. The 50 mm and the 36 × 24 mm are gone; only `yfov` remains.
- **Re-import** → the tool has no sensor size, so to name a focal length it must **assume** one and solve for the rest. Assume a sensor half as tall and the same `yfov` yields a 25 mm "lens"; assume a different filmback and the answer changes again.

Every such reconstruction can preserve the same vertical field of view, but only the one matching the original filmback is the lens that was actually used. (Other gate-fit modes also account for the output frame; see [Relationship to the core perspective camera](#relationship-to-the-core-perspective-camera).) The conversion is only lossless one way:

- **filmback + focal length → field of view** — exact for the chosen fit. ✅
- **`yfov` → filmback + focal length** — undetermined without an assumed sensor. ❌

Storing `focalLength` and `sensorSize` alongside `yfov` removes the guess.

This loss matters for content that round-trips between digital content creation (DCC) tools, VFX and virtual-production pipelines, OpenUSD workflows, camera-matching / match-move, product visualization and physically based renderers.

`EXT_camera_physical` preserves that metadata **additively**. It does not replace or redefine the core camera: a viewer that does not understand the extension continues to use `camera.perspective` exactly as before.

### Evidence from current exporters

This loss is not hypothetical; it is the present behavior of shipping exporters. Cameras exported to glTF from two independent, widely used tools — Unreal Engine 5.7.1 and the Khronos Blender glTF I/O exporter v5.1.20 — reduce each camera to `yfov` and `aspectRatio` (plus `znear`/`zfar`). Neither exporter records the sensor size or focal length anywhere: not in an `extensions` block, not in `extras`, not in any vendor field. These are the tools' default glTF exports and are reproducible with the named versions.

The Unreal case is the clearest illustration. The source object is an explicit cinematic camera (`CineCameraActor`) that carries a defined filmback and focal length in the authoring tool, yet the export preserves neither — it does not even emit `zfar`. Because many `(focalLength, sensorSize)` pairs collapse to the same `yfov`, the discarded values cannot be reconstructed from the exported file. The interoperability gap this extension addresses is therefore observable today, in default exports, rather than a theoretical edge case.

## Scope and Priority

**glTF preserves projection today; this extension additionally preserves the focal-length/sensor-size pair needed to reconstruct the authored physical camera.**

The primary focus of this proposal is `focalLength` and `sensorSize`. They form the minimal inseparable pair needed to solve the interoperability gap described above and are the only required extension properties.

The draft also includes `fStop` and `focusDistance` as related optional candidates within the same physical-camera model. These properties are secondary and separable: they may be retained, revised or deferred without changing the core proposal. Sensor/lens shift (off-center frustums) is a recognized but separable topic and is deferred to keep this first version focused — see [Non-goals](#non-goals).

## Extending Cameras

The extension is added to a perspective `camera`. The following example describes the primary focal-length/sensor-size pair for a 35 mm lens on a 36 × 24 mm full-frame sensor, together with the two secondary optional candidates. For a centered camera rendered with vertical sensor fit to a matching 3:2 frame, the core `perspective` block is a consistent fallback (`yfov ≈ 2·atan(0.024 / (2·0.035)) ≈ 0.66059`, `aspectRatio = 0.036 / 0.024 = 1.5`); see [Relationship to the core perspective camera](#relationship-to-the-core-perspective-camera) for the general case.

```json
{
  "extensionsUsed": ["EXT_camera_physical"],
  "cameras": [
    {
      "type": "perspective",
      "perspective": {
        "yfov": 0.66059,
        "aspectRatio": 1.5,
        "znear": 0.01
      },
      "extensions": {
        "EXT_camera_physical": {
          "focalLength": 0.035,
          "sensorSize": [0.036, 0.024],
          "fStop": 2.8,
          "focusDistance": 5.0
        }
      }
    }
  ]
}
```

### Properties

| Name              | Type        | Description                                                   | Required |
| ----------------- | ----------- | ------------------------------------------------------------- | -------- |
| **focalLength**   | `number`    | Lens focal length, in meters.                                 | ✅ Yes   |
| **sensorSize**    | `number[2]` | Physical sensor size `[width, height]`, in meters.            | ✅ Yes   |
| **fStop**         | `number`    | Lens f-number, used by renderers that support depth of field. | No       |
| **focusDistance** | `number`    | Distance from the camera to the plane of focus, in meters.    | No       |

#### focalLength

The physical focal length of the lens, in **meters**. Must be greater than `0`. A typical 35 mm cinema/photo lens is `0.035`.

#### sensorSize

The physical sensor size as `[width, height]`, in **meters**. Both values must be greater than `0`. A 36 × 24 mm full-frame sensor is `[0.036, 0.024]`.

`sensorSize` is what makes `focalLength` unambiguous. Together with `focalLength` it describes the physical filmback. How that filmback maps to the rendered field of view and aspect ratio also depends on the camera's sensor-fit mode and the output frame — see [Relationship to the core perspective camera](#relationship-to-the-core-perspective-camera).

#### fStop

The lens f-number (focal length divided by the entrance-pupil diameter). Must be greater than `0`. This value is not required to build the frustum; it is provided for renderers that simulate depth of field.

#### focusDistance

The distance from the camera to the plane of focus, in **meters** (a scene-space distance). Must be greater than or equal to `0`. Like `fStop`, it is informative for depth-of-field rendering and does not affect the frustum.

## Relationship to the core perspective camera

This extension is **additive**: the core `camera.perspective` object remains **authoritative** for the rendered projection, and a viewer that does not understand the extension uses it exactly as before.

`focalLength` and `sensorSize` describe the physical filmback. The rendered field of view is **not** a function of the filmback alone — it also depends on the camera's **sensor-fit (gate-fit) mode** and the **output frame's aspect ratio**, which together decide which sensor axis is mapped to the rendered frame. In particular, `camera.perspective.aspectRatio` reflects the output frame, which is frequently _not_ equal to `sensorSize[0] / sensorSize[1]` (for example, a 36 × 24 mm / 3:2 filmback rendered to a 16:9 frame).

Exporters **should** keep `camera.perspective` consistent with the physical parameters under the camera's actual fit mode. For the common centered, vertical-fit case where the rendered aspect ratio equals the sensor aspect ratio:

```
camera.perspective.yfov        = 2 * atan(sensorSize[1] / (2 * focalLength))
camera.perspective.aspectRatio = sensorSize[0] / sensorSize[1]
```

In the general case (horizontal or fill fit, or a rendered aspect ratio that differs from the filmback) only the fitted sensor axis constrains the field of view, and `camera.perspective.aspectRatio` follows the output frame. Clients that read this extension to recover `(focalLength, sensorSize)` **must not** assume `aspectRatio = sensorSize[0] / sensorSize[1]`; `camera.perspective` remains the source of truth for the projection.

Because a valid fallback is always provided, this extension **should** be listed in `extensionsUsed` and **should not** be listed in `extensionsRequired`.

## Non-goals

To keep this first proposal small and reviewable, the following are intentionally **out of scope**. Several may warrant their own extensions.

- Exposure / image formation: ISO, shutter speed, exposure compensation, tone mapping / LUTs (see [#2128](https://github.com/KhronosGroup/glTF/pull/2128), [#2391](https://github.com/KhronosGroup/glTF/pull/2391)).
- Lens imperfections: distortion, vignetting, chromatic aberration, focus breathing.
- Aperture/bokeh shaping: blade count, anamorphic squeeze, T-stop.
- Rolling shutter and other temporal/sensor-readout effects.
- **Off-center / asymmetric frustums** (sensor or lens shift, tilt-shift lenses, two-point-perspective architectural cameras, stereo convergence). This is a recognized but separable topic: the core glTF perspective camera cannot represent a non-symmetric frustum exactly (see [#1813](https://github.com/KhronosGroup/glTF/issues/1813)). It is deferred to keep this first version focused on the focal-length/sensor-size pair, and may warrant its own treatment.
- **Animation of physical-camera properties** (focus pulls, zoom). In this version `focalLength`, `sensorSize`, `fStop` and `focusDistance` are **static** metadata: they are not registered in the glTF Object Model for [`KHR_animation_pointer`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_animation_pointer), and animating the core `yfov` would not update `focalLength`. Animated physical cameras are deferred to a future version.
- Calibrated computer-vision intrinsics (`fx`, `fy`, `cx`, `cy`, `skew`, `imageSize`) and distortion models. These solve an overlapping but distinct problem (pixel-space projection for reconstruction / photogrammetry / robotics) and may be better served by a separate `*_camera_intrinsics` extension.
- Arbitrary / explicit projection matrices.

## Open Questions

This section is the main purpose of publishing the proposal; feedback is requested on:

1. **Core proposal.** Should glTF preserve the authored `focalLength` and `sensorSize` pair in addition to its existing projection parameters?
2. **Units.** This draft uses **meters** for consistency with glTF's linear-distance convention. The optics industry universally uses **millimeters** for `focalLength` and `sensorSize`. Should those two properties be millimeters (matching authoring tools) while scene-space `focusDistance` stays in meters, or should everything stay in meters for internal consistency? (Note that OpenUSD is not a clean precedent here: `UsdGeomCamera` specifies focal length and aperture in _tenths of a scene unit_, which equals millimeters only when the stage's unit is centimeters.)
3. **Secondary properties.** Keep `fStop` and `focusDistance` in the initial version, or defer them without changing the core proposal? (`sensorShift` / off-center frustums have been moved to [Non-goals](#non-goals) for this version.)
4. **Attachment point.** Is `camera` the right object, versus the `node` that references it?

## Known Implementations

None yet — this is a proposal. Implementer interest from exporters (e.g. the Blender glTF exporter) and runtimes/loaders (e.g. three.js, Babylon.js, model-viewer) is actively sought; please comment on the pull request.

## Resources

- glTF 2.0 perspective camera schema: [`camera.perspective.schema.json`](https://github.com/KhronosGroup/glTF/blob/main/specification/2.0/schema/camera.perspective.schema.json)
- Related issue — arbitrary / non-symmetric frustum: [#1813](https://github.com/KhronosGroup/glTF/issues/1813)
- Adjacent PR — `KHR_image_formation` (exposure / tone mapping): [#2128](https://github.com/KhronosGroup/glTF/pull/2128)
- Adjacent PR — framebuffer light intensity mapping (explicitly _not_ a physical camera): [#2391](https://github.com/KhronosGroup/glTF/pull/2391)
- Prior-art camera model — OpenUSD `UsdGeomCamera`: [https://openusd.org/release/api/class_usd_geom_camera.html](https://openusd.org/release/api/class_usd_geom_camera.html)

## Schema

- [camera.EXT_camera_physical.schema.json](schema/camera.EXT_camera_physical.schema.json)
