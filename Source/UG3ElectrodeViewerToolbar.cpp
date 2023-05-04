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
const std::vector<int> UG3ElectrodeViewerToolbar::impedanceOptions = {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 5000000, 10000000, 50000000 };

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
    
    subSelectButton = new UtilityButton("OFF", Font("Default", "Plain", 15));
    subSelectButton->setRadius(5.0f);
    subSelectButton->setEnabledState(true);
    subSelectButton->setCorners(true, true, true, true);
    subSelectButton->addListener(this);
    subSelectButton->setClickingTogglesState(true);
    subSelectButton->setToggleState(false, sendNotification);
    addAndMakeVisible(subSelectButton);
    
    voltageSelector = new ComboBox("Voltage Selector");
    int i = 0;
    for (auto option : voltageOptions) {
        voltageSelector->addItem(calculateMetricString(option, "V"), i + 1);
        i++;
    }
    voltageSelector->setSelectedId(1, dontSendNotification);
    addAndMakeVisible(voltageSelector);

    canvas->setColorScaleFactor(voltageOptions[voltageSelector->getSelectedItemIndex()], voltageSelector->getText());

    voltageSelector->addListener(this);

    impedanceSelector = new ComboBox("Impedance Selector");
    i = 0;
    for (auto option : impedanceOptions) {
        //unit offset of 2 because impedance will be in Ohms (0th metric string in micro)
        impedanceSelector->addItem(calculateMetricString(option, "Ohm", 2), i + 1);
        i++;
    }
    impedanceSelector->setSelectedId(1, dontSendNotification);
    impedanceSelector->addListener(this);
    
    subselectHorDecButton = new UtilityButton("-", Font("Default", "Plain", 15));
    subselectHorDecButton->setRadius(5.0f);
    subselectHorDecButton->setEnabledState(true);
    subselectHorDecButton->setCorners(true, true, true, true);
    subselectHorDecButton->addListener(this);
    addAndMakeVisible(subselectHorDecButton);
    
    subselectHorIncButton = new UtilityButton("+", Font("Default", "Plain", 15));
    subselectHorIncButton->setRadius(5.0f);
    subselectHorIncButton->setEnabledState(true);
    subselectHorIncButton->setCorners(true, true, true, true);
    subselectHorIncButton->addListener(this);
    addAndMakeVisible(subselectHorIncButton);
    
    subselectVertDecButton = new UtilityButton("-", Font("Default", "Plain", 15));
    subselectVertDecButton->setRadius(5.0f);
    subselectVertDecButton->setEnabledState(true);
    subselectVertDecButton->setCorners(true, true, true, true);
    subselectVertDecButton->addListener(this);
    addAndMakeVisible(subselectVertDecButton);
    
    subselectVertIncButton = new UtilityButton("+", Font("Default", "Plain", 15));
    subselectVertIncButton->setRadius(5.0f);
    subselectVertIncButton->setEnabledState(true);
    subselectVertIncButton->setCorners(true, true, true, true);
    subselectVertIncButton->addListener(this);
    addAndMakeVisible(subselectVertIncButton);

}

UG3ElectrodeViewerToolbar::~UG3ElectrodeViewerToolbar(){}

void UG3ElectrodeViewerToolbar::resized(){
    int leftEdge = 8;

    voltageSelector->setBounds(leftEdge + 10, getHeight() - 30, 160, 22);
    impedanceSelector->setBounds(leftEdge + 10, getHeight() - 30, 160, 22);
    impedanceButton->setBounds(voltageSelector -> getRight() + 10, getHeight() - 30, 60, 22);
    zeroCenterButton->setBounds(impedanceButton->getRight() + 50, getHeight() - 30, 60, 22);
    subSelectButton->setBounds(zeroCenterButton->getRight() + 50, getHeight() - 30, 60, 22);
    
    subselectHorDecButton->setBounds(subSelectButton->getRight() + 30, getHeight() - 30, 60, 22);
    subselectHorIncButton->setBounds(subselectHorDecButton->getRight(), getHeight() - 30, 60, 22);
    
    subselectVertDecButton->setBounds(subselectHorIncButton->getRight() + 30, getHeight() - 30, 60, 22);
    subselectVertIncButton->setBounds(subselectVertDecButton->getRight(), getHeight() - 30, 60, 22);


}

void UG3ElectrodeViewerToolbar::paint(Graphics& g){
    g.fillAll(Colours::black);
    g.setColour(Colour(100,100,100));
    g.setFont(Font("Fira Sans", 16, Font::plain));
    String selectorText = impedanceButton->getToggleState() ? "Impedance Selector (Ohms)" : "Voltage Selector (V)";
    g.drawText(selectorText, voltageSelector->getX(), voltageSelector->getY()-22, 300, 20, Justification::left, false);
    g.drawText("Impedance Mode", impedanceButton->getX(), impedanceButton->getY()-22, 300, 20, Justification::left, false);
    g.drawText("Zero Center (+/-)", zeroCenterButton->getX(), zeroCenterButton->getY() - 22, 300, 20, Justification::left, false);
    g.drawText("Subselect", subSelectButton->getX(), subSelectButton->getY() - 22, 300, 20, Justification::left, false);
   
    g.drawText("Subselect Horizontal", subselectHorDecButton->getX(), subselectHorDecButton->getY() - 22, 300, 20, Justification::left, false);
    g.drawText("Subselect Vertical", subselectVertDecButton->getX(), subselectVertDecButton->getY() - 22, 300, 20, Justification::left, false);


}

void UG3ElectrodeViewerToolbar::comboBoxChanged (ComboBox* combo){
    if(combo == voltageSelector) {
        canvas -> setColorScaleFactor(voltageOptions[combo->getSelectedItemIndex()], combo->getText());
    } else if (combo == impedanceSelector) {
        canvas->setColorScaleFactor(impedanceOptions[combo->getSelectedItemIndex()], combo->getText());
    }

}

void UG3ElectrodeViewerToolbar::buttonClicked (Button* button){

    if(button == impedanceButton) {
        canvas -> toggleImpedanceMode(button->getToggleState());
        static_cast<UtilityButton*>(button)->setLabel(button->getToggleState() ? "ON" : "OFF");
        removeChildComponent((button->getToggleState() ? voltageSelector : impedanceSelector));
        addAndMakeVisible((button->getToggleState() ? impedanceSelector : voltageSelector));

        if (button->getToggleState()) {
            canvas->setColorScaleFactor(impedanceOptions[impedanceSelector->getSelectedItemIndex()], impedanceSelector -> getText());
        }
        else {
            canvas->setColorScaleFactor(voltageOptions[voltageSelector->getSelectedItemIndex()], voltageSelector -> getText());
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
    else if (button == subSelectButton) {
        canvas->toggleSubselect(button->getToggleState());
        static_cast<UtilityButton*>(button)->setLabel(button->getToggleState() ? "ON" : "OFF");
        resized();
        return;
    }
    else if (button == subselectHorIncButton){
        canvas -> updateSubselectWindow(subselectWindowOptions::HorInc);
    }
    else if (button == subselectHorDecButton){
        canvas -> updateSubselectWindow(subselectWindowOptions::HorDec);
    }
    else if (button == subselectVertIncButton){
        canvas -> updateSubselectWindow(subselectWindowOptions::VertInc);
    }
    else if (button == subselectVertDecButton){
        canvas -> updateSubselectWindow(subselectWindowOptions::VertDec);
    }
}

void UG3ElectrodeViewerToolbar::toggleEnabled(bool enabled) {
    impedanceButton->setEnabled(enabled);
    voltageSelector->setEnabled(enabled);
}
