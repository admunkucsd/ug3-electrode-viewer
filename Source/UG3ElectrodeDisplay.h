//
//  UG3GridDisplay.hpp
//  ug3-electrode-viewer
//
//  Created by Allen Munk on 4/28/23.
//

#ifndef UG3GridDisplay_h
#define UG3GridDisplay_h

#include <stdio.h>
#include <VisualizerWindowHeaders.h>
#include <stack>
#include <map>
#include <set>

#include "ColourScheme.h"

class UG3ElectrodeViewerCanvas;

class Electrode : public Component
{
public:
    Electrode(juce::Rectangle<int> rect) : c(Colours::grey), rect(rect) { }

    void setColour(Colour c);
    
    Colour getColour();
    
    const juce::Rectangle<int> getRectangle();

private:
    Colour c;
    juce::Rectangle<int> const rect;
};

class UG3ElectrodeDisplay : public Component{
public:
    UG3ElectrodeDisplay(UG3ElectrodeViewerCanvas* canvas, Viewport* viewport);
        
    void refresh(const float * value);
        
    void paint(Graphics& g);
        
    void setElectrodeLayout(int layoutXMax, int layoutYMax, std::vector<int> layout);
    
    int getTotalHeight() {return totalHeight;}
    
    int getTotalWidth() {return totalWidth;}
    
    void setVoltageScale(int microVolts_);
    

    
    
protected:
    const static int LEFT_BOUND = 20;
    const static int TOP_BOUND = 20;
    const static int SPACING = 4;
    const static int HEIGHT = 8;
    const static int WIDTH = 8;
    const static int SECTION_SPACING = 9;
    int filterHorizonatal=8;
    int filterVertical=8;
    
private:
    UG3ElectrodeViewerCanvas* canvas;
    Viewport* viewport;
    OwnedArray<Electrode> electrodes;
    OwnedArray<juce::Rectangle<int>> sectionDividers;
    
    Colour selectedColor;
    Colour highlightedColor;
    
    int voltageScale;

    int totalHeight;
    int totalWidth;

    
};
#endif /* UG3GridDisplay_h */
