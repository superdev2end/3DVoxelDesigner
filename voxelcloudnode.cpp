#include "voxelcloudnode.h"

VoxelCloudNode::VoxelCloudNode(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;
    // Child Entities
    m_pChildEntity_selected = new Qt3DCore::QEntity(this);
    m_pChildEntity_visible = new Qt3DCore::QEntity(this);
    m_pChildEntity_unvisible = new Qt3DCore::QEntity(this);
    m_pChildEntity_visible_deactive = new Qt3DCore::QEntity(this);
    m_pChildEntity_unvisible_deactive = new Qt3DCore::QEntity(this);
    m_pChildEntity_reactor_candidates = new Qt3DCore::QEntity(this);
    m_pChildEntity_Straight = new Qt3DCore::QEntity(this);

    // Mesh Geometries
    Qt3DRender::QGeometryRenderer *customMeshRenderer1 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_visible);
    customMeshRenderer1->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    m_pGeometry_visible = new CustomeMeshGeometry(m_pChildEntity_visible);
    customMeshRenderer1->setGeometry(m_pGeometry_visible);
    m_pChildEntity_visible->addComponent(customMeshRenderer1);

    Qt3DRender::QGeometryRenderer *customMeshRenderer2 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_unvisible);
    customMeshRenderer2->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    m_pGeometry_unvisible = new CustomeMeshGeometry(m_pChildEntity_unvisible);
    customMeshRenderer2->setGeometry(m_pGeometry_unvisible);
    m_pChildEntity_unvisible->addComponent(customMeshRenderer2);

    Qt3DRender::QGeometryRenderer *customMeshRenderer3 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_selected);
    customMeshRenderer3->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    m_pGeometry_selected = new CustomeMeshGeometry(m_pChildEntity_selected);
    customMeshRenderer3->setGeometry(m_pGeometry_selected);
    m_pChildEntity_selected->addComponent(customMeshRenderer3);

    Qt3DRender::QGeometryRenderer *customMeshRenderer4 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_visible_deactive);
    customMeshRenderer4->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    m_pGeometry_visible_deactive = new CustomeMeshGeometry(m_pChildEntity_visible_deactive);
    customMeshRenderer4->setGeometry(m_pGeometry_visible_deactive);
    m_pChildEntity_visible_deactive->addComponent(customMeshRenderer4);

    Qt3DRender::QGeometryRenderer *customMeshRenderer5 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_unvisible_deactive);
    customMeshRenderer5->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    m_pGeometry_unvisible_deactive = new CustomeMeshGeometry(m_pChildEntity_unvisible_deactive);
    customMeshRenderer5->setGeometry(m_pGeometry_unvisible_deactive);
    m_pChildEntity_unvisible_deactive->addComponent(customMeshRenderer5);

    Qt3DRender::QGeometryRenderer *customMeshRenderer6 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_reactor_candidates);
    customMeshRenderer6->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    m_pGeometry_reactor_candidates = new CustomeMeshGeometry(m_pChildEntity_reactor_candidates);
    customMeshRenderer6->setGeometry(m_pGeometry_reactor_candidates);
    m_pChildEntity_reactor_candidates->addComponent(customMeshRenderer6);

    Qt3DRender::QGeometryRenderer *customMeshRenderer7 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_Straight);
    customMeshRenderer7->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    m_pGeometry_Straight = new CustomeMeshGeometry(m_pChildEntity_Straight);
    customMeshRenderer7->setGeometry(m_pGeometry_Straight);
    m_pChildEntity_Straight->addComponent(customMeshRenderer7);

    m_pointSize = 5.0f;

    // Material Components
    m_pMaterial_visible = new Qt3DExtras::QPhongMaterial(m_pChildEntity_visible);
    m_pMaterial_visible->setAmbient(Qt::blue);

    auto effect1 = m_pMaterial_visible->effect();
    for (auto t : effect1->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(m_pointSize);
            rp->addRenderState(pointSize);
        }
    }
    m_pChildEntity_visible->addComponent(m_pMaterial_visible);

    m_pMaterial_unvisible = new Qt3DExtras::QPhongAlphaMaterial(m_pChildEntity_unvisible);
    m_pMaterial_unvisible->setAmbient(QColor(245, 245, 245));
    auto effect2 = m_pMaterial_unvisible->effect();
    for (auto t : effect2->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(m_pointSize);
            rp->addRenderState(pointSize);
        }
    }
    m_pChildEntity_unvisible->addComponent(m_pMaterial_unvisible);

    m_pMaterial_selected = new Qt3DExtras::QPhongMaterial(m_pChildEntity_selected);
    m_pMaterial_selected->setAmbient(QColor(255,100, 100));
    auto effect3 = m_pMaterial_selected->effect();
    for (auto t : effect3->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(m_pointSize * 1.4f);
            rp->addRenderState(pointSize);
        }
    }
    m_pChildEntity_selected->addComponent(m_pMaterial_selected);

    m_pMaterial_visible_deactive = new Qt3DExtras::QPhongAlphaMaterial(m_pChildEntity_visible_deactive);
    m_pMaterial_visible_deactive->setAmbient(Qt::blue);
    m_pMaterial_visible_deactive->setAlpha(0.1f);
    auto effect4 = m_pMaterial_visible_deactive->effect();
    for (auto t : effect4->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(m_pointSize);
            rp->addRenderState(pointSize);
        }
    }
    m_pChildEntity_visible_deactive->addComponent(m_pMaterial_visible_deactive);

    m_pMaterial_unvisible_deactive = new Qt3DExtras::QPhongAlphaMaterial(m_pChildEntity_unvisible_deactive);
    m_pMaterial_unvisible_deactive->setAmbient(QColor(245, 245, 245));
    m_pMaterial_unvisible_deactive->setAlpha(0.1f);
    auto effect5 = m_pMaterial_unvisible_deactive->effect();
    for (auto t : effect5->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(m_pointSize);
            rp->addRenderState(pointSize);
        }
    }
    m_pChildEntity_unvisible_deactive->addComponent(m_pMaterial_unvisible_deactive);

    m_pMaterial_reactor_candidates = new Qt3DExtras::QPhongAlphaMaterial(m_pChildEntity_reactor_candidates);
    m_pMaterial_reactor_candidates->setAmbient(Qt::green);
    m_pMaterial_reactor_candidates->setAlpha(1.0f);
    auto effect6 = m_pMaterial_reactor_candidates->effect();
    for (auto t : effect6->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(m_pointSize * 1.8f);
            rp->addRenderState(pointSize);
        }
    }
    m_pChildEntity_reactor_candidates->addComponent(m_pMaterial_reactor_candidates);

    m_pMaterial_Straight = new Qt3DExtras::QPhongAlphaMaterial(m_pChildEntity_Straight);
    m_pMaterial_Straight->setAmbient(Qt::red);
    m_pMaterial_Straight->setAlpha(1.0f);
    auto effect7 = m_pMaterial_Straight->effect();
    for (auto t : effect7->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(m_pointSize * 1.8f);
            rp->addRenderState(pointSize);
        }
    }
    m_pChildEntity_Straight->addComponent(m_pMaterial_Straight);

    // Picker Components
    m_pPicker = new Qt3DRender::QObjectPicker(this);
    QObject::connect(m_pPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(onPressed(Qt3DRender::QPickEvent*)));
    QObject::connect(m_pPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(onClicked(Qt3DRender::QPickEvent*)));
    this->addComponent(m_pPicker);
}

