#include "qwtquick2plot.h"
#include "plotdata.h"
#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_renderer.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_canvas.h>
#include <qwt_text.h>

#include <QObject>
#include <QDebug>
#include <QPainter>

class PlotPicker: public QwtPlotPicker
{
public:
    explicit PlotPicker( QWidget *canvas ) : QwtPlotPicker(canvas) {}

    virtual QwtText trackerText( const QPoint & p ) const
    {
        auto t = QwtPlotPicker::trackerText(p);
        return t;
    }

    virtual QwtText trackerTextF( const QPointF & p ) const
    {
        auto t = QwtPlotPicker::trackerTextF(p);
        return t;
    }

    virtual void move( const QPoint & p ) {
        QwtPlotPicker::move(p);
    }

    virtual void append( const QPoint & p ) {
        QwtPlotPicker::append(p);
    }

    virtual bool end( bool ok = true ) {
        auto b = QwtPlotPicker::end(ok);

        return b;
    }

    virtual bool eventFilter( QObject *o, QEvent *e) {
        // qDebug() << "PlotPicker::eventFilter";

        if(e->type() == QEvent::Paint) {
            // qDebug() << "paint";
            return true;
        }
        return QwtPlotPicker::eventFilter(o, e);
    }

    virtual void widgetMousePressEvent (QMouseEvent *e)
    {
        // qDebug() << "widgetMousePressEvent";
        QwtPlotPicker::widgetMousePressEvent(e);
    }

    virtual void widgetMouseReleaseEvent (QMouseEvent *e)
    {
        QwtPlotPicker::widgetMouseReleaseEvent(e);
    }

    virtual void widgetMouseDoubleClickEvent (QMouseEvent *e)
    {
        QwtPlotPicker::widgetMouseDoubleClickEvent(e);
    }

    virtual void widgetMouseMoveEvent (QMouseEvent *e)
    {
        QwtPlotPicker::widgetMouseMoveEvent(e);
    }
};

class PickerDragPointMachine : public QwtPickerDragPointMachine
{
    virtual QList<Command> transition(const QwtEventPattern &ep, const QEvent * e) {
        return QwtPickerDragPointMachine::transition(ep, e);
    }
};

class PlotCanvas : public QwtPlotCanvas
{
    virtual void mousePressEvent(QMouseEvent* event) {
        // qDebug() << "mousePressEvent: " << event;

        QwtPlotCanvas::mousePressEvent(event);
    }
    virtual void mouseReleaseEvent(QMouseEvent* event) {
        // qDebug() << "mouseReleaseEvent: " << event;

        QwtPlotCanvas::mouseReleaseEvent(event);
    }
    virtual void mouseMoveEvent(QMouseEvent* event) {

        // qDebug() << "mouseMoveEvent: " << event;
        QwtPlotCanvas::mouseMoveEvent(event);
    }
};

class Plot : public QWidget
{
public:
    Plot() {
        m_plot = new QwtPlot();
        m_plot->setVisible(true);
    }

    void resizeEvent(QResizeEvent* e) {
        QWidget::resizeEvent(e);

        m_plot->setGeometry(0, 0, size().width(), size().height());
    }

    void mouseMoveEvent(QMouseEvent* event) {

        QMouseEvent* newEvent = new QMouseEvent(event->type(), event->localPos(), event->windowPos(), event->screenPos(),
                                                event->button(), event->buttons(),
                                                event->modifiers());
        QCoreApplication::postEvent(m_plot->canvas(), newEvent);

        // QCoreApplication::sendEvent(m_plot, event);

        update();
    }

    void mousePressEvent(QMouseEvent* event) {

        QMouseEvent* newEvent = new QMouseEvent(event->type(), event->localPos(), event->windowPos(), event->screenPos(),
                                                event->button(), event->buttons(),
                                                event->modifiers());
        QCoreApplication::postEvent(m_plot->canvas(), newEvent);

        // QCoreApplication::sendEvent(m_plot, event);

        update();
    }

