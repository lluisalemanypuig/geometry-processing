#include <QFileDialog>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

// PRIVATE

void MainWindow::on_action_Open_triggered() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open PLY"), ".", tr("*.ply"));
	ui->openGLWidget->load_mesh(filename);
}

void MainWindow::on_action_Quit_triggered() {
	 QApplication::quit();
}

void MainWindow::on_cBFill_toggled(bool checked) {
	 ui->openGLWidget->set_polygon_mode(checked);
}

void MainWindow::on_rBCurvatureG_toggled(bool checked) {
	if (checked) {
		ui->openGLWidget->set_curvature_display(curvature::Gauss);
	}
}

void MainWindow::on_rBCurvatureH_toggled(bool checked) {
	if (checked) {
		ui->openGLWidget->set_curvature_display(curvature::Mean);
	}
}

void MainWindow::on_rBNoCurvature_toggled(bool checked) {
	if (checked) {
		ui->openGLWidget->set_curvature_display(curvature::none);
	}
}

// PUBLIC

MainWindow::MainWindow(QWidget *parent)
:
	QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow() {
	delete ui;
}


