/*
  ==============================================================================

    MainComponent.h
    Created: 12 Dec 2017 10:53:15am
    Author:  maria kourousi

  ==============================================================================
*/

#pragma once

#include "AudioIO.h"
#include "LoopAudio.h"
#include "WaveformThumbnail.h"
#include "OpenGLWaveform.h"
#include "AudioOscilloscope.h"
#include "CustomAudioThumbnail.h"

#include "ipps.h"
#include "ippvm.h"
/*
 ==============================================================================

                         This file was auto-generated!

 ==============================================================================
 */

//==============================================================================
/*
 This class implements the desktop window that contains an instance of
 our MainContentComponent class.
 */

class MainContentComponent;

class MainWindow    : public DocumentWindow,
                        private AsyncUpdater
{
public:
    MainWindow (String name);
    ~MainWindow();

    static MainWindow* getMainAppWindow(); // returns the MainWindow if it exists.

    // called by the OS when the window's close button is pressed.
    void closeButtonPressed() override;

    // (return the command manager object used to dispatch command events)
    static ApplicationCommandManager& getApplicationCommandManager();

    StringArray getRenderingEngines() const;
    int getActiveRenderingEngine() const;
    void setRenderingEngine (int index);
    void setOpenGLRenderingEngine();

    /* Note: Be careful if you override any DocumentWindow methods - the base
     class uses a lot of them, so by overriding you might break its functionality.
     It's best to do all your work in your content component instead, but if
     you really have to override any DocumentWindow methods, make sure your
     subclass also calls the superclass's method.
     */

    enum CommandIDs
    {
        renderingEngineOne          = 0x2300,
        renderingEngineTwo          = 0x2301,
        renderingEngineThree        = 0x2302, // these must be contiguous!
    };

private:
    ScopedPointer<MainContentComponent> contentComponent;
    ScopedPointer<BubbleMessageComponent> currentBubbleMessage;

#if JUCE_OPENGL
    OpenGLContext openGLContext;
#endif

    void handleAsyncUpdate() override;
    void showMessageBubble (const String&);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};
