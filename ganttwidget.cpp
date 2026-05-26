#include "ganttwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QFont>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
//  GanttCanvas
// ─────────────────────────────────────────────────────────────────────────────
GanttCanvas::GanttCanvas(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(140);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void GanttCanvas::setBlocks(const std::vector<GanttBlock>& blocks) {
    m_blocks = blocks;
    m_totalTime = blocks.empty() ? 0 : blocks.back().end;
    setFixedWidth(std::max(canvasWidth(), parentWidget() ? parentWidget()->width() : 800));
    setFixedHeight(m_paddingY + m_barH + m_labelH + m_paddingY);
    update();
}

void GanttCanvas::setDarkMode(bool dark) {
    m_dark = dark;
    update();
}

int GanttCanvas::pxPerUnit() const {
    if (m_totalTime == 0) return m_minPxPer;
    // Scale so chart fills width, but never smaller than minPxPer
    int availWidth = std::max(800, width()) - 2 * m_paddingX;
    return std::max(m_minPxPer, availWidth / std::max(1, m_totalTime));
}

int GanttCanvas::canvasWidth() const {
    return 2 * m_paddingX + m_totalTime * m_minPxPer;
}

QSize GanttCanvas::sizeHint() const {
    return QSize(canvasWidth(), m_paddingY + m_barH + m_labelH + m_paddingY);
}

void GanttCanvas::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor bg     = m_dark ? QColor(18, 18, 28)  : QColor(248, 249, 252);
    QColor text   = m_dark ? QColor(230, 230, 245): QColor(30, 30, 50);
    QColor grid   = m_dark ? QColor(50, 50, 70)  : QColor(200, 200, 220);
    QColor idleC  = m_dark ? QColor(45, 45, 60)  : QColor(200, 200, 215);

    p.fillRect(rect(), bg);

    if (m_blocks.empty()) {
        p.setPen(grid);
        p.drawText(rect(), Qt::AlignCenter, "No data — run a simulation first");
        return;
    }

    int ppu   = pxPerUnit();
    int barTop = m_paddingY;
    int barBot = barTop + m_barH;
    int tickY  = barBot + 2;

    // ── Draw grid lines ──────────────────────────────────────────────────────
    p.setPen(QPen(grid, 1, Qt::DotLine));
    for (int t = 0; t <= m_totalTime; t++) {
        int x = m_paddingX + t * ppu;
        p.drawLine(x, barTop, x, barBot);
    }

    // ── Draw blocks ──────────────────────────────────────────────────────────
    for (const auto& blk : m_blocks) {
        int x1 = m_paddingX + blk.start * ppu;
        int x2 = m_paddingX + blk.end   * ppu;
        int w  = x2 - x1;
        if (w <= 0) continue;

        QRectF rect(x1 + 1, barTop + 1, w - 2, m_barH - 2);

        // Rounded rect fill with gradient
        QLinearGradient grad(rect.topLeft(), rect.bottomLeft());
        QColor base = (blk.pid < 0) ? idleC : blk.color;
        grad.setColorAt(0, base.lighter(130));
        grad.setColorAt(1, base.darker(110));

        QPainterPath path;
        path.addRoundedRect(rect, 6, 6);
        p.fillPath(path, grad);

        // Border
        p.setPen(QPen(base.darker(140), 1.5));
        p.drawPath(path);

        // Label inside block
        QString lbl = (blk.pid < 0) ? "IDLE" : QString("P%1").arg(blk.pid);
        QFont font("Courier New", 10, QFont::Bold);
        p.setFont(font);
        p.setPen(Qt::white);
        p.drawText(QRectF(x1 + 1, barTop + 1, w - 2, m_barH - 2),
                   Qt::AlignCenter, lbl);
    }

    // ── Time axis ticks ──────────────────────────────────────────────────────
    QFont tickFont("Courier New", 8);
    p.setFont(tickFont);

    int step = 1;
    if (m_totalTime > 60) step = 5;
    if (m_totalTime > 200) step = 10;

    for (int t = 0; t <= m_totalTime; t += step) {
        int x = m_paddingX + t * ppu;
        p.setPen(QPen(text, 1));
        p.drawLine(x, barBot, x, tickY + 4);
        p.drawText(x - 10, tickY + 6, 20, 14,
                   Qt::AlignCenter, QString::number(t));
    }
    // Always draw final tick
    if (m_totalTime % step != 0) {
        int t = m_totalTime;
        int x = m_paddingX + t * ppu;
        p.setPen(QPen(text, 1));
        p.drawLine(x, barBot, x, tickY + 4);
        p.drawText(x - 10, tickY + 6, 20, 14,
                   Qt::AlignCenter, QString::number(t));
    }

    // ── Drop shadow under bar ─────────────────────────────────────────────────
    QLinearGradient shadow(0, barBot, 0, barBot + 8);
    shadow.setColorAt(0, QColor(0, 0, 0, 60));
    shadow.setColorAt(1, QColor(0, 0, 0, 0));
    p.fillRect(m_paddingX, barBot, m_totalTime * ppu, 8, shadow);
}

// ─────────────────────────────────────────────────────────────────────────────
//  GanttWidget  (scroll wrapper)
// ─────────────────────────────────────────────────────────────────────────────
GanttWidget::GanttWidget(QWidget* parent) : QScrollArea(parent) {
    m_canvas = new GanttCanvas(this);
    setWidget(m_canvas);
    setWidgetResizable(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setMinimumHeight(150);
}

void GanttWidget::setBlocks(const std::vector<GanttBlock>& blocks) {
    m_canvas->setBlocks(blocks);
}

void GanttWidget::setDarkMode(bool dark) {
    m_canvas->setDarkMode(dark);
}

void GanttWidget::clear() {
    m_canvas->setBlocks({});
}