void VoxelCloudNode::initialize(int sizeX, int sizeY, int sizeZ, int spaceSize)
{
    m_workspaceSizeX = sizeX;
    m_workspaceSizeY = sizeY;
    m_workspaceSizeZ = sizeZ;
    m_spacingSize = spaceSize;

    for(int x = 0 ; x < m_workspaceSizeX; x ++)
    {
        for(int y = 0 ; y < m_workspaceSizeY; y ++)
        {
            for(int z = 0 ; z < m_workspaceSizeZ; z ++)
            {
                QVector3D pos = (QVector3D(x , y, z) - QVector3D(static_cast<float>(m_workspaceSizeX - 1) / 2.0f,
                                                                 static_cast<float>(m_workspaceSizeY - 1) / 2.0f,
                                                                 static_cast<float>(m_workspaceSizeZ - 1) / 2.0f)) * m_spacingSize;
                m_vertics_visible << pos;
                m_vertics_unvisible << pos;
                m_vertics_selected << pos;
                m_vertics_visible_deactive << pos;
                m_vertics_unvisible_deactive << pos;
                m_vertics_reactor_candidates << pos;
                m_vertics_Straight << pos;
            }
        }
    }

    for (int x = 0; x < m_workspaceSizeX; x++) {
        for (int y = 0; y < m_workspaceSizeY; y++) {
            for( int z = 0; z < m_workspaceSizeZ; z++){
                int i = z + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ;

                int tmp_cnt = 0;
                tmp_cnt = x == 0 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = y == 0 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = z == 0 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = x == m_workspaceSizeX - 1 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = y == m_workspaceSizeY - 1 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = z == m_workspaceSizeZ - 1 ? tmp_cnt + 1: tmp_cnt;

                // Lines of Cube is visible
                if(tmp_cnt >= 2)
                {
                    m_indices_visible << i;
                }else{
                    m_indices_unvisible << i;
                }

            }
        }
    }
    m_pGeometry_visible->setVertics(m_vertics_visible, m_indices_visible);
    m_pGeometry_unvisible->setVertics(m_vertics_unvisible, m_indices_unvisible);
    m_pGeometry_selected->setVertics(m_vertics_selected);
    m_pGeometry_visible_deactive->setVertics(m_vertics_visible_deactive);
    m_pGeometry_unvisible_deactive->setVertics(m_vertics_unvisible_deactive);
    m_pGeometry_reactor_candidates->setVertics(m_vertics_reactor_candidates);
    m_pGeometry_Straight->setVertics(m_vertics_Straight);
}

