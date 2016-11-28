/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
FeedbackCutAudioProcessorEditor::FeedbackCutAudioProcessorEditor (FeedbackCutAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), 
	button1 ("Test button Click"),
	filterSpeedSlider(Slider::Rotary, Slider::NoTextBox),
	filterSeveritySlider(Slider::Rotary, Slider::NoTextBox),
	filterQSlider(Slider::Rotary, Slider::NoTextBox)
	//Timer

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

	//Testing stuff
	addAndMakeVisible(&button1); // Step [3] and [4]
	setResizable(true, true);

	//add sliders to editor
	filterSpeedSlider.setRange(0.0, 100.0);
	filterSeveritySlider.setRange(-50.0, 0.0);
	filterQSlider.setRange(0.01, 50.0);

	filterSpeedSlider.addListener(this);
	filterSeveritySlider.addListener(this);
	filterQSlider.addListener(this);
	//button1.addListener(this);


	// Add listeners
	filterSpeedSliderLabel.addListener(this);
	filterSeveritySliderLabel.addListener(this);
	filterQSliderLabel.addListener(this);

	addAndMakeVisible(&filterSpeedSlider);
	addAndMakeVisible(&filterSeveritySlider);
	addAndMakeVisible(&filterQSlider);
	addAndMakeVisible(&filterSpeedSliderLabel);
	addAndMakeVisible(&filterSeveritySliderLabel);
	addAndMakeVisible(&filterQSliderLabel);

	//Timer refresh rate for spectrogram
	startTimerHz(60);

    setSize (400, 300);
	
}

FeedbackCutAudioProcessorEditor::~FeedbackCutAudioProcessorEditor()
{
}

//==============================================================================
void FeedbackCutAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::mediumpurple);

    /*g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);*/

	


	

	// drawing spectroscope
	PathStrokeType stroke(1.0f);
	g.setColour(Colours::white);
	g.strokePath(spectroscope,
		stroke,
		AffineTransform::verticalFlip(getHeight())
	);
	
	

}

void FeedbackCutAudioProcessorEditor::timerCallback()
{
	//if (nextFFTBlockReady)
	//{
	const float w = (float)getWidth();
	const float h = (float)getHeight();
	const float maxFreqRatio = 0.25; // set the highest frequency displayed by the spectroscope (1 = nyquist)

	//fftFreqData = log

	spectroscope.clear();
	for (int i = 0; i<processor.fftNyquist; i++) {
		//const int fftDataIndex = (int)i*maxFreqRatio;
		const int fftDataIndex = (int)i;
		const int x = (int)i*w / (processor.fftNyquist);
		spectroscope.lineTo(x, (3*h / 4) + 2*20*log10(abs(processor.fftFreqData[fftDataIndex]))); // creating the path of the FFT block
	}
		repaint();
	//}
}

void FeedbackCutAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

	//testing stuff
	//button1.setBounds(10, 10, getWidth() - 20, getHeight() - 20);

	filterSpeedSliderLabel.setEditable(true);
	filterSeveritySliderLabel.setEditable(true);
	filterQSliderLabel.setEditable(true);




	filterSpeedSlider.setBoundsRelative(0.05, 0.7, 0.2, 0.2);
	filterSeveritySlider.setBoundsRelative(0.35, 0.7, 0.2, 0.2);
	filterQSlider.setBoundsRelative(0.65, 0.7, 0.2, 0.2);

	filterSpeedSliderLabel.setBoundsRelative(0.05, 0.62, 0.25, 0.1);
	filterSeveritySliderLabel.setBoundsRelative(0.35, 0.62, 0.25, 0.1);
	filterQSliderLabel.setBoundsRelative(0.65, 0.62, 0.25, 0.1);
}

void FeedbackCutAudioProcessorEditor::buttonClicked(Button* button)
{
	if (&button1 == button)
		filterSpeedSlider.setValue(72.0, sendNotification);
}

void FeedbackCutAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (&filterSpeedSlider == slider) {
		filterSpeedSliderLabel.setText(String(filterSpeedSlider.getValue()),
			sendNotification);

		//for (int i = 0; i < sizeof(processor.EQs); i++) {
		//	processor.EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Q_-_Filter_Bandwidth", 180);
		//}

	}
	if (&filterSeveritySlider == slider) {
		filterSeveritySliderLabel.setText(String(filterSeveritySlider.getValue()),
			sendNotification);

		for (int i = 0; i < sizeof(processor.EQs); i++) {
			processor.EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Level", filterSeveritySlider.getValue());
		}

	}
	if (&filterQSlider == slider) {
		filterQSliderLabel.setText(String(filterQSlider.getValue()),
			sendNotification);
		for (int i = 0; i < sizeof(processor.EQs); i++) {
			processor.EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Q_-_Filter_Bandwidth", filterQSlider.getValue());
		}

	}
}
void FeedbackCutAudioProcessorEditor::labelTextChanged(Label* label)
{
	if (&filterSpeedSliderLabel == label) {
		filterSpeedSlider.setValue(filterSpeedSliderLabel.getText().getDoubleValue(),
			sendNotification);
	}
	if (&filterSeveritySliderLabel == label) {
		filterSeveritySlider.setValue(filterSeveritySliderLabel.getText().getDoubleValue(),
			sendNotification);
	}
	if (&filterQSliderLabel == label) {
		filterQSlider.setValue(filterQSliderLabel.getText().getDoubleValue(),
			sendNotification);
	}
}