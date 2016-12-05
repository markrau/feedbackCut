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
	bypass(),
	filterKeep1("*",Colours::red, Colours::pink,Colours::green)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

	// This is for when I extend to multiple feedbacks
	//filterKeep1.setShape();
	//addAndMakeVisible(&filterKeep1);

	//add bypass button
	addAndMakeVisible(&bypass); 
	
	setResizable(true, true);


	//add sliders to editor
	filterSpeedSlider.setRange(0.01, 1.0);
	filterSpeedSlider.setValue(0.5);
	filterSpeedSlider.setColour(0x1001311, Colour::Colour(48, 0, 9));
	filterSpeedSlider.setColour(0x1001312, Colour::Colour(48, 0, 9));
	filterSeveritySlider.setRange(-40, 0.0);
	filterSeveritySlider.setValue(-20);
	filterSeveritySlider.setColour(0x1001311, Colour::Colour(48, 0, 9));
	filterSeveritySlider.setColour(0x1001312, Colour::Colour(48, 0, 9));
	filterQSlider.setRange(0.01, 50.0);
	filterQSlider.setValue(20);
	filterQSlider.setColour(0x1001311, Colour::Colour(48, 0, 9));
	filterQSlider.setColour(0x1001312, Colour::Colour(48, 0, 9));
	feedbackThreshold.setRange(0.0, 1.0);
	feedbackThreshold.setValue(0.5);
	feedbackThreshold.setColour(0x1001311, Colour::Colour(48, 0, 9));
	feedbackThreshold.setColour(0x1001312, Colour::Colour(48, 0, 9));
	
	

	filterSpeedSlider.addListener(this);
	filterSeveritySlider.addListener(this);
	filterQSlider.addListener(this);
	feedbackThreshold.addListener(this);



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
	const float w = (float)getWidth();
	const float h = (float)getHeight();

	g.setGradientFill(ColourGradient(Colours::darkblue,
		0.0f, 0.0f,
		Colours::darkslateblue,
		w, h,
		false));

	g.fillAll();
	g.setFont(Font("Tekton Pro", 88.00f, Font::plain));






	if (processor.filterBuffCount > 0) {
		g.setColour(Colours::darkred);
	}
	else {
		g.setColour(Colours::black);
	}


	g.setFont(h / 12);
	g.drawFittedText("FeedBackCut", (int)(0.015*w), (int)(0.015*h), (int)(0.4*w), (int)(0.1*h), Justification::topLeft, 1);

	g.setColour(Colours::black);
	g.setFont(18.0f);
	g.drawFittedText("Filter Speed (s)", (int)(0.025*w), (int)(0.9*h), (int)(0.2*w), (int)(0.1*h), Justification::centred, 2);
	g.drawFittedText("Feedback Reduction (dB)", (int)(0.275*w), (int)(0.9*h), (int)(0.2*w), (int)(0.1*h), Justification::centred, 2);
	g.drawFittedText("Bandwidth (Hz)", (int)(0.525*w), (int)(0.9*h), (int)(0.2*w), (int)(0.1*h), Justification::centred, 2);
	g.drawFittedText("Feedback Threshold", (int)(0.775*w), (int)(0.9*h), (int)(0.2*w), (int)(0.1*h), Justification::centred, 2);
	g.drawFittedText("Bypass", (int)(0.88*w), (int)(0.05*h), (int)(0.1*w), (int)(0.02*h), Justification::centred, 2);
	



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
	g.setColour(Colour::Colour(48,0,9));
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

		float yVal = log10(abs(processor.fftFreqData[minIndex]));
		if (yVal < -3.0f) {
			yVal = -3.0f;
		}
		spectroscope.startNewSubPath(0.89*log10((float)minIndex)*w / (log10((float)maxIndex)) + 0.055*w - offset, h*(0.69 + 0.1*yVal));
		
	
		int numPaths = 400;
		for (int i = minIndex+1; i < numPaths; i++) {
			//const int fftDataIndex = (int)i*maxFreqRatio;
			const int fftDataIndex = (int)(pow(10,(float)i*((float)log10((float)maxIndex)/numPaths)));
			const float x = 0.89*log10((float)fftDataIndex)*w / (log10((float)maxIndex));
			yVal = log10(abs(processor.fftFreqData[fftDataIndex]));
			if (yVal < -3.0f) {
				yVal = -3.0f;
			}


			spectroscope.lineTo(x + 0.055*w, h*(0.69 + 0.1*yVal));
		}
		repaint();

}

void FeedbackCutAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..



	filterSpeedSliderLabel.setEditable(true);
	filterSeveritySliderLabel.setEditable(true);
	filterQSliderLabel.setEditable(true);
	feedbackThresholdLabel.setEditable(true);

	filterSpeedSliderLabel.setFont(Font("Tekton Pro", 17.0f, Font::plain));
	filterSeveritySliderLabel.setFont(Font("Tekton Pro", 17.0f, Font::plain));
	filterQSliderLabel.setFont(Font("Tekton Pro", 17.0f, Font::plain));
	feedbackThresholdLabel.setFont(Font("Tekton Pro", 17.0f, Font::plain));







	filterSpeedSlider.setBoundsRelative(0.035, 0.7, 0.18, 0.18);
	filterSeveritySlider.setBoundsRelative(0.285, 0.7, 0.18, 0.18);
	filterQSlider.setBoundsRelative(0.535, 0.7, 0.18, 0.18);
	feedbackThreshold.setBoundsRelative(0.785, 0.7, 0.18, 0.18);
	bypass.setBoundsRelative(0.87, 0.03, 0.1, 0.05);
	//bypass.setCentreRelative(0.9,0.05);


	filterSpeedSliderLabel.setBoundsRelative(0.072, 0.835, 0.2, 0.1);
	filterSeveritySliderLabel.setBoundsRelative(0.322, 0.835, 0.2, 0.1);
	filterQSliderLabel.setBoundsRelative(0.572, 0.835, 0.2, 0.1);
	feedbackThresholdLabel.setBoundsRelative(0.822, 0.835, 0.2, 0.1);
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