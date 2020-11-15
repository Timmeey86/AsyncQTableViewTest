#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtCore/QTimer>
#include <QtGui/QStandardItemModel>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui { new Ui::MainWindow }
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    // Create a database with a sample table
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    db.open();

    QSqlQuery query(db);
    query.exec("CREATE TABLE employees( ID INTEGER, name TEXT, phone TEXT, address TEXT)");
    const auto queryTemplate = QString("INSERT INTO employees(ID, name, phone, address) VALUES (%1, 'Bob_%1', '123456', 'Random Street 37b, 12345 Sometown')");
    for(auto row = 0; row < 100000; row++)
    {
        query.exec(queryTemplate.arg(row));
    }

    // Simulate a state machine for robust thread handling etc
    m_stateMachine = new ModelStateMachine();

    // Simulate a GUI which displays a manually managed model
    // We use a standard item model for simplicity. In a real application, this would probably be too slow and cause troubles with string conversion
    m_uiThreadModel = new QStandardItemModel(0, 4);
    ui->tableView->setModel(m_uiThreadModel);
    m_uiThreadModel->setHeaderData(0, Qt::Horizontal, "ID#");
    m_uiThreadModel->setHeaderData(1, Qt::Horizontal, "Name");
    m_uiThreadModel->setHeaderData(2, Qt::Horizontal, "Phone");
    m_uiThreadModel->setHeaderData(3, Qt::Horizontal, "Address");

    // Simulate a worker thread with its own database model, but do not load yet
    m_workerThread = new QThread();

    m_workerModel = new QSqlTableModel(nullptr);
    m_workerModel->setTable("employees");
    m_workerModel->moveToThread(m_workerThread);

    m_workerObject = new ModelUpdateHandler(m_workerModel, m_stateMachine);
    m_workerObject->moveToThread(m_workerThread);

    // Start the worker's event loop and make it wait for signals
    m_workerThread->start();

    // Connect UI buttons to state machine slots
    connect(ui->saveButton, &QPushButton::clicked, m_stateMachine, &ModelStateMachine::startDbCommit);
    connect(ui->reloadButton, &QPushButton::clicked, m_stateMachine, &ModelStateMachine::startDbLoad);

    // Make the worker thread reselect the model as soon as requested
    connect(m_stateMachine, &ModelStateMachine::dbLoadStateEntered, m_workerObject, &ModelUpdateHandler::reselectModel, Qt::QueuedConnection);

    // Update the model in the worker thread with the changes in the UI model automatically - This would require a complex synchronisation handler
    // in productive code most likely. Either Mutexes or a more detailed state machine will probably be required, too
    connect(m_uiThreadModel, &QAbstractItemModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
       if(roles.isEmpty() || roles.contains(Qt::EditRole))
       {
           for( auto row = topLeft.row(); row <= bottomRight.row(); row++ )
           {
               for( auto column = topLeft.column(); column <= bottomRight.column(); column++ )
               {
                   // Copy changes over to the worker thread model 1:1
                   m_workerModel->setData(m_workerModel->index(row, column), m_uiThreadModel->index(row, column).data());
               }
           }
       }
    });
    // Make the worker thread save the model as soon as requested. In productive code, you would have to make sure this can't happen before the slot above has finished its work.
    connect(m_stateMachine, &ModelStateMachine::dbCommitStateEntered, m_workerObject, &ModelUpdateHandler::commitModel);

    connect(m_stateMachine, &ModelStateMachine::uiUpdateStateEntered, this, &MainWindow::updateUI, Qt::QueuedConnection);
    connect(m_stateMachine, &ModelStateMachine::idleStateEntered, this, &MainWindow::enableUI);
    connect(m_stateMachine, &ModelStateMachine::idleStateLeft, this, &MainWindow::disableUI);

    // TODO: Detect when the scroll area is at the end, or write a custom model that can handle fetchMore()
}

void MainWindow::enableUI()
{
    ui->reloadButton->setEnabled(true);
    ui->saveButton->setEnabled(true);
}

void MainWindow::disableUI()
{
    // Note: This is just for visual feedback: If the user clicks twice before the event loop runs this code,
    //       the state machine would ignore the second click as a safety measure anyway.
    ui->saveButton->setEnabled(false);
    ui->reloadButton->setEnabled(false);
}

void MainWindow::updateUI()
{
    m_uiThreadModel->removeRows(0, m_uiThreadModel->rowCount());
    // Note: We are not using any mutexes since in this state, the worker thread does not do anything
    for(auto row = 0; row < m_workerModel->rowCount() - 1; row++)
    {
        m_uiThreadModel->appendRow(
        {
            new QStandardItem(m_workerModel->index(row, 0).data().toString()),
            new QStandardItem(m_workerModel->index(row, 1).data().toString()),
            new QStandardItem(m_workerModel->index(row, 2).data().toString()),
            new QStandardItem(m_workerModel->index(row, 3).data().toString()),
        });
    }

    m_stateMachine->finishUiUpdate();
}
