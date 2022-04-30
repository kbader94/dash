#include <QElapsedTimer>
#include <unistd.h>
#include <QString>
#include <QProcessEnvironment>
#include <QDBusMessage>
#include <QDBusConnection>

#include "app/config.hpp"
#include "app/arbiter.hpp"
#include "app/session.hpp"
#include "app/pages/launcher.hpp"

#include "home.hpp"

XManager::XManager(ILauncherPlugin *plugin)
{

    this->plugin = plugin;
    QTabWidget *tabs = this->plugin->tabs;
    //tabs->setTabsClosable(true);

    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &XManager::windowAdded);
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &XManager::windowRemoved);
    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this, &XManager::activeWindowChanged);
    connect(this->plugin->tabs, &QTabWidget::tabCloseRequested, this, [tabs, this](int idx){
        this->removeApp(idx);
    });
    connect(plugin->arb, &Arbiter::mode_changed, this, &XManager::mode_changed);
    //set state on main dash window to dock
    KWindowSystem::setType(tabs->winId(), NET::WindowType::Dock); 
    KWindowSystem::setState(tabs->winId(), NET::Sticky | NET::SkipTaskbar |
                            NET::SkipPager | NET::SkipSwitcher);

    //On page change, focus corresponding window if page is an appPage
    connect(this->plugin->arb, &Arbiter::curr_page_index_changed, this, [this](int index){
        
        for(auto app : apps_){

            KWindowSystem::minimizeWindow(app->getWId());

        }

        //check if current page is an AppPage 
        auto page = this->plugin->arb->get_curr_page();
        auto appPage = dynamic_cast<AppPage*>(page);
        if (appPage != nullptr)
        {
            
            //TODO: return if page already active to prevent flickering

            auto id = appPage->id;
            if(id != 0)
            {
                KWindowSystem::forceActiveWindow(id);
            }

        }

    });

}

XManager::~XManager(){
  
}

/*
 * Set size & position of windows inside of Kwin Config according to 
 * size of Dash App Body content
 */
void XManager::setWindowPositions()
{

    QString filename = QDir::homePath() + "/.config/kwinrulesrc";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&file);
        QScreen *screen = QGuiApplication::primaryScreen();

        QRect homeRect = plugin->home->geometry();
        QPoint topLeft = QPoint(0,0);
        int width = homeRect.width();
        int height = homeRect.height();
        int x = screen->geometry().width() - width;
        int y = plugin->home->mapToGlobal(topLeft).y(); 

        qDebug() << "x: " << x;
        qDebug() << "y: " << y;
        
        auto command = "size=" + QString::number(width) + "," + QString::number(height);

        QByteArray ca = command.toLocal8Bit();
        const char *c_str = ca.data();
        
        stream << "[1]" << endl;
        stream << "Description=Dash" << endl;
        stream << "ignoregeometry=false" << endl;
        stream << "ignoregeometryrule=2" << endl;
        stream << "noborder=true" << endl;
        stream << "noborderrule=2" << endl;
        stream << "position=" << x << "," << y << endl;
        stream << "positionrule=2" << endl;
        stream << c_str << endl;
        stream << "sizerule=2" << endl;
        stream << "strictgeometry=true" << endl;
        stream << "strictgeometryrule=2" << endl;
        stream << "types=353" << endl;
        stream << "wmclass=^((?!dash).)*$" << endl;
        stream << "wmclasscomplete=false" << endl;
        stream << "wmclassmatch=3" << endl;
        stream << "" << endl;
        stream << "[General]" << endl;
        stream << "count=1" << endl;

        //update window positions
        QDBusMessage message = QDBusMessage::createSignal("/KWin","org.kde.KWin", "reloadConfig");
        QDBusConnection::sessionBus().send(message);

        file.close();
    }            

}

