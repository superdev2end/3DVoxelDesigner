#ifndef TEXTUREIMAGE_H
#define TEXTUREIMAGE_H

#include <Qt3DRender>
#include <Qt3DRender/QPaintedTextureImage>

class TextureImage : public Qt3DRender::QPaintedTextureImage
{
public:
    TextureImage(float);
    void setImage(QImage img);
    void setPixmap(QPixmap pix);
    void setOpacity(float opacity);
protected:
    void	paint(QPainter *painter);


private:
    QImage mImage;
    QPixmap mPixmap;
    float mOpacity;
    QPainter*   m_pPainter;
};

#endif // TEXTUREIMAGE_H
