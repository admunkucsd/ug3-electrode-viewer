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


UG3ElectrodeViewerCanvas::UG3ElectrodeViewerCanvas(UG3ElectrodeViewer* processor_)
	: node(processor_)
{
    refreshRate = 30;
    
    viewport = std::make_unique<UG3ElectrodeViewerViewport>(this);

    display = std::make_unique<UG3ElectrodeDisplay>(this, viewport.get());
    
    viewport->setViewedComponent(display.get(), false);
    viewport->setScrollBarsShown(true, true, true, true);

    //scrollBarThickness = viewport->getScrollBarThickness();
    
    addAndMakeVisible (viewport.get());
    
    //toolbar = std::make_unique<UG3InterfaceToolbar>(this);
    //addAndMakeVisible(toolbar.get());
    
    update();

}


UG3ElectrodeViewerCanvas::~UG3ElectrodeViewerCanvas()
{

}


void UG3ElectrodeViewerCanvas::resized()
{
//    int componentsEndX = 8;
//    int toolbarHeight = 55;
//    viewport->setBounds(0, 0, getWidth(), getHeight()-toolbarHeight); // leave space at bottom for buttons
//
    viewport->setBounds(0, 0, getWidth(), getHeight());
//    gridDisplay->setBounds(0,0, std::max(gridDisplay->getTotalHeight(), getWidth() - scrollBarThickness), std::max(gridDisplay->getTotalHeight(), getHeight() - toolbarHeight));
//    toolbar->setBounds(0,getHeight() - toolbarHeight, std::max(gridDisplay->getTotalHeight(), getWidth() - scrollBarThickness), toolbarHeight);
    display->setBounds(0,0, getWidth(), getHeight());
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
    
}


void UG3ElectrodeViewerCanvas::refresh()
{
    const float* values = node->getLatestValues();

    display->refresh(values);

    repaint();
    
}


void UG3ElectrodeViewerCanvas::paint(Graphics& g)
{

	g.fillAll(Colours::black);

}

UG3ElectrodeViewerViewport::UG3ElectrodeViewerViewport(UG3ElectrodeViewerCanvas* canvas) : Viewport(), canvas(canvas) {}
UG3ElectrodeViewerViewport::~UG3ElectrodeViewerViewport() {}

void UG3ElectrodeViewerViewport::visibleAreaChanged(const juce::Rectangle<int>& newVisibleArea) {
    canvas->repaint(getBoundsInParent());
}