void XManager::activeWindowChanged (WId id){

    if(!this->containsApp(id))
        return;

    //TODO: remove? or set active page

} 
void XManager::windowAdded (WId id){

    qDebug() << "[XManager] Received Request to add window " << id;

    KWindowInfo info(id, NET::WMWindowType | NET::WMState | NET::WMName);

    if(!info.valid() || info.hasState(NET::SkipTaskbar))
        return;

    if(info.windowType(NET::AllTypesMask) == NET::Desktop)
        return;

    if(info.windowType(NET::AllTypesMask) == NET::Dock)
        return;

    if(info.visibleName() == "dash")
        return;    
    qDebug() << "[XManager] Adding Window " << id;

    if(plugin->arb->initialized) updateAppWId(id);

}

void XManager::windowRemoved(WId id){

    qDebug() << "[XManager] Received request to remove window " << id;
    
    this->removeApp(id);
     
}

void XManager::mode_changed(Session::Theme::Mode mode){

    //TODO: set theme

}

bool XManager::containsApp(WId id){

    for(XManagedApp *app : this->apps_){

        if(app->getWId() == id) 
            return true;

    }
    return false;

}

//TODO: eliminate! this was for old tab based launcher
bool XManager::containsApp(int idx){

    for(XManagedApp *app : this->apps_){

        if(this->plugin->tabs->indexOf(app->tab) == idx) 
            return true;

    }
    return false;

}


//REFACTOR: rename to updateApps!!
void XManager::checkLoadingApps()
{

    //get untracked apps, and add untracked apps
    QList<WId> windows = KWindowSystem::windows();
    for (auto wid: windows) {
        bool tracked = false;
        for(auto app: this->apps_){
            
            if(app->getWId() == wid){
                tracked = true;
                break;
            }    

        }

        if(!tracked && plugin->arb->initialized){
            addApp(wid);
        }

    }

    //remove tracked apps that aren't running
    //if getWId == 0 it's probably a loading placeholder
    for(auto app: this->apps_){
        
        if(app->getWId() == 0 || !windows.contains(app->getWId())){
            if(app->proc != NULL && app->proc->atEnd() == true){
                removeApp(app);
            }
        }
        
    }

}

/* 
 *  Add App before you have the WId and info(ie just the Desktop Entry and PID)
 *  Displays icon(loading) and adds page
 *  TODO: create loading animation, and possibly refactor as separate function
 */
XManagedApp* XManager::addApp(int PID, DesktopEntry *entry, QProcess *proc)
{
    //return NULL;//TODO: temporary disable
    qDebug() << "Adding App with PID" << PID;
    XManagedApp *app = new XManagedApp(this,PID);
    app->proc = proc;
    this->apps_.push_back(app);

    auto qico = QIcon::fromTheme(entry->get_icon());
    QPixmap ico = qico.pixmap(qico.actualSize(QSize(128, 128)));
    app->appPage = new AppPage(*plugin->arb);
    this->plugin->arb->add_page(app->appPage, ico);

    return app;
}

/* 
 *  TODO: eliminate, made redundant by addApp WID overload
 */
XManagedApp* XManager::addApp(int PID, QIcon icon)
{
    XManagedApp *app = new XManagedApp(this,PID);
    this->apps_.push_back(app);

    QPixmap ico = icon.pixmap(icon.actualSize(QSize(128, 128)));
    app->appPage = new AppPage(*plugin->arb);
    this->plugin->arb->add_page(app->appPage, ico);
    
    return app; 
}

XManagedApp* XManager::addApp(WId wid){
    KWindowInfo info(wid, NET::WMPid | NET::WMWindowType | NET::WMState | NET::WMName);

    //TODO: create function for checking if we should manage this window, based on window info
    if(!info.valid() || info.hasState(NET::SkipTaskbar))
        return NULL;

    if(info.windowType(NET::AllTypesMask) == NET::Desktop)
        return NULL;

    if(info.windowType(NET::AllTypesMask) == NET::Dock)
        return NULL;

    if(info.visibleName() == "dash")
        return NULL;    
                    
    auto PID = info.pid();
    auto icon = KWindowSystem::icon(wid,64,64);
    XManagedApp *app = new XManagedApp(this,PID);
    this->apps_.push_back(app);

    //QPixmap ico = icon.pixmap(icon.actualSize(QSize(128, 128)));
    app->appPage = new AppPage(*plugin->arb);
    this->plugin->arb->add_page(app->appPage, icon);

    //Set Wid on app
    app->setWID(wid);

    //Set Wid on Page TODO: move to setWID()
    app->appPage->id = wid;

    app->appPage->setName(info.name());
    return app; 
}

