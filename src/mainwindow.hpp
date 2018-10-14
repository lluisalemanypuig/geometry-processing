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

	private slots:
		/* Menu buttons */
		void on_action_Open_triggered();
		void on_action_Quit_triggered();

		/* Interface elements */
		void on_cBWireframe_toggled(bool checked);

		/* Curvature radio buttons */
		void on_rBCurvatureH_toggled(bool checked);
		void on_rBCurvatureG_toggled(bool checked);
		void on_rBNoCurvature_toggled(bool checked);

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
};

