#pragma once

#include <QtWidgets>
#include <QPluginLoader>
#include <QStackedWidget>
#include <KWindowSystem>

#include "app/arbiter.hpp"
#include "app/config.hpp"
#include "app/window.hpp"
#include "app/session.hpp"
#include "app/window.hpp"
#include "app/pages/page.hpp"

class Arbiter;

class AppPage : public QWidget, public Page {
    Q_OBJECT

   public:
    AppPage(Arbiter &arbiter, QWidget *parent = nullptr);
    void init() override;

    WId id = 0;
    void setName(QString name);
    QString name();
   private:
  //  QString name;
    Arbiter *arbiter;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *icon_label;
    QSpacerItem *horizontalSpacer_2;
    QLabel *title_label;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *verticalSpacer;
    void setup_ui();
};