/*
 *  Updates existing app with Wid Info(Window is open now)
 * TODO: refactor: rename to.. idk something more intuitive!
 */
void XManager::updateAppWId(WId wid){

    
    checkLoadingApps();

    auto app = this->getAppFromWId(wid);
    emit plugin->arb->curr_page_changed(app->appPage);

}

void XManager::removeApp(XManagedApp *app)
{

    this->plugin->arb->remove_page(app->appPage);
    this->apps_.removeOne(app);
    qDebug() << "[XManager] App Removed with PID" << app->PID;

}

void XManager::removeApp(WId id){

    XManagedApp *app = this->getAppFromWId(id);
    if(app == NULL) return;
    this->plugin->arb->remove_page(app->appPage);
    this->apps_.removeOne(app);
    qDebug() << "[XManager] App Removed " << id;

}

//TODO: remove, was used for old code
void XManager::removeAppAtIndex(int idx){

    if(!this->containsApp(idx))
        return;
        
    XManagedApp *app = this->getAppFromIndex(idx);
    QTabWidget *tabs = this->plugin->tabs;
    tabs->removeTab(idx);
    //delete app;
    this->apps_.removeOne(app);
    qDebug() << "[XManager] App Removed " << app->getWId();

}

XManagedApp *XManager::getAppFromWId(WId id){

    for(XManagedApp *app : this->apps_){

        if(app->getWId() == id) 
            return app;

    }
    return nullptr;
}

//TODO: remove
XManagedApp *XManager::getAppFromIndex(int idx){

    for(XManagedApp *app : this->apps_){
        
        if(app->index == idx)
            return app;

    }
    return nullptr;

}

XManagedApp *XManager::getAppFromPID(int PID)
{

    for(XManagedApp *app : this->apps_){
        
        if(app->PID == PID)
            return app;

    }
    return NULL;

}

//TODO: remove? not likely used
int XManager::countPId(int pid){

    int cnt = 0;
    for(XManagedApp *app : this->apps_){

        KWindowInfo info(app->getWId(), NET::WMPid);
        if(info.pid() == pid)
            cnt++;

    }
    return cnt;

}

void XManager::startApp(DesktopEntry *entry){

    QProcess *proc = new QProcess;
    proc->setProgram(entry->get_exec());
    proc->setStandardOutputFile(QProcess::nullDevice());
    proc->setStandardErrorFile(QProcess::nullDevice());
    qint64 pid;
    proc->startDetached(&pid);
    
    this->addApp(pid, entry, proc);

}

void XManager::killApp(WId id){

    KWindowInfo info(id, NET::WMPid);
    if (info.valid() && countPId(info.pid()) == 1){
        
        //TODO: make sure no other windows use this pid first
        auto command = "kill -15 " + QString::number(info.pid());
        QByteArray ca = command.toLocal8Bit();
        const char *c_str = ca.data();
        qDebug() << "[XManager][System] Sending Kill Command to " << info.pid();
        system(c_str);

    }

    //remove from apps!
}


XManagedApp::XManagedApp(XManager *xMan, int PID, DesktopEntry *entry){
    this->xMan_ = xMan;
    this->PID = PID;
    this->entry = entry;
   // this->open();//TODO: refactor, remove wId, not actually used rn, should be reparent
}

XManagedApp::XManagedApp(XManager *xMan, int PID)
{
    this->xMan_ = xMan;
    this->PID = PID;
}

XManagedApp::~XManagedApp(){

  this->xMan_->killApp(this->getWId());

}

