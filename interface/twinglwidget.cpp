#include "twinglwidget.hpp"

// C++ includes
#include <iostream>
using namespace std;

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// geoproc includes
#include <geoproc/smoothing/iterative_local.hpp>
#include <geoproc/high-frequencies/highfrequencies.hpp>

// OTHERS

static inline
void print_smoothing_configuration
(
	const high_frequencies::smoothing_configuration& conf,
	size_t nt,
	const string& tab = ""
)
{
	cout << tab << "operator ";
	if (conf.so == smoothing::smooth_operator::Laplacian) {
		cout << "'Laplacian'";
	}
	else if (conf.so == smoothing::smooth_operator::BiLaplacian) {
		cout << "'biLaplacian'";
	}
	else if (conf.so == smoothing::smooth_operator::TaubinLM) {
		cout << "'Taubin'";
	}
	cout << endl;

	cout << tab << "weight type ";
	if (conf.sw == smoothing::smooth_weight::uniform) {
		cout << "'uniform'";
	}
	else if (conf.sw == smoothing::smooth_weight::cotangent) {
		cout << "'cotangent'";
	}
	cout << endl;

	cout << tab << "lambda: " << conf.lambda << endl;
	cout << tab << "# iterations: " << conf.n_it << endl;
	cout << tab << "# threads: " << nt << endl;
}

// PROTECTED

void TwinGLWidget::mousePressEvent(QMouseEvent *event) {
	GLWidget::mousePressEvent(event);
}

void TwinGLWidget::mouseMoveEvent(QMouseEvent *event) {
	GLWidget::mouseMoveEvent(event);
	twin->set_display(angleX, angleY, distance);
}

// PUBLIC

TwinGLWidget::TwinGLWidget(QWidget *parent) : GLWidget(parent) {
	op = smoothing::smooth_operator::Laplacian;
	twin = nullptr;
}

TwinGLWidget::~TwinGLWidget() {

}

// SETTERS

void TwinGLWidget::set_twin(TwinGLWidget *t) {
	twin = t;
}

void TwinGLWidget::set_display(float aX, float aY, float d) {
	angleX = aX;
	angleY = aY;
	distance = d;

	makeCurrent();
	set_modelview();
	doneCurrent();
	update();
}

void TwinGLWidget::set_n_iterations(size_t n) {
	nit = n;
}

void TwinGLWidget::set_lambda(float l) {
	lambda = l;
	mu = 1.0f/(0.1f - 1.0f/lambda);
}

void TwinGLWidget::set_smooth_operator(const smoothing::smooth_operator& o) {
	op = o;
}

void TwinGLWidget::set_smooth_weight_type(const smoothing::smooth_weight& w) {
	wt = w;
}

// OTHERS

void TwinGLWidget::run_smoothing_algorithm() {
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	cout << "Smooth with:" << endl;
	cout << "    operator ";
	if (op == smoothing::smooth_operator::Laplacian) {
		cout << "'Laplacian'";
	}
	else if (op == smoothing::smooth_operator::BiLaplacian) {
		cout << "'biLaplacian'";
	}
	else if (op == smoothing::smooth_operator::TaubinLM) {
		cout << "'Taubin'";
	}
	cout << endl;
	cout << "    lambda: " << lambda << endl;
	cout << "    # iterations: " << nit << endl;
	cout << "    # threads: " << nt << endl;

	timing::time_point begin = timing::now();
	if (op == smoothing::smooth_operator::Laplacian) {
		smoothing::laplacian(wt, lambda, nit, nt, mesh);
	}
	else if (op == smoothing::smooth_operator::BiLaplacian) {
		smoothing::bilaplacian(wt, lambda, nit, nt, mesh);
	}
	else if (op == smoothing::smooth_operator::TaubinLM) {
		smoothing::TaubinLM(wt, lambda, nit, nt, mesh);
	}
	timing::time_point end = timing::now();

	mesh.make_normal_vectors();

	// output execution time
	cout << "    in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds" << endl;

	if (current_curv_display != curv_type::none) {
		compute_curvature();
	}

	// update mesh
	if (current_curv_display != curv_type::none) {

		// show curvature already has
		// makeCurrent() .. doneCurrent()
		show_curvature(false, true);
	}
	else {
		makeCurrent();
		mesh.make_vertices_normals(program);
		doneCurrent();
	}
	update();
}

void TwinGLWidget::run_high_freqs_algorithm() {
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	high_frequencies::smoothing_configuration C;
	C.lambda = lambda;
	C.n_it = nit;
	C.so = op;
	C.sw = wt;

	cout << "Highlight high frequencies with:" << endl;;
	cout << "    operator ";
	if (op == smoothing::smooth_operator::Laplacian) {
		cout << "'Laplacian'";
	}
	else if (op == smoothing::smooth_operator::BiLaplacian) {
		cout << "'biLaplacian'";
	}
	else if (op == smoothing::smooth_operator::TaubinLM) {
		cout << "'Taubin'";
	}
	cout << endl;
	cout << "    lambda: " << lambda << endl;
	cout << "    # iterations: " << nit << endl;
	cout << "    # threads: " << nt << endl;

	timing::time_point begin = timing::now();
	high_frequencies::high_frequency_details(C, nt, mesh);
	timing::time_point end = timing::now();

	mesh.make_normal_vectors();

	// output execution time
	cout << "    in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds" << endl;

	if (current_curv_display != curv_type::none) {
		compute_curvature();
	}

	// update mesh
	if (current_curv_display != curv_type::none) {

		// show curvature already has
		// makeCurrent() .. doneCurrent()
		show_curvature(false, true);
	}
	else {
		makeCurrent();
		mesh.make_vertices_normals(program);
		doneCurrent();
	}
	update();
}

