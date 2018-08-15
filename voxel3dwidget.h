#ifndef VOXEL3DWIDGET_H
#define VOXEL3DWIDGET_H

#include <QObject>
#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <QEntity>
#include <QMesh>
#include <QSceneLoader>
#include <Qt3DRender>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QPointLight>
#include <Qt3DInput/QInputAspect>
#include <Qt3DRender/QCamera>

#include <QAttribute>
#include <QBuffer>
#include <QDiffuseMapMaterial>
#include <QFirstPersonCameraController>
#include <QPhongMaterial>
#include <QTextureImage>
#include <Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>
#include <QFileInfo>
#include <QMessageBox>
#include <QMatrix4x4>
#include <QQuaternion>

#include "custom3dwindow.h"

class Voxel3DWidget : public QWidget
{
    Q_OBJECT
public:
    Voxel3DWidget(QWidget *parent = nullptr);
    ~Voxel3DWidget();

    void BuildCustomFrameGraphRenderer();
    void setupRootEntity(Qt3DCore::QEntity* rootEntity);
    void initWorkspace(int workspace_size, int spacing_size);
    Qt3DRender::QCamera* camera();

    void setCameraPositionTop();
    void setCameraPositionBottom();
    void setCameraPositionFront();
    void setCameraPositionBack();
    void setCameraPositionLeft();
    void setCameraPositionRight();
    void setCameraPositionDefault();

    void setCameraPerspectiveProjection();
    void setCameraOrthographicProjection();
    void setCameraOff();
    bool isCameraOff(){ return m_off; }
    void updateLightPosition();
    Qt3DRender::QLayer* getTransparentLayer(){ return m_pTransparentLayer; }
    Qt3DRender::QLayer* getOpaqueLayer(){ return m_pOpaqueLayer; }

    bool getArrowSelectedFlag();
    void setArrowSelectedFlag(bool bFlag);

    bool getMarkerSelectedFlag();
    void setMarkerSelectedFlag(bool bFlag);

    float   getZoom();
    void    setZoom(float);

    QPoint getPosStart();

signals:
    void undo();
    void redo();
    void selectTransparent();
    void upOpacity();
    void downOpacity();
    void ArrowMoving(QPoint);
    void MarkerMoving(QPoint);
    void MarkersRotating();
    void zoomChanged(float);
    void topView();
    void bottomView();
    void rightView();
    void leftView();
    void frontView();
    void backView();
    void defaultView();
    void triggerShowHide();
    void triggerResetPoint();
    void triggerColorVoxel();
    void triggerClearColor();
    void triggerSelectReactorGuide(bool);
    void MouseRelease();
    void MousePressed();
    void MouseMove();

protected:
    void resizeEvent(QResizeEvent* event);

public slots:
    void resizeView(QSize size);

private slots:
    void cameraPositionChanged(QVector3D cam_pos);

    void onMousePressEvent(QMouseEvent* event);
    void onMouseReleaseEvent(QMouseEvent *event);
    void onMouseMoveEvent(QMouseEvent *event);
    void onWheelEvent(QWheelEvent *event);
    void onKeyPressEvent(QKeyEvent *event);
    void onKeyReleaseEvent(QKeyEvent *event);
private:
    Qt3DCore::QEntity*                          m_pRootEntity;
    Qt3DRender::QLayer*                         m_pTransparentLayer;
    Qt3DRender::QLayer*                         m_pOpaqueLayer;

    QWidget*                                    m_pContainer;
    Custom3DWindow*                             m_pView;
    Qt3DCore::QTransform *                      m_pLightTransform;
    QVector3D                                   m_vLightDiff;
    int                                         m_workSpacePixelSize;

    QPoint                                      m_posStart;
    bool                                        m_off;

    float                                       m_zoom;

    bool                                        m_bArrowSelectedFlag;
    bool                                        m_bMarkerSelectedFlag;

    int                                         m_nSpacingSize;
    bool                                        m_bMousePressed;
};

#endif // VOXEL3DWIDGET_H
