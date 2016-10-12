// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Adam Jaffe
// Description: A program that translates
//              text into sound using an FM
//              synthesizer and the ASCII
//              encoding of each character.

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"
#include "Smooth.h"
#include "mydsp.h"
#include <string>
#include <sstream>
#include <vector>

#define DEFAULT_FREQ 440

class MainContentComponent :
    public AudioAppComponent,
    private ToggleButton::Listener,
    private TextEditor::Listener
{
public:
    MainContentComponent()
    : carrierFrequency(440.0),
    index(0.0),
    onOff (0.0)
    {
        // Customize program look
        customLook.setDefaultSansSerifTypefaceName("Arial");
	    customLook.setDefaultLookAndFeel(&customLook);
        
        // Set the flange parameters
        flangerControl.setParamValue("/flange/dmax",20);
        flangerControl.setParamValue("/flange/depth",1);
        flangerControl.setParamValue("/flange/fb",5);
        
        // Set the beginning of the text iterator
        it = text.begin();
        
        // Make text box and add it to the window
        addAndMakeVisible(textBox);
        textBox.setMultiLine(true, true);
        textBox.addListener(this);
        
        // Make play button and add it to the window
        addAndMakeVisible(playButton);
        playButton.setButtonText("PLAY");
        playButton.addListener(this);
        
        setSize (600, 200);
        nChans = 2;
        setAudioChannels (0, nChans);
        audioBuffer = new float*[nChans];
    }
    
    ~MainContentComponent() {
        shutdownAudio();
        delete [] audioBuffer;
    }
    
    void resized() override {
        // placing the UI elements in the main window
        const int margin = 20;
        const int buttonWidth = 100;
        const int buttonHeight = 60;
        textBox.setBounds(margin, margin, getWidth() - 2 * margin, getHeight() - 3 * margin - buttonHeight);
        playButton.setBounds((getWidth() - buttonWidth) / 2, getHeight() - margin - buttonHeight, buttonWidth, buttonHeight);
    }
    
    
    void paint(Graphics& g) {
        g.fillAll(Colours::grey);
    }
     
    void buttonClicked (Button* button) override {
        if(button == &playButton) {
            onOff = 1.0;
            split(textBox.getText().toStdString(), ' ', text);
            for(it = text.begin(); it != text.end(); ++it) {
                // update synthesizer
                carrierFrequency = charToFreq((*it)[0]);
                modulatorFrequency = charToFreq((*it)[1]);
                index = charToFreq((*it)[(*it).length() - 1]);
                double duration = strToTime(*it);
                // play the sound
                onOff = 1.0;
                Time::waitForMillisecondCounter(Time::getMillisecondCounter() + duration);
            }
            onOff = 0.0;
        }
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        carrierFrequency = (it == text.end()) ? DEFAULT_FREQ : charToFreq((*it)[0]);
        carrier.setSamplingRate(sampleRate);
        carrier.setFrequency(carrierFrequency);
        modulatorFrequency = (it == text.end()) ? DEFAULT_FREQ : charToFreq((*it)[1]);
        modulator.setSamplingRate(sampleRate);
        modulator.setFrequency(modulatorFrequency);
        for(int i = 0; i < 4; i++) {
            smooth[i].setSmooth(0.9999);
        }
        flanger.init(sampleRate);
		flanger.buildUserInterface(&flangerControl);
    }
    
    void releaseResources() override{
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override {
        const float level = 1.0;
        for(int i = 0; i < nChans; i++) {
			audioBuffer[i] = bufferToFill.buffer->getWritePointer (i, bufferToFill.startSample);
		}

        for (int sample = 0; sample < bufferToFill.numSamples; ++sample) {
            carrier.setFrequency(smooth[0].tick(carrierFrequency) + modulator.tick()*smooth[3].tick(index));
            audioBuffer[0][sample] = carrier.tick() * smooth[1].tick(level) * smooth[2].tick(onOff);
        }
		flanger.compute(bufferToFill.numSamples, audioBuffer, audioBuffer); //apply the flange effect!
    }
    
    
private:
    // Look and Feel
    LookAndFeel_V3 customLook;
    
    // UI elements
    TextButton playButton;
    TextEditor textBox;
    
    // textBox data
    std::vector<std::string> text;
    std::vector<std::string>::iterator it;
    
    // flanger effect
    mydsp flanger;
    MapUI flangerControl;
    
    // FM synthesis data
    Sine carrier, modulator;
    Smooth smooth[4];
    double carrierFrequency, modulatorFrequency, index, onOff;
    
    // Misc audio data
    int nChans;
    float** audioBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
    
    
    // some useful private methods
    double charToFreq(char a) {
        if (a > 63) a -= 64;
        return 200 + 12.5 * a;
    }
    
    double strToTime(const std::string &s) {
        return 250 * s.length();
    }
    
    void split(const std::string &s, char delim, std::vector<std::string> &elems) {
        elems.clear();
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
