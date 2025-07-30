#include "UIComponents.h"
#include "..\Hardware\BufferedDisplay.h"
#include "..\Hardware\Keypad.h"
#include "Utils.h"
#include <Fonts/FreeSansBold9pt7b.h>
#include "MenuHost.h"
#include "..\App\Images.h"
#include "..\App\MenuApp.h"

ListItem::ListItem(MenuHost* host, int height){
    Host = host;
    itemHeight = height;
}
StringListItem::StringListItem(MenuHost* host, Image* icon, String str, int _endTrimLength, int height):ListItem(host, height){
    ItemText = str;
    this->Icon = icon;
    endTrimLength = _endTrimLength;
}
void StringListItem::Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected) {
    
    uint8_t opBkp = g->GetOpacity();
    g->SetOpacity(op);
    g->setTextColor(TextColor);
    // }
    // else
    //     g->setTextColor(TextColor);
    //Serial.printf("Printing SD File %d: %s @ %d, %d\n", i, files[i].c_str(), Host->appWidth() / 2, FileLineCenter(i));
    int16_t w = 0;    
    if (selected){ // b.t.w. we can discard opacity here coz the selected is 100% op
        g->SetOpacity(15);
        g->fillRect(0, y - itemHeight / 2, g->width(), itemHeight, 0);
        g->SetOpacity(op);
    }
    centerStringWithImage(g, Icon, ItemText.substring(0, ItemText.length() - endTrimLength).c_str(), Host->appWidth() / 2, y, &w, 0, true);
    // if (selected){
    //     w = Host->appWidth() - w - 8;
    //     g->drawLine(0, y, w / 2, y, TextColor);
    //     g->drawLine(Host->appWidth(), y, Host->appWidth() - w / 2, y, TextColor);
    // }

    g->SetOpacity(opBkp);
}

FileNameListItem::FileNameListItem(MenuHost* host, String str, String dosName, int _endTrimLength, int height):StringListItem(host, 0, str, _endTrimLength, height) {
    DOSName = dosName;
}
void ListSeparatorItem::Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected) {
    
    uint8_t opBkp = g->GetOpacity();
    uint8_t lineMargin = 10;
    g->SetOpacity(10);
    for (int i =0; i < 5; i++)
        g->drawLine(lineMargin + i * 2, y, Host->appWidth() - lineMargin - i * 2, y, TextColor);
    g->SetOpacity(opBkp);
}

