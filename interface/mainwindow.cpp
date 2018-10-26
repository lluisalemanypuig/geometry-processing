#include <QFileDialog>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

// PRIVATE

void MainWindow::renderer_change_poly_mode() {
	if (current_tab == 0) {
		ui->SingleView_Renderer->change_polygon_mode();
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->change_polygon_mode();
		ui->DualView_RightRenderer->change_polygon_mode();
	}
}

void MainWindow::renderer_change_curvature() {
	if (current_tab == 0) {
		ui->SingleView_Renderer->change_curvature_display();
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->change_curvature_display();
		ui->DualView_RightRenderer->change_curvature_display();
	}
}

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
	renderer_change_poly_mode();
}

void MainWindow::on_CBWireframe_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::wireframe);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::wireframe);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::wireframe);
	}
	renderer_change_poly_mode();
}

void MainWindow::on_CBRefLines_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::reflection_lines);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::reflection_lines);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::reflection_lines);
	}
	renderer_change_poly_mode();
}

/* Curvature radio buttons */

void MainWindow::on_RBCurvatureG_toggled(bool checked) {
	if (checked) {
		ui->SingleView_Renderer->set_curvature_display(curv_type::Gauss);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::Gauss);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::Gauss);
		renderer_change_curvature();
	}
}

void MainWindow::on_RBCurvatureH_toggled(bool checked) {
	if (checked) {
		ui->SingleView_Renderer->set_curvature_display(curv_type::Mean);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::Mean);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::Mean);
		renderer_change_curvature();
	}
}

void MainWindow::on_RBNoCurvature_toggled(bool checked) {
	if (checked) {
		ui->SingleView_Renderer->set_curvature_display(curv_type::none);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::none);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::none);
		renderer_change_curvature();
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

void MainWindow::on_PB_RunSmooth_clicked() {
	size_t nit = ui->lE_Iter->text().toInt();
	float lambda = ui->lE_Lambda->text().toFloat();

	ui->DualView_RightRenderer->set_n_iterations(nit);
	ui->DualView_RightRenderer->set_lambda(lambda);
	ui->DualView_RightRenderer->run_smoothing_algorithm();
}

void MainWindow::on_PB_ResetSmooth_clicked() {
	ui->DualView_RightRenderer->set_mesh(
		ui->DualView_LeftRenderer->get_mesh()
	);
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

void MainWindow::on_TW_View_currentChanged(int index) {
	current_tab = index;
	renderer_change_poly_mode();
	renderer_change_curvature();
}

/* Settings stuff */

void MainWindow::on_comboBox_currentIndexChanged(const QString& arg1) {
	if (arg1 == "Uniform") {
		ui->DualView_RightRenderer->set_weight_type(smooth_weight::uniform);
	}
	else if (arg1 == "Cotangent") {
		ui->DualView_RightRenderer->set_weight_type(smooth_weight::cotangent);
	}
}

// PUBLIC

MainWindow::MainWindow(QWidget *parent)
:
	QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	current_tab = ui->TW_View->currentIndex();
	ui->SingleView_Renderer->set_num_threads(1);
	ui->DualView_LeftRenderer->set_twin( ui->DualView_RightRenderer );
	ui->DualView_RightRenderer->set_twin( ui->DualView_LeftRenderer );
}

MainWindow::~MainWindow() {
	delete ui;
}
