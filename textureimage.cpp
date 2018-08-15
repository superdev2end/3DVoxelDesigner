#include "textureimage.h"

TextureImage::TextureImage(float fOpacity) : Qt3DRender::QPaintedTextureImage()
{
    mOpacity = fOpacity;
}

void TextureImage::paint(QPainter *painter)
{
////    QImage image(size(), QImage::Format_ARGB32); //Image with given size and format.
////    image.fill(Qt::transparent); //fills with transparent
////    QPainter p(&image);
////    p.setCompositionMode(QPainter::CompositionMode_Source);
////    p.drawPixmap(0, 0, mPixmap); // given pixmap into the paint device.
////    p.end();

//    QRect rect(0, 0, width(), height());
//    qDebug() << "Opacity: " << mOpacity;
//    painter->setOpacity(static_cast<qreal>(mOpacity));
//    painter->drawImage(rect, mImage);

////    painter->setBackgroundMode(Qt::TransparentMode);
////    painter->setRenderHint(QPainter::Antialiasing);

////    painter->fillRect(rect, Qt::transparent);
////    painter->setBrush(QBrush(Qt::white));
///
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setOpacity(static_cast<qreal>(mOpacity));
    painter->fillRect(0, 0, width(), height(), Qt::transparent);
    painter->drawImage(0, 0, mImage);
}

void TextureImage::setImage(QImage img)
{
    mImage = img;
    setSize(mImage.size());
    update();
}

void TextureImage::setPixmap(QPixmap pix)
{
    mPixmap = pix;
    setSize(mPixmap.size());
    update();
}

void TextureImage::setOpacity(float opacity)
{
    mOpacity = opacity;
    update();
}
