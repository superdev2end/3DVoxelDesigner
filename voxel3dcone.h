#ifndef VOXEL3DCONE_H
#define VOXEL3DCONE_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>

#include "custommeshgeometry.h"

class Voxel3DCone : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit Voxel3DCone(Qt3DCore::QEntity *parent = nullptr);

    Qt3DExtras::QPhongMaterial* getMaterial();
    CustomeMeshGeometry* getGeometry();
    Qt3DExtras::QConeMesh* getConeMesh();
    Qt3DCore::QTransform* getTransform();
signals:

public slots:

private:
    Qt3DCore::QEntity*                  m_pRootEntity;
    Qt3DExtras::QPhongMaterial*         m_pMaterial;
    CustomeMeshGeometry*                m_pGeometry;

    Qt3DExtras::QConeMesh*              m_pConeMesh;
    Qt3DCore::QTransform*               m_pConeTransform;

};

#endif // VOXEL3DCONE_H
