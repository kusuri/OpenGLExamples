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
    WaveformThumbnail(AudioThumbnail& playbackT, AudioThumbnail& currentT);
    ~WaveformThumbnail();

    void paint (Graphics& g) override;

    void addBackgroundWaveform(AudioSampleBuffer &data, double sampleRate);
    AudioThumbnail& getAudioThumbnail();

    void setGridSize(int newSize) { blockSize = newSize; };
private:
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail staticThumbnail;
    AudioThumbnail& playbackThumbnail;
    AudioThumbnail& thumbnail;

    int sliceSamples;
    int currentSlice = -1;
    int blockSize = 4;

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
        {
            double totalSamples = thumbnail.getTotalLength();
            totalSamples *= 44100;
            int x = (totalSamples / sliceSamples);
            int newSlice = x % blockSize;
            if ( newSlice != currentSlice)
            {
                currentSlice = newSlice;
                repaint();
            }
        }
    }
};
