/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 */
class GainPanTutorialAudioProcessor : public juce::AudioProcessor {
 public:
  //==============================================================================
  GainPanTutorialAudioProcessor();
  ~GainPanTutorialAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  //==============================================================================
  juce::AudioProcessorValueTreeState parameters{
      *this,
      nullptr,
      juce::Identifier("PARAMETERS"),
      {
          std::make_unique<juce::AudioParameterFloat>(
              "gain", "gain", juce::NormalisableRange<float>(-100.0f, 10.0f),
              0.0f),
          std::make_unique<juce::AudioParameterFloat>(
              "panAngle", "panAngle",
              juce::NormalisableRange<float>(-100.0f, 100.0f), 0.0f),
          std::make_unique<juce::AudioParameterChoice>(
              "panRule", "panRule", juce::StringArray("linear", "balanced"), 1),
          std::make_unique<juce::AudioParameterBool>("bypass", "bypass", false),
      }};

  juce::AudioProcessorParameter* getBypassParameter() const {
    return parameters.getParameter("bypass");
  }

 private:
  std::atomic<float>* gain = parameters.getRawParameterValue("gain");
  std::atomic<float>* panAngle = parameters.getRawParameterValue("panAngle");
  std::atomic<float>* panRule = parameters.getRawParameterValue("panRule");
  std::atomic<float>* bypass = parameters.getRawParameterValue("bypass");

  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSmoothed;
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
      panAngleSmoothed;
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dryWetSmoothed;

  void updateParameters() {
    gainSmoothed.setTargetValue(*gain);
    panAngleSmoothed.setTargetValue((*panAngle / 100 + 1) * 0.5);
    dryWetSmoothed.setTargetValue(*bypass ? 0.0f : 1.0f);
  };

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainPanTutorialAudioProcessor)
};
