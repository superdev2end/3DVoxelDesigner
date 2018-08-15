#include "voxel3dwidget.h"

Voxel3DWidget::Voxel3DWidget(QWidget *parent) : QWidget(parent)
{
    m_pRootEntity = nullptr;
    m_pView = new Custom3DWindow();
    setCameraPositionDefault();

    connect(camera(), SIGNAL(positionChanged(QVector3D)), this, SLOT(cameraPositionChanged(QVector3D)));
    connect(m_pView, SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(onMousePressEvent(QMouseEvent*)));
    connect(m_pView, SIGNAL(mouseReleaseSignal(QMouseEvent*)), this, SLOT(onMouseReleaseEvent(QMouseEvent*)));
    connect(m_pView, SIGNAL(mouseMoveSignal(QMouseEvent*)), this, SLOT(onMouseMoveEvent(QMouseEvent*)));
    connect(m_pView, SIGNAL(wheelSignal(QWheelEvent*)), this, SLOT(onWheelEvent(QWheelEvent*)));
    connect(m_pView, SIGNAL(keyPressSignal(QKeyEvent*)), this, SLOT(onKeyPressEvent(QKeyEvent*)));
    connect(m_pView, SIGNAL(keyReleaseSignal(QKeyEvent*)), this, SLOT(onKeyReleaseEvent(QKeyEvent*)));

    // put Qt3DWindow into a container in order to make it possible
    // to handle the view inside a widget
    m_pContainer = createWindowContainer(m_pView,this);

    m_off = false;
    m_zoom = 1.0f;
    m_bMousePressed = false;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAttribute(Qt::WA_NoSystemBackground);
}

Voxel3DWidget::~Voxel3DWidget()
{
    if(m_pContainer != nullptr)
        m_pContainer->deleteLater();

    if(m_pView != nullptr)
        m_pView->deleteLater();

    if(m_pLightTransform != nullptr)
        m_pLightTransform->deleteLater();

    m_pContainer = nullptr;
    m_pView = nullptr;
    m_pLightTransform = nullptr;
}

Qt3DRender::QCamera* Voxel3DWidget::camera()
{
    if(m_pView == nullptr)
        return nullptr;
    return m_pView->camera();
}

void Voxel3DWidget::setupRootEntity(Qt3DCore::QEntity* rootEntity)
{
    if(m_pView == nullptr)
        return;

    m_pRootEntity = rootEntity;
    m_pView->setRootEntity(rootEntity);

    // background color
    m_pView->defaultFrameGraph()->setClearColor(QColor(QRgb(0xf0f0f0))/*Qt::gray*/);
    m_pView->renderSettings()->pickingSettings()->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);
    m_pView->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::PrimitivePicking);
    m_pView->renderSettings()->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
    m_pView->renderSettings()->pickingSettings()->setWorldSpaceTolerance(1.0f);
    m_pView->renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    //////////////////////////////////////////////////////////////
    //================= Point Light =====================//
    QColor lightColor = Qt::white;//QColor(200, 200, 200);
    float lightIntensity = 1.0f;
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(rootEntity);
    light->setColor(lightColor);
    light->setIntensity(lightIntensity);

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    lightEntity->addComponent(light);
    m_pLightTransform = new Qt3DCore::QTransform(lightEntity);
    m_pLightTransform->setTranslation(camera()->position());
    lightEntity->addComponent(m_pLightTransform);

    ///////////////////////////////////////////////////////

//    Qt3DRender::QFrameGraphNode *framegraph = m_pView->activeFrameGraph();
//    Qt3DRender::QSortPolicy *sortPolicy = new Qt3DRender::QSortPolicy(m_pRootEntity);
//    framegraph->setParent(sortPolicy);
//    QVector<Qt3DRender::QSortPolicy::SortType> sortTypes = QVector<Qt3DRender::QSortPolicy::SortType>() << Qt3DRender::QSortPolicy::BackToFront;
//    sortPolicy->setSortTypes(sortTypes);
//    m_pView->setActiveFrameGraph(framegraph);

    BuildCustomFrameGraphRenderer();

}

