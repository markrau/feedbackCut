# feedbackCut

# Short Description
An audio plugin which aims to detect acoustic feedback and cancel it with the use of an adaptive filter. The plugin is mean't to be used in a live sound setting, and will cancel feedback in a musical way by only applying the correction when it is needed, not at all times. There are four user controlled parameters to control the plugin, they are: 

-Speed: determines how fast the filter is applied once a feedback is detected. The reduction in dB is applied incrementally over a certain number of buffers which is proportional to the speed.

-Reduction: sets the maximum gain (reduction) in dB of the filter that is applied.

-Bandwidth: sets the bandwidth of a parametric equalizer which is used to reduce the feedback.

-Threshold: Sets a threshold value between feedback and harmonic sound. If the value is lower it will be more sensitive, and more likely to catch all instances of feedback, but also more likely to misclasify a musical sound as feedback. Values around 0.6 seem to be pretty good for classifying between feedback and musical signals. 

# Implementation and Feedback Detection
The feedback detection is adapted from a method proposed by Rocha and Ferreira [1]. A rolling buffer of size 2048 is used for the frequency analysis and feedback detection. This buffer has the new buffer given by the DAW pushed in at every audio call. Two methods are used to determine if the peak frequency bin is feedback. The first method compares the ratio between the power of the entire spectrum to the energy of the highest peak and it's two neighbors on either side using the following relationship:

<html>
<head>
 <title>MathML in HTML5</title>
</head>
<body>

  <h1>MathML in HTML5</h1>

  <p>
    Square root of two:
    <math>
      <msqrt>
        <mn>2</mn>
      </msqrt>
    </math>
  </p>

</body>
</html>

[1] A. Rocha, A. Ferreira, "An Accurate Method of Detection and Cancellation of Multiple Acoustic Feedbacks", Audio Engineering Society, Convention 118, 2005.

# Build Instructions 
Using the Projucer, build the project like a standard Juce audio plugin.

# Documentation
The image "plannedGUI_draft1" shows a first draft of the user interface. The green curve shows the frequency spectrum and the red curve is the filter response. The orange circle by the notch filter can be pressed to keep the filter, otherwise it will adapt back to a flat response.
