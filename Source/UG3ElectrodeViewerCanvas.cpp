/*
------------------------------------------------------------------

This file is part of a plugin for the Open Ephys GUI
Copyright (C) 2022 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "UG3ElectrodeViewerCanvas.h"

#include "UG3ElectrodeViewer.h"

#include "UG3ElectrodeDisplay.h"

#include "UG3ElectrodeViewerToolbar.h"


UG3ElectrodeViewerCanvas::UG3ElectrodeViewerCanvas(UG3ElectrodeViewer* processor_)
	: node(processor_)
{
    refreshRate = 30;
    
    viewport = std::make_unique<UG3ElectrodeViewerViewport>(this);

    display = std::make_unique<UG3ElectrodeDisplay>(this, viewport.get());
    
    viewport->setViewedComponent(display.get(), false);
    viewport->setScrollBarsShown(true, true, true, true);

    scrollBarThickness = viewport->getScrollBarThickness();
    
    addAndMakeVisible (viewport.get());
    
    toolbar = std::make_unique<UG3ElectrodeViewerToolbar>(this);
    addAndMakeVisible(toolbar.get());
    
    update();

}


UG3ElectrodeViewerCanvas::~UG3ElectrodeViewerCanvas()
{

}


void UG3ElectrodeViewerCanvas::resized()
{
    int toolbarHeight = 55;
    viewport->setBounds(0, 0, getWidth(), getHeight()-toolbarHeight); // leave space at bottom for buttons
    display->setBounds(0,0, std::max(display->getTotalWidth(), getWidth() - scrollBarThickness), std::max(display->getTotalHeight(), getHeight() - toolbarHeight));
    
    toolbar->setBounds(0,getHeight() - toolbarHeight, std::max(display->getTotalWidth(), getWidth() - scrollBarThickness), toolbarHeight);
}

void UG3ElectrodeViewerCanvas::refreshState()
{

    resized();

}


void UG3ElectrodeViewerCanvas::update()
{
    int layoutMaxX;
    int layoutMaxY;
    std::vector<int> layout;
    node -> getLayoutParameters(layoutMaxX, layoutMaxY, layout);
    display -> setElectrodeLayout(layoutMaxX, layoutMaxY, layout);
    toolbar->resized();
}


void UG3ElectrodeViewerCanvas::refresh()
{
    const float* values;
    if(isImpedanceOn){
        values = node->getImpedanceMagnitudes();
    } else {
        values= node->getLatestValues();
    }
    std::cout << "value[0]: " << values[0] << std::endl;
    display->refresh(values);

    repaint();
    
}


void UG3ElectrodeViewerCanvas::paint(Graphics& g)
{

	g.fillAll(Colours::black);

}

void UG3ElectrodeViewerCanvas::beginAnimation() {
    startCallbacks();
}

void UG3ElectrodeViewerCanvas::endAnimation() {
    stopCallbacks();
}

void UG3ElectrodeViewerCanvas::setVoltageScale(int microVolts) {
    display -> setVoltageScale(microVolts);
}

void UG3ElectrodeViewerCanvas::toggleImpedanceMode(bool isImpedanceOn_) {
    isImpedanceOn = isImpedanceOn_;
    
}

UG3ElectrodeViewerViewport::UG3ElectrodeViewerViewport(UG3ElectrodeViewerCanvas* canvas) : Viewport(), canvas(canvas) {}
UG3ElectrodeViewerViewport::~UG3ElectrodeViewerViewport() {}

void UG3ElectrodeViewerViewport::visibleAreaChanged(const juce::Rectangle<int>& newVisibleArea) {
    canvas->repaint(getBoundsInParent());
}
