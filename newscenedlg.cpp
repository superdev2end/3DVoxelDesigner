#include "newscenedlg.h"
#include "ui_newscenedlg.h"

NewSceneDlg::NewSceneDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSceneDlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->edtCubeSizeX->setValidator(new QIntValidator(1, 60, this));
    ui->edtCubeSizeY->setValidator(new QIntValidator(1, 60, this));
    ui->edtCubeSizeZ->setValidator(new QIntValidator(1, 60, this));
    ui->edtVertexInc->setValidator(new QIntValidator(0, 65535, this));
    ui->edtObjMove->setValidator(new QIntValidator(0, 65535, this));
}

NewSceneDlg::~NewSceneDlg()
{
    delete ui;
}

int NewSceneDlg::getCubeSizeX(){
    return ui->edtCubeSizeX->text().toInt();
}

int NewSceneDlg::getCubeSizeY(){
    return ui->edtCubeSizeY->text().toInt();
}

int NewSceneDlg::getCubeSizeZ(){
    return ui->edtCubeSizeZ->text().toInt();
}

int NewSceneDlg::getSpacingSize(){
    return ui->edtVertexInc->text().toInt();
}

int NewSceneDlg::getObjMovementField() {
    return ui->edtObjMove->text().toInt();
}

void NewSceneDlg::on_rad_8VertexInc_toggled(bool checked)
{
    if (checked == true) {
        ui->edtVertexInc->setText("8");
    }
}

void NewSceneDlg::on_rad_16VertexInc_toggled(bool checked)
{
    if (checked == true) {
        ui->edtVertexInc->setText("16");
    }
}

void NewSceneDlg::on_rad_32VertexInc_toggled(bool checked)
{
    if (checked == true) {
        ui->edtVertexInc->setText("32");
    }
}

void NewSceneDlg::on_rad_80VertexInc_toggled(bool checked)
{
    if (checked == true) {
        ui->edtVertexInc->setText("80");
    }
}

void NewSceneDlg::on_rad_43VertexInc_toggled(bool checked)
{
    if (checked == true) {
        ui->edtVertexInc->setText("43");
    }
}

void NewSceneDlg::on_rad_85VertexInc_toggled(bool checked)
{
    if (checked == true) {
        ui->edtVertexInc->setText("85");
    }
}
