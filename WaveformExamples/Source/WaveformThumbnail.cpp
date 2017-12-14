/*
  ==============================================================================

    WaveformThumbnail.cpp
    Created: 5 Dec 2017 3:24:28pm
    Author:  maria kourousi

  ==============================================================================
*/

#include "WaveformThumbnail.h"

WaveformThumbnail::WaveformThumbnail(AudioThumbnail& t)
:thumbnail(t),
 thumbnailCache(5),// The AudioThumbnailCache objects must be constructed with the number of thumbnails to store.
    staticThumbnail(512, formatManager, thumbnailCache)
     // The AudioThumbnail object itself needs to be constructed by telling it how many source samples will be used to create a single thumbnail sample. This governs the resolution of the low resolution version.
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
}

WaveformThumbnail::~WaveformThumbnail()
{
    thumbnail.removeChangeListener(this);
}

void WaveformThumbnail::paint(Graphics& g)
{
    g.fillAll (Colours::darkgrey);
    g.setColour (Colours::lightgrey);

    if (staticThumbnail.getTotalLength() > 0.0)
    {
        const double endTime = staticThumbnail.getTotalLength();

        Rectangle<int> thumbArea (getLocalBounds());
        staticThumbnail.drawChannels (g, thumbArea.reduced (2), 0.0, endTime, 1.0f);
    }

    g.setColour (Colours::yellow);
    if (thumbnail.getTotalLength() > 0.0)
    {
        Rectangle<int> thumbArea (getLocalBounds());
        thumbnail.drawChannels (g, thumbArea.reduced (2), 0.0, /*jmax (10.0, thumbnail.getTotalLength())*/staticThumbnail.getTotalLength(), 1.0f);
    }
//    else
//    {
//        g.setFont (14.0f);
//        g.drawFittedText ("(No file recorded)", getLocalBounds(), Justification::centred, 2);
//    }   
}

void WaveformThumbnail::addBackgroundWaveform(AudioSampleBuffer &data, double sampleRate)
{
    if (data.getNumSamples() > 0)
    {
        staticThumbnail.reset(1, sampleRate, data.getNumSamples());
        staticThumbnail.addBlock(0, data, 0, data.getNumSamples());
    }
}

AudioThumbnail& WaveformThumbnail::getAudioThumbnail()
{
    return thumbnail;
}
