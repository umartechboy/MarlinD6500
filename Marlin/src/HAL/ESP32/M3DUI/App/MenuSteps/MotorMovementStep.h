#ifndef __MOTOR_MOVEMENT_STEP__
#define __MOTOR_MOVEMENT_STEP__

#include "..\..\Components\M3DUI.h"

enum m_movements: byte{
    NoCom = 0,
    HomeAll,
    motorsOff,
    moveX, 
    moveY,
    moveZ,
    moveE0,
    moveE1,
    powerH1,
    powerH2,
    powerFan,
    audio,
};

class MotorMovementStep:public MenuStep
{
private:
public:
    bool fineTest = true;
    MotorMovementStep(MenuHost* host);
    ~MotorMovementStep();
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;  
    void IncrementValue();
    void DecrementValue();
    void LoadComplete() override;
    void UnloadComplete() override;
    String makeG1(char axis, int dir);
    void Tick() override;   
    VerticalList* options;
    StringListItem* homeAllOption;
    StringListItem* motorsOffOption;
    StringListItem* xMoveOption;
    StringListItem* yMoveOption;
    StringListItem* zMoveOption;
    StringListItem* e0MoveOption;
    StringListItem* e1MoveOption;
    StringListItem* h0PowerOption;
    StringListItem* h1PowerOption;
    StringListItem* fanPowerOption;
    StringListItem* audioOption;
};

#endif