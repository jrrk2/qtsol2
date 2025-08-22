// Enhanced sol2qtmainwindow.hpp with script loading capabilities

#ifndef SOL2QTMAINWINDOW_HPP
#define SOL2QTMAINWINDOW_HPP

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>
#include <QWidget>
#include <QMessageBox>
#include <QFont>
#include <QLineEdit>
#include <QSlider>
#include <QProgressBar>
#include <QTimer>
#include <QGroupBox>
#include <QKeyEvent>
#include <QStringList>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTextStream>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <vector>
#include <algorithm>
#include <sol/sol.hpp>
#include <sol/types.hpp>
#include "AcceleratedMatrix.hpp"

// Forward declarations
class LuaWindowFactory;
class LuaControlledWidget;

namespace sol {
    class state;
}

// Custom QLineEdit that handles key events for console
class ConsoleLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit ConsoleLineEdit(QWidget* parent = nullptr);
    void setMainWindow(class Sol2QtMainWindow* window);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    Sol2QtMainWindow* mainWindow;
};

class Sol2QtMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Sol2QtMainWindow(QWidget *parent = nullptr);
    ~Sol2QtMainWindow();
    
    // Public method for console key handling
    void onConsoleKeyPress(QKeyEvent* event);
    void appendToOutput(const std::string& text);

    // Script loading methods
    bool loadScriptFromFile(const QString& filePath);
    bool executeScript(const QString& script);
    
protected:
    // Drag and drop support
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void runScript();
    void executeConsoleLine();
    void clearOutput();
  //    void loadExample();
    void onControlWidgetSignal(const QString& text);
    
    // File operations
    void openScript();
    void saveScript();
    void saveScriptAs();
    void newScript();
    void recentFileTriggered();

private:
    void setupUI();
    void setupMenuBar();
    void setupControlPanel();
    void initializeSol2();
    void navigateHistory(int direction);
    void updateRecentFiles(const QString& filePath);
    void updateRecentFilesMenu();
    sol::table convertMatrixToTable(const AcceleratedMatrix& matrix);
  void addWindowMenuBindings(sol::state* lua, LuaWindowFactory* factory);
  void setupImageDisplayBindings();
  void setupStarCatalogBindings();
  // UI Components
    QTextEdit* scriptEditor;
    QTextEdit* outputDisplay;
    ConsoleLineEdit* consoleInput;
    QPushButton* runButton;
    QPushButton* clearButton;
  //    QPushButton* loadExampleButton;
    QPushButton* executeLineButton;
    QPushButton* openScriptButton;
    QPushButton* saveScriptButton;
  //    QWidget* controlPanel;
    
    // Menu components
    QMenu* fileMenu;
    QMenu* scriptMenu;
    QMenu* recentFilesMenu;
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* saveAsAction;
    QAction* exitAction;
    
    // Lua and Qt integration
    sol::state* lua;
  //    LuaControlledWidget* controlWidget;
    LuaWindowFactory* windowFactory;
    
    // Console functionality
    QStringList commandHistory;
    int historyIndex;
    
    // File management
    QString currentScriptPath;
    QStringList recentFiles;
    static const int MaxRecentFiles = 10;
};

#endif // SOL2QTMAINWINDOW_HPP
