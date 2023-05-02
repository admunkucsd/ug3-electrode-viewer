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

const int UG3ElectrodeDisplay::colorRangeSize = 32;


UG3ElectrodeDisplay::UG3ElectrodeDisplay(UG3ElectrodeViewerCanvas* canvas, Viewport* viewport) : canvas(canvas), viewport(viewport), totalHeight(0), totalWidth(0), maxColorRangeText(""), minColorRangeText("") {
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
    
    jassert(colorRangeSize > 1);

    for (int i = 0; i < colorRangeSize; i++) {
        Electrode* e = new Electrode(Rectangle<int>(totalWidth, totalHeight - TOP_BOUND - HEIGHT * (i + 1), WIDTH, HEIGHT));
        e->setColour(ColourScheme::getColourForNormalizedValue((float)(i) / float(colorRangeSize)));
        colorRange.add(e);
    }
    
    repaint();

}

void UG3ElectrodeDisplay::paint(Graphics& g) {
    g.fillAll(Colours::darkgrey);
    for(Electrode* e : electrodes) {
        g.setColour(e -> getColour());
        g.fillRect(e -> getRectangle());
    }
    for (Electrode* e : colorRange) {
        g.setColour(e->getColour());
        g.fillRect(e->getRectangle());
    }

    g.drawText(maxColorRangeText, colorRange[colorRangeSize - 1]->getRectangle().getRight(), colorRange[colorRangeSize - 1]->getRectangle().getY(), 400, 16, Justification::left);
    g.drawText(minColorRangeText, colorRange[1]->getRectangle().getRight(), colorRange[1]->getRectangle().getY(), 400, 16, Justification::left);

}

void UG3ElectrodeDisplay::refresh(const float * values, bool isZeroCentered, int scaleFactor) {
    
    int count = 0;
    for (auto e : electrodes)
    {
        float normalizedValue; 
        if (isZeroCentered) {
            normalizedValue = values[count] / (2.0f * float(scaleFactor)) + 0.5f;
        }
        else {
            normalizedValue = values[count] / float(scaleFactor);
        }
        e->setColour(ColourScheme::getColourForNormalizedValue(normalizedValue));
        count += 1;
    }

}

void UG3ElectrodeDisplay::setColorRangeText(String max, String min) {
    maxColorRangeText = max;
    minColorRangeText = min;
    repaint();
}

