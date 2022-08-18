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
  
In general, a keyframe calculated and provided by a viewer starts at `0.0` and is incremented to infinity. The current calculated keyframe is the `timestamp`.
  
A glTF animation does have a keyframe start and a keyframe end. Today, it is not clearly defined, if the global keyframe - the `timestamp` - becomes larger than the end value of the animation. Many implementations start to loop the animation, but stopping or reverting the animation is a feasible option as well. This extension is defining this behaviour as well.
  
This extension is defining the calculation of the `timestamp` used for the glTF keyframe animations in a deterministic way.

### Timestamp is smaller than the smallest keyframe or larger than the largest keyframe

If the `timestamp` is smaller or larger than the animation keyframes, the first or the last keyframe of the animation is used respectively. This means, that the animation seems to be stopped during this timeframe.

### Animation clip JSON examples

#### Minimum valid animation clip entry referencing animation 0.

```json
{
	"animations": [
		0
	]
}
```

#### Animation clip with all values set.

```json
{
	"animations": [
		0
	],
	"name": "Forth and back animation.",
	"start": 0.0,
	"end": 10.0,
	"timestamp": 0.0,
	"speed": 1.0,
	"repetitions": 2,
	"reverse": true
}
```

### Examples

`Total time` is the accumulated time in seconds since the start of rendering.
`timestamp` is the internal time in seconds of the animation clip.

#### Example 1

| Total time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| timestamp  | 00  | 01  | 02  | 03  | 04  | 00  | 01  | 02  | 03  | 04  | 00  | 01  | 02  | 03  | 04  | 00  | 01  | 02  | 03  | 04  | 00  |

```json
{
	"animations": [
		0
	],
	"name": "Example 1",
	"start": 0.0,
	"end": 4.0,
	"timestamp": 0.0,
	"speed": 1.0,
	"repetitions": -1,
	"reverse": false
}
```

#### Example 2

| Total time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| timestamp  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  | 22  |

```json
{
	"animations": [
		0
	],
	"name": "Example 2",
	"start": 2.0,
	"timestamp": 2.0,
	"speed": 1.0,
	"repetitions": -1,
	"reverse": false
}
```

#### Example 3

| Total time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| timestamp  | 03  | 04  | 05  | 06  | 07  | 08  | 03  | 04  | 05  | 06  | 07  | 08  | 03  | 04  | 05  | 06  | 07  | 08  | 03  | 04  | 05  |

```json
{
	"animations": [
		0
	],
	"name": "Example 3",
	"start": 3.0,
	"end": 8.0,
	"timestamp": 3.0,
	"speed": 1.0,
	"repetitions": -1,
	"reverse": false
}
```

#### Example 4

| Total time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| timestamp  | 00  | 01  | 02  | 03  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  | 04  |

```json
{
	"animations": [
		0
	],
	"name": "Example 4",
	"start": 0.0,
	"end": 4.0,
	"timestamp": 0.0,
	"speed": 1.0,
	"repetitions": 1,
	"reverse": false
}
```

#### Example 5

| Total time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| timestamp  | 00  | 01  | 02  | 03  | 04  | 05  | 04  | 03  | 02  | 01  | 00  | 01  | 02  | 03  | 04  | 05  | 05  | 05  | 05  | 05  | 05  |

```json
{
	"animations": [
		0
	],
	"name": "Example 5",
	"start": 0.0,
	"end": 5.0,
	"timestamp": 0.0,
	"speed": 1.0,
	"repetitions": 3,
	"reverse": true
}
```

#### Example 6

| Total time | 00  | 01  | 02  | 03  | 04  | 05  | 06  | 07  | 08  | 09  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
| ---------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| timestamp  | 04  | 04  | 03  | 03  | 02  | 02  | 01  | 01  | 00  | 00  | 01  | 01  | 02  | 02  | 03  | 03  | 04  | 04  | 04  | 04  | 04  |

```json
{
	"animations": [
		0
	],
	"name": "Example 6",
	"start": 0.0,
	"end": 4.0,
	"timestamp": 4.0,
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

	// Increase timestamp depending on delta time and speed.
	ac.timestamp += ac.speed * deltaTime;

	// Check, if the timestamp is larger or smaller than the bounds.	
	if (ac.timestamp > ac.end)
	{
		float overtime = ac.timestamp - ac.end;

		if (ac.reverse)
		{
			ac.timestamp = ac.end - overtime;

			ac.speed *= -1.0f;
		}
		else
		{
			ac.timestamp = ac.start + overtime;
		}

		// Note: repetitions = -1 is infinite execution, repetitions = 0 is animation off.
		if (ac.repetitions > 0)
		{
			ac.repetitions--;
		}
	}
	else if (ac.timestamp < ac.start)
	{
		float overtime = ac.start - ac.timestamp;

		if (ac.reverse)
		{
			ac.timestamp = ac.start + overtime;

			ac.speed *= -1.0f;
		}
		else
		{
			ac.timestamp = ac.end - overtime;
		}

		// Note: repetitions = -1 is infinite execution, repetitions = 0 is animation off.
		if (ac.repetitions > 0)
		{
			ac.repetitions--;
		}
	}

	// Always keep start <= timestamp <= end.
	// Required for repetitions = -1 or robustness (very small differences between start and end).
	if (ac.timestamp < ac.start)
	{
		ac.timestamp = ac.start;
	}
	else if (ac.timestamp > ac.end)
	{
		ac.timestamp = ac.end;
	}
}
```
