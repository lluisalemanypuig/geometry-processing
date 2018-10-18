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

void MainWindow::on_CBSolid_toggled(bool toggled) {
	if (toggled) {
		ui->openGLWidget->set_polygon_mode(polymode::solid);
	}
}

void MainWindow::on_CBWireframe_toggled(bool toggled) {
	if (toggled) {
		ui->openGLWidget->set_polygon_mode(polymode::wireframe);
	}
}

void MainWindow::on_CBRefLines_toggled(bool toggled) {
	if (toggled) {
		ui->openGLWidget->set_polygon_mode(polymode::reflection_lines);
	}
}

void MainWindow::on_RBCurvatureG_toggled(bool checked) {
	if (checked) {
		ui->openGLWidget->set_curvature_display(curvature::Gauss);
	}
}

void MainWindow::on_RBCurvatureH_toggled(bool checked) {
	if (checked) {
		ui->openGLWidget->set_curvature_display(curvature::Mean);
	}
}

void MainWindow::on_RBNoCurvature_toggled(bool checked) {
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

