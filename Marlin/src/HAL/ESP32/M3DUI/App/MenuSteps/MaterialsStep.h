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
    void HandleKeyUp(Keys key) override;         
    void IncrementValue();
    void DecrementValue();
    void Tick() override; 
    void LoadBegin() override;
private:
    int defaultExtruder = 0;
    ColorSelectorListItem* extruder0Color;
    ColorSelectorListItem* extruder1Color;
    ColorSelectorListItem* defaultExtruderColor;
    ColorSelectorListItem* extruderColors[2];
    StringListItem* change0, *change1;
    int selectedExtruderItemIndex();
    Preferences prefs;

};


#endif