    void mouseReleaseEvent(QMouseEvent* event) {

        QMouseEvent* newEvent = new QMouseEvent(event->type(), event->localPos(), event->windowPos(), event->screenPos(),
                                                event->button(), event->buttons(),
                                                event->modifiers());
        QCoreApplication::postEvent(m_plot->canvas(), newEvent);

        // QCoreApplication::sendEvent(m_plot, event);

        update();
    }

    QwtPlot* plot() {
        return m_plot;
    }

    virtual void paintEvent(QPaintEvent *event) {
        QPixmap picture(size());

        QwtPlotRenderer renderer;
        renderer.renderTo(m_plot, picture);

        QPainter p(this);
        p.fillRect(geometry(), Qt::gray);

        auto painter = &p;
        painter->drawPixmap(QPoint(), picture);
    }

    QwtPlot* m_plot;
};

QwtQuick2Plot::QwtQuick2Plot(QQuickItem* parent) : QQuickPaintedItem(parent)
    , m_qwtPlot(nullptr)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);

    connect(this, &QQuickPaintedItem::widthChanged, this, &QwtQuick2Plot::updatePlotSize);
    connect(this, &QQuickPaintedItem::heightChanged, this, &QwtQuick2Plot::updatePlotSize);

    m_qwtPlot = new QwtPlot();
    m_qwtPlot->setCanvas(new PlotCanvas());
    m_qwtPlot->setAutoReplot(false);

    updatePlotSize();

    m_qwtPlot->setAxisTitle(m_qwtPlot->xBottom, tr("t"));
    m_qwtPlot->setAxisTitle(m_qwtPlot->yLeft, tr("S"));

    replotAndUpdate();


    setAcceptHoverEvents(true);
    setFlag(ItemAcceptsInputMethod, true);

    {
        QwtPlotPicker* picker = new PlotPicker(m_qwtPlot->canvas());

        picker->setAxis( QwtPlot::xBottom, QwtPlot::yLeft );
        picker->setRubberBand( QwtPlotPicker::CrossRubberBand );
        picker->setRubberBandPen( QColor( Qt::green ) );

        picker->setTrackerMode( QwtPicker::AlwaysOn );
        picker->setTrackerPen( QColor( Qt::black ) );

        picker->setStateMachine( new PickerDragPointMachine () );
        picker->setEnabled(true);

        connect( picker, &QwtPlotPicker::moved, this, [&](auto p) {
            qDebug() << "moved: " << p;
        });

        connect( picker, static_cast<void(QwtPlotPicker::*)(const QPointF&)>(&QwtPlotPicker::selected), this, [&](auto p) {
            qDebug() << "selected: " << p;
        });

    }


    auto ppp = new Plot();
    p = ppp;
    p->show();
    ppp->plot()->setCanvas(new PlotCanvas());
    ppp->plot()->setAutoReplot(false);

    {
        QwtPlotPicker* picker = new PlotPicker(ppp->plot()->canvas());

        picker->setAxis( QwtPlot::xBottom, QwtPlot::yLeft );
        picker->setRubberBand( QwtPlotPicker::CrossRubberBand );
        picker->setRubberBandPen( QColor( Qt::green ) );

        picker->setTrackerMode( QwtPicker::AlwaysOn );
        picker->setTrackerPen( QColor( Qt::black ) );

        picker->setStateMachine( new PickerDragPointMachine () );
        picker->setEnabled(true);

        connect( picker, &QwtPlotPicker::moved, this, [&](auto p) {
            qDebug() << "moved: " << p;
        });

        connect( picker, static_cast<void(QwtPlotPicker::*)(const QPointF&)>(&QwtPlotPicker::selected), this, [&](auto p) {
            qDebug() << "selected: " << p;
        });
    }

    /*
    connect( picker, &QwtPlotPicker::selected, this, [&](auto p) {
        qDebug() << "selected: " << p;
    });
    */
}

QwtQuick2Plot::~QwtQuick2Plot()
{
    delete m_qwtPlot;
    m_qwtPlot = nullptr;
}

void QwtQuick2Plot::replotAndUpdate()
{
    m_qwtPlot->replot();
    update();
}

QwtPlot *QwtQuick2Plot::plot() const
{
    return m_qwtPlot;
}

