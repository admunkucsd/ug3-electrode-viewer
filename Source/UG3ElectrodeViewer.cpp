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
    : GenericProcessor("UG3 Electrode Viewer"), layoutMaxX(0), layoutMaxY(0), currentStreamName(""), effectiveSampleRate(0), probeCols(0)
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
    acquisitionCapabilitiesStrings.clear();
    selectedCapability = std::nullopt;
    std::cout << "requesting layout" << std::endl;
    requestInputInfo();

    for(const auto &stream : dataStreams) {
        availableStreams.insert((stream->group.name != "default") ? stream->group.name : stream->getName());
    }
}


void UG3ElectrodeViewer::process(AudioBuffer<float>& buffer)
{
    int count = 0;
    for (auto channel : continuousChannels)
    {
        DataStream* stream = getDataStream(channel->getStreamId());
        String streamName = stream -> group.name != "default" ? stream -> group.name: stream -> getName();

        if (streamName == currentStreamName)
        {

            int globalIndex = channel->getGlobalIndex();
            int bufferIndex;

            auto electrodeMapIt= electrodeMaps.find(selectedCapability.value());

            //If there is a mapping from acquisition to visual buffer then use mapping
            //Else, just use linear mapping
            if(electrodeMapIt != electrodeMaps.end() &&
                    (*electrodeMapIt).second.hasMap()) {
                std::optional mapping = (*electrodeMapIt).second.getChannelMapping(channel->getName().toStdString(), stream -> getName().toStdString());
                //It is possible for a channel not to have a mapping (If visual buffer < acquisition buffer)
                //In this case, skip adding this channel to the visual buffer
                if(mapping.has_value()) {
                    bufferIndex = mapping.value();
                }
                else {
                    continue;
                }
            }
            else {
                bufferIndex = count++;
            }
            currentValues.set(bufferIndex,*(buffer.getReadPointer(globalIndex, 0)));
        }
    }
}


bool UG3ElectrodeViewer::startAcquisition() {

    effectiveSampleRate = 0;

    return true;
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
    BroadcastPayload payload;
    if (BroadcastParser::getPayloadForCommand("", "IMPEDANCESREADY", message, payload)) {
        loadImpedances();
    }
    else if (BroadcastParser::getPayloadForCommand("UG3ElectrodeViewer", "LOADINPUTINFO", message, payload)) {

        const DynamicObject::Ptr payloadMap = payload.getPayload();
        if (payloadMap->hasProperty("capabilities") && payloadMap->getProperty("capabilities").isArray()) {
            acquisitionCapabilitiesStrings.clear();
            for(const auto& capability : *(payloadMap->getProperty("capabilities").getArray())) {
                acquisitionCapabilitiesStrings.add(capability);
            }
        }

        if (payloadMap->hasProperty("currentCapability") && payloadMap ->getProperty("currentCapability").isString()) {
            String capability = payloadMap->getProperty("currentCapability");
            selectedCapability = capability;
        }

        if (payloadMap->hasProperty("electrodeLayoutPath") && payloadMap ->getProperty("electrodeLayoutPath").isString()) {
            String newElectrodeLayoutPath = payloadMap -> getProperty("electrodeLayoutPath");
            if(!electrodeLayoutPath.has_value() || electrodeLayoutPath.value() != newElectrodeLayoutPath) {
                electrodeLayoutPath = newElectrodeLayoutPath;
                loadElectrodeLayoutFile();
            }
        }

    }
    return "";
}



void UG3ElectrodeViewer::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void UG3ElectrodeViewer::loadCustomParametersFromXml(XmlElement* parentElement)
{

}

void UG3ElectrodeViewer::requestInputInfo() {
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

    String message = BroadcastParser::build("", "GETINPUTINFO", payload);
    sendConfigMessage(sn, message);
}

void UG3ElectrodeViewer::sendUpdateActiveCapabilityRequest(const String& capability){
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
    payload["acquisitionCapability"] = capability;

    String message = BroadcastParser::build("", "PUTACTIVECAPABILITY", payload);
    sendConfigMessage(sn, message);
}



void UG3ElectrodeViewer::setLayoutParameters(int layoutMaxX_, int layoutMaxY_, const std::vector<int>& layout_, int probeCols_) {
    layoutMaxX = layoutMaxX_;
    layoutMaxY = layoutMaxY_;
    layout = layout_;
    currentValues.clear();
    currentValues.insertMultiple(0, 0, layoutMaxX * layoutMaxY);
    impedanceValues.clear();
    impedanceValues.insertMultiple(0, 0, layoutMaxX * layoutMaxY);
    probeCols = probeCols_;
}


void UG3ElectrodeViewer::getLayoutParameters(const String& acquisitionModeName, int& layoutMaxX_, int& layoutMaxY_,std::vector<int>& layout_, int& probeCols_){
    layout_.clear();
    probeCols_ = 0;
    layoutMaxX_ = 0;
    layoutMaxY_ = 0;
    if(electrodeMaps.find(acquisitionModeName) != electrodeMaps.end()) {
        ElectrodeMap modeMap = electrodeMaps.at(acquisitionModeName);
        layoutMaxX_ = modeMap.getDimensions().first;
        layoutMaxY_ = modeMap.getDimensions().second;
    }
}

