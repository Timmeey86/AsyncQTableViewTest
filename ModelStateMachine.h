#ifndef MODELSTATEMACHINE_H
#define MODELSTATEMACHINE_H

#include <QtCore/QStateMachine>


class ModelStateMachine : public QObject
{
    Q_OBJECT


public:
    ModelStateMachine();


public slots:

    void startDbLoad();
    void finishDbLoad();

    void startDbCommit();
    void finishDbCommit();

    void finishUiUpdate();

signals:

    // Used for connecting to the state machine
    void dbLoadStateEntered();
    void dbCommitStateEntered();
    void uiUpdateStateEntered();
    void idleStateEntered();
    void idleStateLeft();

    // Used for internally advancing the state machine
    void dbLoadRequested();
    void uiUpdateRequested();
    void dbCommitRequested();
    void uiUpdateFinished();

private:

    QStateMachine m_machine;
    QState* Idle; ///< The machine is waiting for input.
    QState* LoadingFromDB; ///< The user wants to load data from the database. UI Interaction is possible, but changes are not.
    QState* CommittingToDB; ///< The user wants to push his changes to the database. UI Interaction is not possible.
    QState* UpdatingUI; ///< The UI is being updated. UI Interaction is not possible.
};

#endif // MODELSTATEMACHINE_H
