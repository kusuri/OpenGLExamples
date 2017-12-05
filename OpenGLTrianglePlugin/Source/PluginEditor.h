/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "OpenGLComponent.h"
#include "AudioWaveform.h"

//==============================================================================
/**
*/
class OpenGltrianglePluginAudioProcessorEditor  : public AudioProcessorEditor,
                                                public ButtonListener
{
public:
    OpenGltrianglePluginAudioProcessorEditor (OpenGltrianglePluginAudioProcessor&);
    ~OpenGltrianglePluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked(Button* b) override;
    
//    ScopedPointer<AudioWaveform> waveform;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OpenGltrianglePluginAudioProcessor& processor;
   
    // button panel
    ScopedPointer<ToggleButton> buttonPanel;
    // Discrete knob
    ScopedPointer<Slider> slider;
    
    OpenGLComponent glComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGltrianglePluginAudioProcessorEditor)
};