void Voxel3DWidget::BuildCustomFrameGraphRenderer()
{
    m_pOpaqueLayer = new Qt3DRender::QLayer;
    m_pTransparentLayer = new Qt3DRender::QLayer;

    auto renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector(m_pRootEntity);
    renderSurfaceSelector->setSurface(m_pView);

    auto viewport = new Qt3DRender::QViewport(renderSurfaceSelector);
    auto cameraSelector = new Qt3DRender::QCameraSelector(viewport);

    auto clearBuffers = new Qt3DRender::QClearBuffers(cameraSelector);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    clearBuffers->setClearColor(Qt::white);

    cameraSelector->setCamera(camera());

    auto opaqueFilter = new Qt3DRender::QLayerFilter(camera());
    opaqueFilter->addLayer(m_pOpaqueLayer);

    Qt3DRender::QSortPolicy *sortPolicy = new Qt3DRender::QSortPolicy(cameraSelector);
    QVector< Qt3DRender::QSortPolicy::SortType> sorts;
    sorts.append(Qt3DRender::QSortPolicy::SortType::BackToFront);
    sortPolicy->setSortTypes(sorts);

    auto transparentFilter = new Qt3DRender::QLayerFilter(camera());
    transparentFilter->addLayer(m_pTransparentLayer);

    m_pView->renderSettings()->setActiveFrameGraph(renderSurfaceSelector);
}

void Voxel3DWidget::initWorkspace(int workspace_size, int spacing_size)
{
    m_nSpacingSize = spacing_size;
    m_workSpacePixelSize = (workspace_size - 1) * spacing_size;

    setCameraPositionDefault();

    m_pView->renderSettings()->pickingSettings()->setWorldSpaceTolerance(m_nSpacingSize / 10 + 1);
}

void Voxel3DWidget::setCameraPositionTop()
{
    camera()->setPosition(QVector3D(0, m_workSpacePixelSize * 2, 0));
    camera()->setUpVector(QVector3D(0, 0, 1));
    camera()->setViewCenter(QVector3D(0, 0, 0));
}

void Voxel3DWidget::setCameraPositionBottom()
{
    camera()->setPosition(QVector3D(0, -m_workSpacePixelSize * 2, 0));
    camera()->setUpVector(QVector3D(0, 0, 1));
    camera()->setViewCenter(QVector3D(0, 0, 0));
}

void Voxel3DWidget::setCameraPositionFront()
{
    camera()->setPosition(QVector3D(0, 0, m_workSpacePixelSize * 2));
    camera()->setUpVector(QVector3D(0, 1, 0));
    camera()->setViewCenter(QVector3D(0, 0, 0));
}

void Voxel3DWidget::setCameraPositionBack()
{
    camera()->setPosition(QVector3D(0, 0, -m_workSpacePixelSize * 2));
    camera()->setUpVector(QVector3D(0, 1, 0));
    camera()->setViewCenter(QVector3D(0, 0, 0));
}

void Voxel3DWidget::setCameraPositionLeft()
{
    camera()->setPosition(QVector3D(-m_workSpacePixelSize * 2, 0, 0));
    camera()->setUpVector(QVector3D(0, 1, 0));
    camera()->setViewCenter(QVector3D(0, 0, 0));
}

void Voxel3DWidget::setCameraPositionRight()
{
    camera()->setPosition(QVector3D(m_workSpacePixelSize * 2, 0, 0));
    camera()->setUpVector(QVector3D(0, 1, 0));
    camera()->setViewCenter(QVector3D(0, 0, 0));
}

void Voxel3DWidget::setCameraPositionDefault()
{
    camera()->setPosition(QVector3D(m_workSpacePixelSize * 1.2f, m_workSpacePixelSize * 1.4f, m_workSpacePixelSize * 2.0f));
    camera()->setUpVector(QVector3D(-0.269887f, 0.857422f, -0.438166f));
    camera()->setViewCenter(QVector3D(3.93608f, -0.700223f, 0.42894f));
}

