#include "TrackGroupView.h"
#include "BaseTrackView.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QEvent>
#include <QDebug>
#include <QMenu>
#include <QActionGroup>
#include "PeakMeter.h"

TrackGroupView::TrackGroupView(QWidget *parent) :
    QFrame(parent)
{
    setupUI();

    //context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &TrackGroupView::customContextMenuRequested, this, &TrackGroupView::showContextMenu);
}

void TrackGroupView::populateContextMenu(QMenu &contextMenu)
{
    // create painting on/off actions in context menu
    contextMenu.addSeparator();
    QAction *audioPeaksAction = contextMenu.addAction(tr("Show peak meter only"));
    QAction *audioRmsAction = contextMenu.addAction(tr("Show RMS meter only"));
    QAction *audioPeaPlusRmsAction = contextMenu.addAction(tr("Show Peak and RMS meters"));
    contextMenu.addSeparator();
    QAction *maxPeakMarkerAction = contextMenu.addAction(tr("Show max peak marker"));

    audioPeaksAction->setCheckable(true);
    audioRmsAction->setCheckable(true);
    audioPeaPlusRmsAction->setCheckable(true);
    maxPeakMarkerAction->setCheckable(true);

    connect(audioPeaksAction, SIGNAL(triggered(bool)), this, SLOT(showPeakMeterOnly()));
    connect(audioRmsAction, SIGNAL(triggered(bool)), this, SLOT(showRmsOnly()));
    connect(audioPeaPlusRmsAction, SIGNAL(triggered(bool)), this, SLOT(showPeakAndRms()));
    connect(maxPeakMarkerAction, SIGNAL(triggered(bool)), this, SLOT(showMaxPeakMarker(bool)));

    maxPeakMarkerAction->setChecked(PeakMeter::isPaintintMaxPeakMarker());
    bool showingPeakAndRms = PeakMeter::isPaintingRMS() && PeakMeter::isPaintingPeaks();
    if (showingPeakAndRms) {
        audioPeaPlusRmsAction->setChecked(true);
    }
    else{
        audioPeaksAction->setChecked(PeakMeter::isPaintingPeaks());
        audioRmsAction->setChecked(PeakMeter::isPaintingRMS());
    }

    QActionGroup *actionGroup = new QActionGroup(&contextMenu);
    actionGroup->addAction(audioPeaksAction);
    actionGroup->addAction(audioRmsAction);
    actionGroup->addAction(audioPeaPlusRmsAction);
}

void TrackGroupView::showContextMenu(const QPoint &pos){
    QMenu contextMenu(this);
    populateContextMenu(contextMenu); //populate is overrided in subclasses to add more menu actions
    contextMenu.exec(mapToGlobal(pos));
}

void TrackGroupView::showPeakMeterOnly()
{
    PeakMeter::setPaintingPeaks(true);
    PeakMeter::setPaintingRMS(false);
}

void TrackGroupView::showRmsOnly()
{
    PeakMeter::setPaintingPeaks(false);
    PeakMeter::setPaintingRMS(true);
}

void TrackGroupView::showPeakAndRms()
{
    PeakMeter::setPaintingPeaks(true);
    PeakMeter::setPaintingRMS(true);
}

void TrackGroupView::showMaxPeakMarker(bool showMarker)
{
    PeakMeter::setPaintMaxPeakMarker(showMarker);
}

void TrackGroupView::setupUI()
{
    setObjectName(QStringLiteral("TrackGroupView"));
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    topPanel = new QWidget(this);
    topPanel->setObjectName(QStringLiteral("topPanel"));

    groupNameField = new QLineEdit(topPanel);
    groupNameField->setObjectName(QStringLiteral("groupNameField"));
    groupNameField->setAttribute(Qt::WA_MacShowFocusRect, 0);// disable blue border when QLineEdit has focus in mac

    topPanelLayout = new QHBoxLayout(topPanel);
    topPanelLayout->setSpacing(3);
    topPanelLayout->setContentsMargins(6, 12, 6, 12);
    topPanelLayout->addWidget(groupNameField);

    tracksLayout = new QHBoxLayout();
    tracksLayout->setContentsMargins(0, 0, 0, 0);
    tracksLayout->setSpacing(1);

    mainLayout->addWidget(topPanel);
    mainLayout->addLayout(tracksLayout, 1);

    translateUi();
}

void TrackGroupView::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        translateUi();
    }
    QFrame::changeEvent(e);
}

void TrackGroupView::translateUi()
{
    //overrided in subclasses
}

bool TrackGroupView::isUnlighted() const
{
    return property("unlighted").toBool();
}

void TrackGroupView::refreshStyleSheet()
{
    style()->unpolish(this);
    style()->polish(this);

    style()->unpolish(topPanel);
    style()->polish(topPanel);
}

void TrackGroupView::setUnlightStatus(bool unlighted)
{
    setProperty("unlighted", QVariant(unlighted));
    refreshStyleSheet();

    foreach (BaseTrackView *localTrack, trackViews)
        localTrack->setUnlightStatus(unlighted);
    update();
}

void TrackGroupView::updateGuiElements()
{
    foreach (BaseTrackView *trackView, trackViews)
        trackView->updateGuiElements();
}

TrackGroupView::~TrackGroupView()
{

}


BaseTrackView *TrackGroupView::addTrackView(long trackID)
{
    BaseTrackView* newTrackView = createTrackView(trackID);//this is a factory method and is overrided in some places
    if (tracksLayout) {
        tracksLayout->addWidget(newTrackView);
        trackViews.append(newTrackView);

        if (trackViews.size() > 1) {
            foreach (BaseTrackView *trackView, trackViews)
                trackView->setToNarrow();
            updateGeometry();
            newTrackView->setUnlightStatus(isUnlighted());
        }
    }
    return newTrackView;
}

void TrackGroupView::setGroupName(const QString &groupName)
{
    groupNameField->setText(groupName);
}

QString TrackGroupView::getGroupName() const
{
    return groupNameField->text();
}

// +++++++++++++++++++++++++++++++++++++++++
void TrackGroupView::removeTrackView(BaseTrackView *trackView)
{
    tracksLayout->removeWidget(trackView);
    trackViews.removeOne(trackView);
    trackView->deleteLater();
    if (trackViews.size() == 1)
        trackViews.at(0)->setToWide();
    updateGeometry();
}

void TrackGroupView::removeTrackView(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < trackViews.size())
        removeTrackView(trackViews.at(trackIndex));
    else
        qCritical() << "Invalid index " << trackIndex;
}

// +++++++++++++++++++++++++++++++++++++++++

QSize TrackGroupView::minimumSizeHint() const
{
    return sizeHint();
}

QSize TrackGroupView::sizeHint() const
{
    int width = 0;
    foreach (BaseTrackView *trackView, trackViews)
        width += trackView->minimumSizeHint().width();
    return QSize(width, 10);
}
