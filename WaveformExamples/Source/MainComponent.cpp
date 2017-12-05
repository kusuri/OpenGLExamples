/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "AudioIO.h"
#include "LoopAudio.h"
#include "WaveformThumbnail.h"

#include "ipps.h"
#include "ippvm.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent,
                                public SliderListener,
                                public ButtonListener
{
public:
    //==============================================================================
    MainContentComponent()
    :thumbnailToUpdate(waveformthumbnail.getAudioThumbnail()), nextSampleNum(0)
    {
        
        addAndMakeVisible(loadButton = new TextButton("Load clip"));
        loadButton->addListener(this);
        addAndMakeVisible(playButton = new TextButton("PLAY"));
        playButton->setClickingTogglesState(true);
        playButton->addListener(this);

        addAndMakeVisible(gainKnob = new Slider);
        gainKnob->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        gainKnob->setRange(0.0, 1.0, 0.1);
        gainKnob->setValue(gain.get(), dontSendNotification);
        gainKnob->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        gainKnob->addListener(this);

        addAndMakeVisible(waveformthumbnail);

        setSize (800, 600);

        audioIO.registerBasicFormats();
        audioLooper.setLooping(true);

        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    ~MainContentComponent()
    {
        shutdownAudio();

        loadButton->removeListener(this);
        playButton->removeListener(this);
        gainKnob->removeListener(this);
    }

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        // This function will be called when the audio device is started, or when
        // its settings (i.e. sample rate, block size, etc) are changed.

        // You can use this function to initialise any resources you might need,
        // but be careful - it will be called on the audio thread, not the GUI thread.

        // For more details, see the help for AudioProcessor::prepareToPlay()
        sumBuffer = new AudioSampleBuffer(1, samplesPerBlockExpected);
        sumBuffer->clear();
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // Your audio-processing code goes here!

        // For more details, see the help for AudioProcessor::getNextAudioBlock()

        // Right now we are not producing any data, in which case we need to clear the buffer
        // (to prevent the output of random noise)
        bufferToFill.clearActiveBufferRegion();

        if (audioLooper.isPlaying())
        {
            audioLooper.render(*bufferToFill.buffer);

            const auto numSamples = bufferToFill.numSamples;
            bufferToFill.buffer->applyGain(0, numSamples, gain.get());

            sumBuffer->clear();
            ippsAdd_32f(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getWritePointer(1), sumBuffer->getWritePointer(0), numSamples);
            ippsMulC_32f_I(0.5f, sumBuffer->getWritePointer(0), numSamples);
            thumbnailToUpdate.addBlock(nextSampleNum, *sumBuffer, 0, numSamples);
            nextSampleNum += numSamples;
            if (nextSampleNum > audioLooper.getNumSamples())
            {
                nextSampleNum = 0;
                thumbnailToUpdate.reset(sumBuffer->getNumChannels(), 44100);
            }
        }
    }

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

        // You can add your drawing code here!
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        auto rect = getLocalBounds();

        const int buttonSize = 100;
        loadButton->setBounds(rect.removeFromLeft(buttonSize).withHeight(buttonSize));
        playButton->setBounds(rect.removeFromLeft(buttonSize).withHeight(buttonSize));
        gainKnob->setBounds(rect.removeFromLeft(buttonSize).withHeight(buttonSize));
        rect.removeFromTop(buttonSize);

        rect.setLeft(0);
        waveformthumbnail.setBounds(rect);
    }

    void sliderValueChanged(Slider* slider) override
    {
        if (slider == gainKnob)
        {
            gain.set(gainKnob->getValue());
        }
    }

    void buttonClicked(Button* button) override
    {
        if (button == loadButton)
        {
            File fileToLoad = audioIO.browse();
            AudioIO::AudioDataPair<float> audioDataPair;
            if (audioIO.checkAndLoadFile(fileToLoad, audioDataPair))
            {
                audioLooper.load(*audioDataPair.getRawBuffer(), audioDataPair.metadata.sampleRate);

                sumBuffer->setSize(1, audioDataPair.getRawBuffer()->getNumSamples());
                sumBuffer->clear();
                int numSamples = audioDataPair.getRawBuffer()->getNumSamples();
                ippsAdd_32f(audioDataPair.getRawBuffer()->getWritePointer(0), audioDataPair.getRawBuffer()->getWritePointer(1), sumBuffer->getWritePointer(0), numSamples);
                ippsMulC_32f_I(0.5f, sumBuffer->getWritePointer(0), numSamples);
                waveformthumbnail.addBackgroundWaveform(*sumBuffer, audioDataPair.metadata.sampleRate);
                waveformthumbnail.repaint();

                thumbnailToUpdate.reset(sumBuffer->getNumChannels(), audioDataPair.metadata.sampleRate);
                nextSampleNum = 0;
            }
        }
        else if (button == playButton)
        {
            auto state = playButton->getToggleState();
            audioLooper.setStartReadPoint(0);
            if (state)
            {
                button->setButtonText("STOP");
                audioLooper.play();
            }
            else
            {
                button->setButtonText("PLAY");
                audioLooper.stop();

                thumbnailToUpdate.reset(sumBuffer->getNumChannels(), 44100);
                nextSampleNum = 0;
            }
        }
    }
private:
    //==============================================================================

    // Your private member variables go here...
//==============================================================================
    AudioIO audioIO;
    LoopAudio<float> audioLooper; /*!< LoopAudio object in charge of playback. */
    Atomic<float> gain = 1.0f;
    ScopedPointer<AudioSampleBuffer> sumBuffer;
//==============================================================================
    ScopedPointer<TextButton> loadButton;
    ScopedPointer<TextButton> playButton;
    ScopedPointer<Slider> gainKnob; // [0, 1] values
//==============================================================================
    int64 nextSampleNum;
    AudioThumbnail& thumbnailToUpdate;
    WaveformThumbnail waveformthumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }
