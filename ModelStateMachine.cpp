#include "ModelStateMachine.h"
#include <QtCore/QDebug>

ModelStateMachine::ModelStateMachine()
{
    Idle = new QState();
    LoadingFromDB = new QState();
    UpdatingUI = new QState();
    CommittingToDB = new QState();

    Idle->addTransition(this, &ModelStateMachine::dbLoadRequested, LoadingFromDB);
    LoadingFromDB->addTransition(this, &ModelStateMachine::uiUpdateRequested, UpdatingUI);

    Idle->addTransition(this, &ModelStateMachine::dbCommitRequested, CommittingToDB);
    CommittingToDB->addTransition(this, &ModelStateMachine::uiUpdateRequested, UpdatingUI);

    UpdatingUI->addTransition(this, &ModelStateMachine::uiUpdateFinished, Idle);

    // Forward signals
    connect(Idle, &QState::entered, this, &ModelStateMachine::idleStateEntered);
    connect(Idle, &QState::exited, this, &ModelStateMachine::idleStateLeft);
    connect(LoadingFromDB, &QState::entered, this, &ModelStateMachine::dbLoadStateEntered);
    connect(CommittingToDB, &QState::entered, this, &ModelStateMachine::dbCommitStateEntered);
    connect(UpdatingUI, &QState::entered, this, &ModelStateMachine::uiUpdateStateEntered);

    m_machine.addState(Idle);
    m_machine.addState(LoadingFromDB);
    m_machine.addState(CommittingToDB);
    m_machine.addState(UpdatingUI);
    m_machine.setInitialState(Idle);
    m_machine.start();
}

void ModelStateMachine::startDbLoad()
{
    if(!Idle->active())
    {
        qDebug() << "Skipping DB Load as not in idle state";
        return;
    }
    qDebug() << "Switching to LoadingFromDB state";
    emit dbLoadRequested();
}

void ModelStateMachine::finishDbLoad()
{
    if(!LoadingFromDB->active())
    {
        qDebug() << "Skipping DB Load finish as not in DB loading state";
        return;
    }
    qDebug() << "Switching to UpdatingUI state";
    emit uiUpdateRequested();
}

void ModelStateMachine::startDbCommit()
{
    if(!Idle->active())
    {
        qDebug() << "Skipping DB Commit as not in idle state";
        return;
    }
    qDebug() << "Switching to CommittingToDB state";
    emit dbCommitRequested();
}

void ModelStateMachine::finishDbCommit()
{
    if(!CommittingToDB->active())
    {
        qDebug() << "Skipping finishDbCommit as not in CommittingToDB state";
        return;
    }
    qDebug() << "Switching to UpdatingUI state";
    emit uiUpdateRequested();
}

void ModelStateMachine::finishUiUpdate()
{
    if(!UpdatingUI->active())
    {
        qDebug() << "Skipping finishUiUpdate as not in UpdatingUI state";
        return;
    }
    qDebug() << "Switching to idle state";
    emit uiUpdateFinished();
}
