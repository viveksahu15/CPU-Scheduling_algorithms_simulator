#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include <QFont>
#include <QFontDatabase>
#include "mainwindow.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Entry point
// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    // Enable Hi-DPI scaling (Qt6: on by default, but be explicit)
    QApplication app(argc, argv);
    app.setApplicationName("CPU Scheduling Simulator");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("Portfolio");

    // Set default application font
    QFont appFont("Segoe UI", 10);
    appFont.setHintingPreference(QFont::PreferFullHinting);
    app.setFont(appFont);

    MainWindow window;
    window.show();

    return app.exec();
}
