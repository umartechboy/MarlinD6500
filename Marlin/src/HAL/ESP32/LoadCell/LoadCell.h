#ifndef LOAD_CELL
#define LOAD_CELL

void LoadCellSetup();
void LoadCellLoop();
int LoadCellProbe();
void ProbeTare(int val);
extern bool ProbeEnable;

#endif // LOAD_CELL