QwtPlot *QwtQuick2Plot::pp() const
{
    return ((Plot*) p)->plot();
}

void QwtQuick2Plot::paint(QPainter* painter)
{
    if (m_qwtPlot) {
        QPixmap picture(boundingRect().size().toSize());

        QwtPlotRenderer renderer;
        renderer.renderTo(m_qwtPlot, picture);

        painter->drawPixmap(QPoint(), picture);
    }
}

void QwtQuick2Plot::mousePressEvent(QMouseEvent* event)
{
    // QQuickPaintedItem::mousePressEvent(event);

    // qDebug() << Q_FUNC_INFO;
    routeMouseEvents(event);
}

void QwtQuick2Plot::mouseReleaseEvent(QMouseEvent* event)
{
    // QQuickPaintedItem::mouseReleaseEvent(event);

    // qDebug() << Q_FUNC_INFO;
    routeMouseEvents(event);
}

void QwtQuick2Plot::mouseMoveEvent(QMouseEvent* event)
{
    // QQuickPaintedItem::mouseMoveEvent(event);

    // qDebug() << Q_FUNC_INFO;
    routeMouseEvents(event);
}

void QwtQuick2Plot::hoverMoveEvent(QHoverEvent *event)
{
    // qDebug() << Q_FUNC_INFO;
}

void QwtQuick2Plot::mouseDoubleClickEvent(QMouseEvent* event)
{
    // qDebug() << Q_FUNC_INFO;
    routeMouseEvents(event);
}

void QwtQuick2Plot::wheelEvent(QWheelEvent* event)
{
    routeWheelEvents(event);
}
void QwtQuick2Plot::childEvent(QChildEvent *event)
{
    if (event->type() == QEvent::ChildAdded) {
        attach(event->child());
    }
}

void QwtQuick2Plot::componentComplete()
{
    QQuickPaintedItem::componentComplete();

    for(QObject *child : children()) {
        attach(child);
    }
}

void QwtQuick2Plot::attach(QObject *child)
{
    if (qobject_cast<QwtQuick2PlotCurve *>(child)) {
        qobject_cast<QwtQuick2PlotCurve *>(child)->attach(this);
    } else if(qobject_cast<QwtQuick2PlotGrid *>(child)) {
        qobject_cast<QwtQuick2PlotGrid *>(child)->attach(this);
    }
}

void QwtQuick2Plot::routeMouseEvents(QMouseEvent* event)
{
    // QCoreApplication::sendEvent(m_qwtPlot, event);

    if (m_qwtPlot) {
        auto mappedLocalPos = event->localPos();
        mappedLocalPos.setX(mappedLocalPos.x() - m_qwtPlot->canvas()->x());
        mappedLocalPos.setY(mappedLocalPos.y() - m_qwtPlot->canvas()->y());

        QMouseEvent* newEvent = new QMouseEvent(event->type(), mappedLocalPos, event->windowPos(), event->screenPos(),
                                                event->button(), event->buttons(),
                                                event->modifiers());
        QCoreApplication::postEvent(m_qwtPlot->canvas(), newEvent);

        update();
    }
}

void QwtQuick2Plot::routeWheelEvents(QWheelEvent* event)
{
    if (m_qwtPlot) {
        QWheelEvent* newEvent = new QWheelEvent(event->pos(), event->delta(),
                                                event->buttons(), event->modifiers(),
                                                event->orientation());
        QCoreApplication::postEvent(m_qwtPlot, newEvent);
    }
}

void QwtQuick2Plot::updatePlotSize()
{
    if (m_qwtPlot) {
        m_qwtPlot->setGeometry(0, 0, static_cast<int>(width()), static_cast<int>(height()));
    }
}

