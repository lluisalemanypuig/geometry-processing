#pragma once

// Qt includes
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_action_Quit_triggered();

	void on_checkBoxFill_toggled(bool checked);

	void on_action_Open_triggered();

private:
	Ui::MainWindow *ui;
};

