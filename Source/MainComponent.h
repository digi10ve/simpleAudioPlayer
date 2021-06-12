#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent,
                      public juce::ChangeListener {
 public:
  //==============================================================================
  MainComponent();
  ~MainComponent() override;

  //==============================================================================
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void getNextAudioBlock(
      const juce::AudioSourceChannelInfo& bufferToFill) override;
  void releaseResources() override;

  //==============================================================================
  void paint(juce::Graphics& g) override;
  void resized() override;

 private:
  juce::AudioDeviceManager otherDeviceManager;
  std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettings;

  enum TransportState { Stopped, Starting, Stopping, Playing };

  TransportState state;

  void openButtonClicked();
  void playButtonClicked();
  void stopButtonClicked();
  void transportStateChanged(TransportState newState);
  void changeListenerCallback(juce::ChangeBroadcaster* source) override;

  juce::AudioFormatManager formatManager;
  std::unique_ptr<juce::AudioFormatReaderSource> playSource;
  juce::AudioTransportSource transport;

  juce::TextButton openButton;
  juce::TextButton playButton;
  juce::TextButton stopButton;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
