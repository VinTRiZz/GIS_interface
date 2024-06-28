#ifndef MAPDRAWINTERFACE_H
#define MAPDRAWINTERFACE_H

#include <QDebug>
#include <math.h>

#include "mapdrawconfigurehelper.h"

namespace MapDraw
{

class MapDrawInterface
{
public:
    MapDrawInterface();
    ~MapDrawInterface();

    // Текст последней ошибки в формате "[ERROR] Function: [ callingFunction() ] Line: [ lineInCpp ] | Function [ mapFunction() ] returned [ CODE ]"
    QString error() const;

    // Подготавливает движок карт для отображения без Х11
    void prepareForOutput();

    // Параметры карты
    double minX() const;
    double minY() const;
    double maxX() const;
    double maxY() const;

    // Конфигурация интерфейса
    void setConfig(DrawConfiguration * cfg);
    DrawConfiguration * config() const;
    MapDrawConfigureHelper & configurator(); // Конфигуратор позволяет настроить всю конфигурацию внутренней структуры

    // Обновляет всю инфу по карте, чтобы потом её можно было запросить
    void updateMapParameters();

    // Рисуют фигуры
    bool drawDot(const Utility::Point point);
    bool drawLine(const Utility::Point startPoint, const Utility::Point endPoint);
    bool drawRect(const Utility::Point leftBottom, const Utility::Point rightTop);
    bool drawCircle(const Utility::Point center, const double R);
    bool drawEllypse(const Utility::Point center, const double a, const double b);
    bool drawFigure(QVector<Utility::Point> points);
    bool drawBezier(QVector<Utility::Point> points);

    // Дополнительные фигуры (не примитивы)
    bool drawMultiCircle(const Utility::Point& center, QVector<CircleParams> params);
    bool drawTrace(const QVector<Utility::Point> points, const double pointRadius);
    bool drawText(const Utility::Point & center, const QString & data);

    // Удаляет любой последний объект на текущем HSITE карты
    bool removeLastFigure(const char * mapFilePath);
    bool removeObject(unsigned long number, const char *mapFilePath);

    // excode -- Код объекта в классификаторе
    bool drawRscObject(const Utility::Point where, const long excode);

    // Конвертирует позицию на виджете в позицию на карте, параметр global дополнительно конвертирует результат в точку на глобальной карте
    // Не рекомендуется использование global с плоскими картами (наблюдаются ошибки в отображении, карта не определяет позицию)
    Utility::Point convertPoint(const Utility::Point & posOnWidget, bool global = false);

private:
    MapDrawConfigureHelper m_configHelper;
    DrawConfiguration * m_config {nullptr};
    QString errorText;

    // Подготавливает карту к началу рисования объекта
    bool setupForDrawing(const long local, const int imgtype);

    // Завершает рисование объекта, сохраняя изменения
    bool commitObjectAdding();

    // Рисует фигуру по точкам и возвращает true если отрисовка успешна, иначе задаёт текст ошибки и возвращает false
    // Также заполняет фигуру выбранным способом
    bool drawLinearFigure(QVector<Utility::Point> & points);

    // Внутренняя копия функции
    bool drawText(const Utility::Point & center, const QString & data, IMGTEXT &textConf);

    // Добавить точки на карту
    bool appendDrawPoints(QVector<Utility::Point> & points);
};

}


#endif // MAPDRAWINTERFACE_H
