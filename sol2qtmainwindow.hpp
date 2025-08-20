// sol2qtmainwindow.hpp
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
#include <vector>
#include <algorithm>

// Add forward declaration
class LuaWindowFactory;
// Forward declarations
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
  LuaWindowFactory* windowFactory;
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

private slots:
    void runScript();
    void executeConsoleLine();
    void clearOutput();
    void loadExample();
    void onControlWidgetSignal(const QString& text);

private:
    void setupUI();
    void setupControlPanel();
    void initializeSol2();
    void navigateHistory(int direction);

    // UI Components
    QTextEdit* scriptEditor;
    QTextEdit* outputDisplay;
    ConsoleLineEdit* consoleInput;
    QPushButton* runButton;
    QPushButton* clearButton;
    QPushButton* loadExampleButton;
    QPushButton* executeLineButton;
    QWidget* controlPanel;
    
    // Lua and Qt integration
    sol::state* lua;
    LuaControlledWidget* controlWidget;
    LuaWindowFactory* windowFactory;    
    // Console functionality
    QStringList commandHistory;
    int historyIndex;
};

#endif // SOL2QTMAINWINDOW_HPP
