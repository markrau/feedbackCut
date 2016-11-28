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
fftNyquist(fftSize / 2)
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
	//	EQs[i] = new peakEQ;
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

	////initialize reverb
	//for (int i = 0; i < sizeof(EQs); i++)
	//{
	//	EQs[i].init(sampleRate);
	//	EQs[i].buildUserInterface(&EQcontrols[i]);



	//	// setting default values for the Faust module parameters
	//	EQcontrols[i].setParamValue("/PeakEqualizer/level", 0);
	//	EQcontrols[i].setParamValue("/PeakEqualizer/freq", 1000);
	//	EQcontrols[i].setParamValue("/PeakEqualizer/Q", 100);

	//}

	// Print the list of parameters address of "saw"
	// To get the current (default) value of these parameters, sawControl.getParamValue("paramPath") can be used
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
		float* tempBuff;

        // ..do something to the data...
		for (int i = 0; i < buffer.getNumSamples(); ++i) {
			//tempBuff[i] = buffer[i];
		}
			pushBufferIntoFifo(channelData, buffer.getNumSamples());
			//pushNextSampleIntoFifo(channelData[i]);
			//fftData[i+fftFillCounter*buffer.getNumSamples()] = buffer.getSample(channel,i);

		//buffer.
    }



	// render the FFT data..
	fftInputAudio.performFrequencyOnlyForwardTransform(fftFreqData);
	

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
