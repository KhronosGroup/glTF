<!-----



Conversion time: 1.967 seconds.


Using this Markdown file:

1. Paste this output into your source file.
2. See the notes and action items below regarding this conversion run.
3. Check the rendered output (headings, lists, code blocks, tables) for proper
   formatting and use a linkchecker before you publish this page.

Conversion notes:

* Docs to Markdown version 1.0β36
* Fri Jun 14 2024 17:25:52 GMT-0700 (PDT)
* Source doc: KHR Audio Graph Design
* Tables are currently converted to HTML tables.
----->



## KHR Audio Graph Design




## **Contributors**



* Chintan Shah, Meta
* Alexey Medvedev, Meta


## Context

During the recent Khronos 3D formats working group meeting held on 5/29, we reviewed the [proposal to define the KHR audio glTF specification using an audio graph framework](https://docs.google.com/presentation/d/1IrrQaE-jHyzOtFRabjtLAzeP5UOirFOEj8FRADAceqk/edit?usp=sharing). The purpose of this document is to delve deeper into that proposal, offering a comprehensive design of the KHR audio graph. This includes a detailed description of each node object within the graph along with functionality, the specific properties associated with it, and how it interacts with other nodes in the graph. The document is structured to facilitate clear understanding and to solicit feedback on the proposed design. Based on the feedback we will update and finalize the design before it is formally schematized into KHR core audio spec, extensions, animations, and interactivity.


## 1. Introduction

This document provides a detailed design for managing audio routing, mixing, and processing in various applications for desktop, mobile, and wearable devices. The core idea involves an audio graph consisting of multiple interconnected audio node objects to create the final audio output. This design seeks to incorporate the audio capabilities found in modern web, game, and XR engines as well as processing, mixing, and filtering functions available in audio production softwares. Although this system is designed with diverse use cases in mind, it might not cover every specialized feature found in state-of-the-art audio tools. In such instances, users are encouraged to develop custom extensions. Nevertheless, the proposed system will support many complex audio applications by default and has been designed to facilitate future expansion with more sophisticated features.


## 2. Features

The core specification should support these primary features:



* Graph based audio routing for simple or complex mixing and processing architectures.
* Processing of audio data stored in memory buffer or accessed via file paths.
* Capturing audio metadata such as encoding properties.
* Audio playback functionalities including playing, stopping, pausing, looping, and controlling playback speed.
* Spatialized audio supporting a wide range of 3D applications and immersive environments with 6DoF source/listener capabilities, panning models (equal power, HRTF), distance attenuation, and sound cones.
* Basic audio signal processing to control gain, delay, and pitch.
* Flexible handling of channels in an audio stream, allowing splitting, merging, up-mixing, or down-mixing.
* Audio mixing, reverb, and filtering with a set of low-order audio filters.
* Animation control and dynamic update of node properties.


## 3. Graph based audio processing

Audio nodes are the building blocks of an audio graph for rendering audio to the audio hardware. Graph based audio routing allows arbitrary connections between different audio node objects. An audio graph can be represented by audio sources, the audio destination/sink, and intermediate processing nodes. Each node can have inputs and/or outputs. A source node has no inputs and a single output. A destination or sink node has one input and no outputs. In the simplest case, a single source can be routed directly to the output.

One or more intermediate processing nodes such as filters can be placed between the source and destination nodes. Most processing nodes will have one input and one output. Each type of audio node differs in the details of how it processes or synthesizes audio. But, in general, an audio node will process its inputs (if it has any), and generate audio for its outputs (if it has any). An output may connect to one or more audio node inputs, thus fan-out is supported. An input (except source and sink) may be connected to one or more audio node outputs, thus fan-in is supported. Each input and output has one or more channels. The exact number of channels depends on the details of the specific audio node.


## 4. Audio source


### 4.1 Source node (0 input / 1 output)

Audio sources reference audio data and define playback properties for it. An audio source node has no inputs and exactly one output, which has the same number of channels as indicated in encoding properties.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>data
   </td>
   <td>object
   </td>
   <td>audio or oscillator
<p>
See 4.2 Audio data.
<p>
See 4.3 Oscillator data.
   </td>
   <td>
   </td>
   <td>Only selective source node properties apply with oscillator data.
   </td>
  </tr>
  <tr>
   <td>priority
   </td>
   <td>integer
   </td>
   <td>Determines the priority of this audio source among all the ones that coexist in the scene (0 = most important, 256 = least important, default = 128).
   </td>
   <td>
   </td>
   <td>Need to  persist and propagate priority with downstream processing.
   </td>
  </tr>
  <tr>
   <td>state
   </td>
   <td>string
   </td>
   <td>Playback state (paused, playing, stopped).
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>auto play
   </td>
   <td>boolean
   </td>
   <td>Play on load.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>loop
   </td>
   <td>boolean
   </td>
   <td>Playback in a loop (false = no-loop, true = loop). If set to true, then once playback reaches the time specified by “loop end position” (or the end of the asset, whichever is first), the source node will continue playback again from a position specified by the “loop start position” property.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>position
   </td>
   <td>number
   </td>
   <td>Play position in ms.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>gain
   </td>
   <td>number
   </td>
   <td>Gain value applied to the audio.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>playback speed
   </td>
   <td>number
   </td>
   <td>Rate of playback. A value of 1.0 would playback the audio at the standard rate. A value of 2.0 would play back the asset at double the speed.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>loop start position
   </td>
   <td>number
   </td>
   <td>The starting position in ms when looping.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>loop end position
   </td>
   <td>number
   </td>
   <td>The ending position (ms) of the loop.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>duration
   </td>
   <td>number
   </td>
   <td>Length of the underlying audio data in ms.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>encoding properties
   </td>
   <td>object
   </td>
   <td>See 4.4 Encoding properties.
   </td>
   <td>
   </td>
   <td>Could be a part of audio data property  instead.
   </td>
  </tr>
</table>



### 4.2 Audio data

Audio data objects define where audio data is located and what format the data is in. The data is either accessed via a bufferView or uri.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>buffer view
   </td>
   <td>integer
   </td>
   <td>The index of the buffer view that contains the audio data. The buffer represents an audio asset residing in memory, created from decoding an audio file, or from raw data.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>mime type
   </td>
   <td>string
   </td>
   <td>The audio's MIME type. Required if buffer view is defined.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>uri
   </td>
   <td>string
   </td>
   <td>The uri of the audio file. Relative paths are relative to the .gltf file.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 4.3 Oscillator data

This represents an audio source generating a periodic waveform. It can be set to a few commonly used waveforms. Oscillators are common foundational building blocks in audio synthesis.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>type
   </td>
   <td>string
   </td>
   <td>Specifies the waveform type (saw, square, triangle, sine, custom).
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>frequency
   </td>
   <td>number
   </td>
   <td>The Oscillator frequency, from 0-20kHz. The default value is 440 Hz.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>pulse width
   </td>
   <td>number
   </td>
   <td>The amount of pulse width modulation applied when the “square” waveform is selected. A 0.5 value will produce a pure square wave, and increasing or decreasing the value will add harmonics which change the timbre of the sound.
   </td>
   <td>
   </td>
   <td>Applies to square waveform.
   </td>
  </tr>
</table>



### 4.4 Encoding properties


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>bits per sample
   </td>
   <td>integer
   </td>
   <td>Number of bits per audio sample.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>samples
   </td>
   <td>integer
   </td>
   <td>Number of samples.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>sample rate
   </td>
   <td>number
   </td>
   <td>Audio sampling rate (Hz).
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>channels
   </td>
   <td>integer
   </td>
   <td>Number of audio channels.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>properties
   </td>
   <td>object
   </td>
   <td>Additional user-defined properties.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



## 5. Audio sink/destination


### 5.1 Emitter node (1 input / 0 output)

Audio emitter of type “Global” is non-spatialized, whereas a “Spatial” emitter is used for spatial audio. A scene can contain one or more global emitters. Spatial emitter is connected to a scene node. A scene node can have one or more spatial emitters. A spatial emitter by default inherits the pose (position and orientation) properties of the scene node, hence we do not support updating these properties within the spatial emitter node.

Using a spatial emitter, an audio stream can be spatialized or positioned in space relative to a listener node. A scene has a single listener node. Both spatial emitters and listeners have a position in 3D space. Spatial emitters have an orientation representing in which direction the sound is projecting. Additionally, they have a sound cone representing how directional the sound is. For example, the sound could be omnidirectional, in which case it would be heard anywhere regardless of its orientation, or it can be more directional and heard only if it is facing the listener.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>type
   </td>
   <td>string
   </td>
   <td>Emitter type (global, spatial).
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>gain
   </td>
   <td>number
   </td>
   <td>gain applied to the signal by the emitter.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>spatial properties
   </td>
   <td>object
   </td>
   <td>See 5.2 Spatial properties.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 5.2 Spatial properties


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>spatialization model
   </td>
   <td>string
   </td>
   <td>Determines which spatialization model will be used to position the audio in 3D space (equal power, HRTF, Custom).
<p>
equalpower: Represents the equal-power panning algorithm, generally regarded as simple and efficient. equalpower is the default value.
<p>
HRTF: Renders a stereo output of higher quality than equalpower — it uses a convolution with measured impulse responses from human subjects.
<p>
Custom: User-defined panning algorithm.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>attenuation
   </td>
   <td>object
   </td>
   <td>See 5.3 Attenuation properties.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>shape
   </td>
   <td>object
   </td>
   <td>See 5.4 Shape properties
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 5.3 Attenuation properties


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>distance model
   </td>
   <td>string
   </td>
   <td>Specifies the distance model for the audio emitter  linear, inverse, exponential, custom).
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>ref distance
   </td>
   <td>number
   </td>
   <td>A reference distance for reducing volume as the emitter moves further from the listener.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>max distance
   </td>
   <td>number
   </td>
   <td>The maximum distance between the emitter and listener, beyond which the audio cannot be heard.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>rolloff factor
   </td>
   <td>number
   </td>
   <td>Describes how quickly the volume is reduced as the emitter moves away from the listener.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 5.4 Shape properties


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>type
   </td>
   <td>string
   </td>
   <td>Shape in which emitter emits audio (cone, omnidirectional, custom).
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>cone inner angle
   </td>
   <td>number
   </td>
   <td>The angular diameter of a cone inside of which there will be no angular volume reduction.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>cone outer angle
   </td>
   <td>number
   </td>
   <td>A parameter for directional audio sources that is an angle, in degrees, outside of which the volume will be reduced to a constant value of coneOuterGain.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>cone outer gain
   </td>
   <td>number
   </td>
   <td>A parameter for directional audio sources that is the gain outside of the cone outer angle.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 5.5 Audio listener node (0 input / 0 output)

