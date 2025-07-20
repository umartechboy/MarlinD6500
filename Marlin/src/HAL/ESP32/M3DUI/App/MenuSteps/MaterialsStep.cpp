#include "MaterialsStep.h"
#include "..\Images.h"
#include "..\MenuApp.h"

MaterialsStep::MaterialsStep(MenuHost* host):MenuStep(host) {
    ButtonColor = ST7735_WHITE;
    BackColor = ST7735_WHITE;
    TextColor = ST7735_BLACK;            
    Icon = &img_ChangeFilament;    
    NextStep = &toolsMenuStep;

    options = new VerticalList(Host, 24, 128);

    extruder0Color = new ColorSelectorListItem(Host, "Extruder 1", 0);
    extruder1Color = new ColorSelectorListItem(Host,"Extruder 2", 1);
    change0 = new StringListItem(Host,"Change Filament", 0);
    change1 = new StringListItem(Host, "Change Filament", 0);

    extruder0Color->selectedColorIndex = 0;
    extruder1Color->selectedColorIndex = 1;
    extruderColors[0] = extruder0Color;
    extruderColors[1] = extruder1Color;
    defaultExtruderColor = new ColorSelectorListItem(Host, "Default", extruderColors[defaultExtruder]->selectedColorIndex);
    defaultExtruderColor->selectedColorIndex = extruderColors[defaultExtruder]->selectedColorIndex;
    defaultExtruderColor->Label = String(defaultExtruder + 1);

    // Filament Load/Unload
    options->Add(extruder0Color);
    options->Add(change0);
    options->Add(new ListSeparatorItem(Host));
    options->Add(extruder1Color);
    options->Add(change1);
    options->Add(new ListSeparatorItem(Host));
    options->Add(defaultExtruderColor);
    TickPeriod = 50;
}
MaterialsStep::~MaterialsStep(){
    delete options;
}
void MaterialsStep::LoadBegin(){
    // Load default
    prefs.begin("material");
    defaultExtruder = prefs.getInt("def_e", 0);
    int e0Color = prefs.getInt("e1_c", 0);
    int e1Color = prefs.getInt("e2_c", 1);
    extruder0Color->selectedColorIndex = e0Color;
    extruder1Color->selectedColorIndex = e1Color;
    defaultExtruderColor->selectedColorIndex = extruderColors[defaultExtruder]->selectedColorIndex;
    prefs.end();
    
    SERIAL_IMPL.printf("Resumed Colors: ex1 = %d, ex2 = %d, de = %d\n", e0Color, e1Color, defaultExtruder);
}
void MaterialsStep::Tick() {
    NeedsRedraw = true;
}
void MaterialsStep::Paint(BufferedDisplay* g) {            
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    options->Paint(g, TextColor);
    if (options->getSelected() == change0){
        PreviousStep = &filament0ChangeStep;
        filament0ChangeStep.NextStep = this;
    }
    else if (options->getSelected() == change1){
        PreviousStep = &filament1ChangeStep;
        filament1ChangeStep.NextStep = this;
    }
    else
        PreviousStep = 0;
}
void MaterialsStep::SaveColors(){
    
    SERIAL_IMPL.printf("Save Colors: ex1 = %d, ex2 = %d, de = %d\n", extruder0Color->selectedColorIndex, extruder1Color->selectedColorIndex, defaultExtruder);
    prefs.begin("material");
    prefs.putInt("def_e", defaultExtruder);
    prefs.putInt("e1_c", extruder0Color->selectedColorIndex);
    prefs.putInt("e2_c", extruder1Color->selectedColorIndex);
    prefs.end();
}
    
void MaterialsStep::ToggleSelected(){
    defaultExtruder = (defaultExtruder + 1) % 2;
    defaultExtruderColor->selectedColorIndex = extruderColors[defaultExtruder]->selectedColorIndex;
    defaultExtruderColor->Label = String(defaultExtruder + 1);
}
int MaterialsStep::selectedExtruderItemIndex(){
    if (options->getSelected() == extruderColors[0])
        return 0;
    if (options->getSelected() == extruderColors[1])
        return 1;
    return -1;
}
void MaterialsStep::IncrementValue() {
    HandleKeyUp(Keys::KEYPAD_UP);
}
void MaterialsStep::DecrementValue() {
    HandleKeyUp(Keys::KEYPAD_DOWN);
}
void MaterialsStep::HandleKeyUp(Keys key) {
    if (key == Keys::KEYPAD_LEFT){
        if (options->getSelected() == extruder0Color || options->getSelected() == extruder1Color){ // extruders
            ((ColorSelectorListItem*)options->getSelected())->incrementColor();
            
            if (defaultExtruder == selectedExtruderItemIndex())
                defaultExtruderColor->selectedColorIndex = ((ColorSelectorListItem*)options->getSelected())->selectedColorIndex;
        }
        else ToggleSelected();
        NeedsRedraw = true;
        SaveColors();
    }
    else if (key == Keys::KEYPAD_RIGHT){
        if (options->getSelected() == extruder0Color || options->getSelected() == extruder1Color){ // extruders
            ((ColorSelectorListItem*)options->getSelected())->decrementColor();
            if (defaultExtruder == selectedExtruderItemIndex())
                defaultExtruderColor->selectedColorIndex = ((ColorSelectorListItem*)options->getSelected())->selectedColorIndex;
        }
        else ToggleSelected();
        NeedsRedraw = true;
        SaveColors();
    }
    else if (key == Keys::KEYPAD_RIGHT){
        Host->GotoNextStep(); // back to options
    }
    else if (key == Keys::KEYPAD_DOWN)
        options->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        options->scrollUp();
}    