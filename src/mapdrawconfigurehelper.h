#ifndef MAPDRAWCONFIGUREHELPER_H
#define MAPDRAWCONFIGUREHELPER_H

#include <QString>
#include <QFileInfo>
#include <QDebug>

#define BU_POINT_FUNCTIONALITY
#define BU_POINT_TO_STRING_CONVERTION
#define BU_POINT_GENERATION_FUNCTIONS
#include <UtilityLibrary/main>

#include "gisdesigner/mapapi.h"
#include "gisdesigner/maptype.h"
#include "gisdesigner/rscapi.h"
#include "gisdesigner/rstapi.h"
#include "gisdesigner/qdmcmp.h"

namespace MapDraw
{

enum LineType
{
    SOLID,
    DOTLINE
};

enum FillType
{
    FILL_NONE    = 0,
    FILL_SOLID   = 1
};

enum FontType
{
    // TODO: Разобраться чё за таблица (IMGTEXT::Type)
    IDK = 0,
    STANDARD = 1,
    NON_STANDARD
};

struct CircleParams
{
    double radius       {0};
    uint8_t color_red   {0};
    uint8_t color_green {0};
    uint8_t color_blue  {0};
};

struct DrawConfiguration
{
    QString rscPath;                                    // Classificator path

    long errorCode {0};                                 // Для ГИСовых ошибок
    long * pErrorCode {nullptr};                        // Для setMap функции с существующей переменной кода ошибки

    // Main
    HMAP        hmap         {0};                       // Map handle
    HSITE       hsite        {0};                       // Site file handle (0 if current map)

    // Map limit coordinates
    double mapMinX {0}, mapMinY {0};
    double mapMaxX {0}, mapMaxY {0};

    // Drawing
    HOBJ        objBuffer    {0};

    PAINTPARM   form         { };                       // Screen object type

    double      dotSize      {1};                       // Size of a dot in a map
    IMGLINE     lineParam    { };                       // Line draw param
    IMGDOT      dotLineParam { };                       // Dot line param
    IMGTEXT     txt          { };
    IMGTEXT     traceText    { };                       // For "Trace" figure

    FillType    fillType     {FillType::FILL_NONE};     // Type of figure filling
    LineType    lineType     {LineType::SOLID};         // Line type, dotline or solid line

    // Etc
    unsigned long drawLayer = 1;                        // Layer drawing on
    unsigned long currentList = 1;                      // List drawing on
    bool referredRotation = false;                      // If true, center of rotation calculated using dots array
    bool isBorderDraw = false;                          // Draw border for every figure

    double currentScale = 1.0;                          // Map scale
    unsigned long resizable = 1;                        // If 1, will be scaled on map scale, if 0, not
    unsigned long minScale = 0;                         // Пределы для отображения объекта
    unsigned long maxScale = 0;

    double drawAccuracy = 1;                            // Используется для метров в эллипсе

    // Rotation
    double rotateDegree = 0;                            // Angle in degree (unused)
    double rotateRad = 0;                               // Angle in rad
    Utility::Point rotateCenter;                                 // Current center to reffer for rotation
};

class MapDrawConfigureHelper
{
public:
    MapDrawConfigureHelper();
    ~MapDrawConfigureHelper();

    DrawConfiguration * createDefaultConfiguration();
    void setStruct(DrawConfiguration * s);
    DrawConfiguration * getStruct();

    // После вызова setMap нужно ОБЯЗАТЕЛЬНО вызвать updateMapParameters() в использующем классе
    void setMap(const QString &filePath);
    void setMap(HMAP hmap, long * errorCodeStorageVariable = 0);
    HMAP map();
    void closeMap();

    void setSite(HSITE sit);
    HSITE site();

    // Точка относительно которой вращать рисуемые фигуры
    void setRotationCenter(const Utility::Point & center);

    // Если true, ищет центр любого многоугольника, который будет отрисован
    void setReferredRotation(bool t = true);

    // Угол поворота
    void setRotationAngle(double degree);
    double rotationAngle() const;

    // Слой на котором рисуется
    void setLayer(uint layer);
    uint layer() const;

    // Количество объектов на карте
    long objectCount() const;

    // Текущий лист
    void setList(long number);
    long list() const;

    /**
     * @brief setDrawAccuracy Set coeff of accuracy, circle and ellypse depend on it
     * @param stepValue Multiplier for degrees in circle and coordinates in ellypse
     */
    void setDrawAccuracy(double stepValue);
    double drawAccuracy() const;

    // Касается всех линий и точек, которые рисует интерфейс
    void setLineWidth(double w);
    double lineWidth() const;

    void setLineType(LineType type);
    LineType lineType() const;

    void setTextConfig(IMGTEXT cfg);
    void setTextColor(long color, long backColor, long shadowColor);
    void setTextGeometry(long height, long wide, bool isHorisontal = true);
    void setTextFont(long weight, bool isItalic, long align, FontType type = FontType::STANDARD, long sizeFlag = IMGTEXT_SCALE);
    void setTextFontAdditional(long interval, bool hasOutline, bool hasUnderline, bool isStrikeOut);
    IMGTEXT textConfig() const;

    void setLineColor(uint8_t r, uint8_t g, uint8_t b);
    void setLineColor(long color);
    long lineColor() const; // Возвращает результат макроса RGB(R,G,B)

    void setLineSpaces(double dashLength, double spaceLength); // Только для пунктирной линии
    double lineDashLength() const;
    double lineSpaceLength() const;

    // Устанавливает способ заливки фигуры (см. enum FillType)
    void setFigureFillType(FillType type);
    FillType fillType() const;

    // Разрешить увеличивать объект при увеличении масштаба карты
    void setResizable(bool r = true);

    // Пределы видимости объектов. Для беспредела нужно поставить 0 в оба параметра
    void setScaleLimits(long minScale, long maxScale);

    void setScale(double newScale);     // Относительно измененить масштаб, 1 : (baseScale * newScale)
    void setBaseScale(long baseScale);  // Базовый масштаб, 1 : baseScale

    void setBorderDraw(bool draw = true);
    bool isBorderDrawing();

private:
    DrawConfiguration * m_config {nullptr};
};

}


#endif // MAPDRAWCONFIGUREHELPER_H
