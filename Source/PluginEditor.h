/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class FeedbackCutAudioProcessorEditor  : public AudioProcessorEditor, 
	public Button::Listener,
	public Slider::Listener,
	public Label::Listener,
	private Timer
{
public:
    FeedbackCutAudioProcessorEditor (FeedbackCutAudioProcessor&);
    ~FeedbackCutAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
	void timerCallback() override;
    void resized() override;
	void buttonClicked(Button* button);
	void sliderValueChanged(Slider* slider);
	void labelTextChanged(Label* label);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FeedbackCutAudioProcessor& processor;

	//Testing some stuff
	TextButton button1; // Step [1]

	// Create slider objects
	Slider filterSpeedSlider;
	Slider filterSeveritySlider;
	Slider filterQSlider;

	Label filterSpeedSliderLabel;
	Label filterSeveritySliderLabel;
	Label filterQSliderLabel;

	Path spectroscope;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeedbackCutAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
