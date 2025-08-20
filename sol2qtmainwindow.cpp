// ============================================================================

// sol2qtmainwindow.cpp
#include "sol2qtmainwindow.hpp"
#include "LuaControlledWidget.hpp"
#include "LuaWindowFactory.hpp"
#include <sol/sol.hpp>
#include <sol/types.hpp>

// ConsoleLineEdit implementation
ConsoleLineEdit::ConsoleLineEdit(QWidget* parent) 
    : QLineEdit(parent), mainWindow(nullptr) 
{
}

void ConsoleLineEdit::setMainWindow(Sol2QtMainWindow* window) 
{
    mainWindow = window;
}

void ConsoleLineEdit::keyPressEvent(QKeyEvent* event) 
{
    if (mainWindow) {
        mainWindow->onConsoleKeyPress(event);
        if (event->key() != Qt::Key_Return && event->key() != Qt::Key_Enter &&
            event->key() != Qt::Key_Up && event->key() != Qt::Key_Down) {
            QLineEdit::keyPressEvent(event);
        }
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

// Sol2QtMainWindow implementation
Sol2QtMainWindow::Sol2QtMainWindow(QWidget *parent) 
    : QMainWindow(parent), lua(nullptr), historyIndex(-1)
{
    setupUI();
    initializeSol2();
    // Load initial script examples info
    appendToOutput("=== Sol2 + Qt6 Script Loading Ready ===");
    appendToOutput("Create these example files to test script loading:");
    appendToOutput("1. simple_calculator.lua - Basic calculator");
    appendToOutput("2. data_manager.lua - Data management tool");
    appendToOutput("3. script_loader.lua - Script loading utility");
    appendToOutput("4. multi_script_demo.lua - Multi-script coordinator");
    appendToOutput("");
    appendToOutput("Or drag & drop any .lua file onto this window!");

}

Sol2QtMainWindow::~Sol2QtMainWindow() 
{
  if (windowFactory) {
    windowFactory->closeAllWindows();
  }
  delete lua;
}

void Sol2QtMainWindow::onConsoleKeyPress(QKeyEvent* event) 
{
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        executeConsoleLine();
        break;
    case Qt::Key_Up:
        navigateHistory(-1);
        break;
    case Qt::Key_Down:
        navigateHistory(1);
        break;
    default:
        // Let the line edit handle other keys normally
        break;
    }
}

void Sol2QtMainWindow::runScript() 
{
    QString script = scriptEditor->toPlainText();
    if (script.isEmpty()) {
        QMessageBox::information(this, "Info", "Please enter a Lua script to execute.");
        return;
    }
    
    outputDisplay->append("=== Executing Sol2/Lua Script ===");
    
    try {
        auto result = lua->script(script.toStdString());
        
        // Handle different return types
        if (result.valid()) {
            sol::object obj = result;
            if (obj.is<std::string>()) {
                outputDisplay->append(QString("Result: %1").arg(QString::fromStdString(obj.as<std::string>())));
            } else if (obj.is<double>()) {
                outputDisplay->append(QString("Result: %1").arg(obj.as<double>()));
            } else if (obj.is<bool>()) {
                outputDisplay->append(QString("Result: %1").arg(obj.as<bool>() ? "true" : "false"));
            }
        }
        
        outputDisplay->append("=== Script Completed Successfully ===");
    } catch (const sol::error& e) {
        outputDisplay->append(QString("Sol2 Error: %1").arg(e.what()));
    }
    outputDisplay->append("");
}

void Sol2QtMainWindow::executeConsoleLine() 
{
    QString command = consoleInput->text().trimmed();
    if (command.isEmpty()) return;
    
    // Add to history
    if (commandHistory.isEmpty() || commandHistory.last() != command) {
        commandHistory.append(command);
        if (commandHistory.size() > 100) { // Keep last 100 commands
            commandHistory.removeFirst();
        }
    }
    historyIndex = -1;
    
    // Display the command
    outputDisplay->append(QString("lua> %1").arg(command));
    
    try {
        // Try to execute as expression first (for immediate values)
        std::string expr = "return " + command.toStdString();
        auto result = lua->script(expr);
        
        if (result.valid()) {
            sol::object obj = result;
            if (!obj.is<sol::lua_nil_t>()) {
                if (obj.is<std::string>()) {
                    outputDisplay->append(QString("=> %1").arg(QString::fromStdString(obj.as<std::string>())));
                } else if (obj.is<double>()) {
                    outputDisplay->append(QString("=> %1").arg(obj.as<double>()));
                } else if (obj.is<bool>()) {
                    outputDisplay->append(QString("=> %1").arg(obj.as<bool>() ? "true" : "false"));
                } else if (obj.is<int>()) {
                    outputDisplay->append(QString("=> %1").arg(obj.as<int>()));
                } else {
                    outputDisplay->append("=> (result)");
                }
            }
        }
    } catch (const sol::error& e) {
        // If expression failed, try as statement
        try {
            lua->script(command.toStdString());
        } catch (const sol::error& e2) {
            outputDisplay->append(QString("Error: %1").arg(e2.what()));
        }
    }
    
    consoleInput->clear();
    outputDisplay->ensureCursorVisible();
}

void Sol2QtMainWindow::navigateHistory(int direction) 
{
    if (commandHistory.isEmpty()) return;
    
    historyIndex += direction;
    
    if (historyIndex < 0) {
        historyIndex = -1;
        consoleInput->clear();
    } else if (historyIndex >= commandHistory.size()) {
        historyIndex = commandHistory.size() - 1;
    }
    
    if (historyIndex >= 0 && historyIndex < commandHistory.size()) {
        consoleInput->setText(commandHistory[historyIndex]);
    }
}

void Sol2QtMainWindow::clearOutput() 
{
    outputDisplay->clear();
}

void Sol2QtMainWindow::loadExample() 
{
    QString example = R"(-- Sol2 + Qt6 Integration Example
print("=== Sol2 Lua Integration Demo ===")

-- Access the widget through Sol2 binding
widget = app_widget

-- Call Qt methods via Sol2
widget:setStatusString("Sol2 script is running!")
print("Status updated via Sol2")

-- Get slider value
slider_val = widget:getSliderValue()
print("Current slider value: " .. slider_val)

-- Set progress based on slider value
widget:setProgress(slider_val)

-- Mathematical operations with slider value
local angle_rad = slider_val * math.pi / 180
local sin_val = math.sin(angle_rad)
local progress_val = math.floor((sin_val + 1) * 50)

print("Sine calculation: sin(" .. slider_val .. "°) = " .. sin_val)
print("Setting progress to: " .. progress_val)

widget:animateProgress(progress_val)

-- Start animation
print("Starting animation...")
widget:startAnimation()

-- Use Lua's built-in functionality
local data = {10, 20, 30, 40, 50}
local sum = 0
for i, v in ipairs(data) do
    sum = sum + v
end
print("Sum of data: " .. sum)

-- String manipulation
local message = "Sol2 + Qt6 = "
local rating = "Excellent!"
local final_msg = message .. rating
widget:setStatusString(final_msg)
print("Final message: " .. final_msg)

-- Function definition in Lua
function fibonacci(n)
    if n <= 1 then
        return n
    else
        return fibonacci(n-1) + fibonacci(n-2)
    end
end

local fib_result = fibonacci(10)
print("Fibonacci(10) = " .. fib_result)

-- Try these in the console:
-- fibonacci(15)
-- widget:getSliderValue()
-- math.sqrt(144)
-- x = 42
-- x * 2

-- Return a value to Qt
return "Sol2 integration successful! Slider: " .. slider_val
)";
    
    scriptEditor->setPlainText(example);
}

