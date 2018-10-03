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
		void on_action_Open_triggered();
		void on_action_Quit_triggered();
		void on_checkBoxFill_toggled(bool checked);

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
};

