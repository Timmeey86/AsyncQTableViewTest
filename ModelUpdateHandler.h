#ifndef MODELUPDATEHANDLER_H
#define MODELUPDATEHANDLER_H

#include <QObject>

class QSqlTableModel;
class ModelStateMachine;

class ModelUpdateHandler : public QObject
{
    Q_OBJECT
public:
    explicit ModelUpdateHandler(QSqlTableModel* model, ModelStateMachine* stateMachine);

signals:

public slots:

    void reselectModel();
    void commitModel();

private:

    QSqlTableModel* m_model;
    ModelStateMachine* m_stateMachine;

};

#endif // MODELUPDATEHANDLER_H
