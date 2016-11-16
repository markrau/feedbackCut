# feedbackCut

# Short Description
An audio plugin that will function to reduce acoustic feedback between a microphone and speakers. The feedback reduction will be adaptive, and will return to "no reduction" if there is no feedback. 

# Build Instructions 
Using the Projucer, build the project like a standard Juce audio plugin.

# Documentation
The image "plannedGUI_draft1" shows a first draft of the user interface. The green curve shows the frequency spectrum and the red curve is the filter response. The orange circle by the notch filter can be pressed to keep the filter, otherwise it will adapt back to a flat response.
