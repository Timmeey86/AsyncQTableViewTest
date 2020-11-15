#include "ModelUpdateHandler.h"
#include "ModelStateMachine.h"
#include <QtSql/QSqlTableModel>

ModelUpdateHandler::ModelUpdateHandler(QSqlTableModel* model, ModelStateMachine* stateMachine)
    : QObject()
    , m_model{ model }
    , m_stateMachine{ stateMachine }
{
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

void ModelUpdateHandler::reselectModel()
{
    m_model->select();
    m_stateMachine->finishDbLoad();
}

void ModelUpdateHandler::commitModel()
{
    m_model->submitAll();
    m_stateMachine->finishDbCommit();
}
