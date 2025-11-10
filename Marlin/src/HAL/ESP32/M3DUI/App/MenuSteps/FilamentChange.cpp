#include "FilamentChange.h"
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\MenuApp.h"
#include "..\Images.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Preferences.h>

static Image* loadBmps[] = {&img_Load0, &img_Load1};
static Image* unloadBmps[] = {&img_Unload0, &img_Unload1};
FilamentChangeStep::FilamentChangeStep(MenuHost* host, int index):MenuStep(host)
{
    filamentIndex = index;
    TextColor = ST7735_WHITE;
    Icon = &img_ChangeFilament;
    TickPeriod = 200;
}

FilamentChangeStep::~FilamentChangeStep()
{
}

bool FilamentChangeStep::CanJumpToMainMenu(){
    return false;
}
// In case the tool is swapped, we need to read/write the correct tool
extern int swapTools;
float ThisReadTemp(int index){
    // We don't do any swapping here anymore
    return readTemp(index);
    //return readTemp(swapTools?1-index:index);
}
void ThisWriteTemp(int index, float temp){
    // We don't do any swapping here anymore
    writeTemp(index, temp);
    //writeTemp(swapTools?1-index:index, temp);
}
void FilamentChangeStep::Tick()
{
    NeedsRedraw = true;
    if (stage == FilamentChangeStage::Preheat){
        if (ThisReadTemp(filamentIndex) > preHeatTemp) {// Done preheating
            stage = FilamentChangeStage::ProcessSelection;
            RetroPreviousStep = &retroMainMenuStep;
        }
        else
            RetroPreviousStep = 0;
    }
    else if (stage  == FilamentChangeStage::Wait){
        if (!hasComsQueued()) {// all done
            stage = FilamentChangeStage::ProcessSelection;
            RetroPreviousStep = &retroMainMenuStep;
        }
        else
            RetroPreviousStep = 0;
    }
    else if (stage == FilamentChangeStage::ProcessSelection){
        RetroPreviousStep = &retroMainMenuStep;
    }
    else
        RetroPreviousStep = 0;
}

bool swapToolsBkp = false;
void FilamentChangeStep::LoadComplete(){
    swapToolsBkp = swapTools;
    swapTools = false; // reset any swapping
    SERIAL_IMPL.printf("Begin Change Filament %d\n", filamentIndex);
    enqueueComs({"G91", "G1 Z5 F1000", "G90"});
    ThisWriteTemp(filamentIndex, preHeatTemp);
    stage = FilamentChangeStage::Preheat;
    
    Preferences prefs;
    prefs.begin("material");
    eColors[0] = AvailableColors[prefs.getInt("e1_c", 0)];
    eColors[1] = AvailableColors[prefs.getInt("e2_c", 1)];
    prefs.end();
}

void FilamentChangeStep::UnloadBegin(){
    swapTools = swapToolsBkp;
    SERIAL_IMPL.println("Drop 5mm");
    enqueueComs({"G91", "G1 Z-5 F1000", "G90"});
    ThisWriteTemp(filamentIndex, 0);
}

