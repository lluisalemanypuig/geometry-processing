#include "mainwindow.hpp"

// C++ includes
#include <iostream>
using namespace std;

// geoproc includes
#include <geoproc/definitions.hpp>
using namespace geoproc;

// Qt includes
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>
#include "ui_mainwindow.h"

#include "err_war_helper.hpp"

// PRIVATE

void MainWindow::change_poly_mode() {
	if (current_tab == 0) {
		ui->SingleView_Renderer->change_polygon_mode();
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->change_polygon_mode();
		ui->DualView_RightRenderer->change_polygon_mode();
	}

	// Update the type of harmonic maps being shown
	if (ui->RB_Render_HarmonicMaps->isChecked()) {
		if (ui->RB_Render_HarmonicMaps_Circle->isChecked()) {
			on_RB_Render_HarmonicMaps_Circle_toggled(true);
		}
		else if (ui->RB_Render_HarmonicMaps_Square->isChecked()) {
			on_RB_Render_HarmonicMaps_Square_toggled(true);
		}
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
	//change_poly_mode();
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

void MainWindow::enable_curvature() {
	polymode pm;
	curv_type ct;

	if (current_tab == 0) {
		pm = ui->SingleView_Renderer->get_polygon_mode();
		ct = ui->SingleView_Renderer->get_curvature_display();
	}
	else {
		pm = ui->DualView_RightRenderer->get_polygon_mode();
		ct = ui->DualView_RightRenderer->get_curvature_display();
	}

	if (pm == polymode::reflection_lines) {
		ui->SB_Render_RefLines_NumSources->setEnabled(true);
	}
	else {
		ui->SB_Render_RefLines_NumSources->setEnabled(false);
	}

	if (pm == polymode::reflection_lines or pm == polymode::harmonic_maps) {
		ui->VS_Curvature_Proportion->setEnabled(false);
		ui->LE_Curvature_Proportion->setEnabled(false);
		ui->RB_Curvature_Gauss->setEnabled(false);
		ui->RB_Curvature_Mean->setEnabled(false);
		ui->RB_Curvature_No->setEnabled(false);
	}
	else {
		// solid or wireframe polygon mode
		ui->VS_Curvature_Proportion->setEnabled(ct != curv_type::none);
		ui->LE_Curvature_Proportion->setEnabled(ct != curv_type::none);

		ui->RB_Curvature_Gauss->setEnabled(true);
		ui->RB_Curvature_Mean->setEnabled(true);
		ui->RB_Curvature_No->setEnabled(true);
	}
}

void MainWindow::set_local_smooth_params() {
	const QString& smooth_operator = ui->CB_Smooth_Local_Operators->currentText();
	if (smooth_operator == "Laplacian") {
		ui->DualView_RightRenderer->set_smooth_operator(
			modifier::Laplacian
		);
	}
	else if (smooth_operator == "Bilaplacian") {
		ui->DualView_RightRenderer->set_smooth_operator(
			modifier::BiLaplacian
		);
	}
	else if (smooth_operator == "TaubinLM") {
		ui->DualView_RightRenderer->set_smooth_operator(
			modifier::TaubinLM
		);
	}
	else {
		cerr << ERR("MainWindow::set_local_smooth_params", "MainWindow") << endl;
		cerr << "    Value in combo box for smoothing operator '"
			 << smooth_operator.toStdString() << "' not recognised." << endl;
	}

	const QString& weight_type = ui->CB_Smooth_Local_WeightType->currentText();
	if (weight_type == "Uniform") {
		ui->DualView_RightRenderer->set_smooth_weight_type(
			weight::uniform
		);
	}
	else if (weight_type == "Cotangent") {
		ui->DualView_RightRenderer->set_smooth_weight_type(
			weight::cotangent
		);
	}
	else {
		cerr << ERR("MainWindow::set_local_smooth_params", "MainWindow") << endl;
		cerr << "    Value in combo box for weight type '"
			 << weight_type.toStdString() << "' not recognised." << endl;
	}

	size_t nit = ui->LE_Smooth_Local_NumIter->text().toInt();
	float lambda = ui->LE_Smooth_Local_Lambda->text().toFloat();

	ui->DualView_RightRenderer->set_n_iterations(nit);
	ui->DualView_RightRenderer->set_lambda(lambda);
}

void MainWindow::set_global_smooth_params() {
	const QString& smooth_operator = ui->CB_Smooth_Global_Operators->currentText();
	if (smooth_operator == "Laplacian") {
		ui->DualView_RightRenderer->set_smooth_operator(
			modifier::Laplacian
		);
	}
	else {
		cerr << ERR("MainWindow::set_global_smooth_params", "MainWindow") << endl;
		cerr << "    Value in combo box for smoothing operator '"
			 << smooth_operator.toStdString() << "' not recognised." << endl;
	}

	const QString& weight_type = ui->CB_Smooth_Global_WeightType->currentText();
	if (weight_type == "Uniform") {
		ui->DualView_RightRenderer->set_smooth_weight_type(
			weight::uniform
		);
	}
	else if (weight_type == "Cotangent") {
		ui->DualView_RightRenderer->set_smooth_weight_type(
			weight::cotangent
		);
	}
	else {
		cerr << ERR("MainWindow::set_global_smooth_params", "MainWindow") << endl;
		cerr << "    Value in combo box for weight type '"
			 << weight_type.toStdString() << "' not recognised." << endl;
	}

	float v = ui->HS_Smooth_Global_Percentage->value();
	float p = 100.0f*v/ui->HS_Smooth_Global_Percentage->maximum();
	ui->DualView_RightRenderer->set_perc_fixed_vertices(p);
}

/* Menu buttons */

void MainWindow::on_action_Open_triggered() {

	QString filename =
		QFileDialog::getOpenFileName(this, tr("Open PLY"), ".", tr("*.ply"));
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

/* Render */

// ------ Solid

void MainWindow::on_RB_Render_Solid_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::solid);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::solid);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::solid);
		change_poly_mode();

		// we may need to enable/disable things
		enable_curvature();
	}
}