void Voxel3DWidget::setCameraPerspectiveProjection()
{
    camera()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 100000000.0f);
}

void Voxel3DWidget::setCameraOrthographicProjection()
{
    qDebug() << "setCameraOrthographicProjection";
    if(m_pContainer == nullptr)
        return;

    float rate = 1.0f;
    if(m_pContainer->size().height() != 0)
        rate = static_cast<float>(m_pContainer->size().width()) / static_cast<float>(m_pContainer->size().height());

    camera()->lens()->setOrthographicProjection(-(m_workSpacePixelSize / 2 + 80) * rate / m_zoom,
                                                (m_workSpacePixelSize / 2 + 80) * rate / m_zoom,
                                                -(m_workSpacePixelSize / 2 + 80) / m_zoom,
                                                (m_workSpacePixelSize / 2 + 80) / m_zoom,
                                                0.1f,
                                                100000000.0f);

    m_off = false;
}

void Voxel3DWidget::setCameraOff()
{
    m_off = true;
    camera()->lens()->setOrthographicProjection(0,
                                                0,
                                                0,
                                                0,
                                                0.1f,
                                                100000.0f);
}

void Voxel3DWidget::updateLightPosition()
{
    qsrand(static_cast<quint64>(QTime::currentTime().msecsSinceStartOfDay()));
    m_vLightDiff = QVector3D(qrand() * 1000 / RAND_MAX , qrand() * 1000 / RAND_MAX , qrand() * 1000 / RAND_MAX );
    m_pLightTransform->setTranslation(camera()->position() + m_vLightDiff);
    qDebug() << "Updated Light Position : " << m_vLightDiff;
}

void Voxel3DWidget::resizeView(QSize size)
{
    qDebug() << "Resize 3D Widget";

    m_pContainer->resize(size);
    if(!m_off);
        setCameraOrthographicProjection();
}

void Voxel3DWidget::resizeEvent ( QResizeEvent * /*event*/ )
{
    qDebug() << this->size();

    resizeView(this->size());
}

void Voxel3DWidget::cameraPositionChanged(QVector3D cam_pos)
{
    m_pLightTransform->setTranslation(cam_pos + m_vLightDiff);
}

void Voxel3DWidget::onMousePressEvent(QMouseEvent* event)
{
    m_bMousePressed = true;
    m_posStart = event->pos();
    emit MousePressed();
}

void Voxel3DWidget::onMouseReleaseEvent(QMouseEvent */*event*/)
{
    qDebug() << "Mouse Released : Voxel3DWindget";

    m_bMousePressed = false;
    m_bArrowSelectedFlag = false;
    m_bMarkerSelectedFlag = false;
    emit MouseRelease();
}

