#include "OptionsStep.h"
#include "..\Images.h"
#include "..\MenuApp.h"

OptionsStep::OptionsStep(MenuHost* host):MenuStep(host) {
    ButtonColor = Charcoal;
    BackColor = Charcoal;
    TextColor = ST7735_WHITE;            
    Icon = &img_Utilities;
}
void OptionsStep::Paint(BufferedDisplay* g) {
    
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    
    // Draw the plus
    uint8_t lineMargin = 10;
    uint8_t opBkp = g->GetOpacity();
    g->SetOpacity(15);
    for (int i =0; i < 3; i++){
        g->drawLine(lineMargin + i * 3, Host->appHeight() / 2, Host->appWidth() - lineMargin - i * 3, Host->appHeight() / 2, TextColor);
        g->drawLine(Host->appWidth() / 2, lineMargin + i * 3, Host->appHeight() / 2, Host->appWidth() - lineMargin - i * 3, TextColor);
    }

    // Draw the four icons
    // Settings 
    // Materials
    // Tools
    // Info
    
    g->SetOpacity(20); // disabled option
    img_Options.DrawCentered(g,        (Host->appWidth() * 3) / 4, (Host->appHeight() * 1) / 4);
    g->SetOpacity(100);
    img_ChangeFilament.DrawCentered(g,  (Host->appWidth() * 1) / 4, (Host->appHeight() * 1) / 4);
    img_M3D.DrawCentered(g,             (Host->appWidth() * 1) / 4, (Host->appHeight() * 3) / 4);
    img_BedLevel.DrawCentered(g,           (Host->appWidth() * 3) / 4, (Host->appHeight() * 3) / 4);
    g->SetOpacity(opBkp);
}
    
void OptionsStep::HandleKeyUp(Keys key) {
    if (key == Keys::KEYPAD_UP_LEFT){ // materials
        PreviousStep = &materialsMenuStep;
        Host->GotoPreviousStep();
    }
    else if (key == Keys::KEYPAD_UP_RIGHT){ // settings
    }
    else if (key == Keys::KEYPAD_DOWN_LEFT){ // Info
        PreviousStep = &printerInfoStep;
        Host->GotoPreviousStep();
    }
    else if (key == Keys::KEYPAD_DOWN_RIGHT){ // Bed Level
        PreviousStep = &bedLevelStep;
        Host->GotoPreviousStep();
    }
    else if (key == KEYPAD_RIGHT){ // Back to main
        Host->GotoNextStep();
    }
    else {
        this->PreviousStep = 0;
    }
}