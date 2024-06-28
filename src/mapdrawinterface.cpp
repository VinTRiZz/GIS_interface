#include "mapdrawinterface.h"
#include "mapdrawinterface_defines.h"

using namespace MapDraw;

MapDrawInterface::MapDrawInterface() :
    m_config { m_configHelper.createDefaultConfiguration() }
{

}

MapDrawInterface::~MapDrawInterface()
{

}

QString MapDrawInterface::error() const { return errorText; }

void MapDrawInterface::prepareForOutput()
{
    mapSetViewType(m_config->hmap, VT_PRINTRST);
    mapSetBackColor(m_config->hmap, RGB(0,0,0));
    mapSetBackPrintColor(m_config->hmap, RGB(0,0,0));
}

double MapDrawInterface::minX() const { return m_config->mapMinX; }
double MapDrawInterface::minY() const { return m_config->mapMinY; }
double MapDrawInterface::maxX() const { return m_config->mapMaxX; }
double MapDrawInterface::maxY() const { return m_config->mapMaxY; }

void MapDrawInterface::setConfig(DrawConfiguration * cfg) { m_config = cfg; }
DrawConfiguration *MapDrawInterface::config() const { return m_config; }

MapDrawConfigureHelper &MapDrawInterface::configurator() { return m_configHelper; }

bool MapDrawInterface::drawDot(const Utility::Point point)
{
    CHECK_MAP
    QVector<Utility::Point> pointsBuffer = Utility::createEllypsePoints(point, m_config->dotSize, m_config->dotSize, m_config->drawAccuracy);
    return drawLinearFigure(pointsBuffer);
}

bool MapDrawInterface::drawLine(const Utility::Point startPoint, const Utility::Point endPoint)
{
    CHECK_MAP
    QVector<Utility::Point> pointsBuffer {startPoint, endPoint};
    return drawLinearFigure(pointsBuffer);
}

bool MapDrawInterface::drawRect(const Utility::Point leftBottom, const Utility::Point rightTop)
{
    CHECK_MAP
    QVector<Utility::Point> pointsBuffer = Utility::createRectPoints(leftBottom, rightTop);
    return drawLinearFigure(pointsBuffer);
}

bool MapDrawInterface::drawCircle(const Utility::Point center, const double R)
{
    CHECK_MAP
    QVector<Utility::Point> pointsBuffer = Utility::createEllypsePoints(center, R, R, m_config->drawAccuracy);
    return drawLinearFigure(pointsBuffer);
}

bool MapDrawInterface::drawEllypse(const Utility::Point center, const double a, const double b)
{
    CHECK_MAP
    QVector<Utility::Point> pointsBuffer = Utility::createEllypsePoints(center, a, b, m_config->drawAccuracy);
    return drawLinearFigure(pointsBuffer);
}

bool MapDrawInterface::drawFigure(QVector<Utility::Point> points)
{
    CHECK_MAP
    return drawLinearFigure(points);
}

bool MapDrawInterface::drawBezier(QVector<Utility::Point> points)
{
    CHECK_MAP
    QVector<Utility::Point> pointsBuffer = Utility::createBezierPoints(points, 0.01);
    return drawLinearFigure(pointsBuffer);
}

bool MapDrawInterface::drawMultiCircle(const Utility::Point& center, QVector<CircleParams> circles)
{
    std::sort(circles.begin(), circles.end(), [](auto & f, auto & s){ return f.radius > s.radius; });

    long currentColor = m_config->lineParam.Color;
    bool result = true;

    for (int i = 0; i < circles.count(); i++)
    {
        m_configHelper.setLineColor(circles[i].color_red, circles[i].color_green, circles[i].color_blue);
        if (!drawCircle(center, circles[i].radius))
        {
            result = false;
            break;
        }
    }

    m_configHelper.setLineColor(currentColor); // Reset color
    return result;
}

