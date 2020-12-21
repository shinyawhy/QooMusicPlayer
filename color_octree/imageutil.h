#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include <QPixmap>
#include <QList>
#include "coloroctree.h"

#define IMAGE_CALC_PIXEL_MAX_SIZE 128 // 计算的最大边长（大图缩小）

class ImageUtil
{
public:
    ImageUtil();

    static QColor getImageAvarageColor(QImage image, int maxPool = IMAGE_CALC_PIXEL_MAX_SIZE);

    static QList<ColorOctree::ColorCount> extractImageThemeColors(QImage image, int count);

    static QList<QColor> extractImageThemeColorsInPalette(QImage image, QList<QColor> paletteColors, int needCount);

    static QColor getInvertColor(QColor color);

    static bool getBgFgColor(QList<ColorOctree::ColorCount> colors, QColor *bg, QColor *fg);

    static bool getBgFgSgColor(QList<ColorOctree::ColorCount> colors, QColor *bg, QColor *fg, QColor *sg);

    static bool getBgFgSgColor(QList<ColorOctree::ColorCount> colors, QColor *bg, QColor *fg, QColor *sbg, QColor *sfg);
};

#endif // IMAGEUTIL_H
