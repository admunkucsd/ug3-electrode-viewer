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

#ifndef VISUALIZERCANVAS_H_INCLUDED
#define VISUALIZERCANVAS_H_INCLUDED

#include <VisualizerWindowHeaders.h>

class UG3ElectrodeViewer;

class UG3ElectrodeDisplay;

class UG3ElectrodeViewerToolbar;

enum subselectWindowOptions {HorDec, HorInc, VertDec, VertInc};

/**
* 
	Draws data in real time

*/
class UG3ElectrodeViewerCanvas : public Visualizer
{
public:

	/** Constructor */
	UG3ElectrodeViewerCanvas(UG3ElectrodeViewer* processor);

	/** Destructor */
	~UG3ElectrodeViewerCanvas();

	/** Updates boundaries of sub-components whenever the canvas size changes */
	void resized() override;

	/** Called when the visualizer's tab becomes visible again */
	void refreshState() override;

	/** Updates settings */
	void update() override;

	/** Called instead of "repaint()" to avoid re-painting sub-components*/
	void refresh() override;

	/** Draws the canvas background */
	void paint(Graphics& g) override;
    
    void setColorScaleFactor(int factor, String unitText);
    
    void toggleImpedanceMode(bool isImpedanceOn);

	void toggleZeroCenter(bool areElectrodeColorsZeroCentered_);
    
    void toggleSubselect(bool isSubselectActive);
    
    void setSubselectedChannels(int start, int rows, int cols, int colsPerRow);


	/** Called when data acquisition is active**/
	void beginAnimation() override;

	/** Called when data acquisition ends**/
	void endAnimation() override;
    
	void setDisplayColorRangeText();
    
    void updateSubselectWindow(subselectWindowOptions option);

private:

	/** Pointer to the processor class */
	UG3ElectrodeViewer* node;
    
    std::unique_ptr<UG3ElectrodeDisplay> display;

    std::unique_ptr<UG3ElectrodeViewerToolbar> toolbar;

    int scrollBarThickness;
	
    std::unique_ptr<class UG3ElectrodeViewerViewport> viewport;

    bool isImpedanceOn;
	bool areElectrodeColorsZeroCentered;
	int colorScaleFactor;
	String colorScaleText;

	bool animationIsActive;

	/** Generates an assertion if this class leaks */
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3ElectrodeViewerCanvas);
};

class UG3ElectrodeViewerViewport : public Viewport
{
public:
    UG3ElectrodeViewerViewport(UG3ElectrodeViewerCanvas* canvas);
    virtual ~UG3ElectrodeViewerViewport() override;
    void visibleAreaChanged(const juce::Rectangle<int>& newVisibleArea) override;

private:
    UG3ElectrodeViewerCanvas* canvas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3ElectrodeViewerViewport);
};


#endif // SPECTRUMCANVAS_H_INCLUDED
