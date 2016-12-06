/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "math.h"





//==============================================================================
FeedbackCutAudioProcessor::FeedbackCutAudioProcessor(): 

fftInputAudio(fftOrder, false),
fftNyquist(fftSize / 2),
cs()
//EQs()

#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	//Make plugin parameters
	addParameter(speedParam = new AudioParameterFloat("Filter Speed", // parameterID
		"Gain", // parameter name
		NormalisableRange<float>(0.0f, 1.0f),   // range
		0.5f)); // default value
	addParameter(filterReductionParam = new AudioParameterFloat("dB Reduction", // parameterID
		"Reduction", // parameter name
		NormalisableRange<float>(0.0f, 1.0f),   // range
		0.5f)); // default value
	addParameter(filterBandwidthParam = new AudioParameterFloat("Bandwidth", // parameterID
		"Bandwidth", // parameter name
		NormalisableRange<float>(0.0f, 1.0f),   // range
		0.5f)); // default value
	addParameter(thresholdParam = new AudioParameterFloat("Threshold", // parameterID
		"Threshold", // parameter name
		NormalisableRange<float>(0.0f, 1.0f),   // range
		0.5f)); // default value


	// Make the Faust objects
		EQs = new peakEQ[5];
		EQcontrols = new MapUI[5];
}

FeedbackCutAudioProcessor::~FeedbackCutAudioProcessor()
{
	//delete EQs;
	//delete EQcontrols;

}

//==============================================================================
const String FeedbackCutAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FeedbackCutAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FeedbackCutAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double FeedbackCutAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FeedbackCutAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FeedbackCutAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FeedbackCutAudioProcessor::setCurrentProgram (int index)
{
}

const String FeedbackCutAudioProcessor::getProgramName (int index)
{
    return String();
}

void FeedbackCutAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FeedbackCutAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

	// Use this method as the place to do any pre-playback
	// initialisation that you need..


	// Initialize Hann window
	int N = fftSize;
	const float PI = 3.141592653589793238463;
	for (int i = 0; i < N; i++) {
		fftWindow[i] =0.5*(1-cos(2*PI*i/(N-1))) ;
	}


	// set size of peakLocations vector to be the number of buffers in 40ms
	peakLocations.resize(0.04*sampleRate/samplesPerBlock);
	// initialize threshold
	energyThreshold = 0.001;
	maxPeakFrequency = 1000;
	maxFilterReduction = -20.0;
	maxFilterBuffCount = (int) (0.5*((float)sampleRate / (float)samplesPerBlock));
	filterBuffCount = 0;
	takingFFT = false;
	bypassTrue = false;




	////initialize faust filters. There are five filters because I plan on extending the plugin to work with multiple feedbacks
	for (int i = 0; i < sizeof(EQs); i++)
	{
		EQs[i].init(sampleRate);
		EQs[i].buildUserInterface(&EQcontrols[i]);

		// setting default values for the Faust module parameters
		EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Level", -20 );
		EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Peak_Frequency", 1000);
		EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Q_-_Filter_Bandwidth", 20);

	}

}

void FeedbackCutAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FeedbackCutAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FeedbackCutAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

		
		// Check to see if there is a feedback peak. If there is, apply a filter which has a gain proportional to the buffer size,
		//The filter is also slowly backed off if the feedback is no longer detected
		if (bypassTrue == false) {

			if (check(peakLocations, peakLocations.size()) && E_p / P_x > energyThreshold && maxPeakLocation > 5) {
				if (filterBuffCount == 0) {
					oldPeakFrequencies[1] = maxPeakFrequency;
				}

				if (filterBuffCount < maxFilterBuffCount) {
					filterBuffCount++;
					EQcontrols[1].setParamValue("/PeakEqualizer/Peak_EQ/Level", maxFilterReduction*((float)filterBuffCount) / ((float)maxFilterBuffCount));
					EQcontrols[1].setParamValue("/PeakEqualizer/Peak_EQ/Peak_Frequency", oldPeakFrequencies[1]);
					EQs[1].compute(buffer.getNumSamples(), buffer.getArrayOfWritePointers(), buffer.getArrayOfWritePointers());

				}
				else {
					EQs[1].compute(buffer.getNumSamples(), buffer.getArrayOfWritePointers(), buffer.getArrayOfWritePointers());
				}

			}
			else if (filterBuffCount > 0) {
				filterBuffCount--;
				EQcontrols[1].setParamValue("/PeakEqualizer/Peak_EQ/Level", maxFilterReduction*((float)filterBuffCount) / ((float)maxFilterBuffCount));
				EQcontrols[1].setParamValue("/PeakEqualizer/Peak_EQ/Peak_Frequency", oldPeakFrequencies[1]);
				EQs[1].compute(buffer.getNumSamples(), buffer.getArrayOfWritePointers(), buffer.getArrayOfWritePointers());
			}

		}

		//This is the protected fft part
		protectSection(channelData,buffer);

    }


	//Do processing to check for feedback
	maxPeakLocation = std::distance(fftFreqData, std::max_element(fftFreqData, fftFreqData + fftNyquist));
	maxPeakFrequency = maxPeakLocation*getSampleRate() / (2*fftNyquist);
	maxPeakFrequency = findPeakFrequency(fftFreqData, maxPeakLocation, getSampleRate(), 2 * fftNyquist);

	// Push new peaks into the memory
	peakLocations.erase(peakLocations.begin());
	peakLocations.push_back(maxPeakLocation);

	// Check peak energy ratio
	P_x = 0.0;
	for (int i = 0; i < fftSize / 2; i++) {
		P_x = P_x + pow(fftFreqData[i], 2);
	}
	E_p = 0.0;
	if (maxPeakLocation > 5) {
		for (int i = maxPeakLocation - 2; i < maxPeakLocation + 2; i++) {
			E_p = E_p + pow(fftFreqData[i], 2);
		}
	}
	

}

void FeedbackCutAudioProcessor::pushBufferIntoFifo(float* newBuff, int buffLength) noexcept
{
	//Fill the time vector from the buffer
	for (int i = buffLength;i< fftSize; i++) {
		fftTimeData[i - buffLength] = fftTimeData[i];
	}
	for (int j = 0;j< buffLength; j++) {
		fftTimeData[fftSize - buffLength + j] = newBuff[j];
	}

	memcpy(fftFreqData, fftTimeData, sizeof(fftTimeData));
	//Window the signal
	for (int i = 0; i < sizeof(fftTimeData); i++) {
		fftFreqData[i] = fftFreqData[i] * fftWindow[i];
	}

}

float FeedbackCutAudioProcessor::findPeakFrequency(float* fftFreqData, int peakLocation, int fs, int N) {
	//Calculate the peak frequency using quadratic interpolation
	float a = 20 * log(fftFreqData[peakLocation - 1]);
	float b = 20 * log(fftFreqData[peakLocation]);
	float c = 20 * log(fftFreqData[peakLocation +1]);
	float p = 0.5 *(a - c) / (a - 2 * b + c);
	float k = (float)peakLocation + p;
	return k* fs/N;
}

void FeedbackCutAudioProcessor::protectSection(float* channelData, AudioSampleBuffer& buffer)
{
	//Trying to mutex the fft operation so the graphics don't skip
	//const ScopedLock lock(cs);
	cs.enter();
	pushBufferIntoFifo(channelData, buffer.getNumSamples());
	fftInputAudio.performFrequencyOnlyForwardTransform(fftFreqData);
	cs.exit();
}

bool FeedbackCutAudioProcessor::check(std::vector<int> peakLocations, int n)
{
	//Check if all of the last peaks occur at the same locations
	for (int i = 0; i < n - 1; i++)
	{
		if (peakLocations[i] != peakLocations[i + 1])
			return false;
	}
	return true;
}



//==============================================================================
bool FeedbackCutAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FeedbackCutAudioProcessor::createEditor()
{
	   return new FeedbackCutAudioProcessorEditor (*this);
}

//==============================================================================
void FeedbackCutAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	MemoryOutputStream(destData, true).writeFloat(*filterBandwidthParam);
	MemoryOutputStream(destData, true).writeFloat(*filterReductionParam);
	MemoryOutputStream(destData, true).writeFloat(*speedParam);
	MemoryOutputStream(destData, true).writeFloat(*thresholdParam);
}

void FeedbackCutAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	*filterBandwidthParam = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*filterReductionParam = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*speedParam = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*thresholdParam = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FeedbackCutAudioProcessor();
}
