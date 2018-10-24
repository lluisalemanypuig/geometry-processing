#include <QFileDialog>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

// PRIVATE

/* Menu buttons */

void MainWindow::on_action_Open_triggered() {

	QString filename = QFileDialog::getOpenFileName(this, tr("Open PLY"), ".", tr("*.ply"));
	if (current_tab == 0) {
		ui->SingleView_Renderer->load_mesh(filename);
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->load_mesh(filename);
		ui->DualView_RightRenderer->set_mesh(
			ui->DualView_LeftRenderer->get_mesh()
		);
	}
}

void MainWindow::on_action_Quit_triggered() {
	 QApplication::quit();
}

/* Render radio buttons */

void MainWindow::on_CBSolid_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::solid);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::solid);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::solid);
	}
}

void MainWindow::on_CBWireframe_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::wireframe);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::wireframe);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::wireframe);
	}
}

void MainWindow::on_CBRefLines_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::reflection_lines);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::reflection_lines);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::reflection_lines);
	}
}

/* Curvature radio buttons */

void MainWindow::on_RBCurvatureG_toggled(bool checked) {
	if (checked) {
		ui->SingleView_Renderer->set_curvature_display(curv_type::Gauss);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::Gauss);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::Gauss);
	}
}

void MainWindow::on_RBCurvatureH_toggled(bool checked) {
	if (checked) {
		ui->SingleView_Renderer->set_curvature_display(curv_type::Mean);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::Mean);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::Mean);
	}
}

void MainWindow::on_RBNoCurvature_toggled(bool checked) {
	if (checked) {
		ui->SingleView_Renderer->set_curvature_display(curv_type::none);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::none);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::none);
	}
}

/* Smoothing radio buttons */

void MainWindow::on_RB_Laplacian_toggled(bool checked) {
	if (checked) {
		ui->DualView_RightRenderer->set_smoothing_operator(smoothing_operator::Laplacian);
	}
}

void MainWindow::on_RB_BiLaplacian_toggled(bool checked) {
	if (checked) {
		ui->DualView_RightRenderer->set_smoothing_operator(smoothing_operator::biLaplacian);
	}
}

void MainWindow::on_RB_Taubin_toggled(bool checked) {
	if (checked) {
		ui->DualView_RightRenderer->set_smoothing_operator(smoothing_operator::Taubin);
	}
}

void MainWindow::on_comboBox_currentIndexChanged(const QString& arg1) {
	if (arg1 == "Uniform") {
		ui->DualView_RightRenderer->set_weight_type(smooth_weight::uniform);
	}
	else if (arg1 == "Cotangent") {
		ui->DualView_RightRenderer->set_weight_type(smooth_weight::cotangent);
	}
}

void MainWindow::on_PB_RunSmooth_clicked() {
	size_t nit = ui->lE_Iter->text().toInt();
	float lambda = ui->lE_Lambda->text().toFloat();

	ui->DualView_RightRenderer->set_n_iterations(nit);
	ui->DualView_RightRenderer->set_lambda(lambda);
	ui->DualView_RightRenderer->run_smoothing_algorithm();
}

/* Performance options */

void MainWindow::on_SBThreads_valueChanged(int val) {
	size_t n_threads = static_cast<size_t>(val);
	ui->SingleView_Renderer->set_num_threads(n_threads);
}

void MainWindow::on_PB_ClearMesh_clicked() {
	if (current_tab == 0) {
		ui->SingleView_Renderer->clear_mesh();
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->clear_mesh();
		ui->DualView_RightRenderer->clear_mesh();
	}
}

/* Tab widget stuff */

void MainWindow::on_tabWidget_2_currentChanged(int index) {
	current_tab = index;
}

// PUBLIC

MainWindow::MainWindow(QWidget *parent)
:
	QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->SingleView_Renderer->set_num_threads(1);
	current_tab = 1;
}

MainWindow::~MainWindow() {
	delete ui;
}
