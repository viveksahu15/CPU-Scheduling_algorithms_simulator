#pragma once
#include <QWidget>
#include <QScrollArea>
#include "process.h"
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
//  GanttWidget  –  custom-painted scrollable Gantt chart
// ─────────────────────────────────────────────────────────────────────────────
class GanttCanvas : public QWidget {
    Q_OBJECT
public:
    explicit GanttCanvas(QWidget* parent = nullptr);
    void setBlocks(const std::vector<GanttBlock>& blocks);
    void setDarkMode(bool dark);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

private:
    std::vector<GanttBlock> m_blocks;
    bool   m_dark     = true;
    int    m_totalTime= 0;
    int    m_barH     = 56;
    int    m_minPxPer = 36;   // min pixels per time unit
    int    m_paddingX = 20;
    int    m_paddingY = 30;
    int    m_labelH   = 22;

    int pxPerUnit() const;
    int canvasWidth() const;
};

// Scroll wrapper
class GanttWidget : public QScrollArea {
    Q_OBJECT
public:
    explicit GanttWidget(QWidget* parent = nullptr);
    void setBlocks(const std::vector<GanttBlock>& blocks);
    void setDarkMode(bool dark);
    void clear();

private:
    GanttCanvas* m_canvas;
};