void Voxel3DWidget::onMouseMoveEvent(QMouseEvent *event)
{
    if(m_pContainer == nullptr)
        return;
    if(!m_bMousePressed)
        return;

    if(event->buttons() == Qt::LeftButton)
    {
        if (m_bArrowSelectedFlag == true) {
            // Arrow Selected (Move the selected node)
            QPoint pos = event->pos();
            emit ArrowMoving(pos);
        } else if (m_bMarkerSelectedFlag == true) {
            // Marker Selected (Move the selected marker)
            QPoint pos = event->pos();
            emit MarkerMoving(pos);            
        } else {
            float rate = 1.0f;
            if(m_pContainer->size().height() != 0)
                rate = static_cast<float>(m_pContainer->size().width()) / static_cast<float>(m_pContainer->size().height());

            QPoint pos = event->pos();
            QVector3D cam_viewcenter = camera()->viewCenter();
            QVector3D cam_pos = camera()->position();

            float r = QVector3D(cam_pos - cam_viewcenter).length();
            float sense = 10.0f / m_zoom;
//            camera()->translate(QVector3D(-(pos.x() - m_posStart.x()) * rate * sense, (pos.y() - m_posStart.y()) * sense, 0));

//            float r_new = QVector3D(camera()->position() - cam_viewcenter).length();
//            camera()->setViewCenter(cam_viewcenter);
//            camera()->translate(QVector3D(0, 0, r_new - r));
//            camera()->setViewCenter(cam_viewcenter);
            QVector3D diffVector = QVector3D(-(pos.x() - m_posStart.x()) * rate * sense, (pos.y() - m_posStart.y()) * sense, 0);

            // Camera Rigth Vector
            double x = camera()->upVector().y() * camera()->viewVector().z() - camera()->upVector().z() * camera()->viewVector().y();
            double y = camera()->upVector().z() * camera()->viewVector().x() - camera()->upVector().x() * camera()->viewVector().z();
            double z = camera()->upVector().x() * camera()->viewVector().y() - camera()->upVector().y() * camera()->viewVector().x();
            double length = qSqrt(x*x + y*y + z*z);
            QVector3D rightVector(x, y, z);
            rightVector = rightVector / length;

            QQuaternion rotx = QQuaternion::fromAxisAndAngle(camera()->upVector(), diffVector.x() / 20);
            QQuaternion roty = QQuaternion::fromAxisAndAngle(rightVector, diffVector.y() / 20);
            camera()->rotateAboutViewCenter(roty * rotx);

            m_posStart = event->pos();
            emit MarkersRotating();
            emit MouseMove();
        }

    }else if(event->buttons() == Qt::RightButton)
    {
        float rate = 1.0f;
        if(m_pContainer->size().height() != 0)
            rate = static_cast<float>(m_pContainer->size().width()) / static_cast<float>(m_pContainer->size().height());

        QPoint pos = event->pos();
        float sense = 4.0f / m_zoom;
        float fSpacingRate = m_nSpacingSize * 0.25f / 77 + 5.7f / 77;   // 8 - > 0.1f, 85 -> 0.35f
        float fCubeRate = (m_workSpacePixelSize / m_nSpacingSize + 1) * 0.7f / 8 + 1.0f / 8;    // 2 -> 0.3f, 10 -> 1.0f
        camera()->translate(QVector3D(-(pos.x() - m_posStart.x()) * rate * sense * fSpacingRate * fCubeRate / 2 , (pos.y() - m_posStart.y()) * sense * fSpacingRate * fCubeRate / 2 , 0));
        m_posStart = event->pos();

        emit MarkersRotating();
        emit MouseMove();
    }
}

void Voxel3DWidget::onWheelEvent(QWheelEvent *event)
{
    float left = camera()->left();
    float right = camera()->right();
    float top = camera()->top();
    float bottom = camera()->bottom();
    float width_old = fabs(camera()->right() - camera()->left());

    float rate = 1.0f;
    if(m_pContainer->size().height() != 0)
        rate = static_cast<float>(m_pContainer->size().width()) / static_cast<float>(m_pContainer->size().height());

    float sense = 100.0f;
    if(event->modifiers() == Qt::ControlModifier)
        sense = sense / 6.0f;

    float new_left = left + m_nSpacingSize * event->delta() / sense * rate;
    float new_right = right - m_nSpacingSize * event->delta() / sense * rate;
    float new_top = top - m_nSpacingSize * event->delta() / sense;
    float new_bottom = bottom + m_nSpacingSize * event->delta() / sense;

    if((new_right - new_left) <= m_nSpacingSize * rate) {
        new_right = (right + left + m_nSpacingSize * rate) / 2;
        new_left = (right + left - m_nSpacingSize * rate) / 2;
    }

    if((new_top - new_bottom) <= m_nSpacingSize) {
        new_top = (top + bottom + m_nSpacingSize) / 2;
        new_bottom = (top + bottom - m_nSpacingSize) / 2;
    }

    float width_new = fabs(new_right - new_left);

    camera()->setLeft(new_left);
    camera()->setRight(new_right);
    camera()->setTop(new_top);
    camera()->setBottom(new_bottom);

    if(width_new != 0.0f)
        m_zoom = m_zoom * width_old / width_new;

    emit zoomChanged(m_zoom);
}

