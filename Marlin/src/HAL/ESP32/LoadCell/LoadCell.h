#ifndef LOAD_CELL
#define LOAD_CELL

#define HxData 7
#define HxClk  15

void LoadCellSetup();
void LoadCellLoop();
int LoadCellProbe();
void ProbeTare(int val);
extern bool ProbeEnable;
#endif // LOAD_CELL