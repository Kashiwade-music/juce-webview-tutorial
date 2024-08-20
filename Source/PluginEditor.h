/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

struct SinglePageBrowser : juce::WebBrowserComponent {
  using WebBrowserComponent::WebBrowserComponent;

  // Prevent page loads from navigating away from our single page web app
  bool pageAboutToLoad(const juce::String& newURL) override {
    return newURL == juce::String("http://localhost:5173/") ||
           newURL == getResourceProviderRoot();
  }
};

//==============================================================================
/**
 */
class GainPanTutorialAudioProcessorEditor : public juce::AudioProcessorEditor {
 public:
  GainPanTutorialAudioProcessorEditor(GainPanTutorialAudioProcessor&);
  ~GainPanTutorialAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;
  //==============================================================================
  int getControlParameterIndex(juce::Component&) override {
    return controlParameterIndexReceiver.getControlParameterIndex();
  }

 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  GainPanTutorialAudioProcessor& audioProcessor;

  //==============================================================================

  juce::WebControlParameterIndexReceiver controlParameterIndexReceiver;

  juce::WebSliderRelay gainRelay{webComponent, "gain"};
  juce::WebSliderRelay panRelay{webComponent, "panAngle"};
  juce::WebComboBoxRelay panRuleRelay{webComponent, "panRule"};
  juce::WebToggleButtonRelay bypassRelay{webComponent, "bypass"};

  //==============================================================================

  SinglePageBrowser webComponent{
      juce::WebBrowserComponent::Options{}
          .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
          .withWinWebView2Options(
              juce::WebBrowserComponent::Options::WinWebView2{}
                  .withUserDataFolder(juce::File::getSpecialLocation(
                      juce::File::SpecialLocationType::tempDirectory)))
          .withOptionsFrom(gainRelay)
          .withOptionsFrom(panRelay)
          .withOptionsFrom(panRuleRelay)
          .withOptionsFrom(bypassRelay)
          .withOptionsFrom(controlParameterIndexReceiver)
          .withResourceProvider(
              [this](const auto& url) { return getResource(url); },
              juce::URL{"http://localhost:5173/"}.getOrigin())};

  std::optional<juce::WebBrowserComponent::Resource> getResource(
      const juce::String& url);

  const char* getMimeForExtension(const juce::String& extension);

  //==============================================================================

  juce::WebSliderParameterAttachment gainAttachment{
      *audioProcessor.parameters.getParameter("gain"), gainRelay, nullptr};
  juce::WebSliderParameterAttachment panAttachment{
      *audioProcessor.parameters.getParameter("panAngle"), panRelay, nullptr};
  juce::WebComboBoxParameterAttachment panModeAttachment{
      *audioProcessor.parameters.getParameter("panRule"), panRuleRelay,
      nullptr};
  juce::WebToggleButtonParameterAttachment bypassAttachment{
      *audioProcessor.parameters.getParameter("bypass"), bypassRelay, nullptr};

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      GainPanTutorialAudioProcessorEditor)
};
