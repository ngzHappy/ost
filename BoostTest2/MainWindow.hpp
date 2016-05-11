#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    bool event(QEvent *)override;
};

#endif // MAINWINDOW_HPP
