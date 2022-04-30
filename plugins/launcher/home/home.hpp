#pragma once

#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QProcess>
#include <QtWidgets>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

#include <KWindowSystem>
#include "app/arbiter.hpp"
#include "app/pages/page.hpp"
#include "app/pages/app.hpp"
#include "app/pages/media.hpp"
#include "plugins/launcher_plugin.hpp"

#include <cstdlib>

class Arbiter;
class Config;
class Theme;
class Home;
class DesktopEntry;
class ILauncherPlugin;
class XManagedApp;
class Session;

/* 
* XManager */
class XManager : public QObject{
    Q_OBJECT

public:
    XManager(ILauncherPlugin *plugin);
    ~XManager();
    ILauncherPlugin *plugin;
    void startApp(DesktopEntry *entry);//TODO: args
    void killApp(WId id);
    void setWindowPositions();

public slots:

    //wm slots
    void activeWindowChanged(WId id);  
    void windowAdded(WId id);
    void windowRemoved(WId id);

    //dash slots
    void mode_changed(Session::Theme::Mode mode);

private:
    void checkLoadingApps();
    bool containsApp(WId id);
    bool containsApp(int tabIdx);
    int  countPId(int pid);
    XManagedApp* getAppFromWId(WId id);
    XManagedApp* getAppFromIndex(int tabIdx);
    XManagedApp* getAppFromPID(int PID);
    void updateAppWId(WId id);
    XManagedApp* addApp(int PID, DesktopEntry *entry, QProcess *proc);
    XManagedApp* addApp(int PID, QIcon icon);
    XManagedApp* addApp(WId wid);
    void removeApp(WId id);
    void removeApp(XManagedApp *app);
    void removeAppAtIndex(int tabIdx);
    QList<XManagedApp *> apps_;
};

class XManagedApp : public QObject{
    Q_OBJECT

public:
    XManagedApp(XManager *xMan, int PID, DesktopEntry *entry);
    XManagedApp(XManager *xMan, int PID);
    ~XManagedApp();
    
    void setWID(WId id) { wId_ = id; };
    WId getWId() {return this->wId_; };
    AppPage *appPage;
    int index = 0;

    QWidget *tab;
    DesktopEntry *entry = NULL;
    QProcess *proc = NULL;
    int PID;

private:
    XManager *xMan_;  
    void open();
    WId wId_ = 0;

};

class ILauncherPlugin : public QObject, LauncherPlugin{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID LauncherPlugin_iid FILE "home.json")
    Q_INTERFACES(LauncherPlugin)

public:
    ILauncherPlugin();
    QList<QWidget *> widgets() override;
    Home *home;
    QTabWidget *tabs;
    Arbiter *arb;
    XManager *xMan_;

    //void remove_widget(int idx) override;
    void add_widget(QWidget *widget) override;
    
    signals:
    void widget_added(QWidget *widget) Q_DECL_FINAL;
   // void app_opened(WId id, QString name, QIcon icon) Q_DECL_FINAL;
//    void page_changed(int idx) Q_DECL_FINAL;

    private:
    void init(Arbiter *arbiter, QTabWidget *tabWidget) override;
    
};

class Home : public QWidget {
    Q_OBJECT

   public:
    Home(Arbiter *arbiter, QSettings &settings, int idx, ILauncherPlugin *plugin, QWidget *parent = nullptr);
    void update_idx(int idx);
    QList<DesktopEntry*> entries;

protected:

    void resizeEvent(QResizeEvent *event) override;

   private:
    //const QString DEFAULT_DIR = QDir().absolutePath();
    QGridLayout *entries_grid;
    QWidget *container;
    QScrollArea *scroll_area;
    QStackedLayout *layout;
    QWidget *config_widget();//not implemented
  
    Arbiter *arbiter;
    QSettings &settings;
    ILauncherPlugin *plugin;
    int idx;

    void setup_ui();
   
};

class DesktopEntry : public QWidget{
    Q_OBJECT

public:
    DesktopEntry(QString fileLocation, Arbiter *arbiter, ILauncherPlugin *plugin, QWidget *parent = nullptr);
    
    static QList<DesktopEntry *> get_entries(Arbiter *arbiter, ILauncherPlugin *plugin);
    static int resolutionFromString(QString string); 
    inline QString get_exec() { return this->exec_; };
    inline QString get_icon() { return this->icon_; };
    inline QString get_name() { return this->name_; };
    inline QString get_path() { return this->path_; };
    inline QList<QString> get_args() {return this->args_;};
    QPixmap get_pixmap();
    QWidget *get_widget();

public slots:

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *iconLabel;
    QLabel *nameLabel;

    Arbiter *arbiter;
    ILauncherPlugin *plugin;
    QString exec_;
    QString path_;
    QString name_;
    QString icon_;
    QList<QString> args_;

    void setup_ui();

};

