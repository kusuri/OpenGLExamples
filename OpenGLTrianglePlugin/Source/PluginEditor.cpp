/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OpenGltrianglePluginAudioProcessorEditor::OpenGltrianglePluginAudioProcessorEditor (OpenGltrianglePluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible(buttonPanel = new ToggleButton);
    buttonPanel->addListener(this);
    
    addAndMakeVisible (slider = new Slider ("new slider"));
    slider->setRange (0, 5, 1);
    slider->setSliderStyle (Slider::RotaryVerticalDrag);
    slider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(glComponent);

//    addAndMakeVisible(waveform = new AudioWaveform);
    
    int width = 700;
    int height = 540;
    
    setResizable(true, false);
    // set resize limits for this plug-in
//    setResizeLimits (width, height, 1024, 700);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (width, height);
}

OpenGltrianglePluginAudioProcessorEditor::~OpenGltrianglePluginAudioProcessorEditor()
{
}

//==============================================================================
void OpenGltrianglePluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void OpenGltrianglePluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto rect = getLocalBounds();
    buttonPanel->setBounds(rect.removeFromLeft(100).withHeight(100));
    slider->setBounds(rect.removeFromTop(100).withWidth(100));
//    waveform->setBounds(rect.removeFromTop(200).withWidth(getWidth()));
    glComponent.setBounds(rect.removeFromBottom(getHeight()/2.0f));
}

void OpenGltrianglePluginAudioProcessorEditor::buttonClicked(Button* b)
{
    bool state = b->getToggleState();
    if (b == buttonPanel)
    {
        setSize(state ? 900 : 700, 540);
    }
}