void TwinGLWidget::run_exagg_high_freqs_algorithm() {
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	high_frequencies::smoothing_configuration C;
	C.lambda = lambda;
	C.n_it = nit;
	C.so = op;
	C.sw = wt;

	cout << "Exaggerate high frequencies with:" << endl;;
	print_smoothing_configuration(C, nt, "    ");

	timing::time_point begin = timing::now();
	high_frequencies::exaggerate_high_frequencies(C, nt, mesh);
	timing::time_point end = timing::now();

	mesh.make_normal_vectors();

	// output execution time
	cout << "    in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds" << endl;

	if (current_curv_display != curv_type::none) {
		compute_curvature();
	}

	// update mesh
	if (current_curv_display != curv_type::none) {

		// show curvature already has
		// makeCurrent() .. doneCurrent()
		show_curvature(false, true);
	}
	else {
		makeCurrent();
		mesh.make_vertices_normals(program);
		doneCurrent();
	}
	update();
}

static inline
bool parseJsonObject
(
	const QJsonObject& obj,
	vector<high_frequencies::smoothing_configuration>& confs
)
{
	// error control
	if (obj.find(QString("A")) == obj.end()) {
		cerr << "Error: missing algorithm parameter A" << endl;
		return false;
	}
	if (obj.find(QString("W")) == obj.end()) {
		cerr << "Error: missing weight type parameter W" << endl;
		return false;
	}
	if (obj.find(QString("N")) == obj.end()) {
		cerr << "Error: missing number of iterations parameter N" << endl;
		return false;
	}
	if (obj.find(QString("L")) == obj.end()) {
		cerr << "Error: missing lambda parameter L" << endl;
		return false;
	}
	QJsonValue alg_value = obj.value("A");
	if (not alg_value.isString()) {
		cerr << "Error: algorithm parameter value is not a string" << endl;
		return false;
	}
	QJsonValue weight_value = obj.value("W");
	if (not weight_value.isString()) {
		cerr << "Error: weight type parameter value is not a string" << endl;
		return false;
	}
	QJsonValue nit_value = obj.value("N");
	if (not nit_value.isDouble()) {
		cerr << "Error: number of iterations parameter value is not a number" << endl;
		return false;
	}
	QJsonValue lambda_value = obj.value("L");
	if (not lambda_value.isDouble()) {
		cerr << "Error: lambda parameter value is not a number" << endl;
		return false;
	}

	high_frequencies::smoothing_configuration conf;

	/* parse ALGORITHM */
	QString alg_str = alg_value.toString();
	if (alg_str == "Laplacian") {
		conf.so = smoothing::smooth_operator::Laplacian;
	}
	else if (alg_str == "Laplacian") {
		conf.so = smoothing::smooth_operator::BiLaplacian;
	}
	else if (alg_str == "Laplacian") {
		conf.so = smoothing::smooth_operator::TaubinLM;
	}
	else {
		cerr << "Error: algorithm parameter value '"
			 << alg_str.toStdString() << "' is not valid" << endl;
		return false;
	}

	/* parse WEIGHT TYPE */
	QString weight_str = weight_value.toString();
	if (weight_str == "uniform") {
		conf.sw = smoothing::smooth_weight::uniform;
	}
	else if (weight_str == "cotangent") {
		conf.sw = smoothing::smooth_weight::cotangent;
	}
	else {
		cerr << "Error: weigth type parameter value '"
			 << weight_str.toStdString() << "' is not valid" << endl;
		return false;
	}

	/* parse LAMBDA */
	conf.lambda = lambda_value.toDouble();

	/* parse # ITERATIONS */
	conf.n_it = nit_value.toInt();

	confs.push_back(conf);
	return true;
}

void TwinGLWidget::run_band_frequencies(const QJsonDocument& doc) {
	vector<high_frequencies::smoothing_configuration> confs;

	if (doc.isObject()) {
		QJsonObject obj = doc.object();
		bool res = parseJsonObject(obj, confs);
		if (not res) {
			return;
		}
	}
	else if (doc.isArray()) {
		QJsonArray arr = doc.array();
		for (int i = 0; i < arr.count(); ++i) {
			QJsonObject obj = arr.at(i).toObject();
			bool res = parseJsonObject(obj, confs);
			if (not res) {
				return;
			}
		}
	}

	if (confs.size() <= 1) {
		cerr << "Error: only one smoothing configuration." << endl;
		cerr << "    Band frequencies require at least 2." << endl;
		return;
	}

	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	timing::time_point begin = timing::now();
	high_frequencies::band_frequencies(confs, nt, mesh);
	timing::time_point end = timing::now();

	mesh.make_normal_vectors();

	// output execution time
	cout << "Computed band frequencies with:" << endl;
	for (size_t i = 0; i < confs.size(); ++i) {
		cout << "    Configuration " << i + 1 << endl;
		print_smoothing_configuration(confs[i], nt, "        ");
	}
	cout << "    in: "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds" << endl;

	if (current_curv_display != curv_type::none) {
		compute_curvature();
	}

	// update mesh
	if (current_curv_display != curv_type::none) {

		// show curvature already has
		// makeCurrent() .. doneCurrent()
		show_curvature(false, true);
	}
	else {
		cout << "Remake vertices and normals" << endl;
		makeCurrent();
		mesh.make_vertices_normals(program);
		doneCurrent();
	}
	update();
}
