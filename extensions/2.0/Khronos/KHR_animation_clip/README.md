# KHR\_animation\_clip

## Contributors

* Ben Houston, Threekit
* Jan Hermes, Continental
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Jim Eckerlein, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The animation clip extension allows to easier define the behaviour of one or more animations.

In general, a keyframe calculated and provided by a viewer starts at zero and is incremented to infinity. The current calculated keyframe is called the *time stamp*.

A glTF animation does have a keyframe start and a keyframe end. Today, it is not clearly defined, if the global keyframe - the *time stamp* - becomes larger than the end value of the animation. Many implementations start to loop the animation, but stopping or reverting the animation are feasible options as well. This extension is defining this behaviour as well.

This extension is defining the calculation of the *time stamp* used for the glTF keyframe animations in a deterministic way.

## *Time stamp* out of bounds

The *time stamp* is clamped to the first and last calculated keyframes. Thus an animation stops playing if it has reached its end.
Due to repetition, the last keyframe is not necessarily equal to the specified end time.

## Properties

See the [schema](schema/clip.schema.json).

## Examples

Minimum valid animation clip entry referencing animation 0:

```json
{
    "animations": [
        0
    ]
}
```

Animation clip with all values set:

```json
{
    "animations": [
        0
    ],
    "name": "Forth and back animation.",
    "start": 0.0,
    "end": 10.0,
    "offset": 0.0,
    "speed": 1.0,
    "repetitions": 2,
    "reverse": true
}
```

*Total Time* is the accumulated time in seconds since the start of rendering.

*Time Stamp* is the internal time in seconds of the animation clip.

### Example 1

| Total Time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Time Stamp | 00  | 01  | 02  | 03  | 04  | 00  | 01  | 02  | 03  | 04  | 00  | 01  | 02  | 03  | 04  | 00  | 01  | 02  | 03  | 04  | 00  |

```json
{
    "animations": [
        0
    ],
    "name": "Example 1",
    "start": 0.0,
    "end": 4.0,
    "offset": 0.0,
    "speed": 1.0,
    "repetitions": -1,
    "reverse": false
}
```

### Example 2

| Total Time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Time Stamp | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  | 22  |

```json
{
    "animations": [
        0
    ],
    "name": "Example 2",
    "start": 2.0,
    "offset": 0.0,
    "speed": 1.0,
    "repetitions": -1,
    "reverse": false
}
```

### Example 3

| Total Time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Time Stamp | 03  | 04  | 05  | 06  | 07  | 08  | 03  | 04  | 05  | 06  | 07  | 08  | 03  | 04  | 05  | 06  | 07  | 08  | 03  | 04  | 05  |

```json
{
    "animations": [
        0
    ],
    "name": "Example 3",
    "start": 3.0,
    "end": 8.0,
    "offset": 0.0,
    "speed": 1.0,
    "repetitions": -1,
    "reverse": false
}
```

### Example 4

| Total Time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Time Stamp | 00  | 01  | 02  | 03  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  |

```json
{
    "animations": [
        0
    ],
    "name": "Example 4",
    "start": 0.0,
    "end": 4.0,
    "offset": 0.0,
    "speed": 1.0,
    "repetitions": 1,
    "reverse": false
}
```

### Example 5

| Total Time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Time Stamp | 03  | 04  | 05  | 04  | 03  | 02  | 01  | 02  | 03  | 04  | 05  | 04  | 03  | 02  | 01  | 01  | 01  | 01  | 01  | 01  | 01  |

```json
{
    "animations": [
        0
    ],
    "name": "Example 5",
    "start": 1.0,
    "end": 5.0,
    "offset": 2.0,
    "speed": 1.0,
    "repetitions": 4,
    "reverse": true
}
```

### Example 6

| Total Time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Time Stamp | 04  | 04  | 03  | 03  | 02  | 02  | 01  | 01  | 00  | 00  | 01  | 01  | 02  | 02  | 03  | 03  | 04  | 04  | 04  | 04  | 04  |

```json
{
    "animations": [
        0
    ],
    "name": "Example 6",
    "start": 0.0,
    "end": 4.0,
    "offset": 4.0,
    "speed": -0.5,
    "repetitions": 2,
    "reverse": true
}
```

### Sample implementation

```cpp
void initTimestamp(AnimationClip& ac, float& timestamp) {
    assert(ac.offset >= 0);
    assert(ac.offset <= ac.end);
    timestamp = ac.start + ac.offset;
}

// Delta time is in seconds. Delta time is the time passed between the current and last frame.
void updateTimestamp(float deltaTime, AnimationClip& ac, float& timestamp) {
    // Early exit if the animation has been repeated the specified number of times.
    if (ac.repetitions == 0) {
        return;
    }
    
    // Early exit if the animation is essentially frozen because its speed is zero.
    if (ac.speed == 0.0f) {
        return;
    }

    // Increase timestamp depending on delta time and speed.
    timestamp += ac.speed * deltaTime;

    // Test if the timestamp is out of the time bounds.
    if (timestamp > ac.end) {
        float overtime = timestamp - ac.end;

        if (ac.reverse)
        {
            timestamp = ac.end - overtime;
            ac.speed *= -1.0f;
        }
        else
        {
            timestamp = ac.start + overtime;
        }

        // Note: repetitions = -1 is infinite execution, repetitions = 0 is animation off.
        if (ac.repetitions > 0)
        {
            ac.repetitions--;
        }
    }
    else if (timestamp < ac.start) {
        float overtime = ac.start - timestamp;

        if (ac.reverse) {
            timestamp = ac.start + overtime;
            ac.speed *= -1.0f;
        }
        else {
            timestamp = ac.end - overtime;
        }

        // Note: repetitions = -1 is infinite execution, repetitions = 0 is animation off.
        if (ac.repetitions > 0) {
            ac.repetitions--;
        }
    }

    // Always keep start <= timestamp <= end.
    // Required for repetitions = -1 or robustness (very small differences between start and end).
    if (timestamp < ac.start) {
        timestamp = ac.start;
    }
    else if (timestamp > ac.end) {
        timestamp = ac.end;
    }
}
```
