#ifndef VOXELOVERLAYIMAGE_H
#define VOXELOVERLAYIMAGE_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>
#include <Qt3DRender>

#include "textureimage.h"

class VoxelOverlayImage : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    explicit VoxelOverlayImage(Qt3DCore::QEntity *parent = nullptr);
    ~VoxelOverlayImage();

    void                                setTextureSource(QUrl sourceUrl);
    Qt3DCore::QTransform*               getTextureTransform();
    void                                setTextureTransform(Qt3DCore::QTransform* pTransform);
    void                                setOpacity(int nOpacity);
    void                                setZoom(float fZoom);
    void                                setPercentSize(int nSpacingSize);
    void                                setScale(int nScale);
    void                                setRotate(int nRotate);
    int                                 getRotate();
    void                                setHorizontal(int nHorizontal);
    int                                 getHorizontal();
    void                                setVertical(int nHorizontal);
    int                                 getVertical();


private:
    Qt3DCore::QEntity*                  m_pRootEntity;

    Qt3DCore::QTransform*               m_pTextureTransform;
    Qt3DExtras::QPlaneMesh*             m_pTextureMesh;
    Qt3DExtras::QTextureMaterial*       m_pTextureMaterial;
    TextureImage*                       m_pTextureImage;
    QPixmap*                            m_pPixmap;
    QImage*                             overlay_image;

    bool                                m_bMirrored;

    float                               m_fPercentSize;
    float                               m_fScale;
    int                                 m_nRotate;
    int                                 m_nHorizontal;
    int                                 m_nVertical;

    float                               m_fWidth;
    float                               m_fHeight;

};

#endif // VOXELOVERLAYIMAGE_H
