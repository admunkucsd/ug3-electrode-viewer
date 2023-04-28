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

#include "UG3ElectrodeViewer.h"

#include "UG3ElectrodeViewerEditor.h"


UG3ElectrodeViewer::UG3ElectrodeViewer() 
    : GenericProcessor("UG3 Electrode Viewer")
{
    requestElectrodeLayout();
}


UG3ElectrodeViewer::~UG3ElectrodeViewer()
{

}


AudioProcessorEditor* UG3ElectrodeViewer::createEditor()
{
    editor = std::make_unique<UG3ElectrodeViewerEditor>(this);
    return editor.get();
}


void UG3ElectrodeViewer::updateSettings()
{


}


void UG3ElectrodeViewer::process(AudioBuffer<float>& buffer)
{

    for (auto channel : continuousChannels)
    {
        if (channel->getStreamId() == currentStream)
        {

            int globalIndex = channel->getGlobalIndex();
            int localIndex = channel->getLocalIndex();

            currentValues.add(*buffer.getReadPointer(globalIndex, 0));
        }
    }

}

void UG3ElectrodeViewer::setParameter(int index, float value)
{
    if (value > 100){
        currentStream = (uint16)value;
    }
}


void UG3ElectrodeViewer::handleTTLEvent(TTLEventPtr event)
{

}


void UG3ElectrodeViewer::handleSpike(SpikePtr spike)
{

}


void UG3ElectrodeViewer::handleBroadcastMessage(String message)
{

}

String UG3ElectrodeViewer::handleConfigMessage(String message) {
    
}



void UG3ElectrodeViewer::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void UG3ElectrodeViewer::loadCustomParametersFromXml(XmlElement* parentElement)
{

}


void UG3ElectrodeViewer::requestElectrodeLayout() {
    GenericProcessor* sn = sourceNode;
    
    if(sn == nullptr)
        return;
    
    while(sn -> sourceNode != nullptr) {
        sn = sn -> sourceNode;
    }
    
    if(!(sn -> isSource())) {
        return;
    }
    std::map<String, var> payload;
    payload["requestNodeId"] = getNodeId();

    String message = BroadcastParser::build("", "GETELECTRODELAYOUT", payload);
    
    sendConfigMessage(sn, message);
    
}
