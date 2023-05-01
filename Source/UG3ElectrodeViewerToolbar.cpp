//
//  UG3InterfaceDisplayInfo.cpp
//  ug3-interface
//
//  Created by Allen Munk on 1/10/23.
//

#include "UG3ElectrodeViewerToolbar.h"
#include "UG3ElectrodeViewerCanvas.h"

const std::vector<int> UG3ElectrodeViewerToolbar::voltageOptions = {1, 5, 10, 50, 100, 500, 1000, 5000};

UG3ElectrodeViewerToolbar::UG3ElectrodeViewerToolbar(UG3ElectrodeViewerCanvas* canvas) : canvas(canvas){
    
    impedanceButton = new UtilityButton("OFF", Font("Default", "Plain", 15));
    impedanceButton->setRadius(5.0f);
    impedanceButton->setEnabledState(true);
    impedanceButton->setCorners(true, true, true, true);
    impedanceButton->addListener(this);
    impedanceButton->setClickingTogglesState(true);
    impedanceButton->setToggleState(false, sendNotification);
    addAndMakeVisible(impedanceButton);
    
    voltageSelector = new ComboBox("Voltage Selector (uV)");
    voltageSelector->setSelectedId (1, dontSendNotification);
    int i = 0;
    for (auto option : voltageOptions) {
        voltageSelector->addItem(String(option), i+1);
        i++;
    }
    addAndMakeVisible(voltageSelector);
    voltageSelector->addListener(this);
    

}

UG3ElectrodeViewerToolbar::~UG3ElectrodeViewerToolbar(){}

void UG3ElectrodeViewerToolbar::resized(){
    int leftEdge = 8;

    voltageSelector->setBounds(leftEdge + 10, getHeight() - 30, 160, 22);
    impedanceButton->setBounds(voltageSelector -> getRight() + 10, getHeight() - 30, 60, 22);

}

void UG3ElectrodeViewerToolbar::paint(Graphics& g){
    g.fillAll(Colours::black);
    g.setColour(Colour(100,100,100));
    g.setFont(Font("Fira Sans", 16, Font::plain));

    g.drawText("Voltage Selector (uV)", voltageSelector->getX(), voltageSelector->getY()-22, 300, 20, Justification::left, false);
    g.drawText("Impedance Mode", impedanceButton->getX(), voltageSelector->getY()-22, 300, 20, Justification::left, false);
   

}

void UG3ElectrodeViewerToolbar::comboBoxChanged (ComboBox* combo){
    if(combo == voltageSelector) {
        canvas -> setVoltageScale(combo->getText().getIntValue());
    }

}

void UG3ElectrodeViewerToolbar::buttonClicked (Button* button){

    if(button == impedanceButton) {
        canvas -> toggleImpedanceMode(button->getToggleState());
        static_cast<UtilityButton*>(button)->setLabel(button->getToggleState() ? "ON" : "OFF");
        resized();
        return;
    }
}

void UG3ElectrodeViewerToolbar::toggleEnabled(bool enabled) {
    impedanceButton->setEnabled(enabled);
    voltageSelector->setEnabled(enabled);
}
