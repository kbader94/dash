#include <QHBoxLayout>

#include "app/arbiter.hpp"

#include "app/quick_views/quick_view.hpp"

QuickView::QuickView(Arbiter &arbiter, QString name, QWidget *widget,  bool toggleable = true)
    : arbiter(arbiter)
    , name_(name)
    , toggleable_(toggleable)
    , widget_(widget)
{
    
}

NullQuickView::NullQuickView(Arbiter &arbiter)
    : QFrame()
    , QuickView(arbiter, "none", this, false)
{
}

void NullQuickView::init()
{

    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    QSpacerItem *spacer = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addSpacerItem(spacer);

}

VolumeQuickView::VolumeQuickView(Arbiter &arbiter)
    : QFrame()
    , QuickView(arbiter, "volume", this)
{
}

void VolumeQuickView::init()
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->arbiter.forge().volume_slider());
}

BrightnessQuickView::BrightnessQuickView(Arbiter &arbiter)
    : QFrame()
    , QuickView(arbiter, "brightness", this)
{
}

void BrightnessQuickView::init()
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->arbiter.forge().brightness_slider());
}
