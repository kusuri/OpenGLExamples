/*
  ==============================================================================

    WaveformThumbnail.h
    Created: 5 Dec 2017 3:24:28pm
    Author:  maria kourousi

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

/**! Simple waveform visualisation using juce::AudioThumbnail class */

class WaveformThumbnail : public Component,
                          private ChangeListener
{
public:
    WaveformThumbnail();
    ~WaveformThumbnail();

    void paint (Graphics& g) override;

    void addBackgroundWaveform(AudioSampleBuffer &data, double sampleRate);
    AudioThumbnail& getAudioThumbnail();
private:
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail staticThumbnail;
    AudioThumbnail thumbnail;

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            repaint();
    }
};
