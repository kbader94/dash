#ifndef CLIMATEDIALOG_H
#define CLIMATEDIALOG_H

//#include <QDialog>
//#include <QButton>
#include <QtWidgets/QPushButton>
#include "app/widgets/dialog.hpp"
#include "app/widgets/climate_state.hpp"
#include "app/arbiter.hpp"

class Arbiter;

class ClimateDialog : public Dialog
{  


public:
    ClimateDialog(Arbiter &arbiter, bool fullscreen, QWidget *parent = nullptr);

    ClimateState *body;
    ClimateState *feet;
    QPushButton *defrost;
    ClimateState *parentClimate;


private:
    void setupUI();
};

#endif // CLIMMATEDIALOG_H
