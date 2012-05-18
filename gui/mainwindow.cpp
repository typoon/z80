#include <QMenuBar>
#include <QToolBar>
#include <QCoreApplication>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTime>


#include <QThread>


#include "mainwindow.h"
#include "mylabel.h"
#include "z80/z80.h"
#include "z80/general.h"
#include "executor.h"

//#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setupUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::exit()
{
    QCoreApplication::quit();
}

void MainWindow::runProgram() {
    // This should be a separate thread, so I can unblock the GUI in order
    // to send interrupts and stuff like that

    //QFuture<void> future = QtConcurrent::run(SLOT(runProgramAsync()));
     //QTimer::singleShot(0, this, SLOT(runProgramAsync()));
    //QMetaObject::invokeMethod(this, "runProgramAsync");

    QThread *t = new QThread();
    Executor *exec = new Executor();

    exec->moveToThread(t);
    t->start();
    qRegisterMetaType<MainWindow*>("MainWindow*");
    QMetaObject::invokeMethod(exec, "execute",  Qt::QueuedConnection,
                              Q_ARG(MainWindow*, this));

}

void MainWindow::runProgramAsync()
{

    int i;
    z80 *z = new_z80(FREQ);
    uchar opcode;
    const double cycles_threshold = (FREQ * THRESHOLD_MS) / 1000;
    char get_time = 1;
    QTime* start_time = new QTime(0,0,0,0);
    int remaining_time = 0;
    memory *mem;

    mem = (memory *)malloc(sizeof(memory) * MEMORY);
    memset(mem, 0, MEMORY * sizeof(memory));

    start_time->start();
    z->allocated_memory = MEMORY;
    z->m = mem;

    mem[0].data = 0x0E; // LD C, 0
    mem[1].data = 0x00; // LD C, 0
    mem[2].data = 0x06; // LD B, 0x10
    mem[3].data = 0x02; // LD B, 0x10
    mem[4].data = 0x0C; // INC C
    mem[5].data = 0x10; // DJNZ -3
    mem[6].data = 0xFD; // DJNZ -3
    mem[7].data = 0x76; // HALT

    for(;;) {

        //if(z->halt != 0) {
        //    break;
        //}

        if(get_time == 1) {
            start_time->restart();
            get_time = 0;
        }

        opcode = z->fetch(z);
        if(z->decode_exec(z, opcode) == ERROR) {
            printf("Invalid opcode [%02X]. PC: [%04X]\n", opcode, z->pc-1);
        }

        // The CPU is capable of executing FREQ cycles per second
        // Every 10 milliseconds, we should delay the execuion, so it does not
        // run too fast
        // 1 second - FREQ
        // 10 millisecond - x
        //
        // x = FREQ * THRESHOLD_MS * 10^-3

        if(z->cycles >= cycles_threshold) {
            printf("i = %d\n", i++);
            while((remaining_time = (THRESHOLD_MS * 1000) - start_time->elapsed()) > 0);
            z->cycles -= cycles_threshold;
            get_time = 1;
        }

        // TODO: Check interrupts here?

        // TODO: Check reset here?

        //

        // Shwo the values in their places
        QString str;
        lnAccumulator->setText(str.sprintf("%02X", z->a));
        lnFlags->setText(str.sprintf("%02X", z->f.flags));
        lnB->setText(str.sprintf("%02X", z->b));
        lnC->setText(str.sprintf("%02X", z->c));
        lnD->setText(str.sprintf("%02X", z->d));
        lnE->setText(str.sprintf("%02X", z->e));
        lnH->setText(str.sprintf("%02X", z->h));
        lnL->setText(str.sprintf("%02X", z->l));

        lnAccumulator2->setText(str.sprintf("%02X", z->a2));
        lnFlags2->setText(str.sprintf("%02X", z->f2.flags));
        lnB2->setText(str.sprintf("%02X", z->b2));
        lnC2->setText(str.sprintf("%02X", z->c2));
        lnD2->setText(str.sprintf("%02X", z->d2));
        lnE2->setText(str.sprintf("%02X", z->e2));
        lnH2->setText(str.sprintf("%02X", z->h2));
        lnL2->setText(str.sprintf("%02X", z->l2));

        // lnFlags->setText(&z->f);

    }

    printf("Fim\n");

    delete start_time;
    free(z);
    free(mem);

}

