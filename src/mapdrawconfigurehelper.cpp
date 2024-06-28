#include "mapdrawconfigurehelper.h"

using namespace MapDraw;

MapDrawConfigureHelper::MapDrawConfigureHelper()
{

}

MapDrawConfigureHelper::~MapDrawConfigureHelper()
{

}

DrawConfiguration * MapDraw::MapDrawConfigureHelper::createDefaultConfiguration()
{
    m_config = new DrawConfiguration();

    const decltype(RGB(0,0,0)) black = RGB(0,0,0);

    // Configure lines
    m_config->lineParam.Color = black;
    m_config->lineParam.Thick = PIX2MKM(1);

    // Configure dotline
    m_config->dotLineParam.Color = black;
    m_config->dotLineParam.Thick = PIX2MKM(1);
    m_config->dotLineParam.Dash  = PIX2MKM(2);
    m_config->dotLineParam.Blank = PIX2MKM(2);

    // Non-depended on text configuration
    m_config->txt.CharSet = OEM_CHARSET;
    m_config->txt.Service = 0;

    // Configure text
    setTextColor(RGB(0,0,0), IMGC_TRANSPARENT2, IMGC_TRANSPARENT);
    setTextGeometry(20, true, UNIW_WIDE);
    setTextFont(FW_MEDIUM, false, 0, FontType::STANDARD, IMGTEXT_SCALE);

        m_config->traceText.Color = RGB(0,0,0);
        m_config->traceText.BkgndColor = IMGC_TRANSPARENT2;
        m_config->traceText.ShadowColor = IMGC_TRANSPARENT;

        m_config->traceText.Height = PIX2MKM(10);
        m_config->traceText.Horizontal = 1;
        m_config->traceText.Wide = 1;

        m_config->traceText.Weight = FW_BOLD;
        m_config->traceText.Italic = 0;
        m_config->traceText.Align = 1;
        m_config->traceText.Type = 1;
        m_config->traceText.Flag = IMGTEXT_MINIFRAME;

        m_config->traceText.Interval = 10;
        m_config->traceText.Outline = 0;
        m_config->traceText.Underline = 0;
        m_config->traceText.StrikeOut = 0;

    setRotationAngle(0);
    setReferredRotation(true);
    setLayer(2);
    setDrawAccuracy(1);

    return m_config;
}

void MapDrawConfigureHelper::setStruct(MapDraw::DrawConfiguration *s) { m_config = s; }
MapDraw::DrawConfiguration *MapDraw::MapDrawConfigureHelper::getStruct() { return m_config; }

void MapDrawConfigureHelper::setMap(const QString &filePath)
{
    QFileInfo fileTester(filePath);
    const QString extension = fileTester.completeSuffix();

    bool    fileExist   = fileTester.exists(),
            isMap       = (extension == "map"),
            isUserMap   = (extension == "sit"),
            isStorage   = (extension == "sitx"),
            isProject   = (extension == "mpt") || (extension == "mptz"),
            isMatrix    = (extension == "mtz") || (extension == "mtq"),
            isRaster    = (extension == "rsw") || (extension == "rst")
            ;

    QString rscFile = filePath;
    rscFile.chop(extension.size());
    rscFile += "rsc";

    if (!fileExist)
    {
        qDebug() << "Map file not exist";
        return;
    }

    if (!isMap && !isUserMap && !isStorage && !isProject && !isMatrix && !isRaster)
    {
        qDebug() << "Map file invalid";
        return;
    }

    m_config->hmap = mapOpenAnyData(filePath.utf16(), GENERIC_READ | GENERIC_WRITE, &m_config->errorCode);

    if (m_config->hmap)
    {
        mapSetViewScale(m_config->hmap, 0, 0, 1000);
        qDebug() << "Map file open success";
    }
}

void MapDrawConfigureHelper::setRotationCenter(const Utility::Point &center) { m_config->rotateCenter = center; }
void MapDrawConfigureHelper::setReferredRotation(bool t) { m_config->referredRotation = t; }
double MapDrawConfigureHelper::rotationAngle() const { return m_config->rotateDegree; }

HMAP MapDrawConfigureHelper::map() { return m_config->hmap; }
void MapDrawConfigureHelper::closeMap() { if (m_config->hmap) mapCloseData(m_config->hmap); }
void MapDrawConfigureHelper::setSite(HSITE sit) { m_config->hsite = sit; }
HSITE MapDrawConfigureHelper::site() { return m_config->hsite; }

void MapDrawConfigureHelper::setDrawAccuracy(double stepValue) { m_config->drawAccuracy = stepValue; }
double MapDrawConfigureHelper::drawAccuracy() const { return m_config->drawAccuracy; }

