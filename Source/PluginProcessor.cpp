/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"

//==============================================================================
GainPanTutorialAudioProcessor::GainPanTutorialAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
}

GainPanTutorialAudioProcessor::~GainPanTutorialAudioProcessor() {}

//==============================================================================
const juce::String GainPanTutorialAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool GainPanTutorialAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool GainPanTutorialAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool GainPanTutorialAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double GainPanTutorialAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int GainPanTutorialAudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int GainPanTutorialAudioProcessor::getCurrentProgram() { return 0; }

void GainPanTutorialAudioProcessor::setCurrentProgram(int index) {}

const juce::String GainPanTutorialAudioProcessor::getProgramName(int index) {
  return {};
}

void GainPanTutorialAudioProcessor::changeProgramName(
    int index, const juce::String& newName) {}

//==============================================================================
void GainPanTutorialAudioProcessor::prepareToPlay(double sampleRate,
                                                  int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  gainSmoothed.reset(sampleRate, 0.01);
  gainSmoothed.setCurrentAndTargetValue(*gain);

  panAngleSmoothed.reset(sampleRate, 0.01);
  panAngleSmoothed.setCurrentAndTargetValue((*panAngle / 100 + 1) * 0.5);

  dryWetSmoothed.reset(sampleRate, 0.01);
  dryWetSmoothed.setCurrentAndTargetValue(*bypass ? 0.0f : 1.0f);
}

void GainPanTutorialAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainPanTutorialAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

template <typename T>
inline void GainPanTutorialAudioProcessor::processBlockImpl(
    juce::AudioBuffer<T>& buffer, juce::MidiBuffer& midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  updateParameters();

  auto* leftAudioBuff = buffer.getWritePointer(0);
  auto* rightAudioBuff = buffer.getWritePointer(1);
  auto buffLength = buffer.getNumSamples();
  bool panIsLinear = (*panRule == 0);

  for (int samplesIdx = 0; samplesIdx < buffLength; samplesIdx++) {
    auto dryWetValue = dryWetSmoothed.getNextValue();
    auto gainValue =
        juce::Decibels::decibelsToGain(gainSmoothed.getNextValue(), -100.0f);
    auto leftMulValue = gainValue * dryWetValue;
    auto rightMulValue = leftMulValue;

    auto panValue = panAngleSmoothed.getNextValue();
    if (panIsLinear) {
      leftMulValue *= (1 - panValue);
      rightMulValue *= panValue;
    } else {
      leftMulValue *= std::min(1.0f, 2 - 2 * panValue);
      rightMulValue *= std::min(1.0f, 2 * panValue);
    }

    leftAudioBuff[samplesIdx] = leftAudioBuff[samplesIdx] * leftMulValue +
                                leftAudioBuff[samplesIdx] * (1 - dryWetValue);
    rightAudioBuff[samplesIdx] = rightAudioBuff[samplesIdx] * rightMulValue +
                                 rightAudioBuff[samplesIdx] * (1 - dryWetValue);
  }
}

//==============================================================================
bool GainPanTutorialAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainPanTutorialAudioProcessor::createEditor() {
  return new GainPanTutorialAudioProcessorEditor(*this);
}

//==============================================================================
void GainPanTutorialAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  auto state = parameters.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void GainPanTutorialAudioProcessor::setStateInformation(const void* data,
                                                        int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(parameters.state.getType()))
      parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new GainPanTutorialAudioProcessor();
}
