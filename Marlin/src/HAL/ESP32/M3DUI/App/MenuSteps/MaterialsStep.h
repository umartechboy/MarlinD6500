#ifndef MATERIALS_STEP
#define MATERIALS_STEP

#include "..\..\Components\M3DUI.h"
#include <Preferences.h>

class MaterialsStep: public MenuStep {
public:
    VerticalList* options;
    MaterialsStep(MenuHost* host);
    ~MaterialsStep();
    void Paint(BufferedDisplay* g) override;
    void SaveColors();   
    void ToggleSelected();
    void HandleKeyPress(Keys key) override;         
    void IncrementValue();
    void DecrementValue();
    void Tick() override; 
    void LoadBegin() override;
    StringListItem* change0, *change1;
    ColorSelectorListItem* extruder0Color;
    ColorSelectorListItem* extruder1Color;
private:
    int defaultExtruder = 0;
    ColorSelectorListItem* defaultExtruderColor;
    ColorSelectorListItem* extruderColors[2];
    int selectedExtruderItemIndex();
    Preferences prefs;

};


#endif