ColorSelectorListItem::ColorSelectorListItem(MenuHost* host, String str, int colorIndex, int height):ListItem(host, height){
    ItemText = str;
    selectedColorIndex = colorIndex;
}
void ColorSelectorListItem::incrementColor(){
    selectedColorIndex ++;
    if (selectedColorIndex >= sizeof(AvailableColors) / sizeof(AvailableColors[0]))
        selectedColorIndex = 0;
}
void ColorSelectorListItem::decrementColor(){
    selectedColorIndex --;
    if (selectedColorIndex < 0)
        selectedColorIndex = sizeof(AvailableColors) / sizeof(AvailableColors[0]) - 1;
}
void ColorSelectorListItem::Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected) {
    
    uint8_t opBkp = g->GetOpacity();
    g->SetOpacity(op);
    g->setTextColor(TextColor);
    // }
    // else
    //     g->setTextColor(TextColor);
    //Serial.printf("Printing SD File %d: %s @ %d, %d\n", i, files[i].c_str(), Host->appWidth() / 2, FileLineCenter(i));
    int16_t w = 0;
    int px = 8;
    int pxe = 2;
    int rSz = 18;
    int arrowSpace = 12;
    int aSz = 8;
      
    if (selected){ // b.t.w. we can discard opacity here coz the selected is 100% op
        g->SetOpacity(15);
        g->fillRect(0, y - itemHeight / 2, g->width(), itemHeight, 0);
        g->SetOpacity(op);
    }
    centerLeftString(g, ItemText.c_str(), px, y, &w);
    g->SetOpacity(100); // force full bright rect.
    g->fillRoundRect(Host->appWidth() - pxe - arrowSpace - rSz, y - rSz/2, rSz, rSz, 3, AvailableColors[selectedColorIndex]);
    g->SetOpacity(op); // revert to list opacity
    g->drawRoundRect(Host->appWidth() - pxe - arrowSpace - rSz, y - rSz/2, rSz, rSz, 3, TextColor);
    // Draw the label
    if (Label.length() > 0){
        centerString(g, Label.c_str(), Host->appWidth() - pxe - arrowSpace - rSz / 2, y);
    }
    if (selected){
        // Show arrows with the color
        g->fillTriangle(
            Host->appWidth() - pxe - arrowSpace - rSz - arrowSpace + aSz, y - aSz / 2, 
            Host->appWidth() - pxe - arrowSpace - rSz - arrowSpace + aSz, y + aSz / 2, 
            Host->appWidth() - pxe - arrowSpace - rSz - arrowSpace, y, 
            TextColor);
        g->fillTriangle(
            Host->appWidth() - pxe - aSz, y - aSz / 2, 
            Host->appWidth() - pxe - aSz, y + aSz / 2, 
            Host->appWidth() - pxe, y, 
            TextColor);
    }
    g->SetOpacity(opBkp);
}
VerticalList::VerticalList(MenuHost* host, int _displayHeight, String emptyString){
    Host = host;
    for (int i =0; i < items.size(); i++)
        items[i] = 0;
    EmptyString = emptyString;
    displayHeight = _displayHeight;
}
VerticalList::~VerticalList(){
    for (int i = 0; i < items.size(); i++){
        if (items[i]) delete items[i];
    }
}
int VerticalList::getSelectedIndex(){
    return selected;
}
ListItem* VerticalList::getSelected(){
    if(selected < items.size())
        return items[selected];
    return 0;
}
// Overload operator[] for non-const access
ListItem* VerticalList::operator[](int index) {
    if (index < 0 || index >= items.size()) {
        return 0;
    }
    return items[index];
}
void VerticalList::Clear(){
    items.clear();
    scrollOffset = 0;
    targetScrollOffset = 0;
    lastSelected = -2;
    selected = -1;
}
void VerticalList::Add(ListItem* item){
    items.push_back(item);
    if (selected < 0)
        selected = 0;
}
int VerticalList::Count(){
    return items.size();
}
void VerticalList::scrollDown(){ // List goes down, selection goes up
    int toScroll = 0;
    if (getSelectedIndex() >= 0 && getSelectedIndex() < Count())
        toScroll += items[getSelectedIndex()]->getHeight() / 2;
    if (getSelectedIndex() + 1  >= 0 && (getSelectedIndex() + 1) < Count())
        toScroll += items[getSelectedIndex() + 1]->getHeight() / 2;
    targetScrollOffset -= toScroll;
    selected++;
    if (selected >= Count()){
        selected--;
    }
    SERIAL_IMPL.printf("Scroll Down: toScroll: %d, targetScrollOffset: %d\n", toScroll, targetScrollOffset);
    if (items[selected ]->IsDummyItem()) { // skip over    
        SERIAL_IMPL.println("Scrolled down on dummy item, skipping");
        scrollDown();
    }
}
void VerticalList::scrollUp(){ // List goes up, selection goes down
    int toScroll = 0;
    if (getSelectedIndex() >= 0 && getSelectedIndex() < Count()) // in case we are coming from indeterminded state
        toScroll += items[getSelectedIndex()]->getHeight() / 2; // Scroll current item's half
    if (getSelectedIndex() - 1  >= 0 && (getSelectedIndex() - 1) < Count())
        toScroll += items[getSelectedIndex() - 1]->getHeight() / 2; // scroll over the half of the next item
    targetScrollOffset += toScroll; // Implement the scroll
    selected--;
    if (selected < 0){
        selected++;
    }
    SERIAL_IMPL.printf("Scroll Up: toScroll: %d, targetScrollOffset: %d\n", toScroll, targetScrollOffset);
    if (items[selected]->IsDummyItem()) {// skip over, double scroll
        SERIAL_IMPL.println("Scrolled Up on dummy item, skipping");
        scrollUp();
    }
}
void VerticalList::SetOnSelectionUpdated(void *_owner, SelectionUpdatedCallback _OnSelectionUpdated){
    this->OnSelectionUpdated = _OnSelectionUpdated;
    this->Owner = _owner;
}
void VerticalList::InvokeSelectionChanged(){
    if (OnSelectionUpdated)
        OnSelectionUpdated(Owner, getSelected(), getSelectedIndex());
}
void VerticalList::Paint(BufferedDisplay* g, Color TextColor){
    g->setTextColor(TextColor);
    if (targetScrollOffset != scrollOffset){
        scrollOffset = (scrollOffset + targetScrollOffset) / 2;
    }
    if (items.size() == 0)
        centerString(g, emptyString.c_str(), Host->appWidth() / 2, displayHeight / 2);
    else{

        // Adjust underflow from the top
        if (scrollOffset > 0) {
            targetScrollOffset = 0;
            SERIAL_IMPL.println("Adjust underflow 1");
        }
        else {
            int maxScroll = 0;
            for (int i = 0; i < Count(); i++){
                if (i == 0 || i == (Count() - 1))
                    maxScroll += items[i]->getHeight() / 2;
                else
                    maxScroll += items[i]->getHeight();
            }
            if (scrollOffset < -maxScroll) {
                targetScrollOffset = -maxScroll;
                SERIAL_IMPL.printf("Adjust underflow 2 %d < %d\n", scrollOffset, -maxScroll);
            }
        }
        int lY = 0;
        for (int i = 0; i < items.size(); i++){
            g->setFont();
            //if (i * lineHeight + scrollOffset > - lineHeight / 2 && i * lineHeight + scrollOffset < lineHeight / 2){
            
            int16_t op = (abs(lY + scrollOffset) * 150) / displayHeight;
            if (op < 0) op = 0;
            if (op > 100) op = 100;
            op = 100 - op;
            // 100, 90, 80, 70, 60
            if (op < 95){ 
                op -= 50; 
                if (op < 0)
                    op = 0;
            }

            //SERIAL_IMPL.printf("i: %d, lY: %d, scrollOffset: %d, op: %d, selected: %d\n", i, lY, scrollOffset, op, selected);
            //Serial.printf("Printing SD File %d: %s @ %d\n", i, files[i].c_str(), op);
            
            // if (op > 100)
            //     op = 100;
            // op = 100 - op;
            // if (op < 95){
            //     op -= 50;
            //     op *= 2;
            // }

            // if (op > 95)
            //     selected = i;
            //SERIAL_IMPL.printf("Rect i = %d, lY = %d, height = %d\n", i, lY, items[i]->getHeight());
            //g->drawRect(0, lY + displayHeight / 2 + scrollOffset - items[i]->getHeight() / 2, 128, items[i]->getHeight(), i%2? ST7735_BLUE:ST7735_ORANGE);
            items[i]->Paint(g, op, TextColor, lY + displayHeight / 2 + scrollOffset, i == selected);
            if (i + 1 < Count()){
                lY += items[i]->getHeight() / 2 + items[i + 1]->getHeight() / 2;
            }
        }
    }
    // We dont have a tick in the list obj
    if(getSelectedIndex() == lastSelected){
        return;
    }
    lastSelected = getSelectedIndex();
    SERIAL_IMPL.printf("Selected index changed: %d\n", getSelectedIndex());
    InvokeSelectionChanged();
}