void XManagedApp::open(){

    KWindowInfo info(this->wId_, NET::WMWindowType | NET::WMState | NET::WMName);
    qDebug() << "Opening Window";
    qDebug() << "Window name: " << info.visibleName();
    qDebug() << "PID: " << this->PID;
   
    QTabWidget *tabs = this->xMan_->plugin->tabs;
    this->tab = new QWidget(tabs);
    tabs->addTab(this->tab, info.visibleName());
    //this->tabIdx = tabs->indexOf(this->tab);

    //get icon
   // for(auto entry : this->xMan_->plugin->home->entries){

   // }


    connect(this->xMan_->plugin->tabs, &QTabWidget::currentChanged, this, [this, tabs](){

        if(tabs->currentIndex() == tabs->indexOf(this->tab)){

                qDebug() << "[XManagedApp] activating window " << tabs->indexOf(this->tab);
                KWindowSystem::forceActiveWindow(this->getWId());
            
        }

    });

}

ILauncherPlugin::ILauncherPlugin(){

   
}

void ILauncherPlugin::init(Arbiter *arbiter, QTabWidget *tabs){
    
    this->dashize(arbiter);
    this->arb = arbiter;
    this->tabs = tabs;
    this->home = new Home(this->arb, this->settings, 0, this);
    this->loaded_widgets.push_front(this->home);
    this->xMan_ = new XManager(this);

}

QList<QWidget *> ILauncherPlugin::widgets()
{
    return this->loaded_widgets;
}

void ILauncherPlugin::add_widget(QWidget *widget){

    this->loaded_widgets.push_back(widget);
    emit widget_added(widget);

}

Home::Home(Arbiter *arbiter, QSettings &settings, int idx, ILauncherPlugin *plugin, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
    , settings(settings)
    , idx(idx)
{
    this->plugin = plugin;
    
    this->setup_ui();
    
}

void Home::setup_ui()
{
    this->setObjectName("Home");
    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    container = new QWidget(this);
    entries = DesktopEntry::get_entries(this->arbiter, this->plugin);
    entries_grid = new QGridLayout(container);
    container->setLayout(entries_grid);
    QScrollArea *scroll_area = new QScrollArea(this);
    Session::Forge::to_touch_scroller(scroll_area);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(container);
    layout->addWidget(scroll_area);

    //TODO: detect width and set appropriately
    int x = 0;
    int y = 0;
    for (int i = 0; i < entries.count(); ++i){

        DesktopEntry *entry = entries[i];
        connect(entry, &DesktopEntry::clicked, [this, entry]() {

            this->plugin->xMan_->startApp(entry);
       
        });
        entries_grid->addWidget(entry, y, x, 1, 1);
        //set x & y
        if(x > 6) {
            x = 0;
            y += 1;
        } else {
            x += 1;
        }

    }
  
}

void Home::resizeEvent(QResizeEvent *event)
{
    qDebug() << this->width();
    qDebug() << this->height();
    this->plugin->xMan_->setWindowPositions();
    QWidget::resizeEvent(event);
}

void Home::update_idx(int idx){
    //TODO: this does nothing..
    if (idx == this->idx)
        return;

    this->idx = idx;

}

QWidget *Home::config_widget()
{
    //NOT IMPLEMENTED
    QWidget *widget = new QWidget(this);
    return widget;
}

DesktopEntry::DesktopEntry(QString fileLocation, Arbiter *arbiter, ILauncherPlugin *plugin, QWidget *parent)
{
    this->arbiter = arbiter;
    this->plugin = plugin;

    QFile inputFile(fileLocation);
    inputFile.open(QIODevice::ReadOnly);

    if (!inputFile.isOpen()) return;  
    //parse desktop entry
    QTextStream stream(&inputFile);
    for (QString line = stream.readLine();
      !line.isNull();
      line = stream.readLine())
    {

        if(line.contains("Exec")){
            QStringList vals = line.split( "=" );
            this->exec_ = vals.value(1);

            QStringList args = vals.value(1).split( " " );
            this->exec_ = args.value(0);
            args.removeAt(0);
            this->args_ = args;
           
        }

        if(line.contains("Name=") && line.indexOf("Name=") == 0){
            QStringList vals = line.split( "=" );
            this->name_ = vals.value(1);
        }

        if(line.contains("Icon")){
            QStringList vals = line.split( "=" );
            this->icon_ = vals.value(1);
        }

        if(line.contains("[Desktop") && !line.contains("Entry]")) break;//skip extra stuff in Desktop Entry

    };

    this->setup_ui();

}

void DesktopEntry::setup_ui(){
    //setup ui
    QFont name_font( "Sans-Serif", 10, QFont::Bold);
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    iconLabel = new QLabel(this);
    iconLabel->setMaximumSize(QSize(64, 64));
    iconLabel->setObjectName(QString::fromUtf8("iconLabel"));
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap (this->get_pixmap().scaled(64,64,Qt::KeepAspectRatio));
    iconLabel->show();
    horizontalLayout->addWidget(iconLabel);
    verticalLayout->addLayout(horizontalLayout);
    nameLabel = new QLabel(this);
    nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
    nameLabel->setFont(name_font);
    QFontMetrics metrics(nameLabel->font());
    QString elidedText = metrics.elidedText(this->get_name(), Qt::ElideRight, (nameLabel->width() * 2) - 24);
    nameLabel->setText(elidedText);
    nameLabel->setMaximumSize(QSize(124, 64));
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);
    verticalLayout->addWidget(nameLabel);
    this->setMaximumSize(QSize(128, 128));

}

