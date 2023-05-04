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
    : GenericProcessor("UG3 Electrode Viewer"), layoutMaxX(0), layoutMaxY(0), currentStream(0), totalSamples(0), lastTimerCallback(0), effectiveSampleRate(0)
{
    isEnabled = false;
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
    requestElectrodeLayout();


}


void UG3ElectrodeViewer::process(AudioBuffer<float>& buffer)
{
    int count = 0;
    for (auto channel : continuousChannels)
    {
        if (channel->getStreamId() == currentStream)
        {

            int globalIndex = channel->getGlobalIndex();
            int localIndex = channel->getLocalIndex();
            currentValues.set(count,*(buffer.getReadPointer(globalIndex, 0)));
            count++;
        }
    }
    int64 current = int64(Time::highResolutionTicksToSeconds(Time::getHighResolutionTicks()) * 1e6);

    float sampleRate = getDataStream(currentStream) -> getSampleRate();
    totalSamples += buffer.getNumSamples();
    if(totalSamples >= sampleRate) {
        int64 currentTime = Time::getHighResolutionTicks();
        int64 timeElapsed = int64(Time::highResolutionTicksToSeconds(currentTime - lastTimerCallback)*1e6);
        effectiveSampleRate = float(totalSamples)/(float(timeElapsed)/1e6);
        
        totalSamples = totalSamples - sampleRate;
        lastTimerCallback = currentTime;
    }
    
    
}

void UG3ElectrodeViewer::setParameter(int index, float value)
{
    if (value > 100){
        currentStream = (uint16)value;
    }
}

bool UG3ElectrodeViewer::startAcquisition() {
    lastTimerCallback = Time::getHighResolutionTicks();
    
    effectiveSampleRate = 0;
    
    totalSamples = 0;
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
    var payload;
    if(BroadcastParser::checkForCommand("UG3ElectrodeViewer", "LOADELECTRODELAYOUT", message, payload)) {
        int tempLayoutMaxX;
        int tempLayoutMaxY;
        std::vector<int> tempLayout;
        if(!BroadcastParser::getIntField(payload.getDynamicObject(), "layoutMaxX", tempLayoutMaxX, 0) || !BroadcastParser::getIntField(payload.getDynamicObject(), "layoutMaxY", tempLayoutMaxY, 0)) {
            return "";
        }
        layoutMaxX = tempLayoutMaxX;
        layoutMaxY = tempLayoutMaxY;
        editor -> updateVisualizer();
        isEnabled = true;
        currentValues.clear();
        currentValues.insertMultiple(0, 0, layoutMaxX * layoutMaxY);
        impedanceValues.clear();
        impedanceValues.insertMultiple(0, 0, layoutMaxX * layoutMaxY);


    }
    else if (BroadcastParser::checkForCommand("", "IMPEDANCESREADY", message, payload)) {
        loadImpedances();
    }
    return "";
}



void UG3ElectrodeViewer::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void UG3ElectrodeViewer::loadCustomParametersFromXml(XmlElement* parentElement)
{

}


void UG3ElectrodeViewer::requestElectrodeLayout() {
    GenericProcessor* sn = getSourceNode();

    if(sn == nullptr) {
        return;
    }
    
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

void UG3ElectrodeViewer::getLayoutParameters(int& layoutMaxX_, int& layoutMaxY_,std::vector<int>& layout_){
    layoutMaxX_ = layoutMaxX;
    layoutMaxY_ = layoutMaxY;
    layout_ = layout;
}

void UG3ElectrodeViewer::loadImpedances() {
    int count = 0;
    for (auto channel : continuousChannels)
    {
        if (channel->getStreamId() == currentStream)
        {
            if (channel->impedance.measured) {
                impedanceValues.set(count, channel->impedance.magnitude);
            }
            count++;
        }
    }
}

void UG3ElectrodeViewer::setSubselectedChannels(int start, int rows, int cols, int colsPerRow) {
    std::map<String, var> valueMap;
    valueMap["split"] = 0;
    valueMap["start"] = start;
    valueMap["rows"] = rows;
    valueMap["cols"] = cols;
    valueMap["colsPerRow"] = colsPerRow;
    String message = BroadcastParser::build("LFPViewer", "filter", valueMap);
    broadcastMessage(message);
}


