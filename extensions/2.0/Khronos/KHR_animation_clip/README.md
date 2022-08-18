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
	"offset": 2.0,
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
	"offset": 3.0,
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
| Time Stamp | 00  | 01  | 02  | 03  | 04  | 05  | 04  | 03  | 02  | 01  | 00  | 01  | 02  | 03  | 04  | 05  | 05  | 05  | 05  | 05  | 05  |

```json
{
	"animations": [
		0
	],
	"name": "Example 5",
	"start": 0.0,
	"end": 5.0,
	"offset": 0.0,
	"speed": 1.0,
	"repetitions": 3,
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
// Delta time is in seconds. Delta time is the time executed between the current and last frame.
void updateTimestamp(float deltaTime, AnimationClip& ac)
{
	// Animation clip is off.
	// Early quit for performance and not having to change the speed value.
	if (ac.repetitions == 0 || ac.speed == 0.0f)
	{
		return;
	}

	// Increase offset depending on delta time and speed.
	ac.offset += ac.speed * deltaTime;

	// Check, if the offset is larger or smaller than the bounds.	
	if (ac.offset > ac.end)
	{
		float overtime = ac.offset - ac.end;

		if (ac.reverse)
		{
			ac.offset = ac.end - overtime;

			ac.speed *= -1.0f;
		}
		else
		{
			ac.offset = ac.start + overtime;
		}

		// Note: repetitions = -1 is infinite execution, repetitions = 0 is animation off.
		if (ac.repetitions > 0)
		{
			ac.repetitions--;
		}
	}
	else if (ac.offset < ac.start)
	{
		float overtime = ac.start - ac.offset;

		if (ac.reverse)
		{
			ac.offset = ac.start + overtime;

			ac.speed *= -1.0f;
		}
		else
		{
			ac.offset = ac.end - overtime;
		}

		// Note: repetitions = -1 is infinite execution, repetitions = 0 is animation off.
		if (ac.repetitions > 0)
		{
			ac.repetitions--;
		}
	}

	// Always keep start <= offset <= end.
	// Required for repetitions = -1 or robustness (very small differences between start and end).
	if (ac.offset < ac.start)
	{
		ac.offset = ac.start;
	}
	else if (ac.offset > ac.end)
	{
		ac.offset = ac.end;
	}
}
```
