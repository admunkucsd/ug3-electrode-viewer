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


#include "UG3ElectrodeViewerEditor.h"

#include "UG3ElectrodeViewerCanvas.h"
#include "UG3ElectrodeViewer.h"


UG3ElectrodeViewerEditor::UG3ElectrodeViewerEditor(GenericProcessor* p)
    : VisualizerEditor(p, "Visualizer", 200)
{

    electrodeViewerNode = (UG3ElectrodeViewer*) p;
    streamSelectionLabel = std::make_unique<Label>("Stream Selection Label", "Display Stream:");
    streamSelectionLabel->setBounds(10, 30, 130, 24);
    addAndMakeVisible(streamSelectionLabel.get());

    streamSelection = std::make_unique<ComboBox>("Stream Selector");
    streamSelection->setBounds(15, 60, 120, 20);
    streamSelection->addListener(this);
    addAndMakeVisible(streamSelection.get());

}


void UG3ElectrodeViewerEditor::comboBoxChanged(ComboBox* cb)
{
    if (cb == streamSelection.get())
    {
        setDrawableStream(cb->getText());
    }

}

void UG3ElectrodeViewerEditor::updateSettings()
{updateStreamSelectorOptions
    ();
}

void UG3ElectrodeViewerEditor::updateStreamSelectorOptions()
{
    bool needsUpdate = false;
    uint16 currentStreamId = streamSelection->getSelectedId();

    streamSelection->clear(dontSendNotification);

    // Add all datastreams to combobox
    int count = 1;
    for (const auto& stream : electrodeViewerNode -> getAvailableStreams())
    {

        streamSelection->addItem(stream, count++);

    }

    if (streamSelection->indexOfItemId(currentStreamId) > -1)
    {
        streamSelection->setSelectedId(currentStreamId, sendNotification);
    }
    else
    {
        if (streamSelection->getNumItems() > 0) {
            streamSelection->setSelectedItemIndex(0, sendNotification);
        }
    }

}

void UG3ElectrodeViewerEditor::setDrawableStream(String streamName)
{
    electrodeViewerNode->setCurrentStreamName(streamName);
}


Visualizer* UG3ElectrodeViewerEditor::createNewCanvas()
{
    return new UG3ElectrodeViewerCanvas((UG3ElectrodeViewer*) getProcessor());;
}

void UG3ElectrodeViewerEditor::startAcquisition()
{
    streamSelection->setEnabled(false);
    enable();
}

void UG3ElectrodeViewerEditor::stopAcquisition()
{
    streamSelection->setEnabled(true);
    disable();
}
