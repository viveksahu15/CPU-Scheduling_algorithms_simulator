#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QStackedWidget>
#include <QFrame>
#include "ganttwidget.h"
#include "scheduler.h"

// ─────────────────────────────────────────────────────────────────────────────
//  MetricCard  –  a small summary card showing one KPI
// ─────────────────────────────────────────────────────────────────────────────
class MetricCard : public QFrame {
    Q_OBJECT
public:
    MetricCard(const QString& title, const QString& icon, QWidget* parent = nullptr);
    void setValue(const QString& val);
    void setDarkMode(bool dark);

private:
    QLabel* m_titleLabel;
    QLabel* m_valueLabel;
    QLabel* m_iconLabel;
    bool    m_dark = true;
    QString m_title;
    void updateStyle();
};

// ─────────────────────────────────────────────────────────────────────────────
//  MainWindow
// ─────────────────────────────────────────────────────────────────────────────
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onAddProcess();
    void onRemoveProcess();
    void onRunSimulation();
    void onLoadSample();
    void onReset();
    void onAlgorithmChanged(int index);
    void onToggleDark();
    void onExportCSV();
    void onExportComparison();
    void onAlgorithmInfoClicked();

private:
    // ── UI Build helpers ───────────────────────────────────────────────────
    void buildMenuBar();
    void buildCentralWidget();
    QWidget* buildSidebar();
    QWidget* buildInputPanel();
    QWidget* buildResultPanel();
    QWidget* buildComparisonPanel();
    void     buildMetricCards(QWidget* container);

    // ── Data helpers ───────────────────────────────────────────────────────
    std::vector<Process> collectProcesses() const;
    void displayResult(const ScheduleResult& result);
    void displayComparison();
    void applyTheme();
    void setDarkMode(bool dark);
    QString formatDouble(double d) const;
    void validateInputs();

    // ── Widgets ────────────────────────────────────────────────────────────
    // Sidebar
    QComboBox*    m_algoCombo      = nullptr;
    QSpinBox*     m_quantumSpin    = nullptr;
    QSpinBox*     m_queuesSpin     = nullptr;
    QLabel*       m_algoDesc       = nullptr;
    QPushButton*  m_runBtn         = nullptr;
    QPushButton*  m_sampleBtn      = nullptr;
    QPushButton*  m_resetBtn       = nullptr;
    QPushButton*  m_darkBtn        = nullptr;
    QPushButton*  m_compareBtn     = nullptr;
    QPushButton*  m_exportBtn      = nullptr;

    // Process table
    QTableWidget* m_processTable   = nullptr;
    QPushButton*  m_addBtn         = nullptr;
    QPushButton*  m_removeBtn      = nullptr;

    // Results panel
    QStackedWidget* m_stack        = nullptr;
    GanttWidget*  m_gantt          = nullptr;
    QTableWidget* m_resultsTable   = nullptr;
    QLabel*       m_algoHeader     = nullptr;

    // Metric cards
    MetricCard*   m_cardAvgWT      = nullptr;
    MetricCard*   m_cardAvgTAT     = nullptr;
    MetricCard*   m_cardAvgRT      = nullptr;
    MetricCard*   m_cardCPU        = nullptr;
    MetricCard*   m_cardThroughput = nullptr;

    // Comparison table
    QTableWidget* m_compTable      = nullptr;

    // State
    bool          m_darkMode       = true;
    ScheduleResult m_lastResult;
};