// ------ Wireframe

void MainWindow::on_RB_Render_Wireframe_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::wireframe);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::wireframe);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::wireframe);
		change_poly_mode();

		// we may need to enable/disable things
		enable_curvature();
	}
}

// ------ Reflection lines

void MainWindow::on_RB_Render_RefLines_toggled(bool toggled) {
	if (toggled) {
		ui->SingleView_Renderer->set_polygon_mode(polymode::reflection_lines);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::reflection_lines);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::reflection_lines);
		change_poly_mode();

		on_SB_Render_RefLines_NumSources_valueChanged(
			ui->SB_Render_RefLines_NumSources->value()
		);
		// we may need to enable/disable things
		enable_curvature();
	}
}

void MainWindow::on_SB_Render_RefLines_NumSources_valueChanged(int v) {
	if (current_tab == 0) {
		ui->SingleView_Renderer->set_light_sources_refl_lines(v);
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->set_light_sources_refl_lines(v);
		ui->DualView_RightRenderer->set_light_sources_refl_lines(v);
	}
}

// ------ Harmonic Maps

void MainWindow::on_RB_Render_HarmonicMaps_toggled(bool checked) {
	ui->RB_Render_HarmonicMaps_Circle->setEnabled(checked);
	ui->RB_Render_HarmonicMaps_Square->setEnabled(checked);
	ui->CB_Render_HarmonicMaps_Wireframe->setEnabled(checked);
	ui->CB_Render_HarmonicMaps_Weights->setEnabled(checked);

	if (checked) {
		// set polygon mode
		ui->SingleView_Renderer->set_polygon_mode(polymode::harmonic_maps);
		ui->DualView_LeftRenderer->set_polygon_mode(polymode::harmonic_maps);
		ui->DualView_RightRenderer->set_polygon_mode(polymode::harmonic_maps);

		// we may need to enable/disable things
		enable_curvature();

		change_poly_mode();

		if (ui->RB_Render_HarmonicMaps_Circle->isChecked()) {
			on_RB_Render_HarmonicMaps_Circle_toggled(true);
		}
		else if (ui->RB_Render_HarmonicMaps_Square->isChecked()) {
			on_RB_Render_HarmonicMaps_Square_toggled(true);
		}
	}
}