Describes the position and other physical characteristics of a listener from which the audio output of spatial emitter nodes is heard when spatial audio processing is used. A listener node is typically attached to the main camera and by default inherits camera pose (position and orientation) properties. Hence, we do not support updating these properties within the listener node.


## 6. Audio processors


### 6.1 Gain node (1 input / 1 output)


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>gain
   </td>
   <td>number
   </td>
   <td>The gain to apply. Once set, the actual gain applied will transition from it’s current setting to the new one set over “duration” milliseconds.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>interpolation
   </td>
   <td>string
   </td>
   <td>The curve to apply when changing gains (linear, custom).
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>duration
   </td>
   <td>number
   </td>
   <td>When changing gain, this parameter controls how long to spend interpolating from the previously set gain value to the one that is specified.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 6.2 Delay node (1 input / 1 output)

The node that causes a delay between the arrival of an input data and its propagation to the output. A delay node always has exactly one input and one output, both with the same amount of channels.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>delay time
   </td>
   <td>number
   </td>
   <td>representing the amount of delay to apply, specified in ms.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 6.3 Pitch shifter node (1 input / 1 output)

Use the node to make the pitch of an audio deeper or higher.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>semitone adjustment
   </td>
   <td>number
   </td>
   <td>Pitch shift in musical semitones. A value of -12 halves the pitch, while 12 doubles the pitch. A value of 0 will not change the pitch of the audio source.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 6.4 Channel splitter node (1 input / N outputs)

