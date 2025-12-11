#include "MaterialsStep.h"
#include "..\Images.h"
#include "..\MenuApp.h"

// DummyMenuStep* dummyColorChangeStep;
void materilaOptionChange(void* caller, ListItem* selectedItem, int selectedIndex){
    MaterialsStep* This = (MaterialsStep*)caller;
    if (selectedItem == This->change0 || selectedItem == This->change1){
        This->NextActionString = "Change";
        This->RetroNextStep = selectedItem == This->change0?(&filament0ChangeStep):(&filament1ChangeStep);
        // Normal mode
        This->PreviousStep = &filament0ChangeStep;
        filament0ChangeStep.NextStep = This;
    }
    else if (selectedItem == This->extruder0Color || selectedItem == This->extruder1Color)
    {   
        This->NextActionString = "Select";
        This->RetroNextStep = 0;
        // Normal mode
        This->PreviousStep = &filament1ChangeStep;
        filament1ChangeStep.NextStep = This;

    }
    else {
        This->NextActionString = "";
        This->RetroNextStep = 0;
        // Normal mode
        This->PreviousStep = 0;
    }
}
MaterialsStep::MaterialsStep(MenuHost* host):MenuStep(host) {
    ButtonColor = ST7735_WHITE;
    BackColor = ST7735_WHITE;
    TextColor = ST7735_BLACK;            
    Icon = &img_ChangeFilament;    
    NextStep = &toolsMenuStep;    

    // dummyColorChangeStep = new DummyMenuStep(Host);
    // dummyColorChangeStep->RetroIcon = &img_RetroHorizontal;
    
    Title = "Materials";
    RetroPreviousStep = &retroMainMenuStep;
    extruder0Color = new ColorSelectorListItem(Host, "Extruder 1", 0, 0, 24);
    extruder1Color = new ColorSelectorListItem(Host,"Extruder 2", 1, 1, 24);
    change0 = new StringListItem(Host, 0, "Change Filament", 0, 24);
    change1 = new StringListItem(Host, 0, "Change Filament", 0, 24);
    
    filament0ChangeStep.RetroPreviousStep = this;
    filament1ChangeStep.RetroPreviousStep = this;

    extruder0Color->selectedColor0Index = 0;
    extruder0Color->selectedColor1Index = 0;
    extruder1Color->selectedColor0Index = 1;
    extruder1Color->selectedColor1Index = 1;
    extruderColors[0] = extruder0Color;
    extruderColors[1] = extruder1Color;
    defaultExtruderColor = new ColorSelectorListItem(Host, "Default", extruderColors[0]->selectedColor0Index, extruderColors[1]->selectedColor0Index, 24);
    if (defaultExtruder >= 0) {
        defaultExtruderColor->selectedColor0Index = extruderColors[defaultExtruder]->selectedColor0Index;
        defaultExtruderColor->selectedColor1Index = extruderColors[defaultExtruder]->selectedColor0Index;
        defaultExtruderColor->Label = String(defaultExtruder + 1);
    }
    else {
        defaultExtruderColor->selectedColor0Index = extruderColors[0]->selectedColor0Index;
        defaultExtruderColor->selectedColor1Index = extruderColors[1]->selectedColor0Index;
        defaultExtruderColor->Label = "A";
    }

    // Filament Load/Unload
    options = new VerticalList(Host);
    options->SetOnSelectionUpdated(this, materilaOptionChange);
    options->Add(extruder0Color);
    options->Add(change0);
    options->Add(new ListSeparatorItem(Host));
    options->Add(extruder1Color);
    options->Add(change1);
    //options->Add(new ListSeparatorItem(Host));
    // options->Add(defaultExtruderColor); // Disable changing filament for now
    TickPeriod = 50;
}
MaterialsStep::~MaterialsStep(){
    // delete dummyColorChangeStep;
    delete options;
}
void MaterialsStep::LoadBegin(){
    // Load default
    prefs.begin("material");
    defaultExtruder = prefs.getInt("def_e", 0);
    int e0Color = prefs.getInt("e1_c", 0);
    int e1Color = prefs.getInt("e2_c", 1);
    extruder0Color->selectedColor0Index = e0Color;
    extruder0Color->selectedColor1Index = e0Color;
    extruder1Color->selectedColor0Index = e1Color;
    extruder1Color->selectedColor1Index = e1Color;
    if (defaultExtruder >= 0){ // 0 or 1
        defaultExtruderColor->selectedColor0Index = extruderColors[defaultExtruder]->selectedColor0Index;
        defaultExtruderColor->selectedColor1Index = extruderColors[defaultExtruder]->selectedColor0Index;
    }
    else{
        defaultExtruderColor->selectedColor0Index = extruderColors[0]->selectedColor0Index;
        defaultExtruderColor->selectedColor1Index = extruderColors[1]->selectedColor0Index;
    }
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
    options->Paint(g, 0, 0, g->width(), g->height(), TextColor);
}
void MaterialsStep::SaveColors(){
    
    SERIAL_IMPL.printf("Save Colors: ex1 = %d, ex2 = %d, de = %d\n", extruder0Color->selectedColor0Index, extruder1Color->selectedColor0Index, defaultExtruder);
    prefs.begin("material");
    prefs.putInt("def_e", defaultExtruder);
    prefs.putInt("e1_c", extruder0Color->selectedColor0Index);
    prefs.putInt("e2_c", extruder1Color->selectedColor0Index);
    prefs.end();
}
    