bool MapDrawInterface::drawTrace(const QVector<Utility::Point> points, const double pointRadius)
{
    m_configHelper.setBorderDraw();
    auto fillType = m_config->fillType;
    m_config->fillType = FillType::FILL_SOLID;
    long colorBuffer = m_configHelper.lineColor();
    m_configHelper.setLineColor(RGB(180, 180, 255));

    bool result = false;

    Utility::Point connectionPointPrev, connectionPointNext;
    QString numStr = QString::number(1);

    if (drawCircle(points.first(), pointRadius))
    {
        drawText(Utility::Point(points[0].x - pointRadius / 8, points[0].y - pointRadius / 8), numStr, m_config->traceText);

        for (int i = 0; i < points.size() - 1; i++)
        {
            if (!drawCircle(points[i + 1], pointRadius))
            {
                result = false;
                break;
            }

            // Write number
            numStr = QString::number(i + 2);
            drawText(Utility::Point(points[i + 1].x - pointRadius / 8, points[i + 1].y - pointRadius / 8), numStr, m_config->traceText);

            connectionPointPrev = points[i + 1];
            connectionPointNext = points[i];
            Utility::calculateImmediate(connectionPointPrev, connectionPointNext, pointRadius);

            if (!drawLine(connectionPointPrev, connectionPointNext))
            {
                result = false;
                break;
            }
        }
        result = true;
    }

    m_config->fillType = fillType;
    m_configHelper.setLineColor(colorBuffer);
    m_configHelper.setBorderDraw(false);

    return result;
}

bool MapDrawInterface::drawText(const Utility::Point &center, const QString &data) { return drawText(center, data, m_config->txt); }
bool MapDrawInterface::removeLastFigure(const char * mapFilePath) { return removeObject(m_configHelper.objectCount() - 1, mapFilePath); }

bool MapDrawInterface::removeObject(unsigned long number, const char * mapFilePath)
{
    long operationResult;
    _createMapSiteObject()

    m_config->objBuffer = mapReadObjectByNumber(m_config->hmap, m_config->hmap, m_config->objBuffer, m_config->currentList, number);

    if (!m_config->objBuffer)
    {
        errorText = "[ERROR] Not found object";
        return false;
    } else if (m_config->objBuffer == 1)
    {
        errorText = "[ERROR] Already removed";
        return false;
    }

    for (int i = 0; i < mapPointCount(m_config->objBuffer, 0); i++)
        operate(mapDeletePointPlane, m_config->objBuffer, i, 0)

    for (int i = 1; i < mapDrawCount(m_config->objBuffer); i++)
        operate(mapDeleteDraw, m_config->objBuffer, i)

    for (int i = 1; i < mapSemanticAmount(m_config->objBuffer); i++)
        operate(mapDeleteSemantic, m_config->objBuffer, i)

    operate(mapDeleteObject, m_config->objBuffer)

//    operate(mapCommitObjectEx, m_config->objBuffer, &m_config->errorCode)
    operate(mapCommit, m_config->objBuffer)
    operate(mapCommitObject, m_config->objBuffer)
    mapFreeObject(m_config->objBuffer);

//    operate(mapDeleteObjectByNumber, m_config->hmap, m_config->currentList, m_configHelper.objectCount() - 1)
    operate(MapSortingWithEvent, mapFilePath, 0, NULL, NULL, 0, 0) // Нуждается в map/map для создания sort.copy__
    return true;
}

bool MapDrawInterface::drawRscObject(const Utility::Point where, const long excode)
{
    m_config->objBuffer = mapCreateObject(m_config->hmap, m_config->currentList);
    if (!m_config->objBuffer)
    {
        errorText = "[ERROR] Create object";
        return false;
    }

    long operationResult = 0;

    operate(mapRegisterObject, m_config->objBuffer, excode, LOCAL_POINT)
    operate(mapAppendPointPlane, m_config->objBuffer, where.x, where.y)
//    operate(mapCommitWithPlace, m_config->objBuffer)
    operate(mapCommitObject, m_config->objBuffer)
    return true;
}

Utility::Point MapDrawInterface::convertPoint(const Utility::Point &posOnWidget, bool global)
{
    if (!m_config->hmap)
        return Utility::Point();

    // Position on map
    Utility::Point PointInPlane;
    PointInPlane.x = posOnWidget.x;
    PointInPlane.y = posOnWidget.y;
    mapPictureToPlane(m_config->hmap, &PointInPlane.x, &PointInPlane.y);

    if (global)
    {
        // Global position
        Utility::Point PointInWGS84;
        PointInWGS84.x = PointInPlane.x;
        PointInWGS84.y = PointInPlane.y;
        mapPlaneToGeoWGS84(m_config->hmap, &PointInWGS84.x, &PointInWGS84.y);
        return PointInWGS84;
    }
    else
        return PointInPlane;

    return Utility::Point(); // Cannot reach here
}

