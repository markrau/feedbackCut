/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "peakEQ.h"
#include <queue>
#include <algorithm>




//==============================================================================
/**
*/
class FeedbackCutAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FeedbackCutAudioProcessor();
    ~FeedbackCutAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

	//Create fifo instance
	void pushBufferIntoFifo(float* newBuff, int buffLength) noexcept;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//function to check if values in array are the same
	bool check(std::vector<int> peakLocations, int n);

	//Try to make method for critical section
	void protectSection();





	enum
	{
		fftOrder = 12,
		fftSize = 4096
	};

	// make these public so I can use them in PluginEditor


	//const int fftOrder = 12;
	//const int fftSize = 4096;

	float fftFreqData [2 * fftSize];
	float previousFftFreqData[2 * fftSize];
	const int fftNyquist;




	// Make array of 5 EQs
	peakEQ* EQs;
	MapUI* EQcontrols;
	float energyThreshold;
	float maxPeakFrequency;
	int maxFilterBuffCount;
	float maxFilterReduction;
	float oldPeakFrequencies[5];
	bool takingFFT;
	bool bypassTrue;


private:
    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FeedbackCutAudioProcessor)

	//Create fft object for feedback analysis as well as visualization
	FFT fftInputAudio;

	CriticalSection cs;

	
	//float fifoTemp[fftSize];
	float fftTimeData [fftSize];

	//int fifoIndex;

	//bool nextFFTBlockReady;
	//int fftFillCounter;
	//int numBufInFFT;

	// variables for feedback prediction
	float E_p;
	float P_x;
	
	//int peakLocations[10];
	std::vector<int> peakLocations;
	int maxPeakLocation;

	// gain of signal
	float originalGain;
	float gainAfterProcess;
	float magnitude;
	int filterBuffCount;



};


#endif  // PLUGINPROCESSOR_H_INCLUDED
