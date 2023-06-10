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
#include "UG3ElectrodeViewerCanvas.h"

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
    
    void resized();

    void refresh(const float * value, bool isZeroCentered, int scaleFactor);
        
    void paint(Graphics& g);
        
    void setGridLayout(int layoutXMax, int layoutYMax, std::vector<int> layout);
    
    void setProbeLayout(int layoutX, int layoutY, int probeCols);
    
    int getTotalHeight() {return totalHeight;}
    
    int getTotalWidth() {return totalWidth;}
    
    void setColorRangeText(String max, String min);
    
    void updateSubselectedElectrodes (int start, int rows, int cols, int colsPerRow);
    
    void switchSubselectState(bool isSubselectActive_);
    
    void updateSubselectWindow(subselectWindowOptions option);
    
    class DisplayMouseListener : public Component {
    public:
        DisplayMouseListener(UG3ElectrodeDisplay* display, int numRows, int numCols);
        void mouseMove(const MouseEvent & event);
        void mouseDown(const MouseEvent & event);
        void mouseDrag(const MouseEvent & event);
        void mouseUp(const MouseEvent & event);
        
        void calculateElectrodesSelected();
        int calculateElectrodeAtCoordinate(int x, int y);
        void toggleSubselect();
        
        void paint(Graphics& g);
        
    private:
        UG3ElectrodeDisplay* display;
        ScopedPointer<juce::Rectangle<int>> selection;
        int selectionStartX;
        int selectionStartY;
        int numRows;
        int numCols;        
    };
    
    
protected:
    const static int LEFT_BOUND = 20;
    const static int TOP_BOUND = 20;
    const static int SPACING = 4;
    const static int HEIGHT = 8;
    const static int WIDTH = 8;
    int subselectHorizonatal=8;
    int subselectVertical=8;
    bool isSubselectActive;
    
    int hoveredElectrode;
    int subselectCorner;
    
private:
    UG3ElectrodeViewerCanvas* canvas;
    Viewport* viewport;
    OwnedArray<Electrode> electrodes;

    OwnedArray<Electrode> colorRange;
    static const int colorRangeSize;

    ScopedPointer<DisplayMouseListener> mouseListener;

    int numChannelsX;
    int numChannelsY;
    
    Colour selectedColor;
    Colour highlightedColor;
    
    String maxColorRangeText;
    String minColorRangeText;

    int totalHeight;
    int totalWidth;

    
};
#endif /* UG3GridDisplay_h */
