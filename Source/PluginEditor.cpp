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
	//button1 ("Test button Click"),
	filterSpeedSlider(Slider::Rotary, Slider::NoTextBox),
	filterSeveritySlider(Slider::Rotary, Slider::NoTextBox),
	filterQSlider(Slider::Rotary, Slider::NoTextBox),
	feedbackThreshold(Slider::Rotary, Slider::NoTextBox),
	bypass("Bypass"),
	filterKeep1("*",Colours::red, Colours::pink,Colours::green)
	//Timer

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.


	//filterKeep1.setShape();

	//add buttons
	addAndMakeVisible(&bypass); 
	addAndMakeVisible(&filterKeep1);

	setResizable(true, true);

	


	//add sliders to editor
	filterSpeedSlider.setRange(0.01, 1.0);
	filterSpeedSlider.setValue(0.5);
	filterSpeedSlider.setColour(0x1001311, Colours::darkmagenta);
	filterSpeedSlider.setColour(0x1001312, Colours::darkmagenta);
	filterSeveritySlider.setRange(-40, 0.0);
	filterSeveritySlider.setValue(-20);
	filterSeveritySlider.setColour(0x1001311, Colours::darkmagenta);
	filterSeveritySlider.setColour(0x1001312, Colours::darkmagenta);
	filterQSlider.setRange(0.01, 50.0);
	filterQSlider.setValue(20);
	filterQSlider.setColour(0x1001311, Colours::darkmagenta);
	filterQSlider.setColour(0x1001312, Colours::darkmagenta);
	feedbackThreshold.setRange(0.0, 1.0);
	feedbackThreshold.setValue(0.5);
	feedbackThreshold.setColour(0x1001311, Colours::darkmagenta);
	feedbackThreshold.setColour(0x1001312, Colours::darkmagenta);
	
	

	filterSpeedSlider.addListener(this);
	filterSeveritySlider.addListener(this);
	filterQSlider.addListener(this);
	feedbackThreshold.addListener(this);
	//button1.addListener(this);


	// Add listeners
	filterSpeedSliderLabel.addListener(this);
	filterSeveritySliderLabel.addListener(this);
	filterQSliderLabel.addListener(this);
	feedbackThresholdLabel.addListener(this);
	bypass.addListener(this);
	

	addAndMakeVisible(&filterSpeedSlider);
	addAndMakeVisible(&filterSeveritySlider);
	addAndMakeVisible(&filterQSlider);
	addAndMakeVisible(&feedbackThreshold);
	addAndMakeVisible(&filterSpeedSliderLabel);
	addAndMakeVisible(&filterSeveritySliderLabel);
	addAndMakeVisible(&filterQSliderLabel);
	addAndMakeVisible(&feedbackThresholdLabel);

	//filterSpeedSlider.setTextBoxStyle();


	//Timer refresh rate for spectrogram
	startTimerHz(60);

    setSize (800, 600);
	
}

FeedbackCutAudioProcessorEditor::~FeedbackCutAudioProcessorEditor()
{
}

