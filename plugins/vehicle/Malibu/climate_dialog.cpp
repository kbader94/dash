#include "climate_dialog.h"
//#include "ui_climatedialog.h"
#include "app/widgets/climate_state.hpp"
#include <QWidget>

ClimateDialog::ClimateDialog(Arbiter &arbiter, bool fullscreen, QWidget *parent) :
    Dialog(arbiter, fullscreen, parent)

{

    this->parentClimate = dynamic_cast<ClimateState*>(parent);
    this->setupUI();
}


void ClimateDialog::setupUI(){


   auto widget = new QWidget();
   auto horizontalLayout = new QHBoxLayout(widget);
   //widget->addWidget(horizontalLayout);
   body = new ClimateState(this->arbiter, this);
   //body->keepDisplayed = true;
   body->toggle_body(true);
   horizontalLayout->addWidget(body);
  // connect(body, &ClimateState::pressed, [this, parentClimate = this->parentClimate](){
       //parentClimate->toggle_body(this->arbiter.vic().climate->toggleBody());
       //parentClimate->toggle_body();

  // });

   feet = new ClimateState(this->arbiter, this);
   //feet->keepDisplayed = true;
   feet->toggle_feet(true);
   horizontalLayout->addWidget(feet);
  // connect(feet, &ClimateState::pressed, [this, parentClimate = this->parentClimate]() {
       //parentClimate->toggle_feet(this->arbiter.vic().climate->toggleFeet());
  // });

   defrost = new QPushButton(this);
   defrost->setFlat(true);
   this->arbiter.forge().iconize("defrost", defrost, 26);
   horizontalLayout->addWidget(defrost);
   connect(defrost, &QPushButton::clicked, [this, parentClimate = this->parentClimate](){
       //this->parentClimate->toggle_defrost(this->arbiter.vic().climate->toggleDefrost());
   });

   this->set_body(widget);

}
