#include "twinglwidget.hpp"

// C++ includes
#include <iostream>
#include <numeric>
using namespace std;

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// geoproc includes
#include <geoproc/smoothing/local.hpp>
#include <geoproc/smoothing/global.hpp>
#include <geoproc/filter_frequencies/band_frequencies.hpp>

// custom includes
#include "err_war_helper.hpp"

// OTHERS

static inline
void print_smoothing_configuration
(
	const filter_frequencies::smoothing_configuration& conf,
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
	wt = smoothing::smooth_weight::uniform;
	twin = nullptr;

	nit = 0;
	lambda = 0.0f;
	mu = 0.0f;

	perc_fix_vertices = 0.0f;
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

void TwinGLWidget::set_perc_fixed_vertices(float p) {
	perc_fix_vertices = p;
}

void TwinGLWidget::set_smooth_operator(const smoothing::smooth_operator& o) {
	op = o;
}

void TwinGLWidget::set_smooth_weight_type(const smoothing::smooth_weight& w) {
	wt = w;
}

// OTHERS

void TwinGLWidget::run_local_smoothing_algorithm() {
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	cout << PROG("TwinGLWidget::run_local_smoothing_algorithm",
				 name,
				 "Local smooth with:")
		 << endl;
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
	cout << "    weight type ";
	if (wt == smoothing::smooth_weight::uniform) {
		cout << "'uniform'";
	}
	else if (wt == smoothing::smooth_weight::cotangent) {
		cout << "'cotangent'";
	}
	cout << endl;
	cout << "    lambda: " << lambda << endl;
	cout << "    # iterations: " << nit << endl;
	cout << "    # threads: " << nt << endl;

	timing::time_point begin = timing::now();
	if (op == smoothing::smooth_operator::Laplacian) {
		smoothing::local::laplacian(wt, lambda, nit, nt, mesh);
	}
	else if (op == smoothing::smooth_operator::BiLaplacian) {
		smoothing::local::bilaplacian(wt, lambda, nit, nt, mesh);
	}
	else if (op == smoothing::smooth_operator::TaubinLM) {
		smoothing::local::TaubinLM(wt, lambda, nit, nt, mesh);
	}
	timing::time_point end = timing::now();

	mesh.make_normal_vectors();

	// output execution time
	cout << "    in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds." << endl;

	if (current_curv_display != curv_type::none) {
		compute_curvature();
		// show curvature already has
		// makeCurrent() .. doneCurrent()
		show_curvature(true);
	}
	else {
		makeCurrent();
		mesh.make_vertices_normals_buffers(program);
		doneCurrent();
	}
	update();
}

void TwinGLWidget::run_global_smoothing_algorithm() {
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	cout << PROG("TwinGLWidget::run_local_smoothing_algorithm",
				 name,
				 "Global smooth with:")
		 << endl;
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
	cout << "    weight type ";
	if (wt == smoothing::smooth_weight::uniform) {
		cout << "'uniform'";
	}
	else if (wt == smoothing::smooth_weight::cotangent) {
		cout << "'cotangent'";
	}
	cout << endl;
	cout << "    percentage: " << perc_fix_vertices << endl;

	int N = mesh.n_vertices();
	vector<int> indices(N);
	std::iota(indices.begin(), indices.end(), 0);
	int max_idx = N - 1;

	cout << "    Fixing vertices..." << endl;

	vector<bool> constant(N, false);
	while ((100.0f*(N - max_idx - 1))/N < perc_fix_vertices) {
		int i = rand()%(max_idx + 1);
		constant[indices[i]] = true;

		std::swap( indices[i], indices[max_idx] );
		--max_idx;
	}

	cout << "    Run global smooth algorithm..." << endl;

	timing::time_point begin = timing::now();
	smoothing::global::smooth(op, wt, constant, mesh);
	timing::time_point end = timing::now();

	mesh.make_normal_vectors();

	// output execution time
	cout << "    in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds." << endl;

	if (current_curv_display != curv_type::none) {
		compute_curvature();
	}

	// update mesh
	if (current_curv_display != curv_type::none) {

		// show curvature already has
		// makeCurrent() .. doneCurrent()
		show_curvature(true);
	}
	else {
		makeCurrent();
		mesh.make_vertices_normals_buffers(program);
		doneCurrent();
	}
	update();
}

static inline
bool parseJsonObject
(
	const QJsonObject& obj,
	vector<filter_frequencies::smoothing_configuration>& confs,
	float& mu, const string& name
)
{
	// error control
	if (obj.find(QString("A")) == obj.end()) {
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Missing algorithm parameter A." << endl;
		return false;
	}
	if (obj.find(QString("W")) == obj.end()) {
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Missing weight type parameter W." << endl;
		return false;
	}
	if (obj.find(QString("N")) == obj.end()) {
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Missing number of iterations parameter N." << endl;
		return false;
	}
	if (obj.find(QString("L")) == obj.end()) {
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Missing lambda parameter L." << endl;
		return false;
	}
	QJsonValue alg_value = obj.value("A");
	if (not alg_value.isString()) {
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Algorithm parameter value is not a string." << endl;
		return false;
	}
	QJsonValue weight_value = obj.value("W");
	if (not weight_value.isString()) {
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Weight type parameter value is not a string." << endl;
		return false;
	}
	QJsonValue nit_value = obj.value("N");
	if (not nit_value.isDouble()) {
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Number of iterations parameter value is not a number." << endl;
		return false;
	}
	QJsonValue lambda_value = obj.value("L");
	if (not lambda_value.isDouble()) {
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Lambda parameter value is not a number." << endl;
		return false;
	}
	if (obj.find(QString("mu")) != obj.end()) {
		QJsonValue mu_value = obj.value("mu");
		if (not mu_value.isDouble()) {
			cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
			cerr << "    Mu parameter value is not a number." << endl;
			return false;
		}
	}

	filter_frequencies::smoothing_configuration conf;

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
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Algorithm parameter value '"
			 << alg_str.toStdString() << "' is not valid." << endl;
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
		cerr << ERR("TwinGLWidget -> parseJsonObject", name) << endl;
		cerr << "    Weigth type parameter value '"
			 << weight_str.toStdString() << "' is not valid." << endl;
		return false;
	}

	/* parse LAMBDA */
	conf.lambda = lambda_value.toDouble();

	/* parse MU */
	if (obj.find(QString("mu")) != obj.end()) {
		QJsonValue mu_value = obj.value("mu");
		mu = mu_value.toDouble();
	}

	/* parse # ITERATIONS */
	conf.n_it = nit_value.toInt();

	confs.push_back(conf);
	return true;
}

