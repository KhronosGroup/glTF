# KHR\_animation\_controller

## Contributors

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

ToDo: Write motivation, explanation and outlook.

### Animation controller JSON examples

#### Minimum valid animation controller entry referencing animation 0.

```json
{
	"animation": 0
}
```

#### Animation controller with all values set.

```json
{
	"animation": 0,
	"name": "Forth and back animation.",
	"start": 0.0,
	"end": 10.0,
	"timestamp": 0.0,
	"direction": 1.0,
	"counter": 2,
	"bounce": true
}
```

### Examples

`Total time` is the accumulated time in seconds since the start of rendering.  
`timestamp` is the internal time in seconds of the animation controller.  

#### Example 1

|Total time|00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|
|----------|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|timestamp |00|01|02|03|04|00|01|02|03|04|00|01|02|03|04|00|01|02|03|04|00|

```json
{
	"animation": 0,
	"name": "Example 1",
	"start": 0.0,
	"end": 4.0,
	"timestamp": 0.0,
	"direction": 1.0,
	"counter": -1,
	"bounce": false
}
```

#### Example 2

|Total time|00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|
|----------|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|timestamp |02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|

```json
{
	"animation": 0,
	"name": "Example 2",
	"start": 2.0,
	"timestamp": 2.0,
	"direction": 1.0,
	"counter": -1,
	"bounce": false
}
```

#### Example 3

|Total time|00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|
|----------|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|timestamp |03|04|05|06|07|08|03|04|05|06|07|08|03|04|05|06|07|08|03|04|05|

```json
{
	"animation": 0,
	"name": "Example 3",
	"start": 3.0,
	"end": 8.0,
	"timestamp": 3.0,
	"direction": 1.0,
	"counter": -1,
	"bounce": false
}
```

#### Example 4

|Total time|00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|
|----------|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|timestamp |00|01|02|03|04|04|04|04|04|04|04|04|04|04|04|04|04|04|04|04|04|

```json
{
	"animation": 0,
	"name": "Example 4",
	"start": 0.0,
	"end": 4.0,
	"timestamp": 0.0,
	"direction": 1.0,
	"counter": 1,
	"bounce": false
}
```

#### Example 5

|Total time|00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|
|----------|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|timestamp |00|01|02|03|04|05|04|03|02|01|00|01|02|03|04|05|05|05|05|05|05|

```json
{
	"animation": 0,
	"name": "Example 5",
	"start": 0.0,
	"end": 5.0,
	"timestamp": 0.0,
	"direction": 1.0,
	"counter": 3,
	"bounce": true
}
```

#### Example 6

|Total time|00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|
|----------|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
|timestamp |04|04|03|03|02|02|01|01|00|00|01|01|02|02|03|03|04|04|04|04|04|

```json
{
	"animation": 0,
	"name": "Example 6",
	"start": 4.0,
	"end": 0.0,
	"timestamp": 4.0,
	"direction": -0.5,
	"counter": 2,
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

			ac.direction *= -1.0f;
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

			ac.direction *= -1.0f;
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
