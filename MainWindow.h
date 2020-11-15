#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/QStandardItemModel>
#include <QtCore/QThread>
#include <QtSql/QSqlTableModel>
#include "ModelStateMachine.h"
#include "ModelUpdateHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setup();

private slots:
    void updateUI();
    void enableUI();
    void disableUI();
private:
    Ui::MainWindow *ui;
    QStandardItemModel* m_uiThreadModel;

    ModelStateMachine* m_stateMachine;

    QThread* m_workerThread;
    QSqlTableModel* m_workerModel;
    ModelUpdateHandler* m_workerObject;
};
#endif // MAINWINDOW_H