void VoxelCloudNode::updateVertex(int x, int y, int z, QVector3D pos)
{
    int i = z + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ;
    if(i >= m_vertics_visible.length())
        return;

    m_vertics_visible.replace(i, pos);
    m_vertics_unvisible.replace(i, pos);
    m_vertics_selected.replace(i, pos);
    m_vertics_visible_deactive.replace(i, pos);
    m_vertics_unvisible_deactive.replace(i, pos);
    m_vertics_reactor_candidates.replace(i, pos);
    m_vertics_Straight.replace(i, pos);

    m_pGeometry_visible->setVertics(m_vertics_visible);
    m_pGeometry_unvisible->setVertics(m_vertics_unvisible);
    m_pGeometry_selected->setVertics(m_vertics_selected);
    m_pGeometry_visible_deactive->setVertics(m_vertics_visible_deactive);
    m_pGeometry_unvisible_deactive->setVertics(m_vertics_unvisible_deactive);
    m_pGeometry_reactor_candidates->setVertics(m_vertics_reactor_candidates);
    m_pGeometry_Straight->setVertics(m_vertics_Straight);
}

void VoxelCloudNode::setVisible(int index_x, int index_y, int index_z)
{
    int index = index_z + index_y * m_workspaceSizeZ + index_x * m_workspaceSizeY * m_workspaceSizeZ;
    foreach(int item, m_indices_visible){
        if(item == index){
            m_indices_visible.removeAll(item);
        }
    }

    foreach(int item, m_indices_unvisible){
        if(item == index){
            m_indices_unvisible.removeAll(item);
        }
    }

    foreach(int item, m_indices_visible_deactive){
        if(item == index){
            m_indices_visible_deactive.removeAll(item);
        }
    }

    m_indices_visible << index;

    m_pGeometry_visible->setIndices(m_indices_visible);
    m_pGeometry_unvisible->setIndices(m_indices_unvisible);
    m_pGeometry_visible_deactive->setIndices(m_indices_visible_deactive);
}

