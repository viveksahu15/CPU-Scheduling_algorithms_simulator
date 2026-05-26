#include "mainwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QScrollArea>
#include <QToolTip>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFrame>
#include <QSizePolicy>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <cmath>
#include <set>
#include <algorithm>

// ═════════════════════════════════════════════════════════════════════════════
//  STYLESHEET CONSTANTS
// ═════════════════════════════════════════════════════════════════════════════
static const QString DARK_QSS = R"(
QMainWindow, QDialog {
    background: #0d0d1a;
    color: #e0e0f0;
}
QWidget {
    color: #e0e0f0;
    font-family: 'Segoe UI', 'SF Pro Display', 'Helvetica Neue', Arial, sans-serif;
    font-size: 13px;
}
/* ── Sidebar ─────────────────────────────────────────────────────── */
#sidebar {
    background: #12121f;
    border-right: 1px solid #252540;
    min-width: 270px;
    max-width: 270px;
}
/* ── Cards ───────────────────────────────────────────────────────── */
#metricCard {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #1a1a2e, stop:1 #15152a);
    border: 1px solid #2a2a4a;
    border-radius: 12px;
}
/* ── Gantt container ─────────────────────────────────────────────── */
#ganttContainer {
    background: #12122a;
    border: 1px solid #252540;
    border-radius: 10px;
}
/* ── Group boxes ─────────────────────────────────────────────────── */
QGroupBox {
    color: #8888cc;
    border: 1px solid #252540;
    border-radius: 8px;
    margin-top: 14px;
    font-weight: 600;
    font-size: 12px;
    letter-spacing: 0.5px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 6px;
    color: #9999dd;
}
/* ── Buttons ─────────────────────────────────────────────────────── */
QPushButton {
    background: #1e1e38;
    color: #c0c0e0;
    border: 1px solid #333366;
    border-radius: 8px;
    padding: 8px 16px;
    font-weight: 500;
}
QPushButton:hover {
    background: #28284e;
    border-color: #5555aa;
    color: #e0e0ff;
}
QPushButton:pressed {
    background: #1a1a30;
}
QPushButton#runBtn {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #6B6BFF, stop:1 #9B59B6);
    color: #ffffff;
    border: none;
    border-radius: 10px;
    padding: 12px 24px;
    font-size: 14px;
    font-weight: 700;
    letter-spacing: 0.5px;
}
QPushButton#runBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #7b7bff, stop:1 #ab69c6);
}
QPushButton#runBtn:pressed {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #5555dd, stop:1 #8b49a6);
}
QPushButton#sampleBtn {
    background: #1c3040;
    border-color: #2a5070;
    color: #6BC5FF;
}
QPushButton#sampleBtn:hover {
    background: #1e3a4c;
    border-color: #3a70a0;
}
QPushButton#dangerBtn {
    background: #301820;
    border-color: #702030;
    color: #FF6B6B;
}
QPushButton#dangerBtn:hover {
    background: #401828;
    border-color: #903040;
}
QPushButton#addBtn {
    background: #1a3020;
    border-color: #2a6040;
    color: #6BFFB8;
}
QPushButton#addBtn:hover {
    background: #1e3828;
    border-color: #3a7050;
}
/* ── ComboBox ────────────────────────────────────────────────────── */
QComboBox {
    background: #1a1a30;
    border: 1px solid #333366;
    border-radius: 8px;
    padding: 6px 12px;
    color: #d0d0f0;
    selection-background-color: #333366;
}
QComboBox::drop-down {
    border: none;
    width: 24px;
}
QComboBox::down-arrow {
    width: 10px;
    height: 10px;
    border-left: 2px solid #6666aa;
    border-bottom: 2px solid #6666aa;
    transform: rotate(-45deg);
    margin-top: -4px;
}
QComboBox QAbstractItemView {
    background: #1a1a30;
    border: 1px solid #333366;
    border-radius: 6px;
    color: #d0d0f0;
    selection-background-color: #333366;
}
/* ── SpinBox ─────────────────────────────────────────────────────── */
QSpinBox {
    background: #1a1a30;
    border: 1px solid #333366;
    border-radius: 8px;
    padding: 6px 10px;
    color: #d0d0f0;
}
QSpinBox::up-button, QSpinBox::down-button {
    background: #252545;
    border: none;
    border-radius: 3px;
    width: 18px;
}
QSpinBox::up-button:hover, QSpinBox::down-button:hover {
    background: #333366;
}
/* ── Tables ──────────────────────────────────────────────────────── */
QTableWidget {
    background: #0f0f20;
    gridline-color: #202040;
    border: 1px solid #252540;
    border-radius: 8px;
    color: #d0d0f0;
    alternate-background-color: #131328;
    selection-background-color: #333366;
}
QTableWidget::item {
    padding: 6px 12px;
    border: none;
}
QTableWidget::item:selected {
    background: #2a2a5a;
    color: #e0e0ff;
}
QHeaderView::section {
    background: #16163a;
    color: #9999cc;
    border: none;
    border-bottom: 2px solid #333366;
    padding: 8px 12px;
    font-weight: 600;
    font-size: 11px;
    letter-spacing: 0.8px;
    text-transform: uppercase;
}
/* ── ScrollBar ───────────────────────────────────────────────────── */
QScrollBar:vertical {
    background: #0f0f20;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #333366;
    border-radius: 4px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover { background: #4444aa; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal {
    background: #0f0f20;
    height: 8px;
    border-radius: 4px;
}
QScrollBar::handle:horizontal {
    background: #333366;
    border-radius: 4px;
}
QScrollBar::handle:horizontal:hover { background: #4444aa; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
/* ── StatusBar ───────────────────────────────────────────────────── */
QStatusBar {
    background: #0a0a18;
    color: #6666aa;
    border-top: 1px solid #1a1a30;
    font-size: 11px;
}
/* ── Label desc ──────────────────────────────────────────────────── */
#algoDesc {
    color: #7777bb;
    font-size: 11px;
    font-style: italic;
    padding: 4px 0;
}
#algoHeader {
    color: #c0c0ff;
    font-size: 16px;
    font-weight: 700;
    letter-spacing: 0.5px;
}
#sectionTitle {
    color: #8888cc;
    font-size: 12px;
    font-weight: 600;
    letter-spacing: 1px;
    text-transform: uppercase;
}
#appTitle {
    color: #ffffff;
    font-size: 20px;
    font-weight: 800;
    letter-spacing: 1px;
}
#appSubtitle {
    color: #5555aa;
    font-size: 10px;
    letter-spacing: 2px;
    text-transform: uppercase;
}
QTabWidget::pane {
    border: 1px solid #252540;
    border-radius: 8px;
    background: #0d0d1a;
}
QTabBar::tab {
    background: #12121f;
    color: #6666aa;
    padding: 8px 20px;
    border: 1px solid #252540;
    border-bottom: none;
    border-radius: 6px 6px 0 0;
    margin-right: 2px;
}
QTabBar::tab:selected {
    background: #1a1a38;
    color: #c0c0ff;
    border-color: #333366;
}
QSplitter::handle {
    background: #1a1a30;
}
QMenu {
    background: #1a1a30;
    border: 1px solid #333366;
    border-radius: 8px;
    color: #d0d0f0;
}
QMenu::item:selected {
    background: #333366;
}
QMenuBar {
    background: #0d0d1a;
    color: #9999cc;
    border-bottom: 1px solid #1a1a30;
}
QMenuBar::item:selected {
    background: #1a1a30;
}
)";

