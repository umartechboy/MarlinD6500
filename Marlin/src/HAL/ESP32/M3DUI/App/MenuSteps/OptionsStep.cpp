#include "OptionsStep.h"
#include "..\Bitmaps.h"
#include "..\MenuApp.h"

OptionsStep::OptionsStep(MenuHost* host):MenuStep(host) {
    ButtonColor = Charcoal;
    BackColor = Charcoal;
    TextColor = ST7735_WHITE;            
    Icon = &bmp_Utilities;
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
        g->drawLine(lineMargin + i * 3, g->height() / 2, g->width() - lineMargin - i * 3, g->height() / 2, TextColor);
        g->drawLine(g->width() / 2, lineMargin + i * 3, g->height() / 2, g->width() - lineMargin - i * 3, TextColor);
    }

    // Draw the four icons
    // Settings 
    // Materials
    // Tools
    // Info
    
    g->SetOpacity(20); // disabled option
    bmp_Settings.DrawCentered(g,        (g->width() * 3) / 4, (g->height() * 1) / 4);
    g->SetOpacity(100);
    bmp_ChangeFilament.DrawCentered(g,  (g->width() * 1) / 4, (g->height() * 1) / 4);
    bmp_M3D.DrawCentered(g,             (g->width() * 1) / 4, (g->height() * 3) / 4);
    bmp_BedLevel.DrawCentered(g,           (g->width() * 3) / 4, (g->height() * 3) / 4);
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