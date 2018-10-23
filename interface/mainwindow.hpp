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
		void on_CBSolid_toggled(bool checked);
		void on_CBWireframe_toggled(bool checked);
		void on_CBRefLines_toggled(bool checked);

		/* Curvature radio buttons */
		void on_RBCurvatureH_toggled(bool checked);
		void on_RBCurvatureG_toggled(bool checked);
		void on_RBNoCurvature_toggled(bool checked);

		/* Performance options */
		void on_SBThreads_valueChanged(int arg1);

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();
};

