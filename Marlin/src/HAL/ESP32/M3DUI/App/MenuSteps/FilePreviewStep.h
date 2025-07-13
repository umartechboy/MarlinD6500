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
    public:
    pngDecodeParams pngParams;
    int printTime = -1;
    float filamentUsed = -1;
    int maxZ = -1;
    int layerCount = -1;
    FilePreviewStep(MenuHost* host);
    void Paint(BufferedDisplay* g) override;
    void LoadBegin() override;
    void HandleKeyUp(Keys key) override;
};


#endif