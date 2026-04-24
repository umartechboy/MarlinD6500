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
    else if (selectedItem == This->homeZOption){
        nextCommand = m_movements::HomeZ;
        SERIAL_IMPL.println("Home Z");
    }
    else if (selectedItem == This->probeOption){
        nextCommand = m_movements::Probe;
        SERIAL_IMPL.println("Probe");
    }
    else if (selectedItem == This->probeSensitityOption){
        nextCommand = m_movements::ProbeSensitivity;
        SERIAL_IMPL.println("Probe Sensitivity");
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
    else if (selectedItem == This->audioOption){
        nextCommand = m_movements::audio;
        SERIAL_IMPL.println("Audio");
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
    homeZOption = new StringListItem(host, 0, "Home Z", 0, 16);
    probeOption = new StringListItem(host, 0, "Probe", 0, 16);
    probeSensitityOption = new StringListItem(host, 0, "PF: -", 0, 16);
    motorsOffOption = new StringListItem(host, 0, "Motors Off", 0, 16);
    xMoveOption = new StringListItem(host, 0, "-- X ++", 0, 16);
    yMoveOption = new StringListItem(host, 0, "-- Y ++", 0, 16);
    zMoveOption = new StringListItem(host, 0, "-- Z ++", 0, 16);
    e0MoveOption = new StringListItem(host, 0, "-- E1 ++", 0, 16);
    e1MoveOption = new StringListItem(host, 0, "-- E2 ++", 0, 16);
    h0PowerOption = new StringListItem(host, 0, "0  H1  1", 0, 16);
    h1PowerOption = new StringListItem(host, 0, "0  H2  1", 0, 16);
    fanPowerOption = new StringListItem(host, 0, "0  Fan  1", 0, 16);
    audioOption = new StringListItem(host, 0, "Audio", 0, 16);
    options->Add(homeAllOption);
    options->Add(homeZOption);
    options->Add(probeOption);
    options->Add(probeSensitityOption);
    options->Add(motorsOffOption);
    options->Add(xMoveOption);
    options->Add(yMoveOption);
    options->Add(zMoveOption);
    options->Add(e0MoveOption);
    options->Add(e1MoveOption);
    options->Add(h0PowerOption);
    options->Add(h1PowerOption);
    options->Add(fanPowerOption);
    options->Add(audioOption);
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
extern float loadCellValueThreshold;
void MotorMovementStep::Tick(){
    NeedsRedraw = true;
    h0PowerOption->ItemText = String("0  H1 (") + String(readTemp1(), 1) + String("C)  1");
    h1PowerOption->ItemText = String("0  H2 (") + String(readTemp2(), 1) + String("C)  1");
    probeSensitityOption->ItemText = String("-- Touch: ") + String(loadCellValueThreshold, 2) + String(" ++");
}

void MotorMovementStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void MotorMovementStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
String MotorMovementStep::makeG1(char axis, int dir) {
    float distance = 10.0 * dir;
    float feedrate = 3000.0F;
    if (fineTest)
    distance /= 10;
    if(axis == 'Z')
        distance /= 10.0F;
    if (axis == 'E'){
        feedrate = 300.0F;
        distance *= 10;
    }
    String command = "G1 ";
    command += axis;
    command += String(distance, 2);
    command += " ";
    command += " F";
    command += String(feedrate);    
    //SERIAL_IMPL.println("Move command: " + command);
    return command;
}
void incrementProbeSensitivity(float increment){ 
    float t = loadCellValueThreshold + increment;
    if (t < 0.5) t = 0.5;
    if (t > 6) t = 6;
    float f = (t / 3) * 0.002F; // Adjust floating factor based on threshold, with 3 as the reference threshold
    String com1 = String("M39 F") + String(f, 5);
    String com2 = String("M39 T") + String(t, 2);
    SERIAL_IMPL.printf("Setting probe sensitivity: %s, %s\n", com1.c_str(), com2.c_str());
    enqueueComs({com1, com2});
}
void MotorMovementStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        options->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        options->scrollUp();
    else if (key == Keys::KEYPAD_LEFT){
        switch (nextCommand)
        {
            case m_movements::moveX: enqueueComs({"G91", makeG1('X', -1), "G90"}); break;
            case m_movements::moveY: enqueueComs({"G91", makeG1('Y', -1), "G90"}); break;
            case m_movements::moveZ: enqueueComs({"G91", makeG1('Z', -1), "G90"}); break;
            case m_movements::moveE0: enqueueComs({"M83", "T0", makeG1('E', -1), "M82"}); break;
            case m_movements::moveE1: enqueueComs({"M83", "T1", makeG1('E', -1), "M82"}); break;
            case m_movements::powerH1: enqueueComs({"M104 T0 S0"}); SERIAL_IMPL.println("H1 Off"); break;
            case m_movements::powerH2: enqueueComs({"M104 T1 S0"}); SERIAL_IMPL.println("H2 Off"); break;
            case m_movements::powerFan: enqueueComs({"M106 S0"}); SERIAL_IMPL.println("Fan Off"); break;
            case m_movements::ProbeSensitivity: incrementProbeSensitivity(-0.25); break;
            default:
                break;
        }
    }
    else if (key == Keys::KEYPAD_RIGHT){
        switch (nextCommand)
        {
            case m_movements::moveX: enqueueComs({"G91", makeG1('X', 1), "G90"}); break;
            case m_movements::moveY: enqueueComs({"G91", makeG1('Y', 1), "G90"}); break;
            case m_movements::moveZ: enqueueComs({"G91", makeG1('Z', 1), "G90"}); break;
            case m_movements::moveE0: enqueueComs({"M83", "T0", makeG1('E', 1), "M82"}); break;
            case m_movements::moveE1: enqueueComs({"M83", "T1", makeG1('E', 1), "M82"}); break;
            case m_movements::powerH1: enqueueComs({"M104 T0 S200"}); SERIAL_IMPL.println("H1 On"); break;
            case m_movements::powerH2: enqueueComs({"M104 T1 S200"}); SERIAL_IMPL.println("H2 On"); break;
            case m_movements::powerFan: enqueueComs({"M106 S255"}); SERIAL_IMPL.println("Fan On"); break;
            case m_movements::ProbeSensitivity: incrementProbeSensitivity(0.25); break;

            default:
                break;
        }
    }
    
    
    if (key == Keys::KEYPAD_LEFT || key == Keys::KEYPAD_RIGHT || key == Keys::KEYPAD_MIDDLE){
        if (nextCommand == m_movements::HomeAll) {
            enqueueComs({"G28"}); SERIAL_IMPL.println("Home All");
        }
        else if (nextCommand == m_movements::HomeZ) {
            enqueueComs({"G28 Z"}); SERIAL_IMPL.println("Home Z");
        }
        else if (nextCommand == m_movements::Probe) {
            enqueueComs({"G30"}); SERIAL_IMPL.println("Probe");
        }
        else if (nextCommand == m_movements::motorsOff){
            enqueueComs({"M18"}); SERIAL_IMPL.println("Motors Off");
        }
        else if (nextCommand == m_movements::audio){
            enqueueComs({"M37 Mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g,"}); SERIAL_IMPL.println("Motors Off");
        }
    }
}    
void MotorMovementStep::LoadComplete() {   
    enqueueComs({"M302 P1"}); // Home all axes on load
}
void MotorMovementStep::UnloadComplete() {   
    enqueueComs({"M302 P0"}); // Home all axes on load
}