void MainWindow::on_RB_Render_HarmonicMaps_Circle_toggled(bool checked) {
	if (checked) {
		string w = ui->CB_Render_HarmonicMaps_Weights->currentText().toStdString();
		weight weight;
		if (w == "Uniform") {
			weight = weight::uniform;
		}
		else if (w == "Cotangent") {
			weight = weight::cotangent;
		}

		if (current_tab == 0) {
			ui->SingleView_Renderer->set_harmonic_map(
				polymode::harmonic_maps_Circle, weight
			);
		}
		else if (current_tab == 1) {
			ui->DualView_LeftRenderer->set_harmonic_map(
				polymode::harmonic_maps_Circle, weight
			);
			ui->DualView_RightRenderer->set_harmonic_map(
				polymode::harmonic_maps_Circle, weight
			);
		}
	}
}

void MainWindow::on_RB_Render_HarmonicMaps_Square_toggled(bool checked) {
	if (checked) {
		string w = ui->CB_Render_HarmonicMaps_Weights->currentText().toStdString();
		weight weight;
		if (w == "Uniform") {
			weight = weight::uniform;
		}
		else if (w == "Cotangent") {
			weight = weight::cotangent;
		}

		if (current_tab == 0) {
			ui->SingleView_Renderer->set_harmonic_map(
				polymode::harmonic_maps_Square, weight
			);
		}
		else if (current_tab == 1) {
			ui->DualView_LeftRenderer->set_harmonic_map(
				polymode::harmonic_maps_Square, weight
			);
			ui->DualView_RightRenderer->set_harmonic_map(
				polymode::harmonic_maps_Square, weight
			);
		}
	}
}

void MainWindow::on_CB_Render_HarmonicMaps_Wireframe_toggled(bool checked) {
	if (current_tab == 0) {
		ui->SingleView_Renderer->set_harmonic_map_remeshing(checked);
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->set_harmonic_map_remeshing(checked);
		ui->DualView_RightRenderer->set_harmonic_map_remeshing(checked);
	}
}

void MainWindow::on_CB_Render_HarmonicMaps_Weights_currentTextChanged
(
	const QString& t
)
{
	string w = t.toStdString();
	weight weight;
	if (w == "Uniform") {
		weight = weight::uniform;
	}
	else if (w == "Cotangent") {
		weight = weight::cotangent;
	}
	else {
		cerr << ERR("MainWindow::on_CB_Render_HarmonicMaps_Weights_currentTextChanged",
					"MainWindow") << endl;
		cerr << "    Value in combobox '" << w << "' not valid" << endl;
		return;
	}

	polymode pm;
	if (ui->RB_Render_HarmonicMaps_Circle->isChecked()) {
		pm = polymode::harmonic_maps_Circle;
	}
	else if (ui->RB_Render_HarmonicMaps_Square->isChecked()) {
		pm = polymode::harmonic_maps_Square;
	}
	else {
		cerr << ERR("MainWindow::on_CB_Render_HarmonicMaps_Weights_currentTextChanged",
					"MainWindow") << endl;
		cerr << "    Neither the radio button for 'Circle' or 'Square' are checked." << endl;
		return;
	}

	if (current_tab == 0) {
		ui->SingleView_Renderer->set_harmonic_map(pm, weight);
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->set_harmonic_map(pm, weight);
		ui->DualView_RightRenderer->set_harmonic_map(pm, weight);
	}
}

