#include "togglebutton.h"

#include <QMouseEvent>
#include <QPainter>

// ---------------------------------------------------------------------------
// Constructor
//
// The button starts unchecked. If you want it to start checked, call
// setChecked(true) after construction instead of relying on a default.
// ---------------------------------------------------------------------------
ToggleButton::ToggleButton(QWidget *parent) : QAbstractButton(parent)
{
    setCheckable(true);

    trackEdit.setStyle(Qt::SolidPattern);
    trackEdit.setColor(QStringLiteral("#b3d9ff"));

    trackReadonly.setStyle(Qt::SolidPattern);
    trackReadonly.setColor(QStringLiteral("#a0a0a4"));

    circleEdit.setStyle(Qt::SolidPattern);
    circleEdit.setColor(QStringLiteral("#2492ff"));

    circleReadonly.setStyle(Qt::SolidPattern);
    circleReadonly.setColor(QStringLiteral("#eaeaf0"));
}

bool ToggleButton::getState() const { return isChecked(); }

// ---------------------------------------------------------------------------
// setChecked:
//   * Delegates to QAbstractButton so that isChecked() and the toggled()
//     signal stay in sync with the visual state.
//   * Repaints.
// ---------------------------------------------------------------------------
void ToggleButton::setChecked(bool checked)
{
    QAbstractButton::setChecked(checked);
    update();
}

// ---------------------------------------------------------------------------
// paintEvent
//
// All geometry is computed from width() / height() so the button scales
// correctly on HiDPI screens. Hard-coded pixel positions would drift between
// X11 (fractional QT_SCALE_FACTOR) and Wayland (integer compositor scale).
// ---------------------------------------------------------------------------
void ToggleButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    const bool on = isChecked();

    // Reserve some outer margin so the track is never flush with the border.
    const qreal margin = qMin(width(), height()) * 0.15;
    const qreal trackW = width() - 2 * margin;
    const qreal trackH = height() * 0.35;
    const qreal trackY = (height() - trackH) / 2.0;
    const qreal radius = trackH / 2.0;
    const qreal knobR = radius * 0.85;
    const qreal knobY = trackY + trackH / 2.0;
    const qreal knobOnX = margin + trackW - radius;
    const qreal knobOffX = margin + radius;

    p.setBrush(on ? trackEdit : trackReadonly);
    p.drawRoundedRect(QRectF(margin, trackY, trackW, trackH), radius, radius);

    p.setBrush(on ? circleEdit : circleReadonly);
    p.drawEllipse(QPointF(on ? knobOnX : knobOffX, knobY), knobR, knobR);
}