Node for accessing the individual channels of an audio stream in the routing graph. It has a single input, and a number of outputs which equals the number of channels in the input audio stream. For example, if a stereo input is connected to this node then the number of active outputs will be two (one from the left channel and one from the right).


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>input channels
   </td>
   <td>integer
   </td>
   <td>Number of channels in input audio.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>channel interpretation
   </td>
   <td>string
   </td>
   <td>speakers, discrete
<p>
Channel ordering for speaker channel interpretation are captured <a href="https://webaudio.github.io/web-audio-api/#ChannelOrdering">here</a>. When the number of channels do not match any of the basic speaker layouts, use discrete to  maps channels to outputs.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 6.5 Channel merger node (N inputs / 1 output)

Node for combining channels from multiple audio streams into a single audio stream. It has a variable number of inputs, and a single output whose audio stream has a number of channels equal to the number of inputs. To merge multiple inputs into one stream, each input should be a single channel audio stream.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>output channels
   </td>
   <td>integer
   </td>
   <td>Number of channels in output audio.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>channel interpretation
   </td>
   <td>string
   </td>
   <td>speakers, discrete
<p>
Channel ordering for speaker channel interpretation are captured <a href="https://webaudio.github.io/web-audio-api/#ChannelOrdering">here</a>. When the number of channels do not match any of the basic speaker layouts, use discrete as it maps inputs to channels.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 6.6 Channel mixer node (1 input / 1 output)