/* Curvature radio buttons */

bool MainWindow::get_prop_curvature_values(float& p) {
	bool ok = true;
	p = ui->LE_Curvature_Proportion->text().toFloat(&ok);
	if (not ok) {
		cerr << ERR("MainWindow::on_LE_Curvature_Proportion_returnPressed",
					"MainWindow") << endl;
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

void MainWindow::on_LE_Curvature_Proportion_returnPressed() {
	float p;
	if (get_prop_curvature_values(p)) {
		change_curvature_prop_display(p);
		if (p < 85.0f) {
			ui->VS_Curvature_Proportion->setValue(1);
		}
		else if (p > 99.0f) {
			ui->VS_Curvature_Proportion->setValue(10000);
		}
		else {
			ui->VS_Curvature_Proportion->setValue(int((p - 85.0f)*10000.0f/14.0f));
		}
	}
}

void MainWindow::on_VS_Curvature_Proportion_sliderMoved(int value) {
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
	ui->LE_Curvature_Proportion->setText(QString::fromStdString(s));
}

void MainWindow::on_RB_Curvature_Gauss_toggled(bool checked) {
	if (checked) {
		set_prop_values_to_all();
		ui->SingleView_Renderer->set_curvature_display(curv_type::Gauss);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::Gauss);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::Gauss);
		change_curvature();

		// we may need to enable/disable things
		enable_curvature();
	}
}

void MainWindow::on_RB_Curvature_Mean_toggled(bool checked) {
	if (checked) {
		set_prop_values_to_all();
		ui->SingleView_Renderer->set_curvature_display(curv_type::Mean);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::Mean);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::Mean);
		change_curvature();

		// we may need to enable/disable things
		enable_curvature();
	}
}

void MainWindow::on_RB_Curvature_No_toggled(bool checked) {
	if (checked) {
		ui->SingleView_Renderer->set_curvature_display(curv_type::none);
		ui->DualView_LeftRenderer->set_curvature_display(curv_type::none);
		ui->DualView_RightRenderer->set_curvature_display(curv_type::none);
		change_curvature();

		// we may need to enable/disable things
		enable_curvature();
	}
}

/* Smoothing */

void MainWindow::on_HS_Smooth_Global_Percentage_valueChanged(int value) {
	float p = 100.0f*float(value)/ui->HS_Smooth_Global_Percentage->maximum();
	p = (int(p*100))/100.0f;
	string ps = std::to_string(p);
	if (ps.find('.') == 1) {
		ps = "0" + ps;
		ps = ps.substr(0,5);
	}
	else if (ps.find('.') == 2) {
		ps = ps.substr(0,5);
	}
	else {
		ps = ps.substr(0,6);
	}
	ui->LABEL_Smooth_Global_Percentage->setText(QString::fromStdString(ps));
}

void MainWindow::on_PB_Smooth_Local_Run_clicked() {
	set_local_smooth_params();
	ui->DualView_RightRenderer->run_local_smoothing_algorithm();
}

void MainWindow::on_PB_Smooth_Global_Run_clicked() {
	set_global_smooth_params();
	ui->DualView_RightRenderer->run_global_smoothing_algorithm();
}

void MainWindow::on_PB_DualView_Reset_clicked() {
	ui->DualView_RightRenderer->set_mesh(
		ui->DualView_LeftRenderer->get_mesh()
	);
}

/* Band frequencies */

void MainWindow::on_PB_BandFreqs_Run_clicked() {
	const QString& txt = ui->TxtEdit_BandFreqs_Program->toPlainText();
	QJsonDocument doc = QJsonDocument::fromJson(txt.toUtf8());

	if (doc.isNull()) {
		cerr << ERR("MainWindow::on_PB_BandFreqs_Run_clicked", "MainWindow") << endl;
		cerr << "    Data entered:" << endl;
		cerr << "-----------------------------------" << endl;
		cerr << txt.toStdString() << endl;
		cerr << "-----------------------------------" << endl;
		cerr << "    is not valid" << endl;
		return;
	}

	ui->DualView_RightRenderer->run_band_frequencies(doc);
}

