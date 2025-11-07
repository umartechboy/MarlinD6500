#include "MotorMovementStep.h"
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\Images.h"
#include "Fonts\FreeMono12pt7b.h"
#include "Fonts\FreeMono9pt7b.h"
#include "..\..\..\..\..\module\printcounter.h"
#include <WiFi.h>
#include "..\MenuApp.h"

m_movements nextCommand = m_movements::NoCom;

void OnSelectionUpdatedCallback(void* caller, ListItem* selectedItem, int selectedIndex){
    MotorMovementStep* This = (MotorMovementStep*)caller;
    if (selectedItem == This->homeAllOption){
        nextCommand = m_movements::HomeAll;
        SERIAL_IMPL.println("Home All");
    }
    else if (selectedItem == This->motorsOffOption){
        nextCommand = m_movements::motorsOff;
        SERIAL_IMPL.println("Motors Off");
    }
    else if (selectedItem == This->xMoveOption){
        nextCommand = m_movements::moveX;
        SERIAL_IMPL.println("Move X");
    }
    else if (selectedItem == This->yMoveOption){
        nextCommand = m_movements::moveY;
        SERIAL_IMPL.println("Move Y");
    }
    else if (selectedItem == This->zMoveOption){
        nextCommand = m_movements::moveZ;
        SERIAL_IMPL.println("Move Z");
    }
    else if (selectedItem == This->e0MoveOption){
        nextCommand = m_movements::moveE0;
        SERIAL_IMPL.println("Move E0");
    }
    else if (selectedItem == This->e1MoveOption){
        nextCommand = m_movements::moveE1;
        SERIAL_IMPL.println("Move E1");
    }
    else if (selectedItem == This->h0PowerOption){
        nextCommand = m_movements::powerH1;
        SERIAL_IMPL.println("Power H0");
    }
    else if (selectedItem == This->h1PowerOption){
        nextCommand = m_movements::powerH2;
        SERIAL_IMPL.println("Power H1");
    }
    else if (selectedItem == This->fanPowerOption){
        nextCommand = m_movements::powerFan;
        SERIAL_IMPL.println("Power Fan");
    }


    else {
        nextCommand = m_movements::NoCom;
    }
}
MotorMovementStep::MotorMovementStep(MenuHost* host):MenuStep(host)
{
    TextColor = ST7735_WHITE;
    BackColor = DarkRed;
    Icon = &img_M3D;
    TickPeriod = 50;
    RetroNextStep = 0;
    Title = "Power Test";
    options = new VerticalList(host);
    homeAllOption = new StringListItem(host, 0, "Home All", 0, 16);
    motorsOffOption = new StringListItem(host, 0, "Motors Off", 0, 16);
    xMoveOption = new StringListItem(host, 0, "-- X ++", 0, 16);
    yMoveOption = new StringListItem(host, 0, "-- Y ++", 0, 16);
    zMoveOption = new StringListItem(host, 0, "-- Z ++", 0, 16);
    e0MoveOption = new StringListItem(host, 0, "-- E1 ++", 0, 16);
    e1MoveOption = new StringListItem(host, 0, "-- E2 ++", 0, 16);
    h0PowerOption = new StringListItem(host, 0, "0  H1  1", 0, 16);
    h1PowerOption = new StringListItem(host, 0, "0  H2  1", 0, 16);
    fanPowerOption = new StringListItem(host, 0, "0  Fan  1", 0, 16);

    options->Add(homeAllOption);
    options->Add(motorsOffOption);
    options->Add(xMoveOption);
    options->Add(yMoveOption);
    options->Add(zMoveOption);
    options->Add(e0MoveOption);
    options->Add(e1MoveOption);
    options->Add(h0PowerOption);
    options->Add(h1PowerOption);
    options->Add(fanPowerOption);
    options->SetOnSelectionUpdated(this, OnSelectionUpdatedCallback);
    //options->InvokeSelectionChanged();
}
MotorMovementStep::~MotorMovementStep(){
    delete options;
}
void MotorMovementStep::Paint(BufferedDisplay* g){            
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    uint8_t opBkp = g->GetOpacity();

    options->Paint(g, 0, retroTitleSectionHeight, Host->appWidth(), Host->appHeight(), ST7735_WHITE);
    g->SetOpacity(opBkp);

}
void MotorMovementStep::Tick(){
    NeedsRedraw = true;
    h0PowerOption->ItemText = String("0  H1 (") + String(readTemp1(), 1) + String("C)  1");
    h1PowerOption->ItemText = String("0  H2 (") + String(readTemp2(), 1) + String("C)  1");
}

void MotorMovementStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void MotorMovementStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
void MotorMovementStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        options->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        options->scrollUp();
    else if (key == Keys::KEYPAD_LEFT){
        switch (nextCommand)
        {
            case m_movements::moveX: enqueueComs({"G91", "G1 X-10 F3000", "G90"}); SERIAL_IMPL.println("Move X -10"); break;
            case m_movements::moveY: enqueueComs({"G91", "G1 Y-10 F3000", "G90"}); SERIAL_IMPL.println("Move Y -10"); break;
            case m_movements::moveZ: enqueueComs({"G91", "G1 Z-10 F3000", "G90"}); SERIAL_IMPL.println("Move Z -5"); break;
            case m_movements::moveE0: enqueueComs({"M83", "T0", "G1 E-10 F300", "M82"}); SERIAL_IMPL.println("Move E0 -10"); break;
            case m_movements::moveE1: enqueueComs({"M83", "T1", "G1 E-10 F300", "M82"}); SERIAL_IMPL.println("Move E1 -10"); break;
            case m_movements::powerH1: enqueueComs({"M104 T0 S0"}); SERIAL_IMPL.println("H1 Off"); break;
            case m_movements::powerH2: enqueueComs({"M104 T1 S0"}); SERIAL_IMPL.println("H2 Off"); break;
            case m_movements::powerFan: enqueueComs({"M106 S0"}); SERIAL_IMPL.println("Fan Off"); break;

            default:
                break;
        }
    }
    else if (key == Keys::KEYPAD_RIGHT){
        switch (nextCommand)
        {
            case m_movements::moveX: enqueueComs({"G91", "G1 X10 F3000", "G90"}); SERIAL_IMPL.println("Move X +10"); break;
            case m_movements::moveY: enqueueComs({"G91", "G1 Y10 F3000", "G90"}); SERIAL_IMPL.println("Move Y +10"); break;
            case m_movements::moveZ: enqueueComs({"G91", "G1 Z10 F3000", "G90"}); SERIAL_IMPL.println("Move Z +5"); break;
            case m_movements::moveE0: enqueueComs({"M83", "T0", "G1 E10 F300", "M82"}); SERIAL_IMPL.println("Move E0 +10"); break;
            case m_movements::moveE1: enqueueComs({"M83", "T1", "G1 E10 F300", "M82"}); SERIAL_IMPL.println("Move E1 +10"); break;
            case m_movements::powerH1: enqueueComs({"M104 T0 S200"}); SERIAL_IMPL.println("H1 On"); break;
            case m_movements::powerH2: enqueueComs({"M104 T1 S200"}); SERIAL_IMPL.println("H2 On"); break;
            case m_movements::powerFan: enqueueComs({"M106 S255"}); SERIAL_IMPL.println("Fan On"); break;

            default:
                break;
        }
    }
    
    if (key == Keys::KEYPAD_LEFT || key == Keys::KEYPAD_RIGHT){
        if (nextCommand == m_movements::HomeAll) {
            enqueueComs({"G28"}); SERIAL_IMPL.println("Home All");
        }
        else if (nextCommand == m_movements::motorsOff){
            enqueueComs({"M18"}); SERIAL_IMPL.println("Motors Off");
        }
    }
}    
void MotorMovementStep::LoadComplete() {   
    enqueueComs({"M302 P1"}); // Home all axes on load
}
void MotorMovementStep::UnloadComplete() {   
    enqueueComs({"M302 P0"}); // Home all axes on load
}