Notification::Notification(String& text, int life)
{
    if (life < 0)
        life = text.length() * 200;
    lifeLeft = life;
    this->Text = text;
}
void Notification::HandleKeyUp(Keys key){
    if (key == KEYPAD_MIDDLE || key == KEYPAD_BACK){
        lifeLeft = 1;
    }
}
void Notification::Paint(BufferedDisplay* g){
    int opBkp = g->GetOpacity();
    int op = 100;
    if (fadeInProgress <= 100){
        // Happens only in fade in
        op = fadeInProgress;
        fadeInProgress += 25;
    }
    else if (lifeLeft <= 1) {
        // Happens only in fade out
        if (fadeOutProgress > 100){
            lifeLeft = 0;
            return; // not possible though. Still making it a safe call.
        }
        op = 100 - fadeOutProgress;
        fadeOutProgress += 25;
    }
    else {
        if (lastDrawing == 0)
            lastDrawing = millis();
        else {
            lifeLeft -= millis() - lastDrawing;
            if (lifeLeft <= 0)
                lifeLeft = 1; // dodge the host for the exit animation
            lastDrawing = millis();
        }
    }
            
    g->setFont(&FreeSansBold9pt7b);
    g->SetOpacity(op / 2);
    g->fillScreen(0);
    g->SetOpacity(op);
    g->setTextColor(ST7735_WHITE);
    drawMultilineCenteredText(g, Text, g->width() / 2, g->height() / 2, g->width() - 10, 17);

    g->setFont();
    g->SetOpacity(opBkp);
}

Notification::~Notification()
{
}

MenuStep::MenuStep(MenuHost* host){
    Host = host;
}
void MenuStep::loop(){
    if (TickPeriod == 0) return; 
    if (millis() - lastTick > TickPeriod){
        lastTick = millis();
        Tick();
    }
}
void MenuStep::PaintRetroTitle(BufferedDisplay* g) {
    // Draw at 0 because g has the offset.
    retro_drawTitleBar(g, Host, 0, RetroIcon, Title.c_str(), TextColor);
}
void MenuStep::PaintRetroOptionsBar(BufferedDisplay* g) {
    // Draw at 0 because g has the offset.
    retro_drawNavigationBar(g, Host, 0,
        GetPreviousStep()?(&img_RetroBack):0,
        GetNextStep()?(&img_RetroEnter):0, GetNextStep()?NextActionString.c_str():"",
        CanJumpToMainMenu()?&img_RetroOptionsKey:0, 
        TextColor);
}
MenuStep* MenuStep::GetPreviousStep(bool returnEvenIfDummy){
    if (Host->Retro) {
        if (RetroPreviousStep)
            if (RetroPreviousStep->IsDummyStep() && !returnEvenIfDummy)
                return 0;
        return RetroPreviousStep;
    }
    else{        
        if (RetroNextStep)
            if (RetroNextStep->IsDummyStep() && !returnEvenIfDummy)
                return 0;
        return PreviousStep;
    }
}
MenuStep* MenuStep::GetNextStep(){
    if (Host->Retro)
        return RetroNextStep;
    else
        return NextStep;
}
bool MenuStep::CanJumpToMainMenu(){
    return true;
}
bool MenuStep::IsDummyStep(){
    return isDummy;
}