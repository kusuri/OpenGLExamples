/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent,
                                public ApplicationCommandTarget,
                                public SliderListener,
                                public ButtonListener,
                                public ComboBoxListener
{
public:
    //==============================================================================
    MainContentComponent()
    :numWaveforms(4), nextSampleNum(0), thumbnailCache(5), thumbnailToUpdate(512, formatManager, thumbnailCache)
    {
        audioLooper.setLooping(false);
        
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

        addAndMakeVisible(waveformChoices = new ComboBox);
        waveformChoices->addItem("Waveform thumbnail", 1);
        waveformChoices->addItem("Audio Oscilloscpe", 2);
        waveformChoices->addItem("OpenGL Waveform", 3);
        waveformChoices->addItem("AudioVisualiserComponent", 4);
        waveformChoices->setSelectedId(2, dontSendNotification);
        waveformChoices->addListener(this);

        for (int i = 0; i < numWaveforms; ++i)
        {
            WaveformThumbnail* wT = new WaveformThumbnail(thumbnailToUpdate);
            addAndMakeVisible(wT);
            waveformthumbnail.add(wT);

            AudioOscilloscope* aO = new AudioOscilloscope();
            addAndMakeVisible(aO);
            vWaveform.add(aO);

            AudioVisualiserComponent* vC = new AudioVisualiserComponent(1);
            addAndMakeVisible(vC);
            visualiserComponents.add(vC);
        }

        
        audioIO.registerBasicFormats();
        audioLooper.setLooping(true);

        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);

//        loadClip(File("~/Music/audiotracks/loops/blankbanshee2.wav"));
//        selectWaveformType(2);

        setSize (800, 1000);
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

            if (waveformChoices->getSelectedId() == 1)
                feedWaveformThumbnail(*bufferToFill.buffer);
            else if (waveformChoices->getSelectedId() == 2)
                feedAudioOscilloscope(*bufferToFill.buffer);
            else if (waveformChoices->getSelectedId() == 3)
                feedOpenGLWaveform(*bufferToFill.buffer);
            else
            {
                for (int i = 0; i < numWaveforms; ++i)
                    visualiserComponents[i]->pushBuffer(*bufferToFill.buffer);
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
        waveformChoices->setBounds(rect.removeFromLeft(buttonSize).withHeight(buttonSize));
        rect.removeFromTop(buttonSize);

        rect.setLeft(0);
        for (int i = 0; i < numWaveforms; ++i)
        {
            waveformthumbnail[i]->setBounds(rect.removeFromTop(100));
            rect.removeFromTop(50);
        }

        rect.setTop(waveformChoices->getBottom() + buttonSize);
        for (int i = 0; i < numWaveforms; ++i)
        {
            vWaveform[i]->setBounds(rect.removeFromTop(100));
            rect.removeFromTop(50);
        }

        rect.setTop(waveformChoices->getBottom() + buttonSize);
        for (int i = 0; i < numWaveforms; ++i)
        {
            if (openGLWaveform[i] != nullptr) {
                openGLWaveform[i]->setBounds(rect.removeFromTop(100));
                rect.removeFromTop(50);
            }
        }

        rect.setTop(waveformChoices->getBottom() + buttonSize);
        for (int i = 0; i < numWaveforms; ++i)
        {
            visualiserComponents[i]->setBounds(rect.removeFromTop(100));
            rect.removeFromTop(50);
        }
    }

    //==============================================================================
    ApplicationCommandTarget* getNextCommandTarget () override
    {
        // this will return the next parent component that is an ApplicationCommandTarget (in this
        // case, there probably isn't one, but it's best to use this method in your own apps).
        return findFirstTargetParentComponent();
    }

    void getAllCommands (Array< CommandID > &commands) override
    {
        const CommandID engineIDs[] = { MainWindow::renderingEngineOne,
                                        MainWindow::renderingEngineTwo,
                                        MainWindow::renderingEngineThree };

        auto renderingEngines = MainWindow::getMainAppWindow()->getRenderingEngines();
        commands.addArray (engineIDs, renderingEngines.size());
    }

    void getCommandInfo (CommandID commandID, ApplicationCommandInfo &result) override
    {
        const String generalCategory ("General");
        switch (commandID)
        {
            case MainWindow::renderingEngineOne:
            case MainWindow::renderingEngineTwo:
            case MainWindow::renderingEngineThree:
            {
                auto& mainWindow = *MainWindow::getMainAppWindow();
                auto engines = mainWindow.getRenderingEngines();
                const int index = commandID - MainWindow::renderingEngineOne;

                result.setInfo ("Use " + engines[index], "Uses the " + engines[index] + " engine to render the UI", generalCategory, 0);
                result.setTicked (mainWindow.getActiveRenderingEngine() == index);

                result.addDefaultKeypress ('1' + index, ModifierKeys::noModifiers);
                break;
            }
            default:
                break;
        }
    }

    bool perform (const InvocationInfo &info) override
    {
        if (auto* mainWindow = MainWindow::getMainAppWindow())
        {
            switch (info.commandID)
            {
                case MainWindow::renderingEngineOne:
                case MainWindow::renderingEngineTwo:
                case MainWindow::renderingEngineThree:
                    mainWindow->setRenderingEngine (info.commandID - MainWindow::renderingEngineOne);
                    break;
                default:
                    return false;
            }
        }
        return true;
    }

    //==============================================================================
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
            loadClip(fileToLoad);
            selectWaveformType(2);
            resized();
        }
        else if (button == playButton)
        {
            auto state = playButton->getToggleState();
            audioLooper.setStartReadPoint(0);
            if (state)
            {
                button->setButtonText("STOP");
                audioLooper.play();

                for (int i = 0; i < numWaveforms; ++i)
                {
                    openGLWaveform[i]->start();
                }

            }
            else
            {
                button->setButtonText("PLAY");
                audioLooper.stop();

                for (int i = 0; i < numWaveforms; ++i) {
                    vWaveform[i]->clear();
                    openGLWaveform[i]->stop();
                }

                thumbnailToUpdate.reset(sumBuffer->getNumChannels(), 44100);
                nextSampleNum = 0;
            }
        }
    }

    void comboBoxChanged (ComboBox *c) override
    {
        int selectedIndex = c->getSelectedId();
        if (c == waveformChoices)
        {
            selectWaveformType(selectedIndex);
        }
    }

    //==============================================================================
    bool isShowingOpenGLDemo() const
    {
        return openGLWaveform[0]->isVisible();
    }
