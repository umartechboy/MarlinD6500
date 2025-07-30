#ifndef _UI_ENUMS_
#define _UI_ENUMS_

enum TransitionDirection{
    Forward = 0,
    Backward,
};


enum StepAnimationStage{
    MainStep = 0,
    GoingToOverLay,
    InOverlay,
    GoingToStep,
};

enum MenuTransitionStage{
    TransitionaingScreens,
    InStep,
};
enum PrintStatus{
    Idle,
    FileToPrint,
    Printing,
    PrintToResume,
    ChangingFilament
};

#endif