void MainWindow::resetCPU()
{
    cmpntCode->insertPlainText("funciona reset!");
}

void MainWindow::setupUi()
{
    QIcon::setThemeName("tango");

    cmpntCode = new QPlainTextEdit();
    cmpntRegGroup = new QGroupBox();

    // Layout setup
    layout = new QGridLayout();
    layout->setColumnMinimumWidth(0, this->width()/2);
    layout->setColumnMinimumWidth(1, this->width()/2);

    // Setup group box with registers
    QFormLayout* layoutReg = new QFormLayout();

    MyLabel* lblAccumulator = new MyLabel("Accumulator");
    MyLabel* lblFlags = new MyLabel("Flags");
    MyLabel* lblB = new MyLabel("B");
    MyLabel* lblC = new MyLabel("C");
    MyLabel* lblD = new MyLabel("D");
    MyLabel* lblE = new MyLabel("E");
    MyLabel* lblH = new MyLabel("H");
    MyLabel* lblL = new MyLabel("L");

    MyLabel* lblAccumulator2 = new MyLabel("Accumulator'");
    MyLabel* lblFlags2 = new MyLabel("Flags'");
    MyLabel* lblB2 = new MyLabel("B'");
    MyLabel* lblC2 = new MyLabel("C'");
    MyLabel* lblD2 = new MyLabel("D'");
    MyLabel* lblE2 = new MyLabel("E'");
    MyLabel* lblH2 = new MyLabel("H'");
    MyLabel* lblL2 = new MyLabel("L'");

    lnAccumulator = new QLineEdit();
    lnFlags = new QLineEdit();
    lnB = new QLineEdit();
    lnC = new QLineEdit();
    lnD = new QLineEdit();
    lnE = new QLineEdit();
    lnH = new QLineEdit();
    lnL = new QLineEdit();

    lnAccumulator2 = new QLineEdit();
    lnFlags2 = new QLineEdit();
    lnB2 = new QLineEdit();
    lnC2 = new QLineEdit();
    lnD2 = new QLineEdit();
    lnE2 = new QLineEdit();
    lnH2 = new QLineEdit();
    lnL2 = new QLineEdit();

    layoutReg->addRow(lblAccumulator, lnAccumulator);
    layoutReg->addRow(lblFlags, lnFlags);
    layoutReg->addRow(lblB, lnB);
    layoutReg->addRow(lblC, lnC);
    layoutReg->addRow(lblD, lnD);
    layoutReg->addRow(lblE, lnE);
    layoutReg->addRow(lblH, lnH);
    layoutReg->addRow(lblL, lnL);

    QFrame* line = new QFrame();
    line->setFrameStyle(QFrame::HLine);
    layoutReg->addRow(line);

    layoutReg->addRow(lblAccumulator2, lnAccumulator2);
    layoutReg->addRow(lblFlags2, lnFlags2);
    layoutReg->addRow(lblB2, lnB2);
    layoutReg->addRow(lblC2, lnC2);
    layoutReg->addRow(lblD2, lnD2);
    layoutReg->addRow(lblE2, lnE2);
    layoutReg->addRow(lblH2, lnH2);
    layoutReg->addRow(lblL2, lnL2);

    cmpntRegGroup->setLayout(layoutReg);


    // Add components to layout
    layout->addWidget(cmpntCode, 0, 0);
    layout->addWidget(cmpntRegGroup, 0, 1);

    // Setup menu
    menuBar = QMainWindow::menuBar();

    // Build file menu
    fileMenu = menuBar->addMenu(tr("&File"));
    fileExit = fileMenu->addAction(QIcon::fromTheme("application-exit"),
                                   tr("&Exit"),
                                   this, SLOT(exit()),
                                   QKeySequence::Quit);

    toolBar = new QToolBar();
    addToolBar(toolBar);

    toolRun = toolBar->addAction(QIcon::fromTheme("media-playback-start"),
                                 tr("Run"),
                                 this, SLOT(runProgram()));

    toolReset = toolBar->addAction(QIcon::fromTheme("media-playback-start"),
                                 tr("Reset"),
                                 this, SLOT(resetCPU()));

    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

}
