#ifndef __FILE_PREV_STEP__
#define __FILE_PREV_STEP__

#include <PNGdec.h>
#include "..\..\Utils\base64Stream.h"
#include "..\..\Components\M3DUI.h"

struct pngDecodeParams{
    BufferedDisplay* g;
    Base64Decoder thumbnailData;
    int pngWidth;
    int pngHeight;
    int x = 0, y = 0;
};


class FilePreviewStep: public MenuStep {
    private:
    uint16_t e0Color = 0;
    uint16_t e1Color = 0;
    public:
    pngDecodeParams pngParams;
    float filamentUsed_mm = 0;
    float filamentUsed_g = 0;    
    String estimatedPrintingTime;
    int maxZ = -1;
    int layerCount = -1;
    bool filament0Used = false, filament1Used = false;
    FilePreviewStep(MenuHost* host);
    void Paint(BufferedDisplay* g) override;
    void LoadBegin() override;
    void HandleKeyPress(Keys key) override;
    void LoadComplete() override;
};


#endif