void UG3ElectrodeViewer::loadImpedances() {
    int count = 0;
    for (auto channel : continuousChannels)
    {
        DataStream* stream = getDataStream(channel->getStreamId());
        String streamName = stream -> group.name != "default" ? stream -> group.name: stream -> getName();

        if ( streamName == currentStreamName)
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

bool UG3ElectrodeViewer::loadElectrodeLayoutJSON(const String& jsonString) {
    var contents = JSON::parse(jsonString);
    if (!contents.isObject()) {
        return false;
    }
    parseElectrodeLayoutFile(contents.getDynamicObject());
    return true;
}


void UG3ElectrodeViewer::loadElectrodeLayoutFile() {
    if(!electrodeLayoutPath.has_value()) {
        LOGE("called loadElectrodeLayoutFile(), but now path to layout file was set!");
        return;
    }
    File layoutFilePath(electrodeLayoutPath.value());

    if(!layoutFilePath.exists()) {
        LOGE("tried to load ", layoutFilePath.getFullPathName(), " but file does not exist!");
        return;
    }

    if(!layoutFilePath.existsAsFile()) {
        LOGE("tried to load ", layoutFilePath.getFullPathName(), " but path is a directory!");
        return;
    }

    var layoutFileResult = JSON::parse(layoutFilePath);
    if(layoutFileResult.isVoid() || !layoutFileResult.isObject()) {
        LOGE("tried to load ", layoutFilePath.getFullPathName(), " but file format is not valid JSON!");
        return;
    }

    DynamicObject::Ptr layoutFileContents = layoutFileResult.getDynamicObject();
    parseElectrodeLayoutFile(layoutFileContents);

}

void UG3ElectrodeViewer::parseElectrodeLayoutFile(const DynamicObject::Ptr layoutFileContents) {
    electrodeMaps.clear();
    //Loop through all JSON entries; there should be String:Object pairs where the string corresponds to
    //an acquisition mode and the Objects contain the rows and columns
    for(const auto & entry : layoutFileContents->getProperties()) {
        std::optional<int> tempRows;
        std::optional<int> tempCols;
        if(!entry.value.isObject()) {
            continue;
        }

        if(entry.value.getDynamicObject()->hasProperty("rows") && entry.value.getDynamicObject()->getProperty("rows").isInt()) {
            tempRows = entry.value.getDynamicObject()->getProperty("rows");
        }

        if(entry.value.getDynamicObject()->hasProperty("cols") && entry.value.getDynamicObject()->getProperty("cols").isInt()) {
            tempCols = entry.value.getDynamicObject()->getProperty("cols");
        }

        if(tempRows.has_value() && tempCols.has_value()) {
            std::optional<std::unordered_map<ElectrodeMapKey, int>> channelMap;
            if(entry.value.getDynamicObject()->hasProperty("map")
               && entry.value.getDynamicObject()->getProperty("map").isArray()) {
                channelMap = parseChannelMap(entry.value.getDynamicObject()->getProperty("map").getArray(),
                                             tempRows.value(),
                                             tempCols.value());
                //Channel map size should match dimensions
                if(channelMap.value().size() != tempRows.value() * tempCols.value()){
                    LOGE("Tried to load a channel map for ", entry.name.toString(), " layout, but map did not match dimensions")
                    channelMap = std::nullopt;
                }
            }
            ElectrodeMap newMap(tempCols.value(), tempRows.value());
            if(channelMap.has_value() ) {
                newMap = newMap.withLayout(channelMap.value());
            }

            electrodeMaps.emplace(entry.name.toString(),newMap);
        }
    }
}


void UG3ElectrodeViewer::updateSourceElectrodeLayoutPath(const String& layoutFilePath){
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
    payload["layoutFilePath"] = layoutFilePath;

    String message = BroadcastParser::build("", "PUTLAYOUTFILEPATH", payload);
    sendConfigMessage(sn, message);
}

std::optional<std::unordered_map<ElectrodeMapKey,int>> UG3ElectrodeViewer::parseChannelMap(Array<var>* mappings, int rows, int cols) {
    std::unordered_map<ElectrodeMapKey, int> ret;
    for(const auto& coord : *mappings) {
        std::optional<int> tempX;
        std::optional<int> tempY;

        if(!coord.isObject()) {
            continue;
        }

        if(coord.getProperty("x",var::undefined()).isInt()) {
            tempX = coord.getProperty("x",0);
        }

        if(coord.getProperty("y",var::undefined()).isInt()) {
            tempY = coord.getProperty("y",0);
        }

        if(tempX.has_value() && tempY.has_value()) {
            if(!(coord.getProperty("channel",var::undefined()).isString())) {
                continue;
            }

            if(!(coord.getProperty("stream",var::undefined()).isString())) {
                continue;
            }

            //Add entry to hash table where
            // K: hash of channel and stream name,
            // V: serial location within visual buffer using x/y coordinates
            String channelName = coord.getProperty("channel", "");
            String streamName = coord.getProperty("stream", "");
            ElectrodeMapKey key(channelName.toStdString(), streamName.toStdString());
            ret.emplace(key,tempX.value() + tempY.value() * cols);
        }
    }

    if(ret.empty()) {
        return std::nullopt;
    }
    else {
        return ret;
    }
}