void MaterialsStep::ToggleSelected(int dir){
    int inds []= {-1, 0, 1};
    if (dir == 1) // change -1, 0, 1 to 1, 0, -1 respectively
        defaultExtruder = inds[((defaultExtruder + 1/* compensate negative */) + 1 /* Increment */ + 3 /* Period */) % 3];
    else // change -1, 0, 1 to 0, 1, -1 respectively
        defaultExtruder = inds[((defaultExtruder + 1 /* compensate negative*/) - 1 /* Decrement */ + 3 /* Period*/) % 3];
        
        
    if (defaultExtruder >= 0) { // 0 or 1
        defaultExtruderColor->selectedColor0Index = extruderColors[defaultExtruder]->selectedColor0Index;
        defaultExtruderColor->selectedColor1Index = extruderColors[defaultExtruder]->selectedColor0Index;
        defaultExtruderColor->Label = String(defaultExtruder + 1);
    }
    else {
        defaultExtruderColor->selectedColor0Index = extruderColors[0]->selectedColor0Index;        
        defaultExtruderColor->selectedColor1Index = extruderColors[1]->selectedColor0Index;
        defaultExtruderColor->Label = "A";
    }
}
int MaterialsStep::selectedExtruderItemIndex(){
    if (options->getSelected() == extruderColors[0])
        return 0;
    if (options->getSelected() == extruderColors[1])
        return 1;
    return -1;
}
void MaterialsStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void MaterialsStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
void MaterialsStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_LEFT){
        if (options->getSelected() == extruder0Color || options->getSelected() == extruder1Color){ // extruders
            ((ColorSelectorListItem*)options->getSelected())->incrementColor();
            
            if (defaultExtruder == selectedExtruderItemIndex() && defaultExtruder >= 0) {
                defaultExtruderColor->selectedColor0Index = ((ColorSelectorListItem*)options->getSelected())->selectedColor0Index;
                defaultExtruderColor->selectedColor1Index = ((ColorSelectorListItem*)options->getSelected())->selectedColor0Index;
            }
            else {
                defaultExtruderColor->selectedColor0Index = extruderColors[0]->selectedColor0Index;
                defaultExtruderColor->selectedColor1Index = extruderColors[1]->selectedColor0Index;
            }
        }
        else ToggleSelected(-1);
        NeedsRedraw = true;
        SaveColors();
    }
    else if (key == Keys::KEYPAD_RIGHT){
        if (options->getSelected() == extruder0Color || options->getSelected() == extruder1Color){ // extruders
            ((ColorSelectorListItem*)options->getSelected())->decrementColor();
            if (defaultExtruder == selectedExtruderItemIndex() && defaultExtruder >= 0) {
                defaultExtruderColor->selectedColor0Index = ((ColorSelectorListItem*)options->getSelected())->selectedColor0Index;
                defaultExtruderColor->selectedColor1Index = ((ColorSelectorListItem*)options->getSelected())->selectedColor0Index;
            }
            else {
                defaultExtruderColor->selectedColor0Index = extruderColors[0]->selectedColor0Index;
                defaultExtruderColor->selectedColor1Index = extruderColors[1]->selectedColor0Index;
            }
        }
        else ToggleSelected(1);
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