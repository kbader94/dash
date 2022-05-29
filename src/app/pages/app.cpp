#include "app/arbiter.hpp"
#include "app/config.hpp"

#include "app/pages/app.hpp"


AppPage::AppPage(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , Page(arbiter, "App", "widgets", true, this)
{
    setup_ui();
    this->arbiter = &arbiter;
}

void AppPage::init()
{

}

void AppPage::setName(QString name){

    this->title_label->setText(name);
}

QString AppPage::name()
{
    return this->title_label->text();
}

void AppPage::setup_ui()
{

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0, 0, 0, 0);

    //vertical
    widget = new QWidget(this);
    vlayout->addWidget(widget);
    verticalSpacer = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vlayout->addItem(verticalSpacer);

    QHBoxLayout *hlayout = new QHBoxLayout(widget);
    hlayout->setContentsMargins(8, 0, 8, 0);

    auto horizontalSpacer = new QSpacerItem(40, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QFont title_font( "Sans-Serif", 10);
    title_label = new QLabel(widget);
    title_label->setFont(title_font);
    title_label->setText("Title");
    pushButton = new QPushButton(widget);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setMaximumSize(QSize(30, 16777215));
    pushButton->setFlat(true);
    pushButton->setText("X");
    auto horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    icon_label = new QLabel(widget);
    icon_label->setText("   ");

    hlayout->addWidget(icon_label);
    hlayout->addItem(horizontalSpacer);
    hlayout->addWidget(title_label);
    hlayout->addItem(horizontalSpacer2);
    hlayout->addWidget(pushButton);

    connect(pushButton, &QPushButton::clicked, [this](){
        
        //TODO: don't rely on additional dependency
        auto command = "wmctrl -ic " + QString::number(this->id);
        QByteArray ca = command.toLocal8Bit();
        const char *c_str = ca.data();
        qDebug() << "[XManager][System] Attempting to close X Window " << this->id;
        system(c_str);
        this->arbiter->set_curr_page(4);//TODO get actual index

    });

}