void VoxelCloudNode::setUnvisible(int index_x, int index_y, int index_z)
{
    int index = index_z + index_y * m_workspaceSizeZ + index_x * m_workspaceSizeY * m_workspaceSizeZ;
    foreach(int item, m_indices_visible){
        if(item == index){
            m_indices_visible.removeAll(item);
        }
    }

    foreach(int item, m_indices_unvisible){
        if(item == index){
            m_indices_unvisible.removeAll(item);
        }
    }

    foreach(int item, m_indices_unvisible_deactive){
        if(item == index){
            m_indices_unvisible_deactive.removeAll(item);
        }
    }

    m_indices_unvisible << index;

    m_pGeometry_visible->setIndices(m_indices_visible);
    m_pGeometry_unvisible->setIndices(m_indices_unvisible);
    m_pGeometry_unvisible_deactive->setIndices(m_indices_unvisible_deactive);
}

void VoxelCloudNode::addSelectedVoxel(int index_x, int index_y, int index_z)
{
    int index = index_z + index_y * m_workspaceSizeZ + index_x * m_workspaceSizeY * m_workspaceSizeZ;

//    qDebug() << "Selected : " << index_x << "," << index_y << "," << index_z;
    m_indices_selected << index;

    m_pGeometry_selected->setIndices(m_indices_selected);

}

void VoxelCloudNode::clearSelectedVoxels()
{
    foreach(int item, m_indices_selected){
        m_indices_selected.removeAll(item);
    }

    m_indices_selected.clear();
    m_pGeometry_selected->setIndices(m_indices_selected);
}

void VoxelCloudNode::setVisibleDeactive(int index_x, int index_y, int index_z)
{
    int index = index_z + index_y * m_workspaceSizeZ + index_x * m_workspaceSizeY * m_workspaceSizeZ;
    foreach(int item, m_indices_visible){
        if(item == index){
            m_indices_visible.removeAll(item);
        }
    }

    m_indices_visible_deactive << index;

    m_pGeometry_visible->setIndices(m_indices_visible);
    m_pGeometry_visible_deactive->setIndices(m_indices_visible_deactive);
}

void VoxelCloudNode::setUnvisibleDeactive(int index_x, int index_y, int index_z)
{
    int index = index_z + index_y * m_workspaceSizeZ + index_x * m_workspaceSizeY * m_workspaceSizeZ;
    foreach(int item, m_indices_unvisible){
        if(item == index){
            m_indices_unvisible.removeAll(item);
        }
    }

    m_indices_unvisible_deactive << index;

    m_pGeometry_unvisible->setIndices(m_indices_unvisible);
    m_pGeometry_unvisible_deactive->setIndices(m_indices_unvisible_deactive);
}

void VoxelCloudNode::setReactorCandidate(int index_x, int index_y, int index_z)
{
    int index = index_z + index_y * m_workspaceSizeZ + index_x * m_workspaceSizeY * m_workspaceSizeZ;

//    qDebug() << "Selected : " << index_x << "," << index_y << "," << index_z;
    m_indices_reactor_candidates << index;

    m_pGeometry_reactor_candidates->setIndices(m_indices_reactor_candidates);

}

void VoxelCloudNode::clearReactorCandidates()
{
    foreach(int item, m_indices_reactor_candidates){
        m_indices_reactor_candidates.removeAll(item);
    }

    m_indices_reactor_candidates.clear();
    m_pGeometry_reactor_candidates->setIndices(m_indices_reactor_candidates);
}