void TwinGLWidget::run_band_frequencies(const QJsonDocument& doc) {
	vector<filter_frequencies::smoothing_configuration> confs;
	vector<float> mus;

	if (doc.isObject()) {
		QJsonObject obj = doc.object();
		float mu;
		bool res = parseJsonObject(obj, confs, mu, name);
		if (not res) {
			return;
		}
		mus.push_back(mu);
	}
	else if (doc.isArray()) {
		QJsonArray arr = doc.array();
		for (int i = 0; i < arr.count(); ++i) {
			float mu;
			QJsonObject obj = arr.at(i).toObject();
			bool res = parseJsonObject(obj, confs, mu, name);
			if (not res) {
				return;
			}
			if (i < arr.count() - 1) {
				mus.push_back(mu);
			}
		}
	}

	if (confs.size() <= 1) {
		cerr << ERR("TwinGLWidget::run_band_frequencies", name) << endl;
		cerr << "    Only one smoothing configuration." << endl;
		cerr << "    Band frequencies require at least 2." << endl;
		return;
	}

	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	cout << PROG("TwinGLWidget::run_band_frequencies",
				 name,
				 "Computing band frequencies with:")
		 << endl;
	for (size_t i = 0; i < confs.size(); ++i) {
		cout << "    Configuration " << i + 1 << endl;
		print_smoothing_configuration(confs[i], nt, "        ");
	}
	cout << "    using weights:";
	for (size_t i = 0; i < mus.size(); ++i) {
		cout << " " << mus[i];
	}
	cout << endl;

	timing::time_point begin = timing::now();
	filter_frequencies::band_frequencies(confs, mus, nt, mesh);
	timing::time_point end = timing::now();

	mesh.make_normal_vectors();

	// output execution time
	cout << "    in: "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds." << endl;

	if (current_curv_display != curv_type::none) {
		compute_curvature();
	}

	// update mesh
	if (current_curv_display != curv_type::none) {

		// show curvature already has
		// makeCurrent() .. doneCurrent()
		show_curvature(true);
	}
	else {
		cout << PROG("TwinGLWidget::run_band_frequencies",
					 name,
					 "Remake buffers for vertices and normals")
			 << endl;
		makeCurrent();
		mesh.make_vertices_normals_buffers(program);
		doneCurrent();
	}
	update();
}
