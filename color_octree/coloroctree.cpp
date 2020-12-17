#include "coloroctree.h"

ColorOctree::ColorOctree()
{

}

ColorOctree::ColorOctree(QImage image, int maxPool, int maxCount)
{
   // 缩减尺寸
    long long hCount = image.width();
    long long vCount = image.height();

}

void ColorOctree::buildTree(QImage image, int maxCount)
{

}

QList<ColorOctree::ColorCount> ColorOctree::result()
{

}

void ColorOctree::addColor(ColorOctree::OctreeNode *node, ColorOctree::RGB *color, int level)
{

}

bool ColorOctree::reduceTree()
{

}

void ColorOctree::colorStats(ColorOctree::OctreeNode *node, QList<ColorOctree::ColorCount> *colors)
{

}
