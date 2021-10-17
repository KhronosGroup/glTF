# KHR\_aniamtion\_controller

## Contributors

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

ToDo: Write motivation and explanation.

### Examples

#### Minimum valid controller entry referencing animation 0.

```json
{
	"animation": 0
}
```

#### Valid controller with all values set.

```json
{
	"animation": 0,
	"name": "All values set",
	"start": 0.0,
	"end": 10.0,
	"timestamp": 0.0,
	"direction": 1.0,
	"counter": 1,
	"bounce": true
}
```

### Sample implementation

```cpp
// Delta time is in seconds. Delta time is the time executed between the current and last frame.
void updateTimestamp(float deltaTime, AnimationController& ac)
{
	// Animation controller is off.
	// Early quit for performance and not having to change the direction value.
	if (ac.counter == 0)
	{
		return;
	}

	// Increase timestamp depending on delta time and direction.
	ac.timestamp += ac.direction * deltaTime;

	// Check, if the timestamp is larger or smaller than the bounds.	
	if (ac.timestamp > ac.end)
	{
		float overtime = ac.timestamp - ac.end;

		if (ac.bounce)
		{
			ac.timestamp = ac.end - overtime;
		}
		else
		{
			ac.timestamp = ac.start + overtime;
		}

		// Note: counter = -1 is infinite execution, counter = 0 is animation off.
		if (ac.counter > 0)
		{
			ac.counter--;
		}
	}
	else if (ac.timestamp < ac.start)
	{
		float overtime = ac.start - ac.timestamp;

		if (ac.bounce)
		{
			ac.timestamp = ac.start + overtime;
		}
		else
		{
			ac.timestamp = ac.end - overtime;
		}

		// Note: counter = -1 is infinite execution, counter = 0 is animation off.
		if (ac.counter > 0)
		{
			ac.counter--;
		}
	}

	// Always keep start <= timestamp <= end.
	// Required for counter = -1 or robustness (very small differences between start and end).
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