//==============================================================================
void FeedbackCutAudioProcessorEditor::paint(Graphics& g)
{
	g.fillAll(Colour::Colour(107,4,0));

	/*g.setColour (Colours::black);
	g.setFont (15.0f);
	g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);*/

	const float w = (float)getWidth();
	const float h = (float)getHeight();



	g.setColour(Colours::black);
	g.setFont(h / 14);
	g.drawFittedText("FeedBackCut", (int)(0.015*w), (int)(0.015*h), (int)(0.4*w), (int)(0.1*h), Justification::topLeft, 1);

	g.setColour(Colours::black);
	g.setFont(15.0f);
	g.drawFittedText("Filter Speed (s)", (int)(0.025*w), (int)(0.9*h), (int)(0.2*w), (int)(0.1*h), Justification::centred, 2);
	g.drawFittedText("Feedback Reduction (dB)", (int)(0.275*w), (int)(0.9*h), (int)(0.2*w), (int)(0.1*h), Justification::centred, 2);
	g.drawFittedText("Bandwidth (Hz)", (int)(0.525*w), (int)(0.9*h), (int)(0.2*w), (int)(0.1*h), Justification::centred, 2);
	g.drawFittedText("Feedback Threshold", (int)(0.775*w), (int)(0.9*h), (int)(0.2*w), (int)(0.1*h), Justification::centred, 2);

	



	//draw rectangle and axes for FFT
	Rectangle<float> spect((float)w*0.05, (float)h*0.12, (float)w*0.9, (float)h*0.5);
	Rectangle<float> spectOutline((float)w*0.05, (float)h*0.12, (float)w*0.9, (float)h*0.5);

	g.setColour(Colour::Colour(108,77,108));
	//g.drawRect((float)w*0.05, (float)h*0.1, (float)w*0.9, (float)h*0.4);
	g.drawRect(spect);
	g.fillRect(spect);
	g.setColour(Colours::black);
	g.drawRect(spectOutline, 4);

	// Draw axes ticks for FFT
	float freqNormalize = log10(20000);
	float minFreqRatio = log10(20) / log10(20000);
	g.setColour(Colours::black);
	
	float stretchNormalize = 1 - log10(20) / log10(20000);
	int fTick = 50;
	float xTickPosition = w*0.91*(0.05 + ( log10(fTick) / freqNormalize -minFreqRatio) / stretchNormalize);
	g.drawLine(xTickPosition, h*0.605, xTickPosition, h*0.62, 4);
	g.drawFittedText("50", (int)xTickPosition-0.02*w, (int)h*0.62, (int)(0.05*w), (int)(0.05*h), Justification::centred, 2);
	fTick = 100;
	xTickPosition = w*0.91*(0.05 + (log10(fTick) / freqNormalize - minFreqRatio) / stretchNormalize);
	g.drawFittedText("100", (int)xTickPosition - 0.02*w, (int)h*0.62, (int)(0.05*w), (int)(0.05*h), Justification::centred, 2);
	g.drawLine(xTickPosition, h*0.605, xTickPosition, h*0.62, 4);
	fTick = 400;
	xTickPosition = w*0.91*(0.05 + (log10(fTick) / freqNormalize - minFreqRatio) / stretchNormalize);
	g.drawFittedText("400", (int)xTickPosition - 0.02*w, (int)h*0.62, (int)(0.05*w), (int)(0.05*h), Justification::centred, 2);
	g.drawLine(xTickPosition, h*0.605, xTickPosition, h*0.62, 4);
	fTick = 1000;
	xTickPosition = w*0.91*(0.05 + (log10(fTick) / freqNormalize - minFreqRatio) / stretchNormalize);
	g.drawFittedText("1000", (int)xTickPosition - 0.02*w, (int)h*0.62, (int)(0.05*w), (int)(0.05*h), Justification::centred, 2);
	g.drawLine(xTickPosition, h*0.605, xTickPosition, h*0.62, 4);
	fTick = 4000;
	xTickPosition = w*0.91*(0.05 + (log10(fTick) / freqNormalize - minFreqRatio) / stretchNormalize);
	g.drawFittedText("4000", (int)xTickPosition - 0.02*w, (int)h*0.62, (int)(0.05*w), (int)(0.05*h), Justification::centred, 2);
	g.drawLine(xTickPosition, h*0.605, xTickPosition, h*0.62, 4);
	fTick = 10000;
	xTickPosition = w*0.91*(0.05 + (log10(fTick) / freqNormalize - minFreqRatio) / stretchNormalize);
	g.drawFittedText("10000", (int)xTickPosition - 0.02*w, (int)h*0.62, (int)(0.05*w), (int)(0.05*h), Justification::centred, 2);
	g.drawLine(xTickPosition, h*0.605, xTickPosition, h*0.62, 4);
	






	// drawing spectroscope
	PathStrokeType stroke(1.0f);
	g.setColour(Colours::lightgoldenrodyellow);
	if (!processor.takingFFT) {
		g.strokePath(spectroscope,	stroke, AffineTransform::verticalFlip(getHeight()));
	}
	
	

}

