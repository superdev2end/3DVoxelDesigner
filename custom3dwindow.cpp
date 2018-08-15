#include "custom3dwindow.h"
#include <QDate>
Custom3DWindow::Custom3DWindow()
{

}

void Custom3DWindow::mousePressEvent(QMouseEvent *event)
{
    emit mousePressSignal(event);
}

void Custom3DWindow::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleaseSignal(event);
}

void Custom3DWindow::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoveSignal(event);
}

void Custom3DWindow::wheelEvent(QWheelEvent *event)
{
    emit wheelSignal(event);
}

void Custom3DWindow::keyPressEvent(QKeyEvent *event)
{
    emit keyPressSignal(event);
}

void Custom3DWindow::keyReleaseEvent(QKeyEvent *event)
{
    emit keyReleaseSignal(event);
}