Up-mixing refers to the process of taking a stream with a smaller number of channels and converting it to a stream with a larger number of channels. Down-mixing refers to the process of taking a stream with a larger number of channels and converting it to a stream with a smaller number of channels. Channel mixer node should ideally use these [mixing rules](https://webaudio.github.io/web-audio-api/#mixing-rules).


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>input channels
   </td>
   <td>integer
   </td>
   <td>Number of channels in input audio.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>output channels
   </td>
   <td>integer
   </td>
   <td>Number of channels in output audio.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>channel interpretation
   </td>
   <td>string
   </td>
   <td>speakers, discrete
<p>
Speakers use up-mix / down-mix equations. In cases where the number of channels do not match any of the basic speaker layouts, use "discrete". "Discrete" up-mix by filling channels until they run out then zero out remaining channels; down-mix by filling as many channels as possible, then dropping remaining channels.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 6.7 Audio mixer node (N inputs / 1 output)

Use the Audio mixer node to combine the output from multiple audio sources. Number of channels should be the same across all inputs.

[no properties for this node]


### 6.8 Filter node (1 input / 1 output)

Use the Audio Mixer node to combine the output from multiple audio sources. A filter node always has exactly one input and one output.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>type
   </td>
   <td>string
   </td>
   <td>Defining the kind of filtering algorithm the node is implementing (lowpass, highpass, bandpass, lowshelf, highshelf, peaking, notch, allpass, custom)
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>frequency
   </td>
   <td>number
   </td>
   <td>Frequency in the current filtering algorithm measured in Hz.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>quality factor
   </td>
   <td>number
   </td>
   <td>The lower the Quality, the broader the bandwidth of frequencies cut or boosted. Value range: 0 to 100.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>gain
   </td>
   <td>number
   </td>
   <td>gain applied to the signal by the filter.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>bypass
   </td>
   <td>boolean
   </td>
   <td>Disables this processor while still allowing unprocessed audio signals to pass.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



### 6.9 Reverb node (1 input / 1 output)

Reverberation is the persistence of sound in an enclosure after a sound source has been stopped. This is a result of the multiple reflections of sound waves throughout the room arriving at the ear so closely spaced that they are indistinguishable from one another and are heard as a gradual decay of sound.


<table>
  <tr>
   <td>
   </td>
   <td><strong>Type</strong>
   </td>
   <td><strong>Description</strong>
   </td>
   <td><strong>Required</strong>
   </td>
   <td><strong>Notes</strong>
   </td>
  </tr>
  <tr>
   <td>mix
   </td>
   <td>number
   </td>
   <td>Blend between the source signal ('dry') and the reverb effect. A value of 0 will not add any reverb. A value of 50 will mix the signal 50% dry and 50% reverberation. A value of 100 will result in only reverberation, without any of the source signal.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>early reflections gain
   </td>
   <td>number
   </td>
   <td>Loudness control for the early reflections of the reverberation.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>diffusion gain
   </td>
   <td>number
   </td>
   <td>Loudness control for the reverb decay as it returns to silence.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>room size
   </td>
   <td>number
   </td>
   <td>Approximates the size of the room you want to simulate in meters from wall to wall.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>reflectivity
   </td>
   <td>number
   </td>
   <td>Defines how much of the audio is reflected at each bounce on a wall. Value range: 0 to 100. Low values will simulate softer sounding materials like carpet or curtains. High values will simulate harder materials like wood, glass or metal. A value of 100 will result in self-oscillation and is not recommended.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>reflectivity high
   </td>
   <td>number
   </td>
   <td>Separate value for the reflectivity of high frequencies.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>reflectivity low
   </td>
   <td>number
   </td>
   <td>Separate value for the reflectivity of low frequencies.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>early reflections
   </td>
   <td>number
   </td>
   <td>The number of early reflections of reverberation. The value range is 0 to 32.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>min distance
   </td>
   <td>number
   </td>
   <td>The distance from the centerpoint that the reverb will have full effect at.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>max distance
   </td>
   <td>number
   </td>
   <td>The distance from the centerpoint that the reverb will not have any effect.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>reflection delay
   </td>
   <td>number
   </td>
   <td>Initial reflection delay time in ms.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>reverb delay
   </td>
   <td>number
   </td>
   <td>Late reverberation delay time relative to initial reflection.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>custom properties
   </td>
   <td>object
   </td>
   <td>Application-specific data.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>bypass
   </td>
   <td>boolean
   </td>
   <td>Disables this processor while still allowing unprocessed audio signals to pass.
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



## 7. Audio graph rules



* Multiple audio source nodes can reference the same audio data.
* An output of a source node can serve as input to multiple processor and emitter nodes.
* An output of a processor node can serve as input to multiple processor and emitter nodes.
* One audio emitter node can have only one input.
* A scene can have multiple emitters.
* A node can have multiple spatial emitters.
* A scene can have only one audio listener.