void DesktopEntry::mousePressEvent(QMouseEvent *event){
    emit clicked();
}


QPixmap DesktopEntry::get_pixmap(){

    //return default widget icon if no icon specified
    if(this->icon_ == "") {
        //TODO: change icons on dark / light change
        QPixmap pixmap(":/icons/widgets.svg");
        QSize size(512, 512);
        auto icon_mask = pixmap.scaled(size).createMaskFromColor(Qt::transparent);
        auto color = (this->arbiter->theme().mode == Session::Theme::Light) ? QColor(0, 0, 0) : QColor(255, 255, 255);
        color.setAlpha((this->arbiter->theme().mode == Session::Theme::Light) ? 162 : 134);
        QPixmap icon(size);
        icon.fill(color);
        icon.setMask(icon_mask);
        return icon;

    }

    //check if icon is path
    if(this->icon_.contains("/") && (this->icon_.contains(".png") || this->icon_.contains(".svg") || this->icon_.contains(".xpm"))){
        if(!QFile::exists(this->icon_)){
            return QPixmap(":/icons/widgets.svg");
        }
        return QPixmap(this->icon_);
    }

    //get icon from theme
    auto qico = QIcon::fromTheme(this->icon_);
    return qico.pixmap(qico.actualSize(QSize(128, 128)));

}

QList<DesktopEntry *> DesktopEntry::get_entries(Arbiter *arbiter, ILauncherPlugin *plugin)
{
    /*
     Desktop entry files must reside in the $XDG_DATA_DIRS/applications directory and must have a .desktop file extension.
    If $XDG_DATA_DIRS is not set, then the default path is /usr/share is used.
    If $XDG_DATA_HOME is not set, then the default path ~/.local/share is used.
    Desktop entries are collected from all directories in the $XDG_DATA_DIRS environment variable.
    Directories which appear first in $XDG_CONFIG_DIRS are given precedence when there are several .desktop files with the same name.
    */

    QList<DesktopEntry *> rtn;
    QString xdg = qgetenv("XDG_DATA_DIRS");
    QString xdg_home = qgetenv("XDG_DATA_HOME");

    QList<QString> dirs = xdg.split(":");
    dirs.push_back("/usr/share");
    dirs.push_back(xdg_home + "/.local/share");

    for(QString dir_name : dirs)
    {

        auto dir = dir_name + "/applications";
        QDir source(dir);
        if (!source.exists())
            continue;

        QStringList files = source.entryList(QStringList() << "*.desktop", QDir::Files);
        for (QString file: files)
        {
            //TODO: validate & error handle
            QString path = source.absoluteFilePath(file);
            DesktopEntry *entry = new DesktopEntry(path, arbiter, plugin);

            bool exists = false;
            for(auto exist: rtn){

                if(exist->get_exec() == entry->get_exec()) exists = true;
                continue;

            }
            if(!exists) rtn.push_back(entry);

        }

    }

    return rtn;

}
