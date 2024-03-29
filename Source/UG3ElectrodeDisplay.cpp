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


UG3ElectrodeDisplay::UG3ElectrodeDisplay(UG3ElectrodeViewerCanvas* canvas, Viewport* viewport) : canvas(canvas), viewport(viewport), totalHeight(0), totalWidth(0), maxColorRangeText(""), minColorRangeText(""), isSubselectActive(false), numChannelsX(0), numChannelsY(0), subselectCorner(0), hoveredElectrode(0){
    selectedColor = ColourScheme::getColourForNormalizedValue(.9);

    
}

void UG3ElectrodeDisplay::setGridLayout(int layoutMaxX, int layoutMaxY, std::vector<int> layout) {
    int newTotalHeight = 0;
    const int totalPixels = layoutMaxX * layoutMaxY;
    int layoutIndex = 0;
    newTotalHeight = TOP_BOUND;
    electrodes.clear();
    colorRange.clear();
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
    
    mouseListener = new DisplayMouseListener(this, layoutMaxY, layoutMaxX);
    mouseListener -> setBounds(0,0, getWidth(), getHeight());
    addAndMakeVisible(mouseListener);
    mouseListener -> toFront(true);
    
    numChannelsX = layoutMaxX;
    numChannelsY = layoutMaxY;
    
    repaint();

}

void UG3ElectrodeDisplay::setProbeLayout(int layoutX, int layoutY, int probeCols) {
    int newTotalHeight = 0;
    const int totalPixels = layoutX * layoutY;
    
    newTotalHeight = TOP_BOUND;
    electrodes.clear();
    colorRange.clear();
    
    int electrodesPerProbe = layoutY * probeCols;
    for (int i = 0; i < totalPixels; i++)
    {
        
        int probeIndex = i/electrodesPerProbe;
        
        int column = probeIndex * probeCols + i % probeCols;
        int row = (i - probeIndex * electrodesPerProbe)/probeCols;
        int L = LEFT_BOUND + column * (WIDTH + SPACING) + probeIndex * (WIDTH + SPACING);
        int T = TOP_BOUND + row * (HEIGHT + SPACING);
            
        if(column == 0) {
            newTotalHeight += HEIGHT + SPACING;
        }
        
        if(row == 0 && column == layoutX - 1) {
            totalWidth = L + WIDTH + SPACING;
        }
        
        Electrode* e = new Electrode(Rectangle<int>(L, T, WIDTH, HEIGHT));
        e -> setColour(selectedColor);
        electrodes.add(e);
        
    }

        
    totalHeight = newTotalHeight + TOP_BOUND - SPACING;
    
    jassert(colorRangeSize > 1);

    for (int i = 0; i < colorRangeSize; i++) {
        Electrode* e = new Electrode(Rectangle<int>(totalWidth, totalHeight - TOP_BOUND - HEIGHT * (i + 1), WIDTH, HEIGHT));
        e->setColour(ColourScheme::getColourForNormalizedValue((float)(i) / float(colorRangeSize)));
        colorRange.add(e);
    }
    
    mouseListener = new DisplayMouseListener(this, layoutX, layoutY);
    mouseListener -> setBounds(0,0, getWidth(), getHeight());

    
    numChannelsX = layoutX;
    numChannelsY = layoutY;
    
    repaint();

}


