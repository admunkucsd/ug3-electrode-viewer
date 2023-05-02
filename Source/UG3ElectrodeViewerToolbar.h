//
//  UG3InterfaceDisplayInfo.h
//  ug3-interface
//
//  Created by Allen Munk on 1/10/23.
//

#ifndef UG3InterfaceDisplayInfo_h
#define UG3InterfaceDisplayInfo_h

#include <VisualizerWindowHeaders.h>

class UG3ElectrodeViewerCanvas;

class UG3ElectrodeViewerToolbar : public Component,
public ComboBox::Listener,
public Button::Listener{
public:
    UG3ElectrodeViewerToolbar(UG3ElectrodeViewerCanvas* canvas);
    ~UG3ElectrodeViewerToolbar();
    
    void resized();
    
    void paint(Graphics& g);


    /** ComboBox::Listener callback*/
    void comboBoxChanged (ComboBox* combo);
    
    /** Button::Listener callback*/
    void buttonClicked (Button* button);
    
    void toggleEnabled(bool enabled);
    
    
private:
    
    static const std::vector<int> voltageOptions;
    
    UG3ElectrodeViewerCanvas* canvas;
    ScopedPointer<ComboBox> voltageSelector;
    ScopedPointer<UtilityButton> impedanceButton;

};


#endif /* UG3InterfaceDisplayInfo_h */
