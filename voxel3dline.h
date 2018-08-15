#ifndef VOXEL3DLINE_H
#define VOXEL3DLINE_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>

#include "custommeshgeometry.h"

class Voxel3DLine : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit Voxel3DLine(Qt3DCore::QEntity* parent);

    void setPoints(QVector3D, QVector3D);
    void setColor(QColor color);
    void setAlpha(float a);
signals:

public slots:

private:
    Qt3DCore::QEntity*                  m_pRootEntity;
    Qt3DExtras::QPhongAlphaMaterial*    m_pMaterial;

    CustomeMeshGeometry*                m_pGeometry;
};

#endif // VOXEL3DLINE_H