void FeedbackCutAudioProcessorEditor::timerCallback()
{
	//if (nextFFTBlockReady)
	//{
	const float w = (float)getWidth();
	const float h = (float)getHeight();
	const float maxFreqRatio = 0.25; // set the highest frequency displayed by the spectroscope (1 = nyquist)

	//fftFreqData = log
	//if (!processor.takingFFT) {

	int maxIndex = (int)processor.fftNyquist * 20000 / (processor.getSampleRate() / 2);
	int minIndex = (int)roundf(processor.fftNyquist * 20 / (processor.getSampleRate() / 2));
	float offset =0;

	for (int j = 1; j < minIndex + 1; j++) {
		offset = offset + 0.89*log10((float)j)*w / (log10((float)maxIndex));
	}

		spectroscope.clear();
		spectroscope.startNewSubPath(0.89*log10((float)minIndex)*w / (log10((float)maxIndex)) + 0.055*w-offset, (3 * h / 4) + 20 * log10(abs(processor.fftFreqData[minIndex])));

	
		int numPaths = 400;
		for (int i = minIndex; i < numPaths; i++) {
			//const int fftDataIndex = (int)i*maxFreqRatio;
			const int fftDataIndex = (int)(pow(10,(float)i*((float)log10((float)maxIndex)/numPaths)));
			const float x = 0.89*log10((float)fftDataIndex)*w / (log10((float)maxIndex));
			spectroscope.lineTo(x+0.055*w, (3 * h / 4) + 20 * log10(abs(processor.fftFreqData[fftDataIndex]))); // creating the path of the FFT block
		}
		repaint();
	//}
		
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
	feedbackThresholdLabel.setEditable(true);




	filterSpeedSlider.setBoundsRelative(0.025, 0.7, 0.2, 0.2);
	filterSeveritySlider.setBoundsRelative(0.275, 0.7, 0.2, 0.2);
	filterQSlider.setBoundsRelative(0.525, 0.7, 0.2, 0.2);
	feedbackThreshold.setBoundsRelative(0.775, 0.7, 0.2, 0.2);
	bypass.setBoundsRelative(0.8, 0.015, 0.2, 0.07);
	bypass.setCentreRelative(0.9,0.05);

	filterSpeedSliderLabel.setBoundsRelative(0.025, 0.62, 0.25, 0.1);
	filterSeveritySliderLabel.setBoundsRelative(0.275, 0.62, 0.25, 0.1);
	filterQSliderLabel.setBoundsRelative(0.525, 0.62, 0.25, 0.1);
	feedbackThresholdLabel.setBoundsRelative(0.775, 0.62, 0.25, 0.1);
}

void FeedbackCutAudioProcessorEditor::buttonClicked(Button* button)
{
	if (&bypass == button)
		processor.bypassTrue = bypass.getToggleState();
}

void FeedbackCutAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (&filterSpeedSlider == slider) {
		filterSpeedSliderLabel.setText(String(filterSpeedSlider.getValue()),
			sendNotification);

		processor.maxFilterBuffCount =  (int) (filterSpeedSlider.getValue()*((float)processor.getSampleRate() / (float)processor.getBlockSize()));

		//for (int i = 0; i < sizeof(processor.EQs); i++) {
		//	processor.EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Q_-_Filter_Bandwidth", 180);
		//}

	}
	if (&filterSeveritySlider == slider) {
		filterSeveritySliderLabel.setText(String(filterSeveritySlider.getValue()),
			sendNotification);


		processor.maxFilterReduction = filterSeveritySlider.getValue();

		//for (int i = 0; i < sizeof(processor.EQs); i++) {
			
			//processor.EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Level", filterSeveritySlider.getValue());
		//}

	}
	if (&filterQSlider == slider) {
		filterQSliderLabel.setText(String(filterQSlider.getValue()),
			sendNotification);
		for (int i = 0; i < sizeof(processor.EQs); i++) {
			processor.EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Q_-_Filter_Bandwidth", filterQSlider.getValue());
		}

	}
	if (&feedbackThreshold == slider) {
		feedbackThresholdLabel.setText(String(feedbackThreshold.getValue()), sendNotification);
		processor.energyThreshold = feedbackThreshold.getValue();
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