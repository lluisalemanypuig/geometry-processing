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

void MainWindow::on_checkBoxFill_toggled(bool checked) {
	 ui->openGLWidget->set_polygon_mode(checked);
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