void UG3ElectrodeDisplay::resized() {
    
    mouseListener -> setBounds(0,0, getWidth(), getHeight());
    
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
    
    g.setColour(Colours::black);
    int height = TOP_BOUND;
    g.setFont(16);
    g.drawText("Electrode Name: ", totalWidth, height, 400, 16, Justification::left);
    height += 16;
    g.drawText("Recording Device Serial Number: ", totalWidth, height, 400, 16, Justification::left);
    height += 16;
    g.drawText("Electrode Dimensions: " + String(numChannelsX) + String(" Columns X ") + String(numChannelsY) + String(" Rows"), totalWidth, height, 400, 16, Justification::left);
    height += 16;
    g.drawText("Layout File Path: " + String(canvas->getLayoutFilePath()), totalWidth, height, 600, 16, Justification::left);
    height += 16;
    g.drawText("Map Enabled: " + (canvas->isLayoutUsingMap() ? String("True") : String("False")), totalWidth, height, 400, 16, Justification::left);
    height += 16;
    g.drawText("Mouse is over electrode: "+String(hoveredElectrode), totalWidth, height, 400, 16, Justification::left);
    height += 16;
    if(isSubselectActive){
        g.drawText("Subselection Top Left Channel: "+String(subselectCorner), totalWidth, height, 400, 16, Justification::left);
    }
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

void UG3ElectrodeDisplay::setColorRangeText(const String& max, const String& min) {
    maxColorRangeText = max;
    minColorRangeText = min;
    repaint();
}


void UG3ElectrodeDisplay::updateSubselectedElectrodes (int start, int rows, int cols, int colsPerRow) {
    canvas -> setSubselectedChannels(start, rows, cols, colsPerRow);
}


void UG3ElectrodeDisplay::switchSubselectState(bool isSubselectActive_) {
    isSubselectActive = isSubselectActive_;
    mouseListener -> toggleSubselect();
}

void UG3ElectrodeDisplay::updateSubselectWindow(subselectWindowOptions option) {
    switch (option) {
        case subselectWindowOptions::HorDec:
            subselectHorizonatal = subselectHorizonatal > 1 ? subselectHorizonatal - 1 : 1;
            break;
        case subselectWindowOptions::HorInc:
            subselectHorizonatal = subselectHorizonatal < numChannelsX ? subselectHorizonatal + 1 : subselectHorizonatal;
            break;
        case subselectWindowOptions::VertDec:
            subselectVertical = subselectVertical > 1 ? subselectVertical - 1 : 1;
            break;
        case subselectWindowOptions::VertInc:
            subselectVertical = subselectVertical < numChannelsY ? subselectVertical + 1 : subselectVertical;
            break;
        default:
            break;
    }
    mouseListener->toggleSubselect();
}


UG3ElectrodeDisplay::DisplayMouseListener::DisplayMouseListener(UG3ElectrodeDisplay* display, int numRows, int numCols) : display (display), numRows(numRows), numCols(numCols) {}


void UG3ElectrodeDisplay::DisplayMouseListener::paint(Graphics& g){
    g.fillAll(Colours::transparentWhite);
    g.setColour(Colour(uint8(255), uint8(255), uint8(255), uint8(128)));
    if(selection) {
        if(display -> isSubselectActive) {
            g.fillRect(*selection);
        }
    }
}

void UG3ElectrodeDisplay::DisplayMouseListener::mouseMove(const MouseEvent & event) {
    display->hoveredElectrode = calculateElectrodeAtCoordinate(event.x, event.y);
    if(display->isSubselectActive) {
        if(selection)
            display -> subselectCorner = calculateElectrodeAtCoordinate(selection -> getX(), selection -> getY());
    }
    repaint();
}

void UG3ElectrodeDisplay::DisplayMouseListener::mouseDown(const MouseEvent & event) {
    if(display -> isSubselectActive){
        if(selection -> contains(Point<int>(event.x, event.y))) {
            selectionStartX = event.x - selection -> getX();
            selectionStartY = event.y - selection -> getY();
        }
        else {
            selectionStartX = -1;
            selectionStartY = -1;
        }
    }
}

void UG3ElectrodeDisplay::DisplayMouseListener::mouseDrag(const MouseEvent & event) {
    if(display -> isSubselectActive) {
        if(selectionStartX > 0) {
            selection -> setX(event.x - selectionStartX);
            selection -> setY(event.y - selectionStartY);
        }
        display -> subselectCorner = calculateElectrodeAtCoordinate(selection -> getX(), selection -> getY());
    }
    display->hoveredElectrode = calculateElectrodeAtCoordinate(event.x, event.y);
    repaint();
}

void UG3ElectrodeDisplay::DisplayMouseListener::mouseUp(const MouseEvent & event) {
    if(display -> isSubselectActive) {
        if(selectionStartX > 0) {
            selection -> setX(event.x - selectionStartX);
            selection -> setY(event.y - selectionStartY);
            selectionStartX = -1;
            selectionStartY = -1;
            calculateElectrodesSelected();
        }
    }
    repaint();
}

void UG3ElectrodeDisplay::DisplayMouseListener::calculateElectrodesSelected() {
    
        int maxX = LEFT_BOUND + (WIDTH + SPACING) * numCols - SPACING;
        int maxY = TOP_BOUND + (HEIGHT + SPACING) * numRows - SPACING;

        //Calculate the nearest left edge of an electrode (to the left of the selection left boundary)
        int nearestLeftEdge = selection -> getX() - LEFT_BOUND >= 0 ? (selection -> getX() - LEFT_BOUND)/(WIDTH+SPACING) : 0;
        //Calculate the number of columns selected by calculating the number of left edges present in selection from nLE to top right of selection
        int columnsSelected = selection -> getTopRight().getX() > LEFT_BOUND && selection -> getX() < maxX ? (std::min((int)selection -> getTopRight().getX(), maxX) - (LEFT_BOUND + nearestLeftEdge*(WIDTH+SPACING)))/(WIDTH+SPACING) + 1 : 0;
        
        //Calculate the nearest top edge of an electrode (to the top of selection top boundary)
        int nearestTopEdge = selection -> getY() - TOP_BOUND >= 0 ? (selection -> getY() - TOP_BOUND)/(HEIGHT+SPACING) : 0;
        //Calculate the number of rows by calculating the number of top edges from nTE to bottom left of selection
        int rowsSelected =  selection -> getBottomLeft().getY() > TOP_BOUND && selection -> getY() < maxY ? (std::min((int)selection -> getBottomLeft().getY(), maxY) - (TOP_BOUND + nearestTopEdge*(HEIGHT+SPACING)))/(HEIGHT+SPACING) + 1 : 0;
    
        display -> updateSubselectedElectrodes(nearestTopEdge * numCols + nearestLeftEdge, rowsSelected, columnsSelected, numCols);
    
}

int UG3ElectrodeDisplay::DisplayMouseListener::calculateElectrodeAtCoordinate(int x, int y) {
    int nearestLeftEdge = std::min(x - LEFT_BOUND >= 0 ? (x - LEFT_BOUND)/(WIDTH+SPACING) : 0, display -> numChannelsX - 1);
    int nearestTopEdge = std::min(y - TOP_BOUND >= 0 ? (y - TOP_BOUND)/(HEIGHT+SPACING) : 0, display -> numChannelsY - 1);
    
    return nearestLeftEdge + nearestTopEdge * display -> numChannelsX;
    

}


void UG3ElectrodeDisplay::DisplayMouseListener::toggleSubselect() {
    if(!(display-> isSubselectActive)) {
        if(selection)
            selection = nullptr;
        display -> updateSubselectedElectrodes(0, 0, 0, 0);
    }
    else {
        int startX, startY, width, height;
        startX = selection ? selection -> getX() : LEFT_BOUND;
        startY = selection ? selection -> getY() : TOP_BOUND;
        width =  display->subselectHorizonatal*(WIDTH+SPACING) - SPACING;
        height = display->subselectVertical*(HEIGHT+SPACING) - SPACING;
        selection = new Rectangle<int>(startX,startY,width,height);
        calculateElectrodesSelected();
        display -> subselectCorner = calculateElectrodeAtCoordinate(selection -> getX(), selection -> getY());
    }
    repaint();
}