/* Performance options */

void MainWindow::on_SB_Settings_NumThreads_valueChanged(int val) {
	size_t n_threads = static_cast<size_t>(val);
	ui->SingleView_Renderer->set_num_threads(n_threads);
	ui->DualView_LeftRenderer->set_num_threads(n_threads);
	ui->DualView_RightRenderer->set_num_threads(n_threads);
}

void MainWindow::on_PB_Settings_ClearMesh_clicked() {
	if (current_tab == 0) {
		ui->SingleView_Renderer->clear_mesh();
	}
	else if (current_tab == 1) {
		ui->DualView_LeftRenderer->clear_mesh();
		ui->DualView_RightRenderer->clear_mesh();
	}
}

/* Remeshing */

void MainWindow::on_PB_Remeshing_Run_clicked() {
	bool ok = true;

	int N = ui->LE_Remeshing_N->text().toInt(&ok);
	if (not ok) {
		cerr << ERR("MainWindow::on_PB_Remeshing_Run_clicked", "MainWindow") << endl;
		cerr << "    Invalid contents of textbox for amount of points in x" << endl;
		return;
	}

	int M = ui->LE_Remeshing_M->text().toInt(&ok);
	if (not ok) {
		cerr << ERR("MainWindow::on_PB_Remeshing_Run_clicked", "MainWindow") << endl;
		cerr << "    Invalid contents of textbox for amount of points in y" << endl;
		return;
	}

	string w = ui->CB_Remeshing_Weights->currentText().toStdString();
	weight weight;
	if (w == "Uniform") {
		weight = weight::uniform;
	}
	else if (w == "Cotangent") {
		weight = weight::cotangent;
	}
	else {
		cerr << ERR("MainWindow::on_PB_Remeshing_Run_clicked", "MainWindow") << endl;
		cerr << "    Value in combobox '" << w << "' not valid" << endl;
		return;
	}

	ui->DualView_RightRenderer->set_smooth_weight_type(weight);

	size_t NN = static_cast<size_t>(N);
	size_t MM = static_cast<size_t>(M);
	ui->DualView_RightRenderer->make_remeshing(NN, MM);
}

/* Tab widget stuff */

void MainWindow::on_TW_View_currentChanged(int index) {
	current_tab = index;
	change_poly_mode();
	change_curvature();

	if (ui->SB_Render_RefLines_NumSources->isEnabled()) {
		int v = ui->SB_Render_RefLines_NumSources->value();
		if (current_tab == 0) {
			ui->SingleView_Renderer->set_light_sources_refl_lines(v);
		}
		else if (current_tab == 1) {
			ui->DualView_LeftRenderer->set_light_sources_refl_lines(v);
			ui->DualView_RightRenderer->set_light_sources_refl_lines(v);
		}
	}

	if (ui->RB_Render_HarmonicMaps->isChecked()) {
		bool remesh = ui->CB_Render_HarmonicMaps_Wireframe->isChecked();
		if (current_tab == 0) {
			ui->SingleView_Renderer->set_harmonic_map_remeshing(remesh);
		}
		else if (current_tab == 1) {
			ui->DualView_LeftRenderer->set_harmonic_map_remeshing(remesh);
			ui->DualView_RightRenderer->set_harmonic_map_remeshing(remesh);
		}
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

	ui->SingleView_Renderer->set_name("single view widget");
	ui->DualView_LeftRenderer->set_name("dual view - left widget");
	ui->DualView_RightRenderer->set_name("dual view - right widget");
}

MainWindow::~MainWindow() {
	delete ui;
}