void Sol2QtMainWindow::onControlWidgetSignal(const QString& text) 
{
    outputDisplay->append(QString("Qt Signal: %1").arg(text));
}

void Sol2QtMainWindow::appendToOutput(const std::string& text) 
{
    outputDisplay->append(QString::fromStdString(text));
}

// Additional methods for sol2qtmainwindow.cpp - Script Loading Implementation

// Add these methods to your Sol2QtMainWindow class implementation:

void Sol2QtMainWindow::setupMenuBar() 
{
    // File Menu
    fileMenu = menuBar()->addMenu("&File");
    
    newAction = new QAction("&New Script", this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip("Create a new Lua script");
    connect(newAction, &QAction::triggered, this, &Sol2QtMainWindow::newScript);
    fileMenu->addAction(newAction);
    
    openAction = new QAction("&Open Script...", this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip("Open an existing Lua script");
    connect(openAction, &QAction::triggered, this, &Sol2QtMainWindow::openScript);
    fileMenu->addAction(openAction);
    
    fileMenu->addSeparator();
    
    saveAction = new QAction("&Save Script", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip("Save the current script");
    connect(saveAction, &QAction::triggered, this, &Sol2QtMainWindow::saveScript);
    fileMenu->addAction(saveAction);
    
    saveAsAction = new QAction("Save Script &As...", this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip("Save the script with a new name");
    connect(saveAsAction, &QAction::triggered, this, &Sol2QtMainWindow::saveScriptAs);
    fileMenu->addAction(saveAsAction);
    
    fileMenu->addSeparator();
    
    // Recent Files submenu
    recentFilesMenu = fileMenu->addMenu("Recent Scripts");
    updateRecentFilesMenu();
    
    fileMenu->addSeparator();
    
    exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
    // Script Menu
    scriptMenu = menuBar()->addMenu("&Script");
    
    QAction* runAction = new QAction("&Run Script", this);
    runAction->setShortcut(QKeySequence("F5"));
    runAction->setStatusTip("Execute the current script");
    connect(runAction, &QAction::triggered, this, &Sol2QtMainWindow::runScript);
    scriptMenu->addAction(runAction);
    
    QAction* clearOutputAction = new QAction("&Clear Output", this);
    clearOutputAction->setShortcut(QKeySequence("Ctrl+L"));
    clearOutputAction->setStatusTip("Clear the output window");
    connect(clearOutputAction, &QAction::triggered, this, &Sol2QtMainWindow::clearOutput);
    scriptMenu->addAction(clearOutputAction);
    
    scriptMenu->addSeparator();
    
    QAction* loadExampleAction = new QAction("Load &Example", this);
    loadExampleAction->setStatusTip("Load an example script");
    connect(loadExampleAction, &QAction::triggered, this, &Sol2QtMainWindow::loadExample);
    scriptMenu->addAction(loadExampleAction);
}

void Sol2QtMainWindow::setupUI() 
{
    setWindowTitle("Sol2 + Qt6 Lua Integration - Script Editor");
    setMinimumSize(1200, 800);
    
    // Enable drag and drop
    setAcceptDrops(true);
    
    // Setup menu bar first
    setupMenuBar();
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Left side - Script editor and output
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    
    // Script editor section
    QWidget* editorWidget = new QWidget();
    QVBoxLayout* editorLayout = new QVBoxLayout(editorWidget);
    
    QLabel* editorLabel = new QLabel("Sol2 Lua Script Editor:");
    editorLabel->setStyleSheet("font-weight: bold; margin: 5px;");
    editorLayout->addWidget(editorLabel);
    
    scriptEditor = new QTextEdit();
    scriptEditor->setFont(QFont("Consolas", 12));
    scriptEditor->setPlainText("-- Sol2 + Qt6 script\nprint(\"Hello from Sol2!\")\napp_widget:setStatusString(\"Sol2 is working!\")\nreturn \"Success!\"\n");
    editorLayout->addWidget(scriptEditor);
    
    // Enhanced button layout with file operations
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    openScriptButton = new QPushButton("Open Script");
    openScriptButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-weight: bold; padding: 8px; }");
    connect(openScriptButton, &QPushButton::clicked, this, &Sol2QtMainWindow::openScript);
    
    saveScriptButton = new QPushButton("Save Script");
    saveScriptButton->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; font-weight: bold; padding: 8px; }");
    connect(saveScriptButton, &QPushButton::clicked, this, &Sol2QtMainWindow::saveScript);
    
    runButton = new QPushButton("Run Script (F5)");
    runButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    
    loadExampleButton = new QPushButton("Load Example");
    loadExampleButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; }");
    
    clearButton = new QPushButton("Clear Output");
    clearButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");
    
    buttonLayout->addWidget(openScriptButton);
    buttonLayout->addWidget(saveScriptButton);
    buttonLayout->addWidget(runButton);
    buttonLayout->addWidget(loadExampleButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();
    
    editorLayout->addLayout(buttonLayout);
    splitter->addWidget(editorWidget);
    
    // Output section (rest remains the same as before)
    QWidget* outputWidget = new QWidget();
    QVBoxLayout* outputLayout = new QVBoxLayout(outputWidget);
    
    QLabel* outputLabel = new QLabel("Output & Interactive Console:");
    outputLabel->setStyleSheet("font-weight: bold; margin: 5px;");
    outputLayout->addWidget(outputLabel);
    
    outputDisplay = new QTextEdit();
    outputDisplay->setFont(QFont("Consolas", 11));
    outputDisplay->setReadOnly(true);
    outputDisplay->setStyleSheet("background-color: #f5f5f5; border: 1px solid #ccc;");
    outputLayout->addWidget(outputDisplay);
    
    // Interactive console input
    QHBoxLayout* consoleLayout = new QHBoxLayout();
    QLabel* promptLabel = new QLabel("lua>");
    promptLabel->setStyleSheet("font-weight: bold; color: #2196F3; font-family: Consolas;");
    promptLabel->setMinimumWidth(40);
    
    consoleInput = new ConsoleLineEdit(this);
    consoleInput->setMainWindow(this);
    consoleInput->setFont(QFont("Consolas", 11));
    consoleInput->setPlaceholderText("Enter Lua command or 'dofile(\"script.lua\")' to load file");
    consoleInput->setStyleSheet("border: 2px solid #2196F3; padding: 5px; border-radius: 3px;");
    
    executeLineButton = new QPushButton("Execute");
    executeLineButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 5px 10px; border-radius: 3px; }");
    
    consoleLayout->addWidget(promptLabel);
    consoleLayout->addWidget(consoleInput, 1);
    consoleLayout->addWidget(executeLineButton);
    
    outputLayout->addLayout(consoleLayout);
    
    splitter->addWidget(outputWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    
    leftLayout->addWidget(splitter);
    mainLayout->addWidget(leftWidget, 2);
    
    // Right side - Qt widgets controlled by Lua
    setupControlPanel();
    mainLayout->addWidget(controlPanel, 1);
    
    // Connect signals
    connect(runButton, &QPushButton::clicked, this, &Sol2QtMainWindow::runScript);
    connect(clearButton, &QPushButton::clicked, this, &Sol2QtMainWindow::clearOutput);
    connect(loadExampleButton, &QPushButton::clicked, this, &Sol2QtMainWindow::loadExample);
    connect(executeLineButton, &QPushButton::clicked, this, &Sol2QtMainWindow::executeConsoleLine);
    
    // Set focus to console input for immediate typing
    consoleInput->setFocus();
}

// File operations implementation
void Sol2QtMainWindow::newScript() 
{
    scriptEditor->clear();
    scriptEditor->setPlainText("-- New Lua Script\nprint(\"Hello from new script!\")\n");
    currentScriptPath.clear();
    setWindowTitle("Sol2 + Qt6 Lua Integration - Script Editor [New Script]");
    appendToOutput("New script created");
}

void Sol2QtMainWindow::openScript() 
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open Lua Script",
        "",
        "Lua Scripts (*.lua);;Text Files (*.txt);;All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        loadScriptFromFile(filePath);
    }
}

