/*
	------------------------------------------------------------------

	This file is part of the Open Ephys GUI
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

//This prevents include loops. We recommend changing the macro to a name suitable for your plugin
#ifndef VISUALIZERPLUGINEDITOR_H_DEFINED
#define VISUALIZERPLUGINEDITOR_H_DEFINED

#include <VisualizerEditorHeaders.h>

/** 
	The editor for the VisualizerPlugin

	Includes buttons for opening the canvas in a tab or window
*/

class UG3ElectrodeViewerEditor : public VisualizerEditor, ComboBox::Listener
{
public:

	/** Constructor */
	UG3ElectrodeViewerEditor(GenericProcessor* parentNode);

	/** Destructor */
	~UG3ElectrodeViewerEditor() { }
    
    /** Updates channel count */
    void updateSettings() override;
    
    /** Sets the drawable stream */
    void comboBoxChanged(ComboBox *cb) override;

	/** Creates the canvas */
	Visualizer* createNewCanvas() override;
    
    /** Updates available streams*/
    void updateStreamSelectorOptions();

    /** Disables stream selection*/
    void startAcquisition() override;

    /** Enables stream selection */
    void stopAcquisition() override;
    
private:
    
    Array<int> inputStreamIds;

    class UG3ElectrodeViewer* electrodeViewerNode;

    std::unique_ptr<Label> streamSelectionLabel;
    std::unique_ptr<ComboBox> streamSelection;

    uint16 selectedStream = 0;

    void setDrawableStream(uint16 streamId);

	/** Generates an assertion if this class leaks */
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3ElectrodeViewerEditor);
};


#endif // VISUALIZERPLUGINEDITOR_H_DEFINED
