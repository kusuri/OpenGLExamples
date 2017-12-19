## OpenGL examples

This is GUI application made with Juce.

At the moment the user can load an audio clip and the application with plot its waveform with one of the following ways:

- Waveform thumbnail: a layer waveform view
	- a static waveform with the imported clip
	- a dynamic waveform that plots the playback data (what u see is what u hear)
- Audio Oscilloscope: a waveform implementation of drowAudio
- OpenGL waveform: a pure OpenGL implementation using the Juce OpenGL renderer
- AudioVisualisationComponent: see [here](https://juce.com/doc/classAudioVisualiserComponent)

The user can select the waveform implementation from a combobox.
Also, this application contains a gain knob to adjust the output volume.