static const QString LIGHT_QSS = R"(
QMainWindow, QDialog {
    background: #f5f5fa;
    color: #1a1a2e;
}
QWidget {
    color: #1a1a2e;
    font-family: 'Segoe UI', 'SF Pro Display', 'Helvetica Neue', Arial, sans-serif;
    font-size: 13px;
}
#sidebar {
    background: #ffffff;
    border-right: 1px solid #e0e0ee;
    min-width: 270px;
    max-width: 270px;
}
#metricCard {
    background: #ffffff;
    border: 1px solid #e0e0ee;
    border-radius: 12px;
}
#ganttContainer {
    background: #ffffff;
    border: 1px solid #e0e0ee;
    border-radius: 10px;
}
QGroupBox {
    color: #5555aa;
    border: 1px solid #e0e0ee;
    border-radius: 8px;
    margin-top: 14px;
    font-weight: 600;
    font-size: 12px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 6px;
    color: #5555aa;
}
QPushButton {
    background: #f0f0ff;
    color: #3333aa;
    border: 1px solid #c0c0e0;
    border-radius: 8px;
    padding: 8px 16px;
    font-weight: 500;
}
QPushButton:hover {
    background: #e0e0ff;
    border-color: #8080cc;
}
QPushButton#runBtn {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #6B6BFF, stop:1 #9B59B6);
    color: #ffffff;
    border: none;
    border-radius: 10px;
    padding: 12px 24px;
    font-size: 14px;
    font-weight: 700;
}
QPushButton#runBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #7b7bff, stop:1 #ab69c6);
}
QPushButton#sampleBtn {
    background: #e8f4ff;
    border-color: #a0c8f0;
    color: #2266aa;
}
QPushButton#dangerBtn {
    background: #fff0f0;
    border-color: #f0a0a0;
    color: #aa2222;
}
QPushButton#addBtn {
    background: #e8fff4;
    border-color: #a0f0c0;
    color: #226644;
}
QComboBox {
    background: #ffffff;
    border: 1px solid #c0c0e0;
    border-radius: 8px;
    padding: 6px 12px;
    color: #1a1a2e;
}
QComboBox QAbstractItemView {
    background: #ffffff;
    border: 1px solid #c0c0e0;
    border-radius: 6px;
    color: #1a1a2e;
    selection-background-color: #e0e0ff;
}
QSpinBox {
    background: #ffffff;
    border: 1px solid #c0c0e0;
    border-radius: 8px;
    padding: 6px 10px;
    color: #1a1a2e;
}
QTableWidget {
    background: #ffffff;
    gridline-color: #e8e8f0;
    border: 1px solid #e0e0ee;
    border-radius: 8px;
    color: #1a1a2e;
    alternate-background-color: #f8f8ff;
    selection-background-color: #e0e0ff;
}
QTableWidget::item { padding: 6px 12px; }
QHeaderView::section {
    background: #f0f0ff;
    color: #5555aa;
    border: none;
    border-bottom: 2px solid #c0c0e0;
    padding: 8px 12px;
    font-weight: 600;
    font-size: 11px;
    letter-spacing: 0.8px;
}
QScrollBar:vertical { background: #f0f0ff; width: 8px; border-radius: 4px; }
QScrollBar::handle:vertical { background: #c0c0e0; border-radius: 4px; min-height: 30px; }
QScrollBar:horizontal { background: #f0f0ff; height: 8px; border-radius: 4px; }
QScrollBar::handle:horizontal { background: #c0c0e0; border-radius: 4px; }
QStatusBar { background: #ffffff; color: #9999cc; border-top: 1px solid #e0e0ee; font-size: 11px; }
#algoDesc { color: #7777aa; font-size: 11px; font-style: italic; }
#algoHeader { color: #3333aa; font-size: 16px; font-weight: 700; }
#sectionTitle { color: #7777aa; font-size: 12px; font-weight: 600; letter-spacing: 1px; }
#appTitle { color: #1a1a2e; font-size: 20px; font-weight: 800; }
#appSubtitle { color: #9999cc; font-size: 10px; letter-spacing: 2px; }
QMenu { background: #ffffff; border: 1px solid #c0c0e0; border-radius: 8px; }
QMenuBar { background: #ffffff; border-bottom: 1px solid #e0e0ee; }
)";

// ═════════════════════════════════════════════════════════════════════════════
//  MetricCard
// ═════════════════════════════════════════════════════════════════════════════
MetricCard::MetricCard(const QString& title, const QString& icon, QWidget* parent)
    : QFrame(parent), m_title(title)
{
    setObjectName("metricCard");
    setFixedHeight(90);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 10, 14, 10);
    layout->setSpacing(2);

    auto* row = new QHBoxLayout;
    m_iconLabel  = new QLabel(icon);
    m_iconLabel->setStyleSheet("font-size: 18px;");
    m_titleLabel = new QLabel(title.toUpper());
    m_titleLabel->setStyleSheet("font-size: 10px; font-weight: 700; letter-spacing: 1px; color: #7777aa;");
    row->addWidget(m_iconLabel);
    row->addWidget(m_titleLabel);
    row->addStretch();

    m_valueLabel = new QLabel("—");
    m_valueLabel->setStyleSheet("font-size: 22px; font-weight: 800; color: #9999ff;");

    layout->addLayout(row);
    layout->addWidget(m_valueLabel);
}

void MetricCard::setValue(const QString& val) {
    m_valueLabel->setText(val);
}

void MetricCard::setDarkMode(bool dark) {
    m_dark = dark;
    m_titleLabel->setStyleSheet(
        QString("font-size: 10px; font-weight: 700; letter-spacing: 1px; color: %1;")
        .arg(dark ? "#7777aa" : "#9999bb"));
    m_valueLabel->setStyleSheet(
        QString("font-size: 22px; font-weight: 800; color: %1;")
        .arg(dark ? "#9999ff" : "#4444cc"));
}

// ═════════════════════════════════════════════════════════════════════════════
//  MainWindow – Constructor
// ═════════════════════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("CPU Scheduling Simulator — Portfolio Edition");
    setMinimumSize(1200, 720);
    resize(1400, 820);

    buildMenuBar();
    buildCentralWidget();
    applyTheme();

    statusBar()->showMessage("Ready  |  Add processes and click Run Simulation");
    onLoadSample(); // pre-populate with sample data
}

void MainWindow::buildMenuBar() {
    auto* file = menuBar()->addMenu("&File");
    auto* loadAct  = file->addAction("📂  Load Sample Data");
    auto* csvAct   = file->addAction("💾  Export Results to CSV");
    auto* compAct  = file->addAction("📊  Export Comparison CSV");
    file->addSeparator();
    auto* quitAct  = file->addAction("✕  Quit");

    connect(loadAct,  &QAction::triggered, this, &MainWindow::onLoadSample);
    connect(csvAct,   &QAction::triggered, this, &MainWindow::onExportCSV);
    connect(compAct,  &QAction::triggered, this, &MainWindow::onExportComparison);
    connect(quitAct,  &QAction::triggered, qApp, &QApplication::quit);

    auto* view = menuBar()->addMenu("&View");
    auto* darkAct = view->addAction("🌙  Toggle Dark / Light Mode");
    connect(darkAct, &QAction::triggered, this, &MainWindow::onToggleDark);

    auto* help = menuBar()->addMenu("&Help");
    auto* aboutAct = help->addAction("ℹ️  About");
    connect(aboutAct, &QAction::triggered, this, [this](){
        QMessageBox::about(this, "About",
            "<b>CPU Scheduling Simulator</b><br>"
            "Portfolio-quality desktop application<br>"
            "Built with Qt6 + C++17<br><br>"
            "Algorithms: FCFS, SJF, SRTF, Priority (NP+P),<br>"
            "Round Robin, HRRN, MLQ, MLFQ, LJFS, EDF");
    });
}

void MainWindow::buildCentralWidget() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(buildSidebar());

    // ── Right panel ────────────────────────────────────────────────────────
    auto* right = new QWidget;
    auto* rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(20, 16, 20, 16);
    rightLayout->setSpacing(14);

    // Top: input + results splitter
    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setHandleWidth(6);

    splitter->addWidget(buildInputPanel());
    splitter->addWidget(buildResultPanel());
    splitter->setSizes({220, 580});

    rightLayout->addWidget(splitter, 1);
    mainLayout->addWidget(right, 1);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Sidebar
// ─────────────────────────────────────────────────────────────────────────────
QWidget* MainWindow::buildSidebar() {
    auto* sidebar = new QWidget;
    sidebar->setObjectName("sidebar");

    auto* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(16, 20, 16, 16);
    layout->setSpacing(14);

    // App branding
    auto* title = new QLabel("⚙  CPU SCHEDULER");
    title->setObjectName("appTitle");
    auto* subtitle = new QLabel("SIMULATOR  v2.0");
    subtitle->setObjectName("appSubtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    // Separator
    auto* sep = new QFrame; sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("border: none; border-top: 1px solid #252540;");
    layout->addWidget(sep);

    // Algorithm selection
    auto* algoGroup = new QGroupBox("Algorithm");
    auto* algoLayout = new QVBoxLayout(algoGroup);
    algoLayout->setContentsMargins(10,16,10,12);
    algoLayout->setSpacing(8);

    m_algoCombo = new QComboBox;
    m_algoCombo->addItem("FCFS — First Come First Served");
    m_algoCombo->addItem("SJF — Shortest Job First (NP)");
    m_algoCombo->addItem("SRTF — Shortest Remaining Time");
    m_algoCombo->addItem("Priority — Non-Preemptive");
    m_algoCombo->addItem("Priority — Preemptive");
    m_algoCombo->addItem("Round Robin");
    m_algoCombo->addItem("HRRN — Highest Response Ratio");
    m_algoCombo->addItem("MLQ — Multilevel Queue");
    m_algoCombo->addItem("MLFQ — Multilevel Feedback Queue");
    m_algoCombo->addItem("LJFS — Longest Job First");
    m_algoCombo->addItem("EDF — Earliest Deadline First");
    connect(m_algoCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAlgorithmChanged);

    m_algoDesc = new QLabel();
    m_algoDesc->setObjectName("algoDesc");
    m_algoDesc->setWordWrap(true);
    m_algoDesc->setMaximumWidth(240);

    algoLayout->addWidget(m_algoCombo);
    algoLayout->addWidget(m_algoDesc);
    layout->addWidget(algoGroup);

    // Parameters
    auto* paramGroup = new QGroupBox("Parameters");
    auto* paramLayout = new QGridLayout(paramGroup);
    paramLayout->setContentsMargins(10,16,10,12);
    paramLayout->setSpacing(8);

    paramLayout->addWidget(new QLabel("Time Quantum:"), 0, 0);
    m_quantumSpin = new QSpinBox;
    m_quantumSpin->setRange(1, 100);
    m_quantumSpin->setValue(4);
    m_quantumSpin->setSuffix(" units");
    paramLayout->addWidget(m_quantumSpin, 0, 1);

    paramLayout->addWidget(new QLabel("Queue Levels:"), 1, 0);
    m_queuesSpin = new QSpinBox;
    m_queuesSpin->setRange(2, 6);
    m_queuesSpin->setValue(3);
    paramLayout->addWidget(m_queuesSpin, 1, 1);

    layout->addWidget(paramGroup);

    // Action buttons
    m_runBtn = new QPushButton("▶  Run Simulation");
    m_runBtn->setObjectName("runBtn");
    m_runBtn->setFixedHeight(46);
    m_runBtn->setCursor(Qt::PointingHandCursor);
    connect(m_runBtn, &QPushButton::clicked, this, &MainWindow::onRunSimulation);

    m_compareBtn = new QPushButton("⚖  Compare All Algorithms");
    m_compareBtn->setCursor(Qt::PointingHandCursor);
    connect(m_compareBtn, &QPushButton::clicked, this, &MainWindow::onExportComparison);

    m_sampleBtn = new QPushButton("📋  Load Sample Data");
    m_sampleBtn->setObjectName("sampleBtn");
    m_sampleBtn->setCursor(Qt::PointingHandCursor);
    connect(m_sampleBtn, &QPushButton::clicked, this, &MainWindow::onLoadSample);

    m_resetBtn = new QPushButton("🗑  Reset");
    m_resetBtn->setObjectName("dangerBtn");
    m_resetBtn->setCursor(Qt::PointingHandCursor);
    connect(m_resetBtn, &QPushButton::clicked, this, &MainWindow::onReset);

    m_exportBtn = new QPushButton("💾  Export CSV");
    m_exportBtn->setCursor(Qt::PointingHandCursor);
    connect(m_exportBtn, &QPushButton::clicked, this, &MainWindow::onExportCSV);

    m_darkBtn = new QPushButton("🌙  Toggle Theme");
    m_darkBtn->setCursor(Qt::PointingHandCursor);
    connect(m_darkBtn, &QPushButton::clicked, this, &MainWindow::onToggleDark);

    layout->addWidget(m_runBtn);
    layout->addWidget(m_compareBtn);

    auto* sep2 = new QFrame; sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("border: none; border-top: 1px solid #252540;");
    layout->addWidget(sep2);

    layout->addWidget(m_sampleBtn);
    layout->addWidget(m_resetBtn);
    layout->addWidget(m_exportBtn);
    layout->addWidget(m_darkBtn);

    layout->addStretch();

    // Version stamp
    auto* ver = new QLabel("Qt6 · C++17  |  v2.0.0");
    ver->setStyleSheet("color: #333360; font-size: 10px;");
    ver->setAlignment(Qt::AlignCenter);
    layout->addWidget(ver);

    onAlgorithmChanged(0);
    return sidebar;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Input Panel  (process table)
// ─────────────────────────────────────────────────────────────────────────────
QWidget* MainWindow::buildInputPanel() {
    auto* panel = new QWidget;
    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(8);

    // Header row
    auto* hdr = new QHBoxLayout;
    auto* title = new QLabel("PROCESS LIST");
    title->setObjectName("sectionTitle");

    m_addBtn = new QPushButton("+ Add Process");
    m_addBtn->setObjectName("addBtn");
    m_addBtn->setFixedHeight(32);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addBtn, &QPushButton::clicked, this, &MainWindow::onAddProcess);

    m_removeBtn = new QPushButton("✕ Remove Selected");
    m_removeBtn->setObjectName("dangerBtn");
    m_removeBtn->setFixedHeight(32);
    m_removeBtn->setCursor(Qt::PointingHandCursor);
    connect(m_removeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveProcess);

    hdr->addWidget(title);
    hdr->addStretch();
    hdr->addWidget(m_addBtn);
    hdr->addWidget(m_removeBtn);
    layout->addLayout(hdr);

    // Table
    m_processTable = new QTableWidget(0, 5);
    m_processTable->setHorizontalHeaderLabels({"PID", "Arrival Time", "Burst Time", "Priority", "Time Quantum"});
    m_processTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_processTable->verticalHeader()->setVisible(false);
    m_processTable->setAlternatingRowColors(true);
    m_processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_processTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed);
    m_processTable->setMaximumHeight(200);
    m_processTable->setMinimumHeight(160);

    // Tooltip on headers
    m_processTable->horizontalHeaderItem(0)->setToolTip("Process ID (unique identifier)");
    m_processTable->horizontalHeaderItem(1)->setToolTip("Arrival Time: when the process arrives (≥0)");
    m_processTable->horizontalHeaderItem(2)->setToolTip("Burst Time: CPU time required (>0)");
    m_processTable->horizontalHeaderItem(3)->setToolTip("Priority: lower number = higher priority");
    m_processTable->horizontalHeaderItem(4)->setToolTip("Individual time quantum (for RR variants)");

    layout->addWidget(m_processTable);
    return panel;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Result Panel  (metric cards + gantt + table)
// ─────────────────────────────────────────────────────────────────────────────
QWidget* MainWindow::buildResultPanel() {
    auto* panel = new QWidget;
    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(12);

    // ── Algorithm header ───────────────────────────────────────────────────
    m_algoHeader = new QLabel("Select an algorithm and click ▶ Run Simulation");
    m_algoHeader->setObjectName("algoHeader");
    layout->addWidget(m_algoHeader);

    // ── Metric cards ───────────────────────────────────────────────────────
    auto* cardsRow = new QHBoxLayout;
    cardsRow->setSpacing(10);

    m_cardAvgWT      = new MetricCard("Avg Wait",     "⏱");
    m_cardAvgTAT     = new MetricCard("Avg Turnaround","↩");
    m_cardAvgRT      = new MetricCard("Avg Response", "⚡");
    m_cardCPU        = new MetricCard("CPU Util.",    "🖥");
    m_cardThroughput = new MetricCard("Throughput",   "📈");

    cardsRow->addWidget(m_cardAvgWT);
    cardsRow->addWidget(m_cardAvgTAT);
    cardsRow->addWidget(m_cardAvgRT);
    cardsRow->addWidget(m_cardCPU);
    cardsRow->addWidget(m_cardThroughput);
    layout->addLayout(cardsRow);

    // ── Gantt chart ────────────────────────────────────────────────────────
    auto* ganttGroup = new QGroupBox("Gantt Chart");
    auto* ganttLayout = new QVBoxLayout(ganttGroup);
    ganttLayout->setContentsMargins(10, 14, 10, 10);

    m_gantt = new GanttWidget;
    ganttLayout->addWidget(m_gantt);
    layout->addWidget(ganttGroup);

    // ── Results table ──────────────────────────────────────────────────────
    auto* resGroup = new QGroupBox("Process Metrics");
    auto* resLayout = new QVBoxLayout(resGroup);
    resLayout->setContentsMargins(10, 14, 10, 10);

    m_resultsTable = new QTableWidget(0, 7);
    m_resultsTable->setHorizontalHeaderLabels({
        "PID", "Arrival", "Burst", "Priority",
        "Completion", "Turnaround", "Waiting"
    });
    m_resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_resultsTable->verticalHeader()->setVisible(false);
    m_resultsTable->setAlternatingRowColors(true);
    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setMinimumHeight(160);

    resLayout->addWidget(m_resultsTable);
    layout->addWidget(resGroup);

    return panel;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onAlgorithmChanged(int index) {
    Algorithm algo = static_cast<Algorithm>(index);
    m_algoDesc->setText(Scheduler::algorithmDescription(algo));

    bool needsQuantum = (algo == Algorithm::ROUND_ROBIN ||
                         algo == Algorithm::MLQ ||
                         algo == Algorithm::MLFQ);
    bool needsQueues  = (algo == Algorithm::MLQ || algo == Algorithm::MLFQ);
    m_quantumSpin->setEnabled(needsQuantum);
    m_queuesSpin->setEnabled(needsQueues);
}

void MainWindow::onAddProcess() {
    int row = m_processTable->rowCount();
    m_processTable->insertRow(row);

    int newPid = row + 1;
    m_processTable->setItem(row, 0, new QTableWidgetItem(QString::number(newPid)));
    m_processTable->setItem(row, 1, new QTableWidgetItem("0"));
    m_processTable->setItem(row, 2, new QTableWidgetItem("5"));
    m_processTable->setItem(row, 3, new QTableWidgetItem("1"));
    m_processTable->setItem(row, 4, new QTableWidgetItem("4"));

    // Colour-code the PID cell
    m_processTable->item(row, 0)->setBackground(processColor(newPid - 1).darker(150));
    m_processTable->item(row, 0)->setForeground(processColor(newPid - 1).lighter(160));

    m_processTable->scrollToBottom();
    statusBar()->showMessage(QString("Added process P%1").arg(newPid));
}

void MainWindow::onRemoveProcess() {
    // Get selected rows via selection model
    QList<int> selected;
    for (const auto& idx : m_processTable->selectionModel()->selectedRows())
        selected.append(idx.row());

    if (selected.isEmpty()) {
        // No selection: remove all
        m_processTable->setRowCount(0);
        statusBar()->showMessage("All processes removed");
        return;
    }
    // Sort descending to remove without index shifting
    std::sort(selected.begin(), selected.end(), std::greater<int>());
    for (int r : selected) m_processTable->removeRow(r);
    statusBar()->showMessage(QString("Removed %1 process(es)").arg(selected.size()));
}

void MainWindow::onRunSimulation() {
    auto procs = collectProcesses();
    if (procs.empty()) {
        QMessageBox::warning(this, "No Processes", "Please add at least one process.");
        return;
    }

    Algorithm algo = static_cast<Algorithm>(m_algoCombo->currentIndex());
    int quantum    = m_quantumSpin->value();
    int queues     = m_queuesSpin->value();

    m_lastResult = Scheduler::run(algo, procs, quantum, queues);
    displayResult(m_lastResult);

    statusBar()->showMessage(
        QString("✓  Simulation complete  |  %1  |  %2 processes  |  %3")
        .arg(Scheduler::algorithmName(algo))
        .arg(procs.size())
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void MainWindow::onLoadSample() {
    m_processTable->setRowCount(0);

    struct SampleRow { int pid, at, bt, prio, tq; };
    static const SampleRow samples[] = {
        {1, 0, 8, 2, 4},
        {2, 1, 4, 1, 4},
        {3, 2, 9, 3, 4},
        {4, 3, 5, 2, 4},
        {5, 4, 2, 4, 4},
        {6, 5, 6, 1, 4},
        {7, 6, 3, 3, 4},
    };

    for (const auto& s : samples) {
        int row = m_processTable->rowCount();
        m_processTable->insertRow(row);
        m_processTable->setItem(row, 0, new QTableWidgetItem(QString::number(s.pid)));
        m_processTable->setItem(row, 1, new QTableWidgetItem(QString::number(s.at)));
        m_processTable->setItem(row, 2, new QTableWidgetItem(QString::number(s.bt)));
        m_processTable->setItem(row, 3, new QTableWidgetItem(QString::number(s.prio)));
        m_processTable->setItem(row, 4, new QTableWidgetItem(QString::number(s.tq)));

        QColor c = processColor(s.pid - 1);
        m_processTable->item(row, 0)->setBackground(c.darker(150));
        m_processTable->item(row, 0)->setForeground(c.lighter(160));
    }
    statusBar()->showMessage("Sample data loaded — 7 processes ready");
}

void MainWindow::onReset() {
    m_processTable->setRowCount(0);
    m_resultsTable->setRowCount(0);
    m_gantt->clear();
    m_algoHeader->setText("Select an algorithm and click ▶ Run Simulation");
    m_cardAvgWT->setValue("—");
    m_cardAvgTAT->setValue("—");
    m_cardAvgRT->setValue("—");
    m_cardCPU->setValue("—");
    m_cardThroughput->setValue("—");
    m_lastResult = {};
    statusBar()->showMessage("Reset complete");
}

void MainWindow::onToggleDark() {
    m_darkMode = !m_darkMode;
    setDarkMode(m_darkMode);
}

void MainWindow::onExportCSV() {
    if (m_lastResult.processes.empty()) {
        QMessageBox::warning(this, "No Results", "Run a simulation first.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(this, "Export CSV", "results.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot open file for writing.");
        return;
    }
    QTextStream out(&file);
    out << "PID,ArrivalTime,BurstTime,Priority,StartTime,CompletionTime,"
           "TurnaroundTime,WaitingTime,ResponseTime\n";
    for (const auto& p : m_lastResult.processes) {
        out << p.pid << "," << p.arrivalTime << "," << p.burstTime << ","
            << p.priority << "," << p.startTime << "," << p.completionTime << ","
            << p.turnaroundTime << "," << p.waitingTime << "," << p.responseTime << "\n";
    }
    out << "\nAvg Waiting Time," << m_lastResult.avgWaitingTime << "\n";
    out << "Avg Turnaround Time," << m_lastResult.avgTurnaroundTime << "\n";
    out << "Avg Response Time," << m_lastResult.avgResponseTime << "\n";
    out << "CPU Utilization (%)," << m_lastResult.cpuUtilization << "\n";
    out << "Throughput (proc/unit)," << m_lastResult.throughput << "\n";
    file.close();
    statusBar()->showMessage(QString("Exported to %1").arg(path));
    QMessageBox::information(this, "Export Complete", QString("Results saved to:\n%1").arg(path));
}

void MainWindow::onExportComparison() {
    auto procs = collectProcesses();
    if (procs.empty()) {
        QMessageBox::warning(this, "No Processes", "Add processes first.");
        return;
    }

    // Build comparison dialog
    auto* dlg = new QDialog(this);
    dlg->setWindowTitle("Algorithm Comparison");
    dlg->resize(900, 500);
    auto* layout = new QVBoxLayout(dlg);

    auto* label = new QLabel("Comparison of all scheduling algorithms on the current process set:");
    label->setStyleSheet("font-weight: 600; font-size: 13px; padding: 4px;");
    layout->addWidget(label);

    auto* table = new QTableWidget(11, 6);
    table->setHorizontalHeaderLabels({"Algorithm", "Avg Wait", "Avg Turnaround", "Avg Response", "CPU Util%", "Throughput"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);

    static const Algorithm algos[] = {
        Algorithm::FCFS, Algorithm::SJF_NP, Algorithm::SRTF,
        Algorithm::PRIORITY_NP, Algorithm::PRIORITY_P, Algorithm::ROUND_ROBIN,
        Algorithm::HRRN, Algorithm::MLQ, Algorithm::MLFQ,
        Algorithm::LJFS, Algorithm::EDF
    };

    int quantum = m_quantumSpin->value();
    int queues  = m_queuesSpin->value();

    double bestWT = 1e18;
    int bestRow = -1;

    for (int i = 0; i < 11; i++) {
        auto res = Scheduler::run(algos[i], procs, quantum, queues);
        table->setItem(i, 0, new QTableWidgetItem(Scheduler::algorithmName(algos[i])));
        table->setItem(i, 1, new QTableWidgetItem(formatDouble(res.avgWaitingTime)));
        table->setItem(i, 2, new QTableWidgetItem(formatDouble(res.avgTurnaroundTime)));
        table->setItem(i, 3, new QTableWidgetItem(formatDouble(res.avgResponseTime)));
        table->setItem(i, 4, new QTableWidgetItem(formatDouble(res.cpuUtilization) + "%"));
        table->setItem(i, 5, new QTableWidgetItem(formatDouble(res.throughput)));

        if (res.avgWaitingTime < bestWT) { bestWT = res.avgWaitingTime; bestRow = i; }
    }

    // Highlight best avg waiting time
    if (bestRow >= 0) {
        for (int c = 0; c < 6; c++) {
            if (table->item(bestRow, c))
                table->item(bestRow, c)->setBackground(QColor(30, 80, 50));
        }
    }

    layout->addWidget(table);

    auto* note = new QLabel("✅  Green row = best average waiting time");
    note->setStyleSheet("color: #6BFFB8; font-size: 11px; padding: 4px;");
    layout->addWidget(note);

    auto* exportBtn = new QPushButton("💾  Export this comparison to CSV");
    connect(exportBtn, &QPushButton::clicked, this, [this, table](){
        QString path = QFileDialog::getSaveFileName(this, "Save", "comparison.csv", "CSV (*.csv)");
        if (path.isEmpty()) return;
        QFile f(path); f.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&f);
        out << "Algorithm,AvgWait,AvgTurnaround,AvgResponse,CPUUtil%,Throughput\n";
        for (int r = 0; r < table->rowCount(); r++) {
            for (int c = 0; c < 6; c++) {
                out << (table->item(r,c) ? table->item(r,c)->text() : "");
                if (c < 5) out << ",";
            }
            out << "\n";
        }
        f.close();
        QMessageBox::information(this, "Saved", path);
    });
    layout->addWidget(exportBtn);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, dlg, &QDialog::accept);
    layout->addWidget(buttons);

    dlg->setStyleSheet(m_darkMode ? DARK_QSS : LIGHT_QSS);
    dlg->exec();
}

void MainWindow::onAlgorithmInfoClicked() {
    // Info in status bar via algo description
}

// ─────────────────────────────────────────────────────────────────────────────
//  displayResult
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::displayResult(const ScheduleResult& result) {
    Algorithm algo = static_cast<Algorithm>(m_algoCombo->currentIndex());
    m_algoHeader->setText("▶  " + Scheduler::algorithmName(algo));

    // Metric cards
    m_cardAvgWT->setValue(formatDouble(result.avgWaitingTime));
    m_cardAvgTAT->setValue(formatDouble(result.avgTurnaroundTime));
    m_cardAvgRT->setValue(formatDouble(result.avgResponseTime));
    m_cardCPU->setValue(formatDouble(result.cpuUtilization) + "%");
    m_cardThroughput->setValue(formatDouble(result.throughput));

    // Gantt
    m_gantt->setDarkMode(m_darkMode);
    m_gantt->setBlocks(result.gantt);

    // Results table
    m_resultsTable->setRowCount(0);
    for (const auto& p : result.processes) {
        int row = m_resultsTable->rowCount();
        m_resultsTable->insertRow(row);

        QColor c = processColor(p.pid - 1);
        auto makeItem = [&](const QString& txt) {
            auto* item = new QTableWidgetItem(txt);
            item->setTextAlignment(Qt::AlignCenter);
            return item;
        };
        auto makePidItem = [&](const QString& txt) {
            auto* item = makeItem(txt);
            item->setBackground(c.darker(150));
            item->setForeground(c.lighter(160));
            item->setFont(QFont("Courier New", 10, QFont::Bold));
            return item;
        };

        m_resultsTable->setItem(row, 0, makePidItem(QString("P%1").arg(p.pid)));
        m_resultsTable->setItem(row, 1, makeItem(QString::number(p.arrivalTime)));
        m_resultsTable->setItem(row, 2, makeItem(QString::number(p.burstTime)));
        m_resultsTable->setItem(row, 3, makeItem(QString::number(p.priority)));
        m_resultsTable->setItem(row, 4, makeItem(QString::number(p.completionTime)));
        m_resultsTable->setItem(row, 5, makeItem(QString::number(p.turnaroundTime)));
        m_resultsTable->setItem(row, 6, makeItem(QString::number(p.waitingTime)));

        // Colour waiting time: green=low, red=high
        int maxWT = 0;
        for (const auto& q : result.processes) maxWT = std::max(maxWT, q.waitingTime);
        if (maxWT > 0) {
            double ratio = (double)p.waitingTime / maxWT;
            QColor heat(
                (int)(ratio * 200),
                (int)((1-ratio) * 160),
                60
            );
            m_resultsTable->item(row, 6)->setBackground(heat.darker(200));
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  collectProcesses
// ─────────────────────────────────────────────────────────────────────────────
std::vector<Process> MainWindow::collectProcesses() const {
    std::vector<Process> procs;
    std::set<int> seenPids;

    for (int row = 0; row < m_processTable->rowCount(); row++) {
        auto getText = [&](int col) -> QString {
            auto* item = m_processTable->item(row, col);
            return item ? item->text().trimmed() : "";
        };

        bool ok1, ok2, ok3, ok4;
        int pid  = getText(0).toInt(&ok1);
        int at   = getText(1).toInt(&ok2);
        int bt   = getText(2).toInt(&ok3);
        int prio = getText(3).toInt(&ok4);

        if (!ok1 || !ok2 || !ok3 || !ok4) continue; // skip invalid rows
        if (bt <= 0) continue;                        // burst must be > 0
        if (at < 0)  at = 0;
        if (seenPids.count(pid)) continue;           // skip duplicate PIDs
        seenPids.insert(pid);

        Process p;
        p.pid         = pid;
        p.arrivalTime = at;
        p.burstTime   = bt;
        p.priority    = prio;
        p.remainingTime = bt;
        procs.push_back(p);
    }
    return procs;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Theme helpers
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::applyTheme() {
    qApp->setStyleSheet(m_darkMode ? DARK_QSS : LIGHT_QSS);
    m_gantt->setDarkMode(m_darkMode);
    m_cardAvgWT->setDarkMode(m_darkMode);
    m_cardAvgTAT->setDarkMode(m_darkMode);
    m_cardAvgRT->setDarkMode(m_darkMode);
    m_cardCPU->setDarkMode(m_darkMode);
    m_cardThroughput->setDarkMode(m_darkMode);
    m_darkBtn->setText(m_darkMode ? "☀  Light Mode" : "🌙  Dark Mode");
}

void MainWindow::setDarkMode(bool dark) {
    m_darkMode = dark;
    applyTheme();
}

QString MainWindow::formatDouble(double d) const {
    return QString::number(d, 'f', 2);
}

void MainWindow::validateInputs() {}
void MainWindow::displayComparison() {}