static void drawVThickLine(BufferedDisplay*g, int x0, int y0, int x1, int y1, int thickness, uint16_t color){
    for (int i =0; i < thickness / 2 + 1; i++)
        g->drawLine(x0, y0 + i, x1, y1 + i, color);
    for (int i =0; i < thickness / 2; i++)
        g->drawLine(x0, y0 - i, x1, y1 - i, color);
}
static int animPeriod = 6000;
static void drawAnimatedBitmap(BufferedDisplay*g, Image* bmp, int x0, int y0, int x1, int y1, long animStart, long animEnd, long showStart, long showEnd, MixType mix){
    int duration = animPeriod;
    int time = millis() % duration;
    float p = 0;
    if (time < animStart)
        p = 0;
    else if (time > animEnd)
        p = 1;
    else{
        p = (time - animStart) / (float)(animEnd - animStart);
    }
    
    int x = MixFloats(x0, x1, p, mix);
    int y = MixFloats(y0, y1, p, mix);
    if ((time > showStart && time < showEnd))
        bmp->Draw(g, x, y);
}
void FilamentChangeStep::Paint(BufferedDisplay* g){            
    //SERIAL_IMPL.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    Color red = Color(DarkRed);
    int a = (ThisReadTemp(0) / preHeatTemp) * 255.0F;
    if (a > 255) a = 255;
    else if (a < 0) a = 0;
    red.a(a);
    red.BlendOn(DarkOceanBlue);
    BackColor = red;
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    int opBkp = g->GetOpacity();
    if (stage == FilamentChangeStage::Preheat){
        
        g->setFont(&FreeSans9pt7b);
        centerString(g, "Heating up...", Host->appWidth() / 2, Host->appHeight() / 2 - 10);
        g->setFont();
        String tempStatus = String("(") + String(ThisReadTemp(filamentIndex), 0) + "/" + String(preHeatTemp, 0) + String(")");
        centerString(g, tempStatus.c_str(), Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 + 10);
    }
    else if (stage == FilamentChangeStage::ProcessSelection){
        int trSz = 12;
        int yo = -30;
        g->setFont(&FreeSans12pt7b);
        g->SetOpacity(100);
        centerLeftString(g, "Unload", 5, retroTitleSectionHeight + Host->appHeight() / 2 + yo);
        g->fillTriangle(
            Host->appWidth() - 5 - trSz / 2, retroTitleSectionHeight + Host->appHeight() / 2 + yo - trSz / 2, 
            Host->appWidth() - 5 - trSz, retroTitleSectionHeight + Host->appHeight() / 2 + yo + trSz / 2, 
            Host->appWidth() - 5, retroTitleSectionHeight + Host->appHeight() / 2 + yo + trSz / 2,
            TextColor
        );
        
        yo = +20;
        centerLeftString(g, "Load", 5, retroTitleSectionHeight + Host->appHeight() / 2 + yo);
        g->fillTriangle(
            Host->appWidth() - 5 - trSz / 2, retroTitleSectionHeight + Host->appHeight() / 2 + yo + trSz / 2, 
            Host->appWidth() - 5 - trSz, retroTitleSectionHeight + Host->appHeight() / 2 + yo - trSz / 2, 
            Host->appWidth() - 5, retroTitleSectionHeight + Host->appHeight() / 2 + yo - trSz / 2,
            TextColor
        );
        
    }
    else if (stage == FilamentChangeStage::Warning){
        int y = 2;
        int lineHeight = 9;
        if (action == FilamentChangeActionType::Load){
            // Warn about having an existing filament
            loadBmps[filamentIndex]->Draw(g, Host->appWidth() / 2 - loadBmps[filamentIndex]->width() / 2, y);
            g->setFont();
            y += loadBmps[filamentIndex]->height() + 6;
            String exStr = String("that the extruder ") + String(filamentIndex + 1);
            centerString(g, "Kindly confirm", Host->appWidth() / 2, y); y += lineHeight;
            centerString(g, exStr.c_str(), Host->appWidth() / 2, y); y += lineHeight;
            centerString(g, "is empty", Host->appWidth() / 2, y); y += lineHeight;
        }
        else if (action == FilamentChangeActionType::Unload){
            // Warn about having an existing filament
            unloadBmps[filamentIndex]->Draw(g, Host->appWidth() / 2 - unloadBmps[filamentIndex]->width() / 2, 2);
            g->setFont();
            y += loadBmps[filamentIndex]->height() + 6;
            centerString(g, "I am going to", Host->appWidth() / 2, y); y += lineHeight;
            centerString(g, "pull out the ", Host->appWidth() / 2, y); y += lineHeight;
            centerString(g, "filament now", Host->appWidth() / 2, y); y += lineHeight;
        }
        y += lineHeight / 2;
        int tSz = 12;
        int x = Host->appWidth() / 4;
        g->fillTriangle(x, y + tSz / 2, x - tSz / 2, y - tSz / 2, x + tSz / 2, y - tSz / 2, TextColor);   
        centerString(g, "Continue", x, y + lineHeight + tSz / 2);
        x = (Host->appWidth() * 3) / 4;
        g->fillTriangle(x, y - tSz / 2, x - tSz / 2, y + tSz / 2, x + tSz / 2, y + tSz / 2, TextColor);     
        centerString(g, "Back", x, y + lineHeight + tSz / 2);

    }
    else if (stage == FilamentChangeStage::Demo){ // Only in Load
        // The extruder bmp will be there at the bottom
        img_Extruder.Draw(g, Host->appWidth()/2 - img_Extruder.width() / 2, 6);
        drawAnimatedBitmap(g, &img_ArrowDown, 65, -15, 65, -5, 0, 200, 0, 3500, MixType::EaseInEaseOut);
        drawAnimatedBitmap(g, &img_ArrowUp, 42, 30, 42, 15, 0, 400, 0, 3500, MixType::EaseInEaseOut);
        drawAnimatedBitmap(g, &img_ArrowIn, 18, 75, 42, 59, 1200, 2000, 1200, 2000, MixType::EaseInEaseOut); 
        drawAnimatedBitmap(g, &img_ArrowIn, 18, 75, 42, 59, 2000, 2800, 2000, 4500, MixType::EaseInEaseOut);
        drawAnimatedBitmap(g, &img_ArrowUp, 65, 0, 65, -10, 3500, 4100, 3500, 4500, MixType::EaseInEaseOut);
        drawAnimatedBitmap(g, &img_ArrowDown, 42, 5, 42, 20, 3500, 4100, 3500, 4500, MixType::EaseInEaseOut);
        // img_ArrowIn.Draw(g, 22, 65);
        int lx0 = 35, ly0 = 90;
        int lx1 = 75, ly1 = 63;
        if (millis()%animPeriod > 1200 && millis() % animPeriod < 4500)
            drawVThickLine(g, lx0, ly0, lx1, ly1, 3, eColors[filamentIndex]);

        int y = Host->appHeight() - 22;
        int lineHeight = 9;
        int tSz = 12;
        int x = Host->appWidth() / 4;
        g->fillTriangle(x, y + tSz / 2, x - tSz / 2, y - tSz / 2, x + tSz / 2, y - tSz / 2, TextColor);   
        centerString(g, "Continue", x, y + lineHeight + tSz / 2);
        //centerString(g, String(millis() % 5000).c_str(), x, y + lineHeight + tSz / 2);
        x = (Host->appWidth() * 3) / 4;
        g->fillTriangle(x, y - tSz / 2, x - tSz / 2, y + tSz / 2, x + tSz / 2, y + tSz / 2, TextColor);     
        centerString(g, "Back", x, y + lineHeight + tSz / 2);
    }
    else if (stage == FilamentChangeStage::Action || stage == FilamentChangeStage::Wait){
        int y = 2;
        if (action == FilamentChangeActionType::Load){
            // Warn about having an existing filament
            loadBmps[filamentIndex]->Draw(g, Host->appWidth() / 2 - loadBmps[filamentIndex]->width() / 2, y);
            g->setFont();
            y += (Host->appHeight() - loadBmps[filamentIndex]->height()) / 2 + loadBmps[filamentIndex]->height() - 5;
            centerString(g, "Please wait...", Host->appWidth() / 2, y);
        }
        else if (action == FilamentChangeActionType::Unload){
            // Warn about having an existing filament
            unloadBmps[filamentIndex]->Draw(g, Host->appWidth() / 2 - unloadBmps[filamentIndex]->width() / 2, 2);
            g->setFont();
            y += (Host->appHeight() - loadBmps[filamentIndex]->height()) / 2 + loadBmps[filamentIndex]->height() - 5;
            centerString(g, "Please wait...", Host->appWidth() / 2, y);
        }
    }
    g->setFont();
    g->SetOpacity(opBkp);
}