void Sol2QtMainWindow::saveScript() 
{
    if (currentScriptPath.isEmpty()) {
        saveScriptAs();
    } else {
        QFile file(currentScriptPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << scriptEditor->toPlainText();
            appendToOutput("Script saved: " + currentScriptPath.toStdString());
        } else {
            QMessageBox::warning(this, "Error", "Could not save file: " + currentScriptPath);
        }
    }
}

void Sol2QtMainWindow::saveScriptAs() 
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Save Lua Script",
        "",
        "Lua Scripts (*.lua);;Text Files (*.txt);;All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        currentScriptPath = filePath;
        saveScript();
        updateRecentFiles(filePath);
        setWindowTitle("Sol2 + Qt6 Lua Integration - Script Editor [" + 
                      QFileInfo(filePath).fileName() + "]");
    }
}

bool Sol2QtMainWindow::loadScriptFromFile(const QString& filePath) 
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file: " + filePath);
        return false;
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    
    scriptEditor->setPlainText(content);
    currentScriptPath = filePath;
    updateRecentFiles(filePath);
    
    setWindowTitle("Sol2 + Qt6 Lua Integration - Script Editor [" + 
                  QFileInfo(filePath).fileName() + "]");
    
    appendToOutput("Script loaded: " + filePath.toStdString());
    appendToOutput("Lines: " + std::to_string(content.split('\n').count()));
    
    return true;
}

