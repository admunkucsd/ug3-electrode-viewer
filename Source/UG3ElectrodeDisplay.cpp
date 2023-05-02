//
//  UG3GridDisplay.cpp
//  ug3-electrode-viewer
//
//  Created by Allen Munk on 4/28/23.
//

#include "UG3ElectrodeDisplay.h"
#include "UG3ElectrodeViewerCanvas.h"

void Electrode::setColour(Colour c_)
{
    c = c_;
}

Colour Electrode::getColour() {
    return c;
}

const Rectangle<int> Electrode::getRectangle() {
    return rect;
}

UG3ElectrodeDisplay::UG3ElectrodeDisplay(UG3ElectrodeViewerCanvas* canvas, Viewport* viewport) : canvas(canvas), viewport(viewport), totalHeight(0), totalWidth(0), voltageScale(1){
    selectedColor = ColourScheme::getColourForNormalizedValue(.9);

    
}

void UG3ElectrodeDisplay::setElectrodeLayout(int layoutMaxX, int layoutMaxY, std::vector<int> layout) {
    int newTotalHeight = 0;
    const int totalPixels = layoutMaxX * layoutMaxY;
    int layoutIndex = 0;
    newTotalHeight = TOP_BOUND;
    electrodes.clear();
        for (int i = 0; i < totalPixels; i++)
    {
        

        int column = i % layoutMaxX;
        int row = i / layoutMaxX;
        int L = LEFT_BOUND + column * (WIDTH + SPACING);
        int T = TOP_BOUND + row * (HEIGHT + SPACING);
            
        if(column == 0)
            newTotalHeight += HEIGHT + SPACING;
        
        if(row == 0 && column == layoutMaxX - 1) {
            totalWidth = L + WIDTH + SPACING;
        }
        
        if(layout.size() == 0 || (layoutIndex < layout.size() && layout[layoutIndex] == i)) {
            Electrode* e = new Electrode(Rectangle<int>(L, T, WIDTH, HEIGHT));
            e -> setColour(selectedColor);
            electrodes.add(e);
            layoutIndex++;
        }
    }
    
        
    totalHeight = newTotalHeight + TOP_BOUND - SPACING;
    repaint();

}

void UG3ElectrodeDisplay::paint(Graphics& g) {
    g.fillAll(Colours::darkgrey);
    for(Electrode* e : electrodes) {
        g.setColour(e -> getColour());
        g.fillRect(e -> getRectangle());
    }
}

void UG3ElectrodeDisplay::refresh(const float * values) {
    
    int count = 0;
    for (auto e : electrodes)
    {
        float normalizedVoltage = values[count] / (2.0f * float(voltageScale)) + 0.5f;
        e->setColour(ColourScheme::getColourForNormalizedValue(normalizedVoltage));
        count += 1;
    }

}

void UG3ElectrodeDisplay::setVoltageScale(int microVolts_) {
    if(microVolts_ <= 0)
        return;
    voltageScale = microVolts_;
}
