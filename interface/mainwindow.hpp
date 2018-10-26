#pragma once

// Qt includes
#include <QMainWindow>

// algorithms includes
#include <algorithms/smoothing/iterative_local.hpp>
using namespace algorithms;
using namespace smoothing;

namespace Ui {
	class MainWindow;
} // -- namespace Ui

class MainWindow : public QMainWindow {
	Q_OBJECT
	private:
		Ui::MainWindow *ui;

		int current_tab;

	private slots:
		void renderer_change_poly_mode();
		void renderer_change_curvature();

		/* Menu buttons */
		void on_action_Open_triggered();
		void on_action_Quit_triggered();

		/* Render radio buttons */
		void on_CBSolid_toggled(bool checked);
		void on_CBWireframe_toggled(bool checked);
		void on_CBRefLines_toggled(bool checked);

		/* Curvature radio buttons */
		void on_RBCurvatureH_toggled(bool checked);
		void on_RBCurvatureG_toggled(bool checked);
		void on_RBNoCurvature_toggled(bool checked);

		/* Smoothing radio buttons */
		void on_RB_Laplacian_toggled(bool checked);
		void on_RB_BiLaplacian_toggled(bool checked);
		void on_RB_Taubin_toggled(bool checked);
		void on_PB_RunSmooth_clicked();

		/* Performance options */
		void on_SBThreads_valueChanged(int arg1);
		void on_PB_ClearMesh_clicked();

		/* Tab widget stuff */
		void on_tabWidget_2_currentChanged(int index);

		/* Settings stuff */
		void on_comboBox_currentIndexChanged(const QString &arg1);

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();
};

