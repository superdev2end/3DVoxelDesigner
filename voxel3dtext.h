#ifndef VOXEL3DTEXT_H
#define VOXEL3DTEXT_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>

class Voxel3DText : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit Voxel3DText(Qt3DCore::QEntity *parent = nullptr);

    Qt3DExtras::QPhongMaterial* getMaterial();
    void setMaterial(Qt3DExtras::QPhongMaterial*);

    Qt3DExtras::QExtrudedTextMesh* getTextMesh();

    Qt3DCore::QTransform* getTransform();
    void setTransform(Qt3DCore::QTransform*);

private:
    Qt3DCore::QEntity*                  m_pRootEntity;
    Qt3DExtras::QPhongMaterial*         m_pMaterial;

    Qt3DExtras::QExtrudedTextMesh*      m_pTextMesh;
    Qt3DCore::QTransform*               m_pTextTransform;

};

#endif // VOXEL3DTEXT_H