bool Sol2QtMainWindow::executeScript(const QString& script) 
{
    if (script.isEmpty()) {
        appendToOutput("Error: Empty script");
        return false;
    }
    
    try {
        auto result = lua->script(script.toStdString());
        
        if (result.valid()) {
            sol::object obj = result;
            if (!obj.is<sol::lua_nil_t>()) {
                if (obj.is<std::string>()) {
                    appendToOutput("Script returned: " + obj.as<std::string>());
                } else if (obj.is<double>()) {
                    appendToOutput("Script returned: " + std::to_string(obj.as<double>()));
                } else if (obj.is<bool>()) {
                    appendToOutput("Script returned: " + std::string(obj.as<bool>() ? "true" : "false"));
                }
            }
        }
        
        appendToOutput("Script executed successfully");
        return true;
        
    } catch (const sol::error& e) {
        appendToOutput("Lua Error: " + std::string(e.what()));
        return false;
    }
}

void Sol2QtMainWindow::updateRecentFiles(const QString& filePath) 
{
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    
    while (recentFiles.size() > MaxRecentFiles) {
        recentFiles.removeLast();
    }
    
    updateRecentFilesMenu();
}

void Sol2QtMainWindow::updateRecentFilesMenu() 
{
    recentFilesMenu->clear();
    
    for (int i = 0; i < recentFiles.size(); ++i) {
        const QString& filePath = recentFiles[i];
        QAction* action = new QAction(QString("&%1 %2").arg(i + 1).arg(QFileInfo(filePath).fileName()), this);
        action->setStatusTip(filePath);
        action->setData(filePath);
        connect(action, &QAction::triggered, this, &Sol2QtMainWindow::recentFileTriggered);
        recentFilesMenu->addAction(action);
    }
    
    if (recentFiles.isEmpty()) {
        QAction* noFilesAction = new QAction("No recent files", this);
        noFilesAction->setEnabled(false);
        recentFilesMenu->addAction(noFilesAction);
    }
}