void MapDrawInterface::updateMapParameters()
{
    // Собирает инфу из карты, сюда добавлять все методы с этим связанные
    m_config->mapMinX = mapGetMapX1(m_config->hmap);
    m_config->mapMinY = mapGetMapY1(m_config->hmap);

    m_config->mapMaxX = mapGetMapX2(m_config->hmap);
    m_config->mapMaxY = mapGetMapY2(m_config->hmap);

    const double diffX = m_config->mapMaxX - m_config->mapMinX;
    const double diffY = m_config->mapMaxY - m_config->mapMinY;

    m_config->rotateCenter.x = m_config->mapMinX + diffX / 2;
    m_config->rotateCenter.y = m_config->mapMinY + diffY / 2;

    m_config->dotSize = 0.0006 * (diffX + diffY) / 2.0; // Получено эмпирически
}

bool MapDrawInterface::setupForDrawing(const long local, const int imgtype)
{
    // Устанавливает параметры для рисования объекта
    createMapSiteObject(IDDOUBLE2, 0)

    long operationResult = 0;
    operate(mapRegisterDrawObject, m_config->objBuffer, m_config->drawLayer, local)

    switch (m_config->lineType)
    {
    case LineType::DOTLINE:
        operate(mapAppendDraw, m_config->objBuffer, IMG_DOT, (char*)&m_config->dotLineParam)
        break;

    default:
        operate(mapAppendDraw, m_config->objBuffer, imgtype, (char*)&m_config->lineParam)
    }
    return true;
}

bool MapDrawInterface::commitObjectAdding() // Не трогац
{
    long operationResult;
    operate(mapCommitWithPlace, m_config->objBuffer)
    mapFreeObject(m_config->objBuffer);
    return true;
}

bool MapDrawInterface::drawLinearFigure(QVector<Utility::Point> & points)
{
    if (m_config->isBorderDraw)
    {
        long colorBuffer = m_configHelper.lineColor();
        m_configHelper.setLineColor(RGB(0,0,0));

        if (setupForDrawing(LOCAL_LINE, IMG_LINE))
            if (m_config->referredRotation)
            {
                m_config->rotateCenter = Utility::calculateCenter(points);
                appendDrawPoints(points);
                commitObjectAdding();
            }

        m_configHelper.setLineColor(colorBuffer);
    }

    switch (m_config->fillType)
    {
    case 1: // All fill
        if (!setupForDrawing(LOCAL_SQUARE, IMG_SQUARE))
            return false;
        break;

    // Сюда добавить заполнения

    default:
        if (!setupForDrawing(LOCAL_LINE, IMG_LINE))
            return false;
    }

    if (m_config->referredRotation)
        m_config->rotateCenter = Utility::calculateCenter(points);

    if (!appendDrawPoints(points))
        return false;

    return commitObjectAdding();
}

bool MapDrawInterface::drawText(const Utility::Point &center, const QString &data, IMGTEXT &textConf)
{
    CHECK_MAP
    createMapSiteObject(IDDOUBLE2, 1)

    long operationResult;
    operate(mapRegisterDrawObject, m_config->objBuffer, m_config->drawLayer, LOCAL_TITLE)
    operate(mapPutText, m_config->objBuffer, data.toUtf8().data(), 0)
    operate(mapAppendDraw, m_config->objBuffer, IMG_TEXT, (char*)&textConf);
    operate(mapAppendPointPlane, m_config->objBuffer, center.x, center.y)
    operate(mapCommitWithPlace, m_config->objBuffer)
    mapFreeObject(m_config->objBuffer);
    return true;
}

bool MapDrawInterface::appendDrawPoints(QVector<Utility::Point> & points)
{
    for (Utility::Point & p : points)
        Utility::rotatePoint(p, m_config->rotateCenter, m_config->rotateDegree);

    long operationResult = 0;
    for (const Utility::Point & p : points)
        operate(mapAppendPointPlane, m_config->objBuffer, p.x, p.y)

    operate(mapSetObjectScale, m_config->objBuffer, m_config->resizable)

    if (m_config->minScale)
        mapSetObjectTopScale(m_config->objBuffer, m_config->minScale);

    if (m_config->maxScale)
        mapSetObjectBotScale(m_config->objBuffer, m_config->maxScale);

    return true;
}