void MapDrawConfigureHelper::setLayer(uint layer) { m_config->drawLayer = layer; }
uint MapDrawConfigureHelper::layer() const { return m_config->drawLayer; }

long MapDrawConfigureHelper::objectCount() const { return mapGetObjectCount(m_config->hmap, m_config->currentList); }
void MapDrawConfigureHelper::setList(long number) { m_config->currentList = number; }
long MapDrawConfigureHelper::list() const { return m_config->currentList; }

void MapDrawConfigureHelper::setLineWidth(double w) { m_config->lineParam.Thick = PIX2MKM(w);  m_config->dotLineParam.Thick = PIX2MKM(w); }
double MapDrawConfigureHelper::lineWidth() const { return m_config->lineParam.Thick; }

void MapDrawConfigureHelper::setLineColor(uint8_t r, uint8_t g, uint8_t b) { m_config->lineParam.Color = RGB(r, g, b); m_config->dotLineParam.Color = RGB(g, g, b); }
void MapDrawConfigureHelper::setLineColor(long color) { m_config->lineParam.Color = color; m_config->dotLineParam.Color = color; }
long MapDrawConfigureHelper::lineColor() const { return m_config->lineParam.Color; }

void MapDrawConfigureHelper::setLineSpaces(double dashLength, double spaceLength) { m_config->dotLineParam.Dash  = PIX2MKM(dashLength); m_config->dotLineParam.Blank = PIX2MKM(spaceLength); }
double MapDrawConfigureHelper::lineDashLength() const { return m_config->dotLineParam.Dash; }
double MapDrawConfigureHelper::lineSpaceLength() const { return m_config->dotLineParam.Blank; }

void MapDrawConfigureHelper::setLineType(LineType type) { m_config->lineType = type; }
LineType MapDrawConfigureHelper::lineType() const { return m_config->lineType; }

void MapDrawConfigureHelper::setTextConfig(IMGTEXT cfg) { m_config->txt = cfg; }

void MapDrawConfigureHelper::setTextColor(long color, long backColor, long shadowColor)
{
    m_config->txt.Color = color;
    m_config->txt.BkgndColor = backColor;
    m_config->txt.ShadowColor = shadowColor;
}

void MapDrawConfigureHelper::setTextGeometry(long height, long wide, bool isHorisontal)
{
    m_config->txt.Height = height;
    m_config->txt.Horizontal = isHorisontal;
    m_config->txt.Wide = wide;
}

void MapDrawConfigureHelper::setTextFont(long weight, bool isItalic, long align, FontType type, long sizeFlag)
{
    m_config->txt.Weight = weight;
    m_config->txt.Italic = isItalic;
    m_config->txt.Align = align;
    m_config->txt.Type = (int)type;
    m_config->txt.Flag = sizeFlag;
}

void MapDrawConfigureHelper::setTextFontAdditional(long interval, bool hasOutline, bool hasUnderline, bool isStrikeOut)
{
    m_config->txt.Interval = interval;
    m_config->txt.Outline = hasOutline;
    m_config->txt.Underline = hasUnderline;
    m_config->txt.StrikeOut = isStrikeOut;
}
IMGTEXT MapDrawConfigureHelper::textConfig() const { return m_config->txt; }

void MapDrawConfigureHelper::setFigureFillType(FillType type) { m_config->fillType = type; }
FillType MapDrawConfigureHelper::fillType() const { return m_config->fillType; }

void MapDrawConfigureHelper::setResizable(bool r) { m_config->resizable = r; }
void MapDrawConfigureHelper::setScaleLimits(long minScale, long maxScale)
{
    m_config->minScale = minScale;
    m_config->maxScale = maxScale;
}
void MapDrawConfigureHelper::setScale(double newScale)
{
    m_config->currentScale = newScale;
    mapChangeViewScale(m_config->hmap, 0, 0, newScale);
}

void MapDrawConfigureHelper::setBaseScale(long baseScale) { mapSetViewScale(m_config->hmap, 0, 0, baseScale); }

void MapDrawConfigureHelper::setBorderDraw(bool draw) { m_config->isBorderDraw = draw; }
bool MapDrawConfigureHelper::isBorderDrawing() { return m_config->isBorderDraw; }

void MapDrawConfigureHelper::setMap(HMAP hmap, long * errorCodeStorageVariable)
{
    m_config->hmap = hmap;
    m_config->pErrorCode = errorCodeStorageVariable;
}

void MapDrawConfigureHelper::setRotationAngle(double degree)
{
    degree = degree - (int)(degree / 360) * 360.0;
    m_config->rotateDegree = degree;
    m_config->rotateRad = degree * (M_PI / 180.0);
}
