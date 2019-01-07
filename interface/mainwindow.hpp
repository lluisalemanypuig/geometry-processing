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
		void enable_curvature();

		void set_local_smooth_params();
		void set_global_smooth_params();

	private slots:

		/* Menu */
		void on_action_Open_triggered();
		void on_action_Quit_triggered();

		/* Render */
		// solid
		void on_RB_Render_Solid_toggled(bool checked);
		// wireframe
		void on_RB_Render_Wireframe_toggled(bool checked);
		// reflection lines
		void on_RB_Render_RefLines_toggled(bool checked);
		void on_SB_Render_RefLines_NumSources_valueChanged(int v);
		// harmonic maps
		void on_RB_Render_HarmonicMaps_toggled(bool checked);
		void on_RB_Render_HarmonicMaps_Circle_toggled(bool checked);
		void on_RB_Render_HarmonicMaps_Square_toggled(bool checked);
		void on_CB_Render_HarmonicMaps_Wireframe_toggled(bool checked);
		void on_CB_Render_HarmonicMaps_Weights_currentTextChanged
		(const QString &arg1);

		/* Curvature */
		bool get_prop_curvature_values(float& p);
		void set_prop_values_to_all();
		void on_LE_Curvature_Proportion_returnPressed();
		void on_VS_Curvature_Proportion_sliderMoved(int value);
		void on_RB_Curvature_Mean_toggled(bool checked);
		void on_RB_Curvature_Gauss_toggled(bool checked);
		void on_RB_Curvature_No_toggled(bool checked);

		/* Smoothing */
		void on_HS_Smooth_Global_Percentage_valueChanged(int value);
		void on_PB_Smooth_Local_Run_clicked();
		void on_PB_Smooth_Global_Run_clicked();
		void on_PB_DualView_Reset_clicked();

		/* Band frequencies */
		void on_PB_BandFreqs_Run_clicked();

		/* Performance options */
		void on_SB_Settings_NumThreads_valueChanged(int arg1);
		void on_PB_Settings_ClearMesh_clicked();

		/* Remeshing */
		void on_PB_Remeshing_Run_clicked();

		/* Tab widget stuff */
		void on_TW_View_currentChanged(int index);

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();
};

