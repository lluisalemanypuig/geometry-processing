#include "mainwindow.hpp"

// C++ includes
#include <iostream>
using namespace std;

// geoproc includes
#include <geoproc/smoothing/smoothing_defs.hpp>
using namespace geoproc;

// Qt includes
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>
#include "ui_mainwindow.h"

// PRIVATE

void MainWindow::change_poly_mode() {
	if (current_tab == 0) {
		ui->SingleView_Renderer->change_polygon_mode();
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->change_polygon_mode();
		ui->DualView_RightRenderer->change_polygon_mode();
	}
}

void MainWindow::change_curvature() {
	if (current_tab == 0) {
		ui->SingleView_Renderer->change_curvature_display();
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->change_curvature_display();
		ui->DualView_RightRenderer->change_curvature_display();
	}
}

void MainWindow::change_curvature_prop_display(float p) {
	if (current_tab == 0) {
		ui->SingleView_Renderer->change_display_curvature_proportion(p);
		ui->DualView_LeftRenderer->change_curvature_proportion(p);
		ui->DualView_RightRenderer->change_curvature_proportion(p);
	}
	else if (current_tab == 1) {
		ui->SingleView_Renderer->change_curvature_proportion(p);
		ui->DualView_LeftRenderer->change_display_curvature_proportion(p);
		ui->DualView_RightRenderer->change_display_curvature_proportion(p);
	}
}

void MainWindow::set_local_smooth_params() {
	const QString& smooth_operator = ui->local_smooth_CB_operators->currentText();
	if (smooth_operator == "Laplacian") {
		ui->DualView_RightRenderer->set_smooth_operator(
			smoothing::smooth_operator::Laplacian
		);
	}
	else if (smooth_operator == "Bilaplacian") {
		ui->DualView_RightRenderer->set_smooth_operator(
			smoothing::smooth_operator::BiLaplacian
		);
	}
	else if (smooth_operator == "TaubinLM") {
		ui->DualView_RightRenderer->set_smooth_operator(
			smoothing::smooth_operator::TaubinLM
		);
	}
	else {
		cerr << "Error: value in combo box for smoothing operator '"
			 << smooth_operator.toStdString() << "' not recognised." << endl;
	}

	const QString& weight_type = ui->local_smooth_CB_weight_type->currentText();
	if (weight_type == "Uniform") {
		ui->DualView_RightRenderer->set_smooth_weight_type(
			smoothing::smooth_weight::uniform
		);
	}
	else if (weight_type == "Cotangent") {
		ui->DualView_RightRenderer->set_smooth_weight_type(
			smoothing::smooth_weight::cotangent
		);
	}
	else {
		cerr << "Error: value in combo box for weight type '"
			 << weight_type.toStdString() << "' not recognised." << endl;
	}

	size_t nit = ui->local_smooth_LE_Iter->text().toInt();
	float lambda = ui->local_smooth_LE_Lambda->text().toFloat();

	ui->DualView_RightRenderer->set_n_iterations(nit);
	ui->DualView_RightRenderer->set_lambda(lambda);
}

void MainWindow::set_global_smooth_params() {
	const QString& smooth_operator = ui->global_smooth_CB_operators->currentText();
	if (smooth_operator == "Laplacian") {
		ui->DualView_RightRenderer->set_smooth_operator(
			smoothing::smooth_operator::Laplacian
		);
	}
	else {
		cerr << "Error: value in combo box for smoothing operator '"
			 << smooth_operator.toStdString() << "' not recognised." << endl;
	}

	const QString& weight_type = ui->local_smooth_CB_weight_type->currentText();
	if (weight_type == "Uniform") {
		ui->DualView_RightRenderer->set_smooth_weight_type(
			smoothing::smooth_weight::uniform
		);
	}
	else if (weight_type == "Cotangent") {
		ui->DualView_RightRenderer->set_smooth_weight_type(
			smoothing::smooth_weight::cotangent
		);
	}
	else {
		cerr << "Error: value in combo box for weight type '"
			 << weight_type.toStdString() << "' not recognised." << endl;
	}

	float v = ui->global_smooth_slider->value();
	float p = 100.0f*v/ui->global_smooth_slider->maximum();
	ui->DualView_RightRenderer->set_perc_fixed_vertices(p);
}

/* Menu buttons */

void MainWindow::on_action_Open_triggered() {

	QString filename = QFileDialog::getOpenFileName(this, tr("Open PLY"), ".", tr("*.ply"));
	if (filename == "") {
		return;
	}

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
	change_poly_mode();
}

