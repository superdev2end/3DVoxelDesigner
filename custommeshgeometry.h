#ifndef CUSTOMEMESHGEOMETRY_H
#define CUSTOMEMESHGEOMETRY_H


#include <Qt3DCore/QNode>
#include <Qt3DRender/QBuffer>

#include <QVector3D>

#include <Qt3DRender/QGeometry>

class CustomeMeshGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT

    Q_PROPERTY(int count READ vertexCount NOTIFY countChanged)

public:
    CustomeMeshGeometry( Qt3DCore::QNode *parent = nullptr );

    int vertexCount();

    void setVertics( const QVector<QVector3D> &vertices);
    void setVertics( const QVector<QVector3D> &vertices, const QVector<int> &indices );
    void setIndices(const QVector<int> &indices );

signals:
    void countChanged(int count);

private:
    Qt3DRender::QAttribute *mPositionAttribute = nullptr;
    Qt3DRender::QAttribute *mIndexAttribute = nullptr;
    Qt3DRender::QBuffer *mVertexBuffer = nullptr;
    Qt3DRender::QBuffer *mIndexBuffer = nullptr;

    int mVertexCount = 0;
    int mIndexCount = 0;
};

#endif // CUSTOMEMESHGEOMETRY_H
