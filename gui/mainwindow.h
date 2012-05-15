#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void exit();
    void runProgram();
    void resetCPU();
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setupUi();

private:
    QMenuBar* menuBar;
    QToolBar* toolBar;
    QGridLayout* layout;


    // Screen components
    QPlainTextEdit* cmpntCode;
    QGroupBox* cmpntRegGroup;
    QLineEdit* lnAccumulator;
    QLineEdit* lnFlags;
    QLineEdit* lnB;
    QLineEdit* lnC;
    QLineEdit* lnD;
    QLineEdit* lnE;
    QLineEdit* lnH;
    QLineEdit* lnL;
    QLineEdit* lnAccumulator2;
    QLineEdit* lnFlags2;
    QLineEdit* lnB2;
    QLineEdit* lnC2;
    QLineEdit* lnD2;
    QLineEdit* lnE2;
    QLineEdit* lnH2;
    QLineEdit* lnL2;


    // Toolbar items
    QAction* toolRun;
    QAction* toolReset;

    // File menu
    QMenu*    fileMenu;
    QAction*  fileExit;



};

#endif // MAINWINDOW_H
