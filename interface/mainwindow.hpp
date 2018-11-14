#pragma once

// Qt includes
#include <QMainWindow>

namespace Ui {
	class MainWindow;
} // -- namespace Ui

class MainWindow : public QMainWindow {
	Q_OBJECT
	private:
		Ui::MainWindow *ui;

		int current_tab;

		void change_poly_mode();
		void change_curvature();
		void change_curvature_prop_display(float p);
		void set_Smooth_params();

	private slots:

		/* Menu */
		void on_action_Open_triggered();
		void on_action_Quit_triggered();

		/* Render */
		void on_CBSolid_toggled(bool checked);
		void on_CBWireframe_toggled(bool checked);
		void on_CBRefLines_toggled(bool checked);

		/* Curvature */
		bool get_prop_curvature_values(float& p);
		void set_prop_values_to_all();
		void on_LE_PropCurvValues_returnPressed();
		void on_VE_PropCurvValues_sliderMoved(int value);
		void on_RBCurvatureH_toggled(bool checked);
		void on_RBCurvatureG_toggled(bool checked);
		void on_RBNoCurvature_toggled(bool checked);

		/* Smoothing */
		void on_PB_RunSmooth_clicked();
		void on_PB_ResetDualView_clicked();

		/* Band frequencies */

		void on_PB_RunBandFreqs_clicked();

		/* Performance options */
		void on_SBThreads_valueChanged(int arg1);
		void on_PB_ClearMesh_clicked();

		/* Tab widget stuff */
		void on_TW_View_currentChanged(int index);

public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();
};

