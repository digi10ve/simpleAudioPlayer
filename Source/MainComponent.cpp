#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : state(Stopped),
      openButton("Open"),
      playButton("Play"),
      stopButton("Stop") {
  setSize(300, 150);

  // Some platforms require permissions to open input channels so request that
  // here
  if (juce::RuntimePermissions::isRequired(
          juce::RuntimePermissions::recordAudio) &&
      !juce::RuntimePermissions::isGranted(
          juce::RuntimePermissions::recordAudio)) {
    juce::RuntimePermissions::request(
        juce::RuntimePermissions::recordAudio,
        [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
  } else {
    setAudioChannels(0, 2);
  }

  openButton.onClick = [this] { openButtonClicked(); };
  addAndMakeVisible(&openButton);

  playButton.onClick = [this] { playButtonClicked(); };
  playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
  playButton.setEnabled(true);
  addAndMakeVisible(&playButton);

  stopButton.onClick = [this] { stopButtonClicked(); };
  stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
  stopButton.setEnabled(false);
  addAndMakeVisible(&stopButton);

  formatManager.registerBasicFormats();
  transport.addChangeListener(this);
}

MainComponent::~MainComponent() {
  // This shuts down the audio device and clears the audio source.
  shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected,
                                  double sampleRate) {
  transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::openButtonClicked() {
  // choose a file
  juce::FileChooser chooser(
      "Choose a WAV or AIFF File",
      juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
      "*.wav; *.mp3; *.aiff");

  // if user chooses a file
  if (chooser.browseForFileToOpen()) {
    juce::File myFile;

    // what did the user choose?
    myFile = chooser.getResult();

    // read the file
    juce::AudioFormatReader* reader = formatManager.createReaderFor(myFile);

    if (reader != nullptr) {
      // get the file ready to play
      std::unique_ptr<juce::AudioFormatReaderSource> tempSource(
          new juce::AudioFormatReaderSource(reader, true));

      transport.setSource(tempSource.get(), 0, nullptr, reader->sampleRate);
      transportStateChanged(Stopped);

      playSource.reset(tempSource.release());

      DBG(reader->getFormatName());
    }
  }
}

void MainComponent::playButtonClicked() { transportStateChanged(Starting); }

void MainComponent::stopButtonClicked() { transportStateChanged(Stopping); }

void MainComponent::transportStateChanged(TransportState newState) {
  if (newState != state) {
    state = newState;

    switch (state) {
      case Stopped:
        playButton.setEnabled(true);
        transport.setPosition(0.0);
        break;

      case Playing:
        stopButton.setEnabled(true);
        playButton.setEnabled(false);
        break;

      case Starting:
        stopButton.setEnabled(true);
        playButton.setEnabled(false);
        transport.start();
        break;

      case Stopping:
        playButton.setEnabled(true);
        stopButton.setEnabled(false);
        transport.stop();
        break;
    }
  }
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source) {
  if (source == &transport) {
    if (transport.isPlaying()) {
      transportStateChanged(Playing);
    } else {
      transportStateChanged(Stopped);
    }
  }
}

void MainComponent::getNextAudioBlock(
    const juce::AudioSourceChannelInfo& bufferToFill) {
  bufferToFill.clearActiveBufferRegion();

  transport.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources() {
  // This will be called when the audio device stops, or when it is being
  // restarted due to a setting change.

  // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  // You can add your drawing code here!
}

void MainComponent::resized() {
  openButton.setBounds(10, 10, getWidth() - 20, 30);
  playButton.setBounds(10, 50, getWidth() - 20, 30);
  stopButton.setBounds(10, 90, getWidth() - 20, 30);
}