void VoxelCloudNode::setStraightEdge(int index_x, int index_y, int index_z)
{
    int index = index_z + index_y * m_workspaceSizeZ + index_x * m_workspaceSizeY * m_workspaceSizeZ;

//    qDebug() << "Selected : " << index_x << "," << index_y << "," << index_z;
    m_indices_Straight << index;

    m_pGeometry_Straight->setIndices(m_indices_Straight);

}

void VoxelCloudNode::clearStraightEdges()
{
    foreach(int item, m_indices_Straight){
        m_indices_Straight.removeAll(item);
    }

    m_indices_Straight.clear();
    m_pGeometry_Straight->setIndices(m_indices_Straight);
}

void VoxelCloudNode::onPressed(Qt3DRender::QPickEvent* evt){
    qDebug() << "Node Pressed";
    if(evt->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    int x_min, y_min, z_min, index_min;
    x_min = y_min = z_min = index_min = -1;
    float len_min = 99999999.0f;

    QVector3D cameraPos = m_pCamera->position();
    QRect viewRect(m_pMainView->rect().left(), m_pMainView->rect().top(), m_pMainView->rect().width() * m_pMainView->getZoom(), m_pMainView->rect().height() * m_pMainView->getZoom());
    QVector3D posEvt = evt->worldIntersection().project(m_pCamera->viewMatrix(), m_pCamera->projectionMatrix(), viewRect);

    for (int x = 0; x < m_workspaceSizeX; x++) {
        for (int y = 0; y < m_workspaceSizeY; y++) {
            for( int z = 0; z < m_workspaceSizeZ; z++) {
                int i = z + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ;

                if (m_indices_unvisible_deactive.indexOf(i) > -1) {
                    continue;
                }

                QVector3D posNode = m_vertics_visible.at(i).project(m_pCamera->viewMatrix(), m_pCamera->projectionMatrix(), viewRect);
                if((posNode - posEvt).length() < m_spacingSize * m_pMainView->getZoom()) {
                    if((posNode - posEvt).length()* 100000 + (m_vertics_visible.at(i)-cameraPos).length() < len_min){
                        x_min = x;
                        y_min = y;
                        z_min = z;
                        len_min = (posNode - posEvt).length()* 100000 + (m_vertics_visible.at(i)-cameraPos).length();
                        index_min = i;
                    }
                }
            }
        }
    }

    qDebug() << "Pressed Index : " << index_min;
    if (index_min == -1){
        m_pressedIndex = -1;
        return;
    }

    m_nMinX = x_min;
    m_nMinY = y_min;
    m_nMinZ = z_min;
    m_pressedIndex = index_min;
    m_LastCameraPos = cameraPos;

}

void VoxelCloudNode::onClicked(Qt3DRender::QPickEvent* evt){

    qDebug() << "Node Clicked";
    if(m_pressedIndex < 0)
        return;

    qDebug() << "Index : " << m_pressedIndex;

    if (m_LastCameraPos != m_pCamera->position()) {
        qDebug() << "Camera position is different";
        return;
    }

    if(evt->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    if(evt->modifiers() == Qt::ControlModifier)
    {
        voxelCtrlClicked(m_nMinX, m_nMinY, m_nMinZ, m_vertics_visible.at(m_pressedIndex));
    }else if(evt->modifiers() == Qt::ShiftModifier)
    {
        emit voxelShiftClicked(m_nMinX, m_nMinY, m_nMinZ, m_vertics_visible.at(m_pressedIndex));
    }else{
        emit voxelClicked(m_nMinX, m_nMinY, m_nMinZ, m_vertics_visible.at(m_pressedIndex));
    }
    m_pressedIndex = -1;
}

void VoxelCloudNode::setCamera(Qt3DRender::QCamera* pCamera) {
    m_pCamera = pCamera;
}

void VoxelCloudNode::setMainView(Voxel3DWidget *pMainView) {
    m_pMainView = pMainView;
}