void Sol2QtMainWindow::recentFileTriggered() 
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString filePath = action->data().toString();
        loadScriptFromFile(filePath);
    }
}

// Drag and drop support
void Sol2QtMainWindow::dragEnterEvent(QDragEnterEvent* event) 
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (urls.size() == 1) {
            QString filePath = urls.first().toLocalFile();
            if (filePath.endsWith(".lua") || filePath.endsWith(".txt")) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void Sol2QtMainWindow::dropEvent(QDropEvent* event) 
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString filePath = urls.first().toLocalFile();
        loadScriptFromFile(filePath);
        event->acceptProposedAction();
    }
}

void Sol2QtMainWindow::setupControlPanel() 
{
    controlPanel = new QGroupBox("Qt6 Widgets (Sol2 Controlled)");
    QVBoxLayout* controlLayout = new QVBoxLayout(controlPanel);
    
    controlWidget = new LuaControlledWidget(controlPanel);
    
    QLabel* titleLabel = new QLabel("Sol2 can control these Qt widgets:");
    titleLabel->setStyleSheet("font-weight: bold; margin-bottom: 10px;");
    controlLayout->addWidget(titleLabel);
    
    controlLayout->addWidget(new QLabel("Status:"));
    controlLayout->addWidget(controlWidget->getStatusLabel());
    
    controlLayout->addWidget(new QLabel("Progress:"));
    controlLayout->addWidget(controlWidget->getProgressBar());
    
    controlLayout->addWidget(new QLabel("Slider (Sol2 can read this):"));
    controlLayout->addWidget(controlWidget->getSlider());
    
    controlLayout->addStretch();
    
    // Connect signals
    connect(controlWidget, &LuaControlledWidget::statusChanged,
            this, &Sol2QtMainWindow::onControlWidgetSignal);
    
    connect(controlWidget->getSlider(), QOverload<int>::of(&QSlider::valueChanged),
            [this](int value) {
                outputDisplay->append(QString("Slider value: %1").arg(value));
            });
}
// Enhanced initializeSol2() method for sol2qtmainwindow.cpp