void MainWindow::on_CBWireframe_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::wireframe);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::wireframe);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::wireframe);
	}
	change_poly_mode();
}

void MainWindow::on_CBRefLines_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::reflection_lines);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::reflection_lines);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::reflection_lines);
	}
	change_poly_mode();
}

/* Curvature radio buttons */

bool MainWindow::get_prop_curvature_values(float& p) {
	bool ok = true;
	p = ui->LE_PropCurvValues->text().toFloat(&ok);
	if (not ok) {
		cerr << "MainWindow::on_LE_PropCurvValues_returnPressed - error:" << endl;
		cerr << "    value entered in text box is not valid" << endl;
	}
	return ok;
}

void MainWindow::set_prop_values_to_all() {
	float p;
	if (get_prop_curvature_values(p)) {
		ui->SingleView_Renderer->change_curvature_proportion(p);
		ui->DualView_LeftRenderer->change_curvature_proportion(p);
		ui->DualView_RightRenderer->change_curvature_proportion(p);
	}
}

void MainWindow::on_LE_PropCurvValues_returnPressed() {
	float p;
	if (get_prop_curvature_values(p)) {
		change_curvature_prop_display(p);
		if (p < 85.0f) {
			ui->VE_PropCurvValues->setValue(1);
		}
		else if (p > 99.0f) {
			ui->VE_PropCurvValues->setValue(10000);
		}
		else {
			ui->VE_PropCurvValues->setValue(int((p - 85.0f)*10000.0f/14.0f));
		}
	}
}

void MainWindow::on_VE_PropCurvValues_sliderMoved(int value) {
	float p = value*14.0f/10000.0f + 85.0f;
	change_curvature_prop_display(p);
	int v = int(p*100);
	string s = std::to_string(v);
	if (s.length() == 2) {
		s += ".00";
	}
	else if (s.length() == 3) {
		s = s.substr(0,2) + "." + s[2] + "0";
	}
	else if (s.length() == 4) {
		s = s.substr(0,2) + "." + s.substr(2,2);
	}
	ui->LE_PropCurvValues->setText(QString::fromStdString(s));
}

void MainWindow::on_RBCurvatureG_toggled(bool checked) {
	if (checked) {
		set_prop_values_to_all();
		ui->SingleView_Renderer->set_curvature_display(curv_type::Gauss);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::Gauss);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::Gauss);
		change_curvature();
	}
}

void MainWindow::on_RBCurvatureH_toggled(bool checked) {
	if (checked) {
		set_prop_values_to_all();
		ui->SingleView_Renderer->set_curvature_display(curv_type::Mean);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::Mean);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::Mean);
		change_curvature();
	}
}

void MainWindow::on_RBNoCurvature_toggled(bool checked) {
	if (checked) {
		ui->SingleView_Renderer->set_curvature_display(curv_type::none);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::none);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::none);
		change_curvature();
	}
}

/* Smoothing radio buttons */

void MainWindow::on_PB_RunLocalSmooth_clicked() {
	set_local_smooth_params();
	ui->DualView_RightRenderer->run_local_smoothing_algorithm();
}

void MainWindow::on_PB_RunGlobalSmooth_clicked() {
	set_global_smooth_params();
	ui->DualView_RightRenderer->run_global_smoothing_algorithm();
}

void MainWindow::on_PB_ResetDualView_clicked() {
	ui->DualView_RightRenderer->set_mesh(
		ui->DualView_LeftRenderer->get_mesh()
	);
}

/* Band frequencies */

void MainWindow::on_PB_RunBandFreqs_clicked() {
	const QString& txt = ui->TxtEdit_BandFreqs->toPlainText();
	QJsonDocument doc = QJsonDocument::fromJson(txt.toUtf8());

	if (doc.isNull()) {
		cerr << "Error: data entered:" << endl;
		cerr << txt.toStdString() << endl;
		cerr << "is not valid" << endl;
		return;
	}

	ui->DualView_RightRenderer->run_band_frequencies(doc);
}

/* Performance options */

void MainWindow::on_SBThreads_valueChanged(int val) {
	size_t n_threads = static_cast<size_t>(val);
	ui->SingleView_Renderer->set_num_threads(n_threads);
	ui->DualView_LeftRenderer->set_num_threads(n_threads);
	ui->DualView_RightRenderer->set_num_threads(n_threads);
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
	change_poly_mode();
	change_curvature();
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