QwtQuick2PlotCurve::QwtQuick2PlotCurve(QObject *parent) : QObject(parent)
{
    m_qwtPlotCurve.reset(new QwtPlotCurve());
    m_qwtPlotCurve->setTitle("Curve 1");
    m_qwtPlotCurve->setPen(QPen(Qt::red));
    m_qwtPlotCurve->setStyle(QwtPlotCurve::Lines);
    m_qwtPlotCurve->setRenderHint(QwtPlotItem::RenderAntialiased);

    m_qwtPlotCurve->setData(new PlotData(&m_curveData));

    c.reset(new QwtPlotCurve());
    c->setTitle("Curve 1");
    c->setPen(QPen(Qt::red));
    c->setStyle(QwtPlotCurve::Lines);
    c->setRenderHint(QwtPlotItem::RenderAntialiased);

    c->setData(new PlotData(&m_curveData));
}

QwtQuick2PlotCurve::~QwtQuick2PlotCurve()
{
    m_qwtPlotCurve.release();
}

QwtPlotCurve *QwtQuick2PlotCurve::curve() const
{
    return m_qwtPlotCurve.get();
}

QwtPlotCurve *QwtQuick2PlotCurve::cc() const
{
    return c.get();
}

QwtQuick2Plot *QwtQuick2PlotCurve::plot() const
{
    return m_qwtQuickPlot;
}

void QwtQuick2PlotCurve::attach(QwtQuick2Plot *plot)
{
    m_qwtQuickPlot = plot;
    curve()->attach(plot->plot());

    cc()->attach(plot->pp());
}

QwtQuick2PlotCurve::CurveStyle QwtQuick2PlotCurve::curveStyle() const
{
    return (QwtQuick2PlotCurve::CurveStyle) m_qwtPlotCurve->style();
}

QString QwtQuick2PlotCurve::title() const
{
    return m_qwtPlotCurve->title().text();
}

qreal QwtQuick2PlotCurve::width() const
{
    return m_qwtPlotCurve->pen().widthF();
}

QColor QwtQuick2PlotCurve::color() const
{
    return m_qwtPlotCurve->pen().color();
}

void QwtQuick2PlotCurve::setCurveStyle(QwtQuick2PlotCurve::CurveStyle curveStyle)
{
    m_qwtPlotCurve->setStyle((QwtPlotCurve::CurveStyle) curveStyle);
    Q_EMIT curveStyleChanged(curveStyle);
}

void QwtQuick2PlotCurve::setTitle(QString title)
{
    if (m_qwtPlotCurve->title().text() == title)
        return;

    m_qwtPlotCurve->setTitle(title);
    Q_EMIT titleChanged(title);
}

void QwtQuick2PlotCurve::setWidth(qreal width)
{
    if (qFuzzyCompare(m_qwtPlotCurve->pen().widthF(), width))
        return;

    auto pen = m_qwtPlotCurve->pen();
    pen.setWidthF(width);
    m_qwtPlotCurve->setPen(pen);
    Q_EMIT widthChanged(width);
}

void QwtQuick2PlotCurve::setColor(QColor color)
{
    if (m_qwtPlotCurve->pen().color() == color)
        return;

    auto pen = m_qwtPlotCurve->pen();
    pen.setColor(color);
    m_qwtPlotCurve->setPen(pen);
    Q_EMIT colorChanged(color);
}


QwtQuick2PlotGrid::QwtQuick2PlotGrid(QObject* parent) : QObject(parent)
{
    m_qwtPlotGrid.reset(new QwtPlotGrid());
}

QwtQuick2PlotGrid::~QwtQuick2PlotGrid()
{
    m_qwtPlotGrid.release();
}

void QwtQuick2PlotGrid::attach(QwtQuick2Plot *plot)
{
    m_qwtPlotGrid->attach(plot->plot());
}

bool QwtQuick2PlotGrid::enableXMin() const
{
    return m_qwtPlotGrid->xMinEnabled();
}

bool QwtQuick2PlotGrid::enableYMin() const
{
    return m_qwtPlotGrid->yMinEnabled();
}

QColor QwtQuick2PlotGrid::majorPenColor() const
{
    return m_qwtPlotGrid->majorPen().color();
}

qreal QwtQuick2PlotGrid::majorPenWidth() const
{
    return m_qwtPlotGrid->majorPen().widthF();
}

Qt::PenStyle QwtQuick2PlotGrid::majorPenStyle() const
{
    return m_qwtPlotGrid->majorPen().style();
}

QColor QwtQuick2PlotGrid::minorPenColor() const
{
    return m_qwtPlotGrid->minorPen().color();
}

qreal QwtQuick2PlotGrid::minorPenWidth() const
{
    return m_qwtPlotGrid->minorPen().widthF();
}

Qt::PenStyle QwtQuick2PlotGrid::minorPenStyle() const
{
    return m_qwtPlotGrid->minorPen().style();
}

void QwtQuick2PlotGrid::setEnableXMin(bool enableXMin)
{
    if (m_qwtPlotGrid->xMinEnabled() == enableXMin)
        return;

    m_qwtPlotGrid->enableXMin(enableXMin);
    Q_EMIT enableXMinChanged(m_qwtPlotGrid->xMinEnabled());
}

void QwtQuick2PlotGrid::setEnableYMin(bool enableYMin)
{
    if (m_qwtPlotGrid->yMinEnabled() == enableYMin)
        return;

    m_qwtPlotGrid->enableYMin(enableYMin);
    Q_EMIT enableYMinChanged(m_qwtPlotGrid->yMinEnabled());
}

void QwtQuick2PlotGrid::setMajorPenColor(QColor majorPenColor)
{
    if (m_qwtPlotGrid->majorPen().color() == majorPenColor)
        return;

    auto pen = m_qwtPlotGrid->majorPen();
    pen.setColor(majorPenColor);

    m_qwtPlotGrid->setMajorPen(pen);
    Q_EMIT majorPenColorChanged(m_qwtPlotGrid->majorPen().color());
}

void QwtQuick2PlotGrid::setMajorPenWidth(qreal majorPenWidth)
{
    qWarning("Floating point comparison needs context sanity check");
    if (qFuzzyCompare(m_qwtPlotGrid->majorPen().widthF(), majorPenWidth))
        return;

    auto pen = m_qwtPlotGrid->majorPen();
    pen.setWidthF(majorPenWidth);

    m_qwtPlotGrid->setMajorPen(pen);
    Q_EMIT majorPenWidthChanged(m_qwtPlotGrid->majorPen().widthF());
}

void QwtQuick2PlotGrid::setMajorPenStyle(Qt::PenStyle majorPenStyle)
{
    if (m_qwtPlotGrid->majorPen().style() == majorPenStyle)
        return;

    auto pen = m_qwtPlotGrid->majorPen();
    pen.setStyle(majorPenStyle);

    m_qwtPlotGrid->setMajorPen(pen);
    Q_EMIT majorPenStyleChanged(m_qwtPlotGrid->majorPen().style());
}

void QwtQuick2PlotGrid::setMinorPenColor(QColor minorPenColor)
{
    if (m_qwtPlotGrid->minorPen().color() == minorPenColor)
        return;

    auto pen = m_qwtPlotGrid->minorPen();
    pen.setColor(minorPenColor);

    m_qwtPlotGrid->setMinorPen(pen);
    Q_EMIT minorPenColorChanged(m_qwtPlotGrid->minorPen().color());
}

void QwtQuick2PlotGrid::setMinorPenWidth(qreal minorPenWidth)
{
    qWarning("Floating point comparison needs context sanity check");
    if (qFuzzyCompare(m_qwtPlotGrid->minorPen().widthF(), minorPenWidth))
        return;

    auto pen = m_qwtPlotGrid->minorPen();
    pen.setWidthF(minorPenWidth);

    m_qwtPlotGrid->setMinorPen(pen);
    Q_EMIT minorPenWidthChanged(m_qwtPlotGrid->minorPen().widthF());
}

void QwtQuick2PlotGrid::setMinorPenStyle(Qt::PenStyle minorPenStyle)
{
    if (m_qwtPlotGrid->minorPen().style() == minorPenStyle)
        return;

    auto pen = m_qwtPlotGrid->minorPen();
    pen.setStyle(minorPenStyle);

    m_qwtPlotGrid->setMinorPen(pen);
    Q_EMIT minorPenStyleChanged(m_qwtPlotGrid->minorPen().style());
}
