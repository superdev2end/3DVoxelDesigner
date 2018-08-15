#include "voxeloverlayimage.h"

VoxelOverlayImage::VoxelOverlayImage(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;

    // TextureImage Component
    m_pTextureMesh = new Qt3DExtras::QPlaneMesh();
    m_pTextureMesh->setWidth(80);
    m_pTextureMesh->setHeight(60);

    m_pTextureMaterial = new Qt3DExtras::QTextureMaterial();
    m_pTextureImage = new TextureImage(0.5f);
    m_pPixmap = new QPixmap();

    overlay_image = new QImage(768, 768, QImage::Format_ARGB32);

//    m_pTextureMaterial->texture()->addTextureImage(m_pTextureImage);
//    m_pTextureMaterial->setAlphaBlendingEnabled(true);

    // Transform Component
    m_pTextureTransform = new Qt3DCore::QTransform;
    m_pTextureTransform->setScale(10);

    addComponent(m_pTextureMaterial);
    addComponent(m_pTextureMesh);
    addComponent(m_pTextureTransform);

}

VoxelOverlayImage::~VoxelOverlayImage() {
    delete m_pTextureTransform;
    delete m_pPixmap;
    delete m_pTextureImage;
    delete m_pTextureMaterial;
    delete m_pTextureMesh;
}

void VoxelOverlayImage::setTextureSource(QUrl sourceUrl) {
    qDebug() << "Url: " << sourceUrl.path();
    m_pPixmap->load(sourceUrl.toLocalFile());
    bool loaded = overlay_image->load (sourceUrl.toLocalFile(),  0 );
    m_pTextureImage->setImage(*overlay_image);
    m_pTextureMaterial->texture()->addTextureImage(m_pTextureImage);
    m_pTextureMaterial->setAlphaBlendingEnabled(true);

    m_fWidth = 80.0f;
    m_fHeight = 60.0f;
    if (m_pPixmap->width() > m_pPixmap->height()) {
        m_fWidth = 80.0f;
        m_fHeight = m_fWidth * m_pPixmap->height() / m_pPixmap->width();
    } else {
        m_fHeight = 80.0f;
        m_fWidth = m_fHeight * m_pPixmap->width() / m_pPixmap->height();
    }

    m_pTextureMesh->setWidth(m_fWidth);
    m_pTextureMesh->setHeight(m_fHeight);

}

Qt3DCore::QTransform* VoxelOverlayImage::getTextureTransform() {
    return m_pTextureTransform;
}

void VoxelOverlayImage::setTextureTransform(Qt3DCore::QTransform* pTransform) {
    m_pTextureTransform = pTransform;
}

void VoxelOverlayImage::setOpacity(int nOpacity) {
    float fAlpha = nOpacity / 100.0f;
    m_pTextureMaterial->texture()->removeTextureImage(m_pTextureImage);
    delete m_pTextureImage;
    m_pTextureImage = new TextureImage(fAlpha);
    m_pTextureImage->setImage(*overlay_image);

    m_pTextureMaterial->texture()->addTextureImage(m_pTextureImage);
}

void VoxelOverlayImage::setZoom(float fZoom) {
    m_pTextureTransform->setScale(10 * m_fPercentSize * m_fScale / fZoom);
//        m_pCuboidMesh->setXExtent(160 * m_fPercentSize * m_fScale / fZoom);
//        m_pCuboidMesh->setZExtent(160 * m_fPercentSize * m_fScale / fZoom);
//        m_pTextureMesh->setWidth(m_fWidth * m_fPercentSize * m_fScale / fZoom);
//        m_pTextureMesh->setHeight(m_fHeight * m_fPercentSize * m_fScale / fZoom);
}

void VoxelOverlayImage::setPercentSize(int nSpacingSize) {
    m_fPercentSize = 0.7f / 72 * nSpacingSize + 16.0f / 72;
}

void VoxelOverlayImage::setScale(int nScale) {
    m_fScale = nScale / 100.0f;
}

void VoxelOverlayImage::setRotate(int nRotate) {
    m_nRotate = nRotate;
}

int VoxelOverlayImage::getRotate() {
    return m_nRotate;
}

void VoxelOverlayImage::setHorizontal(int nHorizontal) {
    m_nHorizontal = nHorizontal;
}

int VoxelOverlayImage::getHorizontal() {
    return m_nHorizontal;
}

void VoxelOverlayImage::setVertical(int nVertical) {
    m_nVertical = nVertical;
}

int VoxelOverlayImage::getVertical() {
    return m_nVertical;
}
