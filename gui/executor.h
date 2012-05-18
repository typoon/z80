#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>
#include "mainwindow.h"

class Executor : public QObject
{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);
    
signals:
    
public slots:
    void execute(MainWindow* m);
    
};

#endif // EXECUTOR_H