void Voxel3DWidget::onKeyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Alt) {
        // Alt -> Click Select Button
        emit triggerSelectReactorGuide(true);
    }
}

void Voxel3DWidget::onKeyReleaseEvent(QKeyEvent *event){
    if(event->matches(QKeySequence::Undo))
    {
        // Ctrl + Z
        emit undo();
    } else if(event->matches(QKeySequence::Redo)/* || (event->key() == Qt::Key_R && event->modifiers() == Qt::ControlModifier)*/)
    {
        // Shift + Ctrl + Z, /*Ctrl + R*/
        emit redo();
    } else if(event->key() == Qt::Key_T && event->modifiers() == Qt::ControlModifier) {
        // Ctrl + T
        emit selectTransparent();
    } else if(event->key() == Qt::Key_Up) {
        // UpArrow
        emit upOpacity();
    } else if(event->key() == Qt::Key_Down) {
        // DownArrow
        emit downOpacity();
    } else if(event->key() == Qt::Key_1 && event->modifiers() == Qt::ControlModifier) {
        // Control + 1
        emit topView();
    } else if(event->key() == Qt::Key_2 && event->modifiers() == Qt::ControlModifier) {
        // Control + 2
        emit bottomView();
    } else if(event->key() == Qt::Key_3 && event->modifiers() == Qt::ControlModifier) {
        // Control + 3
        emit rightView();
    } else if(event->key() == Qt::Key_4 && event->modifiers() == Qt::ControlModifier) {
        // Control + 4
        emit leftView();
    } else if(event->key() == Qt::Key_5 && event->modifiers() == Qt::ControlModifier) {
        // Control + 5
        emit frontView();
    } else if(event->key() == Qt::Key_6 && event->modifiers() == Qt::ControlModifier) {
        // Control + 6
        emit backView();
    } else if(event->key() == Qt::Key_7 && event->modifiers() == Qt::ControlModifier) {
        // Control + 7
        emit defaultView();
    } else if(event->key() == Qt::Key_L && event->modifiers() == Qt::ControlModifier) {
        // Ctrl + L
        emit triggerShowHide();
    } else if(event->key() == Qt::Key_R && event->modifiers() == Qt::ControlModifier) {
        // Ctrl + R
        emit triggerResetPoint();
    } else if(event->key() == Qt::Key_C && event->modifiers() == Qt::ShiftModifier) {
        // Shift + C
        emit triggerClearColor();
    } else if(event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier) {
        // Ctrl + C
        emit triggerColorVoxel();
    }  else if(event->key() == Qt::Key_Alt) {
        // Alt -> Click Select Button
        emit triggerSelectReactorGuide(false);
    }
}

bool Voxel3DWidget::getArrowSelectedFlag() {
    return m_bArrowSelectedFlag;
}

void Voxel3DWidget::setArrowSelectedFlag(bool bFlag) {
    m_bArrowSelectedFlag = bFlag;
}

bool Voxel3DWidget::getMarkerSelectedFlag() {
    return m_bMarkerSelectedFlag;
}

void Voxel3DWidget::setMarkerSelectedFlag(bool bFlag) {
    m_bMarkerSelectedFlag = bFlag;
}

float Voxel3DWidget::getZoom() {
    return m_zoom;
}

void Voxel3DWidget::setZoom(float zoom) {
    float left = camera()->left();
    float right = camera()->right();
    float top = camera()->top();
    float bottom = camera()->bottom();
    float width_old = fabs(camera()->right() - camera()->left());
    float width_new = width_old * m_zoom / zoom;
    float height_old = fabs(camera()->top() - camera()->bottom());
    float height_new = height_old * m_zoom / zoom;

    camera()->setLeft(left + (width_old - width_new) / 2);
    camera()->setRight(right - (width_old - width_new) / 2);
    camera()->setTop(top - (height_old - height_new) / 2);
    camera()->setBottom(bottom + (height_old - height_new) / 2);

    m_zoom = zoom;
    emit zoomChanged(m_zoom);
}

QPoint Voxel3DWidget::getPosStart() {
    return m_posStart;
}
