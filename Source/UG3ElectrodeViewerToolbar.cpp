//
//  UG3InterfaceDisplayInfo.cpp
//  ug3-interface
//
//  Created by Allen Munk on 1/10/23.
//

#include "UG3ElectrodeViewerToolbar.h"
#include "UG3ElectrodeViewerCanvas.h"




String UG3ElectrodeViewerToolbar::calculateMetricString(int value, String unit, int prefixShiftsOffset) {
    std::vector<String> metricPrefixes = { "u", "m", "", "K", "M" };
    int prefixShifts = 0;
    String returnString;
    while (value >= 1000) {
        value = value / 1000;
        prefixShifts++;
    }

    prefixShifts += prefixShiftsOffset;
    
    returnString = String(value) + (prefixShifts < metricPrefixes.size() ? metricPrefixes[prefixShifts] : "?") + unit;
    return returnString;

}


const std::vector<int> UG3ElectrodeViewerToolbar::voltageOptions = { 1, 5, 10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 5000000 };
const std::vector<int> UG3ElectrodeViewerToolbar::impedanceOptions = { 1, 5, 10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 5000000 };

UG3ElectrodeViewerToolbar::UG3ElectrodeViewerToolbar(UG3ElectrodeViewerCanvas* canvas) : canvas(canvas){
    
    impedanceButton = new UtilityButton("OFF", Font("Default", "Plain", 15));
    impedanceButton->setRadius(5.0f);
    impedanceButton->setEnabledState(true);
    impedanceButton->setCorners(true, true, true, true);
    impedanceButton->addListener(this);
    impedanceButton->setClickingTogglesState(true);
    impedanceButton->setToggleState(false, sendNotification);
    addAndMakeVisible(impedanceButton);

    zeroCenterButton = new UtilityButton("OFF", Font("Default", "Plain", 15));
    zeroCenterButton->setRadius(5.0f);
    zeroCenterButton->setEnabledState(true);
    zeroCenterButton->setCorners(true, true, true, true);
    zeroCenterButton->addListener(this);
    zeroCenterButton->setClickingTogglesState(true);
    zeroCenterButton->setToggleState(false, sendNotification);
    addAndMakeVisible(zeroCenterButton);
    
    voltageSelector = new ComboBox("Voltage Selector");
    int i = 0;
    for (auto option : voltageOptions) {
        voltageSelector->addItem(calculateMetricString(option, "V"), i + 1);
        i++;
    }
    voltageSelector->setSelectedId(1, dontSendNotification);
    addAndMakeVisible(voltageSelector);
    voltageSelector->addListener(this);

    impedanceSelector = new ComboBox("Impedance Selector");
    i = 0;
    for (auto option : voltageOptions) {
        //unit offset of 2 because impedance will be in Ohms (0th metric string in micro)
        impedanceSelector->addItem(calculateMetricString(option, "Ohm", 2), i + 1);
        i++;
    }
    impedanceSelector->setSelectedId(1, dontSendNotification);
    impedanceSelector->addListener(this);

}

UG3ElectrodeViewerToolbar::~UG3ElectrodeViewerToolbar(){}

void UG3ElectrodeViewerToolbar::resized(){
    int leftEdge = 8;

    voltageSelector->setBounds(leftEdge + 10, getHeight() - 30, 160, 22);
    impedanceSelector->setBounds(leftEdge + 10, getHeight() - 30, 160, 22);
    impedanceButton->setBounds(voltageSelector -> getRight() + 10, getHeight() - 30, 60, 22);
    zeroCenterButton->setBounds(impedanceButton->getRight() + 50, getHeight() - 30, 60, 22);


}

void UG3ElectrodeViewerToolbar::paint(Graphics& g){
    g.fillAll(Colours::black);
    g.setColour(Colour(100,100,100));
    g.setFont(Font("Fira Sans", 16, Font::plain));
    String selectorText = impedanceButton->getToggleState() ? "Impedance Selector (Ohms)" : "Voltage Selector (V)";
    g.drawText(selectorText, voltageSelector->getX(), voltageSelector->getY()-22, 300, 20, Justification::left, false);
    g.drawText("Impedance Mode", impedanceButton->getX(), impedanceButton->getY()-22, 300, 20, Justification::left, false);
    g.drawText("Zero Center (+/-)", zeroCenterButton->getX(), zeroCenterButton->getY() - 22, 300, 20, Justification::left, false);
   

}

void UG3ElectrodeViewerToolbar::comboBoxChanged (ComboBox* combo){
    if(combo == voltageSelector) {
        canvas -> setVoltageScale(voltageOptions[combo->getSelectedItemIndex()]);
    } else if (combo == impedanceSelector) {
        canvas->setVoltageScale(impedanceOptions[combo->getSelectedItemIndex()]);
    }

}

void UG3ElectrodeViewerToolbar::buttonClicked (Button* button){

    if(button == impedanceButton) {
        canvas -> toggleImpedanceMode(button->getToggleState());
        static_cast<UtilityButton*>(button)->setLabel(button->getToggleState() ? "ON" : "OFF");
        removeChildComponent((button->getToggleState() ? voltageSelector : impedanceSelector));
        addAndMakeVisible((button->getToggleState() ? impedanceSelector : voltageSelector));

        if (button->getToggleState()) {
            canvas->setVoltageScale(impedanceOptions[impedanceSelector->getSelectedItemIndex()]);
        }
        else {
            canvas->setVoltageScale(voltageOptions[voltageSelector->getSelectedItemIndex()]);
        }
        resized();
        return;
    }
    else if (button == zeroCenterButton) {
        canvas->toggleZeroCenter(button->getToggleState());
        static_cast<UtilityButton*>(button)->setLabel(button->getToggleState() ? "ON" : "OFF");
        resized();
        return;
    }
}

void UG3ElectrodeViewerToolbar::toggleEnabled(bool enabled) {
    impedanceButton->setEnabled(enabled);
    voltageSelector->setEnabled(enabled);
}