void FilamentChangeStep::HandleKeyPress(Keys key) {
    if (stage == FilamentChangeStage::Preheat){
        if (key == Keys::KEYPAD_RIGHT){
            Host->GotoNextStep(); // back to filament settings
        }
    }
    if (stage == FilamentChangeStage::ProcessSelection){
        if (key == Keys::KEYPAD_UP){
            action = FilamentChangeActionType::Unload;
            stage = FilamentChangeStage::Warning;
        }
        else if (key == Keys::KEYPAD_DOWN){
            action = FilamentChangeActionType::Load;
            stage = FilamentChangeStage::Warning;
        }
        else if (key == Keys::KEYPAD_RIGHT){
            Host->GotoNextStep(); // back to filament settings
        }
    }
    else if (stage == FilamentChangeStage::Warning){
        if (key == Keys::KEYPAD_UP){
            stage = FilamentChangeStage::ProcessSelection;
        }
        else if (key == Keys::KEYPAD_DOWN){             
            if (filamentIndex == 0)
                enqueueComs("T0");
            else
                enqueueComs ("T1");
            if (action == FilamentChangeActionType::Load){
                stage = FilamentChangeStage::Demo;
                enqueueComs({"M83", "G1 E20 F100", "M82"});
            }
            else{
                stage = FilamentChangeStage::Action; // unload does not have a demo
                SERIAL_IMPL.println("Retract");
                enqueueComs({"M83", "G1 E5 F500", "G1 E-100 F4000", "G1 E-100", "G1 E-100", "G1 E-100", "G1 E-100", "G1 E-100", "M82"});
                stage = FilamentChangeStage::Wait;            
            }
        }
        else if (key == Keys::KEYPAD_RIGHT){
            Host->GotoNextStep(); // back to filament settings
        }
    }
    else if (stage == FilamentChangeStage::Demo){
        if (key == Keys::KEYPAD_DOWN){
            stage = FilamentChangeStage::Action;
            if (action == FilamentChangeActionType::Load){
                SERIAL_IMPL.println("Extrude");      
                if (filamentIndex == 0)
                    enqueueComs("T0");
                else
                    enqueueComs ("T1");
                String exCom = ("G1 E") + (filamentIndex == 0?String(100):String(50)); // Ext B s a smaller tube
                enqueueComs({"M83", "G1 E100 F4000", "G1 E100", "G1 E100", "G1 E20", exCom, "G1 E50", "G1 E30 F100", "M82"});
                stage = FilamentChangeStage::Wait;
            }
        }
        else if (key == Keys::KEYPAD_RIGHT){
            stage = FilamentChangeStage::Warning;
        }
    }
}