private:
    //==============================================================================

    // Your private member variables go here...
//==============================================================================
    AudioIO audioIO;
    LoopAudio<float> audioLooper; /*!< LoopAudio object in charge of playback. */
    Atomic<float> gain = 1.0f;
    ScopedPointer<AudioSampleBuffer> sumBuffer;
    int fileSamples;
//==============================================================================
    ScopedPointer<TextButton> loadButton;
    ScopedPointer<TextButton> playButton;
    ScopedPointer<Slider> gainKnob; // [0, 1] values
    ScopedPointer<ComboBox> waveformChoices;
//==============================================================================
    int64 nextSampleNum;
    int numWaveforms;
//==============================================================================
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnailToUpdate;
    OwnedArray<WaveformThumbnail> waveformthumbnail;
//==============================================================================
    OwnedArray<AudioOscilloscope> vWaveform;

//==============================================================================
    OwnedArray<AudioVisualiserComponent> visualiserComponents;
//==============================================================================
    // Audio & GL Audio Buffer
    RingBuffer<float> * ringBuffer;
    OwnedArray<OpenGLWaveform> openGLWaveform;

//==============================================================================
    // auxiliary methods
    void loadClip(File fileToLoad)
    {
        AudioIO::AudioDataPair<float> audioDataPair;
        if (audioIO.checkAndLoadFile(fileToLoad, audioDataPair))
        {
            audioLooper.load(*audioDataPair.getRawBuffer(), audioDataPair.metadata.sampleRate);

            sumBuffer->setSize(1, audioDataPair.getRawBuffer()->getNumSamples());
            sumBuffer->clear();
            fileSamples = audioDataPair.getRawBuffer()->getNumSamples();
            ippsAdd_32f(audioDataPair.getRawBuffer()->getWritePointer(0), audioDataPair.getRawBuffer()->getWritePointer(1), sumBuffer->getWritePointer(0), fileSamples);
            ippsMulC_32f_I(0.5f, sumBuffer->getWritePointer(0), fileSamples);
            for (int i = 0; i < numWaveforms; ++i)
            {
                waveformthumbnail[i]->addBackgroundWaveform(*sumBuffer, audioDataPair.metadata.sampleRate);
                waveformthumbnail[i]->repaint();
            }

            thumbnailToUpdate.reset(sumBuffer->getNumChannels(), audioDataPair.metadata.sampleRate);
            nextSampleNum = 0;

            // Setup Ring Buffer of GLfloat's for the visualizer to use
            // Uses two channels
            ringBuffer = new RingBuffer<GLfloat> (2, fileSamples);
            openGLWaveform.clear();
            for (int i = 0; i < numWaveforms; ++i)
            {
                OpenGLWaveform* openglW = new OpenGLWaveform(ringBuffer, fileSamples);
                addAndMakeVisible(openglW);
                openGLWaveform.add(openglW);
            }
        }
    }

    void selectWaveformType(int type) // type: 1 --> thumbnail, 2 --> audio oscilloscpe, 3 --> opengl
    {
        switch (type)
        {
            case 1:
                for (int i = 0; i < numWaveforms; ++i)
                {
                    openGLWaveform[i]->stop();
                    openGLWaveform[i]->setVisible(false);
                    vWaveform[i]->setVisible(false);
                    visualiserComponents[i]->setVisible(false);
                    waveformthumbnail[i]->setVisible(true);
                }
                break;
            case 2:
                for (int i = 0; i < numWaveforms; ++i)
                {
                    openGLWaveform[i]->stop();
                    openGLWaveform[i]->setVisible(false);
                    waveformthumbnail[i]->setVisible(false);
                    visualiserComponents[i]->setVisible(false);
                    vWaveform[i]->setVisible(true);
                }
                break;
            case 3:
                for (int i = 0; i < numWaveforms; ++i)
                {
                    waveformthumbnail[i]->setVisible(false);
                    vWaveform[i]->setVisible(false);
                    visualiserComponents[i]->setVisible(false);
                    openGLWaveform[i]->start();
                    openGLWaveform[i]->setVisible(true);
                }
                break;
            case 4:
                for (int i = 0; i < numWaveforms; ++i)
                {
                    waveformthumbnail[i]->setVisible(false);
                    vWaveform[i]->setVisible(false);
                    openGLWaveform[i]->stop();
                    openGLWaveform[i]->setVisible(false);
                    visualiserComponents[i]->setVisible(true);
                }
                break;
            default:
                break;
        }
    }

    void feedWaveformThumbnail(AudioSampleBuffer& buffer)
    {
        const auto numSamples = buffer.getNumSamples();
        sumBuffer->clear();
        ippsAdd_32f(buffer.getWritePointer(0), buffer.getWritePointer(1), sumBuffer->getWritePointer(0), numSamples);
        ippsMulC_32f_I(0.5f, sumBuffer->getWritePointer(0), numSamples);

        thumbnailToUpdate.addBlock(nextSampleNum, *sumBuffer, 0, numSamples);
        nextSampleNum += numSamples;
        if (nextSampleNum > audioLooper.getNumSamples())
        {
            nextSampleNum = 0;
            thumbnailToUpdate.reset(sumBuffer->getNumChannels(), 44100);
        }
    }

    void feedAudioOscilloscope(AudioSampleBuffer& buffer)
    {
        const auto numSamples = buffer.getNumSamples();
        sumBuffer->clear();
        ippsAdd_32f(buffer.getWritePointer(0), buffer.getWritePointer(1), sumBuffer->getWritePointer(0), numSamples);
        ippsMulC_32f_I(0.5f, sumBuffer->getWritePointer(0), numSamples);

        for (int i = 0; i < numWaveforms; ++i)
            vWaveform[i]->processBlock(sumBuffer->getReadPointer(0), numSamples);

        nextSampleNum += numSamples;
    }

    void feedOpenGLWaveform(AudioSampleBuffer& buffer)
    {
        const auto numSamples = buffer.getNumSamples();
        // Write to Ring Buffer
        ringBuffer->writeSamples (buffer, 0, numSamples);
        nextSampleNum += numSamples;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

//==============================================================================
static ScopedPointer<ApplicationCommandManager> applicationCommandManager;

MainWindow::MainWindow (String name)
: DocumentWindow (name,
                  Desktop::getInstance().getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                  DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    contentComponent = new MainContentComponent();
    setContentOwned (contentComponent, true);
    setResizable (true, true);

    centreWithSize (getWidth(), getHeight());
    setVisible (true);

    // this lets the command manager use keypresses that arrive in our window to send out commands
    addKeyListener (getApplicationCommandManager().getKeyMappings());

    triggerAsyncUpdate();
}

MainWindow::~MainWindow()
{
    clearContentComponent();
    contentComponent = nullptr;
    applicationCommandManager = nullptr;

#if JUCE_OPENGL
    openGLContext.detach();
#endif
}

void MainWindow::closeButtonPressed() 
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

MainWindow* MainWindow::getMainAppWindow()
{
    for (int i = TopLevelWindow::getNumTopLevelWindows(); --i >= 0;)
        if (auto* maw = dynamic_cast<MainWindow*> (TopLevelWindow::getTopLevelWindow (i)))
            return maw;

    return nullptr;
}

void MainWindow::handleAsyncUpdate()
{
    // This registers all of our commands with the command manager but has to be done after the window has
    // been created so we can find the number of rendering engines available
    auto& commandManager = MainWindow::getApplicationCommandManager();

    commandManager.registerAllCommandsForTarget (contentComponent);
    commandManager.registerAllCommandsForTarget (JUCEApplication::getInstance());
}

ApplicationCommandManager& MainWindow::getApplicationCommandManager()
{
    if (applicationCommandManager == nullptr)
        applicationCommandManager = new ApplicationCommandManager();

    return *applicationCommandManager;
}

static const char* openGLRendererName = "OpenGL Renderer";
StringArray MainWindow::getRenderingEngines() const
{
    StringArray renderingEngines;

    if (auto* peer = getPeer())
        renderingEngines = peer->getAvailableRenderingEngines();

#if JUCE_OPENGL
    renderingEngines.add (openGLRendererName);
#endif

    return renderingEngines;
}

void MainWindow::setRenderingEngine (int index)
{
    showMessageBubble (getRenderingEngines()[index]);

#if JUCE_OPENGL
    if (getRenderingEngines()[index] == openGLRendererName
        && contentComponent != nullptr
        && ! contentComponent->isShowingOpenGLDemo())
    {
        openGLContext.attachTo (*getTopLevelComponent());
        return;
    }

    openGLContext.detach();
#endif

    if (auto* peer = getPeer())
        peer->setCurrentRenderingEngine (index);
}

void MainWindow::setOpenGLRenderingEngine()
{
    setRenderingEngine (getRenderingEngines().indexOf (openGLRendererName));
}

int MainWindow::getActiveRenderingEngine() const
{
#if JUCE_OPENGL
    if (openGLContext.isAttached())
        return getRenderingEngines().indexOf (openGLRendererName);
#endif

    if (auto* peer = getPeer())
        return peer->getCurrentRenderingEngine();

    return 0;
}

void MainWindow::showMessageBubble (const String& text)
{
    currentBubbleMessage = new BubbleMessageComponent (500);
    getContentComponent()->addChildComponent (currentBubbleMessage);

    AttributedString attString;
    attString.append (text, Font (15.0f));

    currentBubbleMessage->showAt ({ getLocalBounds().getCentreX(), 10, 1, 1 },
                                  attString,
                                  500,  // numMillisecondsBeforeRemoving
                                  true,  // removeWhenMouseClicked
                                  false); // deleteSelfAfterUse
}