// Enhanced initializeSol2() method with script loading support

void Sol2QtMainWindow::initializeSol2() 
{
    // Create Sol2 state
    lua = new sol::state();
    
    // Open standard Lua libraries
    lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, 
                       sol::lib::table, sol::lib::io, sol::lib::os);
    
    // Redirect Lua print to Qt output
    lua->set_function("print", [this](sol::variadic_args va) {
        std::string output;
        for (auto arg : va) {
            std::string str = (*lua)["tostring"](arg);
            if (!output.empty()) output += "\t";
            output += str;
        }
        outputDisplay->append(QString::fromStdString(output));
    });
    
    // Enhanced dofile function with error handling and output
    lua->set_function("dofile", [this](const std::string& filename) -> sol::object {
        try {
            appendToOutput("Loading script: " + filename);
            
            QFile file(QString::fromStdString(filename));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                throw std::runtime_error("Cannot open file: " + filename);
            }
            
            QTextStream in(&file);
            QString content = in.readAll();
            
            appendToOutput("Executing script (" + std::to_string(content.length()) + " chars)");
            
            auto result = lua->script(content.toStdString());
            
            if (result.valid()) {
                appendToOutput("Script executed successfully: " + filename);
                return result.get<sol::object>();
            } else {
                throw std::runtime_error("Script execution failed");
            }
            
        } catch (const std::exception& e) {
            appendToOutput("Error in dofile: " + std::string(e.what()));
            throw;
        }
    });
    
    // Load script from string function
    lua->set_function("loadstring", [this](const std::string& script) -> sol::object {
        try {
            appendToOutput("Executing inline script...");
            auto result = lua->script(script);
            if (result.valid()) {
                appendToOutput("Inline script executed successfully");
                return result.get<sol::object>();
            } else {
                throw std::runtime_error("Inline script execution failed");
            }
        } catch (const std::exception& e) {
            appendToOutput("Error in loadstring: " + std::string(e.what()));
            throw;
        }
    });
    
    // File utility functions for Lua
    lua->set_function("file_exists", [](const std::string& filename) -> bool {
        QFile file(QString::fromStdString(filename));
        return file.exists();
    });
    
    lua->set_function("read_file", [this](const std::string& filename) -> std::string {
        QFile file(QString::fromStdString(filename));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("Cannot read file: " + filename);
        }
        QTextStream in(&file);
        return in.readAll().toStdString();
    });
    
    lua->set_function("write_file", [this](const std::string& filename, const std::string& content) -> bool {
        QFile file(QString::fromStdString(filename));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            appendToOutput("Error: Cannot write to file: " + filename);
            return false;
        }
        QTextStream out(&file);
        out << QString::fromStdString(content);
        appendToOutput("File written: " + filename);
        return true;
    });
    
    // Directory listing function
    lua->set_function("list_files", [this](const std::string& directory, const std::string& filter) -> std::vector<std::string> {
        QDir dir(QString::fromStdString(directory));
        QStringList nameFilters;
        if (!filter.empty()) {
            nameFilters << QString::fromStdString(filter);
        } else {
            nameFilters << "*";
        }
        
        QStringList files = dir.entryList(nameFilters, QDir::Files);
        std::vector<std::string> result;
        
        for (const QString& file : files) {
            result.push_back(file.toStdString());
        }
        
        return result;
    });
    
    // App control functions
    lua->set_function("load_script_to_editor", [this](const std::string& filename) {
        loadScriptFromFile(QString::fromStdString(filename));
    });
    
    lua->set_function("get_editor_content", [this]() -> std::string {
        return scriptEditor->toPlainText().toStdString();
    });
    
    lua->set_function("set_editor_content", [this](const std::string& content) {
        scriptEditor->setPlainText(QString::fromStdString(content));
    });
    
    lua->set_function("append_to_editor", [this](const std::string& content) {
        scriptEditor->append(QString::fromStdString(content));
    });
    
    lua->set_function("clear_output", [this]() {
        outputDisplay->clear();
    });
    
    // Bind our custom widget class to Lua
    lua->new_usertype<LuaControlledWidget>("LuaControlledWidget",
        "setStatusString", &LuaControlledWidget::setStatusString,
        "getStatusText", &LuaControlledWidget::getStatusText,
        "setProgress", &LuaControlledWidget::setProgress,
        "getSliderValue", &LuaControlledWidget::getSliderValue,
        "startAnimation", &LuaControlledWidget::startAnimation,
        "stopAnimation", &LuaControlledWidget::stopAnimation,
        "animateProgress", &LuaControlledWidget::animateProgress
    );
    
    // Bind main window methods
    lua->new_usertype<Sol2QtMainWindow>("MainWindow",
        "appendToOutput", &Sol2QtMainWindow::appendToOutput,
        "loadScriptFromFile", [this](const std::string& path) {
            return loadScriptFromFile(QString::fromStdString(path));
        },
        "executeScript", [this](const std::string& script) {
            return executeScript(QString::fromStdString(script));
        }
    );
    
    // Enhanced LuaWindow class binding (same as before)
    lua->new_usertype<LuaWindow>("LuaWindow",
        "addLabel", sol::overload(
            [](LuaWindow* window, const std::string& text) {
                window->addLabel(text);
            },
            [](LuaWindow* window, const std::string& text, const std::string& name) {
                window->addLabel(text, name);
            }
        ),
        "addButton", sol::overload(
            [](LuaWindow* window, const std::string& text) {
                window->addButton(text);
            },
            [](LuaWindow* window, const std::string& text, const std::string& name) {
                window->addButton(text, name);
            }
        ),
        "addTextEdit", sol::overload(
            [](LuaWindow* window) {
                window->addTextEdit();
            },
            [](LuaWindow* window, const std::string& placeholder) {
                window->addTextEdit(placeholder);
            },
            [](LuaWindow* window, const std::string& placeholder, const std::string& name) {
                window->addTextEdit(placeholder, name);
            }
        ),
        "addSlider", sol::overload(
            [](LuaWindow* window) {
                window->addSlider();
            },
            [](LuaWindow* window, int min, int max, int value) {
                window->addSlider(min, max, value);
            },
            [](LuaWindow* window, int min, int max, int value, const std::string& name) {
                window->addSlider(min, max, value, name);
            }
        ),
        "addLineEdit", sol::overload(
            [](LuaWindow* window) {
                window->addLineEdit();
            },
            [](LuaWindow* window, const std::string& placeholder) {
                window->addLineEdit(placeholder);
            },
            [](LuaWindow* window, const std::string& placeholder, const std::string& name) {
                window->addLineEdit(placeholder, name);
            }
        ),
        "setOnButtonClick", &LuaWindow::setOnButtonClick,
        "setOnSliderChange", &LuaWindow::setOnSliderChange,
        "setOnWindowClose", &LuaWindow::setOnWindowClose,
        "setOnWindowShow", &LuaWindow::setOnWindowShow,
        "setOnTextChange", &LuaWindow::setOnTextChange,
        "getWidgetText", &LuaWindow::getWidgetText,
        "setWidgetText", &LuaWindow::setWidgetText,
        "getSliderValue", &LuaWindow::getSliderValue,
        "setSliderValue", &LuaWindow::setSliderValue,
        "show", &LuaWindow::show,
        "setBackgroundColor", &LuaWindow::setBackgroundColor,
        "close", &QWidget::close
    );

    // Create window factory with Lua state
    windowFactory = new LuaWindowFactory(this, lua);

    // Bind window factory
    lua->new_usertype<LuaWindowFactory>("WindowFactory",
        "createWindow", sol::overload(
            [](LuaWindowFactory* factory, const std::string& title) {
                return factory->createWindow(title);
            },
            [](LuaWindowFactory* factory, const std::string& title, int width, int height) {
                return factory->createWindow(title, width, height);
            }
        ),
        "getWindowCount", &LuaWindowFactory::getWindowCount,
        "closeAllWindows", &LuaWindowFactory::closeAllWindows
    );

    // Make objects available to Lua
    (*lua)["app_widget"] = controlWidget;
    (*lua)["window_factory"] = windowFactory;
    (*lua)["main_window"] = this;
    
    outputDisplay->append("Sol2 Lua integration initialized with script loading!");
    outputDisplay->append("Available objects: app_widget, main_window, window_factory");
    outputDisplay->append("");
    outputDisplay->append("=== Script Loading Methods ===");
    outputDisplay->append("1. File Menu: File → Open Script");
    outputDisplay->append("2. Drag & Drop: Drop .lua files onto the window");
    outputDisplay->append("3. Console: dofile('script.lua')");
    outputDisplay->append("4. Console: loadstring('print(\"hello\")')");
    outputDisplay->append("");
    outputDisplay->append("=== File Functions Available in Lua ===");
    outputDisplay->append("  dofile('filename.lua') - Execute Lua file");
    outputDisplay->append("  loadstring('lua code') - Execute Lua string");
    outputDisplay->append("  file_exists('filename') - Check if file exists");
    outputDisplay->append("  read_file('filename') - Read file content");
    outputDisplay->append("  write_file('filename', 'content') - Write to file");
    outputDisplay->append("  list_files('directory', '*.lua') - List files");
    outputDisplay->append("");
    outputDisplay->append("=== Editor Control Functions ===");
    outputDisplay->append("  load_script_to_editor('filename.lua')");
    outputDisplay->append("  get_editor_content() - Get current script");
    outputDisplay->append("  set_editor_content('new content')");
    outputDisplay->append("  append_to_editor('additional content')");
    outputDisplay->append("  clear_output() - Clear this output window");
    outputDisplay->append("");
    outputDisplay->append("=== Interactive Console Ready ===");
    outputDisplay->append("Try: dofile('my_script.lua') or math.sqrt(16)");
    outputDisplay->append("Use ↑/↓ for command history\n");
}
