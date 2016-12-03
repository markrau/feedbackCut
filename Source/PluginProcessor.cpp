/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"




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
	//for (int i = 0; i < sizeof(EQs); i++)
	//{
		EQs = new peakEQ[5];
		EQcontrols = new MapUI[5];
	//}
	//EQs = new peakEQ[5];
	//extern MapUI* EQcontrols;

}

FeedbackCutAudioProcessor::~FeedbackCutAudioProcessor()
{
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

	//EQs[1].init(sampleRate);
	//EQs[1].buildUserInterface(&EQcontrols[1]);

    // Use this method as the place to do any pre-playback
    // initialisation that you need..

	// set size of peakLocations vector
	peakLocations.resize(10);
	// initialize threshold
	energyThreshold = 0.001;
	maxPeakFrequency = 1000;
	maxFilterReduction = -20.0;
	maxFilterBuffCount = (int) (0.5*((float)sampleRate / (float)samplesPerBlock));
	filterBuffCount = 0;
	takingFFT = false;
	bypassTrue = false;

	//for (int j = 0; j < sizeof(fftFreqData); j++) {
	//	fftFreqData[j] = 0;
	//}




	////initialize faust filters
	for (int i = 0; i < sizeof(EQs); i++)
	{
		EQs[i].init(sampleRate);
		EQs[i].buildUserInterface(&EQcontrols[i]);

		// setting default values for the Faust module parameters
		EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Level", -20 );
		EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Peak_Frequency", 1000);
		EQcontrols[i].setParamValue("/PeakEqualizer/Peak_EQ/Q_-_Filter_Bandwidth", 20);

	}

	// Print the list of parameters address of "saw"
	 //To get the current (default) value of these parameters, sawControl.getParamValue("paramPath") can be used
	//for (int i = 0; i<EQcontrols[1].getParamsCount(); i++) {
	//	std::cout << EQcontrols[1].getParamAdress(i) << "\n";
	//}
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
		//float* tempBuff;
		//memcpy(previousFftFreqData, fftFreqData, sizeof(previousFftFreqData));

        // ..do something to the data...
		//for (int i = 0; i < buffer.getNumSamples(); ++i) {
			//tempBuff[i] = buffer[i];
		//}
//		originalGain = buffer.getRMSLevel(channel, 0, buffer.getNumSamples());
		


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

//		gainAfterProcess = buffer.getRMSLevel(channel, 0, buffer.getNumSamples());
//		magnitude = buffer.getMagnitude(channel, 0, buffer.getNumSamples());

		
		protectSection(channelData,buffer);
		//pushNextSampleIntoFifo(channelData[i]);
		//fftData[i+fftFillCounter*buffer.getNumSamples()] = buffer.getSample(channel,i);

		//Do audio processing here

    }



	// render the FFT data..
	//takingFFT = true;

	// try to put this in the critical section part
	//cs.enter();
	//fftInputAudio.performFrequencyOnlyForwardTransform(fftFreqData);
	//cs.exit();
	//takingFFT = false;

	
	
	//Do processing to check for feedback
	maxPeakLocation = std::distance(fftFreqData, std::max_element(fftFreqData, fftFreqData + fftNyquist));
	maxPeakFrequency = maxPeakLocation*getSampleRate() / (2*fftNyquist);

	//if (peakLocations.size() < 10) {
	//	peakLocations.push(maxPeakLocation);
	//}
	//else {
	//	peakLocations.pop();
	//	peakLocations.push(maxPeakLocation);
	//}
	peakLocations.erase(peakLocations.begin());
	peakLocations.push_back(maxPeakLocation);


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
	for (int i = buffLength;i< fftSize; i++) {
		fftTimeData[i - buffLength] = fftTimeData[i];
	}
	for (int j = 0;j< buffLength; j++) {
		fftTimeData[fftSize - buffLength + j] = newBuff[j];
	}
	memcpy(fftFreqData, fftTimeData, sizeof(fftTimeData));

}

void FeedbackCutAudioProcessor::protectSection(float* channelData, AudioSampleBuffer& buffer)
{
	
	const ScopedLock lock(cs);
	pushBufferIntoFifo(channelData, buffer.getNumSamples());
	fftInputAudio.performFrequencyOnlyForwardTransform(fftFreqData);
}

bool FeedbackCutAudioProcessor::check(std::vector<int> peakLocations, int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		if (peakLocations[i] != peakLocations[i + 1])
			return false;
	}
	return true;
}



/*void checkFB()
{
	if (fftFillCounter == numBufInFFT) {

	}
}*/

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
}

void FeedbackCutAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FeedbackCutAudioProcessor();
}
