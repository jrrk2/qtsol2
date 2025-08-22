#include <sol/sol.hpp>
#include "sol2qtmainwindow.hpp"
#include "StarCatalogValidator.hpp"
#include "BrightStarDatabase.hpp"
#include "LuaControlledWidget.hpp"
#include "LuaWindowFactory.hpp"
#include "LuaChartWidget.hpp"
#include "LuaMatrix.hpp"
#include "GenericDataTableWidget.hpp"
#include "ImageDisplayWidget.hpp"
#include "StarStatisticsChartDialog.hpp"
#include "ImageReader.hpp"
#include "StarChartWidget.hpp"

int LuaWindow::windowCounter = 0;

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
/*
void Sol2QtMainWindow::loadExample() 
{
    QString example = R"(-- Sol2 + Qt6 Integration Example
print("=== Sol2 Lua Integration Demo ===")

-- Access the widget through Sol2 binding

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
*/
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
    /*    
    QAction* loadExampleAction = new QAction("Load &Example", this);
    loadExampleAction->setStatusTip("Load an example script");
    connect(loadExampleAction, &QAction::triggered, this, &Sol2QtMainWindow::loadExample);
    scriptMenu->addAction(loadExampleAction);
    */
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
    /*    
    loadExampleButton = new QPushButton("Load Example");
    loadExampleButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; }");
    */
    clearButton = new QPushButton("Clear Output");
    clearButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");
    
    buttonLayout->addWidget(openScriptButton);
    buttonLayout->addWidget(saveScriptButton);
    buttonLayout->addWidget(runButton);
    //    buttonLayout->addWidget(loadExampleButton);
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
    /*    
    // Right side - Qt widgets controlled by Lua
    setupControlPanel();
    mainLayout->addWidget(controlPanel, 1);
    */
    // Connect signals
    connect(runButton, &QPushButton::clicked, this, &Sol2QtMainWindow::runScript);
    connect(clearButton, &QPushButton::clicked, this, &Sol2QtMainWindow::clearOutput);
    //    connect(loadExampleButton, &QPushButton::clicked, this, &Sol2QtMainWindow::loadExample);
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

/*
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
*/

sol::table Sol2QtMainWindow::convertMatrixToTable(const AcceleratedMatrix& matrix) {
    sol::table matrixTable = lua->create_table();
    
    for (size_t i = 0; i < matrix.getRows(); ++i) {
        sol::table row = lua->create_table();
        for (size_t j = 0; j < matrix.getCols(); ++j) {
            row[j + 1] = matrix.get(i, j);  // Lua 1-based indexing
        }
        matrixTable[i + 1] = row;
    }
    
    return matrixTable;
}

// Enhanced initializeSol2() method with script loading support

void Sol2QtMainWindow::addWindowMenuBindings(sol::state* lua, LuaWindowFactory* factory) {
    // Enhance existing WindowFactory binding
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
        "closeAllWindows", &LuaWindowFactory::closeAllWindows,
        
        // New menu-related functions
        "showAllWindows", &LuaWindowFactory::showAllWindows,
        "hideAllWindows", &LuaWindowFactory::hideAllWindows,
        "getWindowTitles", &LuaWindowFactory::getWindowTitles,
        "findWindowByTitle", &LuaWindowFactory::findWindowByTitle,
        "findWindowById", &LuaWindowFactory::findWindowById
    );
    // Add window management functions
    lua->set_function("list_all_windows", [factory]() {
        auto titles = factory->getWindowTitles();
        for (size_t i = 0; i < titles.size(); ++i) {
	  qDebug() << "Window " << (i+1) << ": " << titles[i];
        }
        return titles.size();
    });
    
    lua->set_function("focus_window", [factory](const std::string& title) {
        LuaWindow* window = factory->findWindowByTitle(title);
        if (window) {
            window->show();
            window->raise();
            window->activateWindow();
            return true;
        }
        return false;
    });
}

// Add these bindings to your Sol2QtMainWindow::initializeSol2() method
// Place this after your existing ImageReader bindings

void Sol2QtMainWindow::setupImageDisplayBindings() {
    // Bind StarOverlay structure
    lua->new_usertype<StarOverlay>("StarOverlay",
        "center", &StarOverlay::center,
        "radius", &StarOverlay::radius,
        "flux", &StarOverlay::flux,
        "color", &StarOverlay::color,
        "visible", &StarOverlay::visible
    );
    
    // Bind ImageDisplayWidget class
    lua->new_usertype<ImageDisplayWidget>("ImageDisplayWidget",
        // Constructor
        sol::constructors<ImageDisplayWidget(QWidget*)>(),
        
        // Image data operations
        "setImageData", &ImageDisplayWidget::setImageData,
        "clearImage", &ImageDisplayWidget::clearImage,
        
        // Zoom controls
        "setZoomFactor", &ImageDisplayWidget::setZoomFactor,
        "getZoomFactor", &ImageDisplayWidget::zoomFactor,
        "zoomIn", [](ImageDisplayWidget& widget) {
            widget.setZoomFactor(widget.zoomFactor() * 1.5);
        },
        "zoomOut", [](ImageDisplayWidget& widget) {
            widget.setZoomFactor(widget.zoomFactor() / 1.5);
        },
        "zoomToFit", [](ImageDisplayWidget& widget) {
            // This would need to be made public or accessible
            QMetaObject::invokeMethod(&widget, "onZoomFitClicked");
        },
        "zoom100", [](ImageDisplayWidget& widget) {
            widget.setZoomFactor(1.0);
        },
        
        // Star overlay controls  
        "setStarOverlay", [](ImageDisplayWidget& widget, const sol::table& centers, const sol::table& radii) {
            QVector<QPoint> qCenters;
            QVector<float> qRadii;
            
            // Convert Lua tables to Qt vectors
            for (const auto& pair : centers) {
                if (pair.second.is<sol::table>()) {
                    sol::table point = pair.second.as<sol::table>();
                    int x = point.get_or(1, 0);
                    int y = point.get_or(2, 0);
                    qCenters.append(QPoint(x, y));
                }
            }
            
            for (const auto& pair : radii) {
                if (pair.second.is<double>()) {
                    qRadii.append(static_cast<float>(pair.second.as<double>()));
                }
            }
            
            widget.setStarOverlay(qCenters, qRadii);
        },
        
        "clearStarOverlay", &ImageDisplayWidget::clearStarOverlay,
        "setStarOverlayVisible", &ImageDisplayWidget::setStarOverlayVisible,
        "isStarOverlayVisible", &ImageDisplayWidget::isStarOverlayVisible,
        
        // Catalog overlay controls
        "setCatalogOverlayVisible", &ImageDisplayWidget::setCatalogOverlayVisible,
        "setValidationOverlayVisible", &ImageDisplayWidget::setValidationOverlayVisible,
        "isCatalogOverlayVisible", &ImageDisplayWidget::isCatalogOverlayVisible,
        "isValidationOverlayVisible", &ImageDisplayWidget::isValidationOverlayVisible,
        
        // Legend controls
        "setMagnitudeLegendVisible", &ImageDisplayWidget::setMagnitudeLegendVisible,
        "isMagnitudeLegendVisible", &ImageDisplayWidget::isMagnitudeLegendVisible,
        
        // Display in window
        "show", &ImageDisplayWidget::show,
        "hide", &ImageDisplayWidget::hide,
        "setWindowTitle", [](ImageDisplayWidget& widget, const std::string& title) {
            widget.setWindowTitle(QString::fromStdString(title));
        }
    );
    
    // Enhanced image display functions for Lua
    lua->set_function("create_image_display", [this](const std::string& title) -> ImageDisplayWidget* {
        ImageDisplayWidget* display = new ImageDisplayWidget();
        display->setWindowTitle(QString::fromStdString(title));
        display->setAttribute(Qt::WA_DeleteOnClose);
        display->resize(800, 600);
        
        // Connect signals to Lua callbacks if needed
        connect(display, &ImageDisplayWidget::imageClicked, 
                [this, display](int x, int y, float value) {
            // Call Lua callback if it exists
            if (lua->get<sol::optional<sol::function>>("on_image_clicked")) {
                auto callback = (*lua)["on_image_clicked"];
                try {
                    callback(x, y, value);
                } catch (const sol::error& e) {
                    appendToOutput("Error in image click callback: " + std::string(e.what()));
                }
            } else {
                appendToOutput("Image clicked: (" + std::to_string(x) + ", " + std::to_string(y) + 
                             ") value=" + std::to_string(value));
            }
        });
        
        return display;
    });
    
    // Convenience function to display an image
    lua->set_function("display_image", [this](const ImageData& imgData, const std::string& title) -> ImageDisplayWidget* {
        ImageDisplayWidget* display = new ImageDisplayWidget();
        display->setWindowTitle(QString::fromStdString(title));
        display->setAttribute(Qt::WA_DeleteOnClose);
        display->resize(800, 600);
        
        display->setImageData(imgData);
        display->show();
        
        appendToOutput("Image display window created: " + title);
        return display;
    });
    
    // Function to load and display image in one step
    lua->set_function("load_and_display_image", [this](const std::string& filepath) -> sol::object {
        // Load the image
        ImageReader reader;
        if (reader.readFile(QString(filepath.c_str()))) {
	  const ImageData& imageData = reader.imageData();
            ImageDisplayWidget* display = new ImageDisplayWidget();
            display->setWindowTitle(QString::fromStdString("Image: " + filepath));
            display->setAttribute(Qt::WA_DeleteOnClose);
            display->resize(800, 600);
            
            display->setImageData(imageData);
            display->show();
            
            // Return both the display widget and image data
            sol::table resultTable = lua->create_table();
            resultTable["success"] = true;
            resultTable["display"] = display;
            resultTable["data"] = imageData;
            
            appendToOutput("Image loaded and displayed: " + filepath);
            return resultTable;
        } else {
            sol::table resultTable = lua->create_table();
            resultTable["success"] = false;
            resultTable["error"] = reader.lastError();
            return resultTable;
        }
    });
    
    // Batch display function
    lua->set_function("display_image_grid", [this](const sol::table& imageList, int columns) {
        // This would create a grid layout of multiple ImageDisplayWidgets
        // Implementation depends on your specific needs
        appendToOutput("Grid display function - to be implemented");
    });
    
    // Star detection integration
    lua->set_function("detect_and_display_stars", [this](ImageDisplayWidget* display) {
        if (!display) {
            appendToOutput("Error: No display widget provided");
            return false;
        }
        
        // This would integrate with your star detection algorithms
        // For now, create mock star data
        QVector<QPoint> centers;
        QVector<float> radii;
        
        // Add some example stars (in a real implementation, this would call your star detection)
        centers.append(QPoint(100, 150));
        centers.append(QPoint(250, 200));
        centers.append(QPoint(400, 300));
        
        radii.append(5.0f);
        radii.append(8.0f);
        radii.append(6.0f);
        
        display->setStarOverlay(centers, radii);
        display->setStarOverlayVisible(true);
        
        appendToOutput("Mock stars detected and displayed");
        return true;
    });
    // Bind CatalogStar structure
    lua->new_usertype<CatalogStar>("CatalogStar",
        sol::constructors<
            CatalogStar(),
            CatalogStar(const QString&, double, double, double)
        >(),
        "id", &CatalogStar::id,
        "ra", &CatalogStar::ra,
        "dec", &CatalogStar::dec,
        "magnitude", &CatalogStar::magnitude,
        "spectralType", &CatalogStar::spectralType,
        "pixelPos", &CatalogStar::pixelPos,
        "isValid", &CatalogStar::isValid,
        
        // Add enhanced photometry fields
        "magBP", &CatalogStar::magBP,
        "magRP", &CatalogStar::magRP,
        "colorBP_RP", &CatalogStar::colorBP_RP,
        "hasExtendedData", &CatalogStar::hasExtendedData,
        
        // Add helper methods for Lua
        "setPixelPosition", [](CatalogStar& star, double x, double y) {
            star.pixelPos = QPointF(x, y);
            return 0; // Return value for void functions
        },
        "getX", [](const CatalogStar& star) -> double {
            return star.pixelPos.x();
        },
        "getY", [](const CatalogStar& star) -> double {
            return star.pixelPos.y();
        },
        "toString", [](const CatalogStar& star) -> std::string {
            return QString("Star %1 (RA=%2, Dec=%3, Mag=%4, Type=%5)")
                .arg(star.id)
                .arg(star.ra)
                .arg(star.dec)
                .arg(star.magnitude)
                .arg(star.spectralType).toStdString();
        }
    );
#if 0    
    // Bind AstrometricMetadata class
    lua->new_usertype<pcl::AstrometricMetadata>("AstrometricMetadata",
        sol::constructors<pcl::AstrometricMetadata()>(),
        "IsValid", &pcl::AstrometricMetadata::IsValid,
        "Width", &pcl::AstrometricMetadata::Width,
        "Height", &pcl::AstrometricMetadata::Height
						/*
						  "RA", &pcl::AstrometricMetadata::RA,
						  "Dec", &pcl::AstrometricMetadata::Dec,
						  "FocalLength", &pcl::AstrometricMetadata::FocalLength,
						  "PixelSize", &pcl::AstrometricMetadata::PixelSize
						*/
    );
#endif        
    // Bind StarCatalogValidator class - only include methods that exist in your class
    lua->new_usertype<StarCatalogValidator>("StarCatalogValidator",
        sol::constructors<
            StarCatalogValidator(QObject*)
        >(),
        
        // Only include methods that actually exist in your class
        "getCatalogStars", &StarCatalogValidator::getCatalogStars,
        
        // QueryCatalog wrapper
        "queryCatalog", sol::overload(
            [](StarCatalogValidator& validator, double centerRA, double centerDec, double radius) {
                validator.queryCatalog(centerRA, centerDec, radius);
                return 0; // Return value for void functions
            },
            [](StarCatalogValidator& validator, double centerRA, double centerDec) {
	      validator.queryCatalog(centerRA, centerDec, 1.0);
                return 0; // Return value for void functions
            }
        ),
        
        // Coordinate conversion methods
        "skyToPixel", &StarCatalogValidator::skyToPixel,
        "pixelToSky", &StarCatalogValidator::pixelToSky,
        
        // Information methods
        "showCatalogStats", &StarCatalogValidator::showCatalogStats,
        "getWidth", &StarCatalogValidator::getWidth,
        "getHeight", &StarCatalogValidator::getHeight,
        "getPixScale", &StarCatalogValidator::getPixScale,
        
        // GetCenter wrapper
        "getCenter", [this](StarCatalogValidator& validator) -> sol::table {
            double ra = 0, dec = 0;
            validator.getCenter(ra, dec);
            
            sol::table result = lua->create_table();
            result["ra"] = ra;
            result["dec"] = dec;
            return result;
        },
        
        // Helper methods for Lua
        "createCatalogStar", [](StarCatalogValidator& validator, 
                               const std::string& id, double ra, double dec, double magnitude) -> CatalogStar {
            CatalogStar star(QString::fromStdString(id), ra, dec, magnitude);
            star.pixelPos = validator.skyToPixel(ra, dec);
            return star;
        }
    );

    lua->set_function("queryCatalog", [](double ra, double dec, double radius, 
                                        const pcl::AstrometricMetadata& metadata) -> QVector<CatalogStar> {
        StarCatalogValidator validator(nullptr);
        validator.queryCatalog(ra, dec, radius);
        return validator.getCatalogStars();
    });

}

// Add this to Sol2QtMainWindow::initializeSol2() method
// Place this after your existing chart widget bindings

void Sol2QtMainWindow::setupStarChartBindings() {
    // Bind StarChartWidget class
    lua->new_usertype<StarChartWidget>("StarChartWidget",
        // Constructor
        sol::constructors<StarChartWidget(QWidget*)>(),
        
        // Core star data methods
        "setStarData", [](StarChartWidget& widget, const QVector<CatalogStar>& stars) {
            widget.setStarData(stars);
        },
        
        // Plot configuration methods
        "setPlotMode", &StarChartWidget::setPlotMode,
        "setColorScheme", &StarChartWidget::setColorScheme,
        "setShowLegend", &StarChartWidget::setShowLegend,
        "setShowGrid", &StarChartWidget::setShowGrid,
        "resetZoom", &StarChartWidget::resetZoom,
        
        // Widget methods from QWidget base class
        "show", &QWidget::show,
        "hide", &QWidget::hide,
        "close", &QWidget::close,
        "setWindowTitle", [](StarChartWidget& widget, const std::string& title) {
            widget.setWindowTitle(QString::fromStdString(title));
        },
        "resize", [](StarChartWidget& widget, int width, int height) {
            widget.resize(width, height);
        },
        "move", [](StarChartWidget& widget, int x, int y) {
            widget.move(x, y);
        },
        
        // Signal connections (using lambdas to handle signal connection)
        "connectStarClicked", [this](StarChartWidget& widget, sol::function callback) {
            QObject::connect(&widget, &StarChartWidget::starClicked, 
                           [callback](const CatalogStar& star) {
                               if (callback.valid()) {
                                   try {
                                       callback(star);
                                   } catch (const sol::error& e) {
                                       qDebug() << "Lua callback error:" << e.what();
                                   }
                               }
                           });
        },
        
        "connectStarHovered", [this](StarChartWidget& widget, sol::function callback) {
            QObject::connect(&widget, &StarChartWidget::starHovered, 
                           [callback](const CatalogStar& star) {
                               if (callback.valid()) {
                                   try {
                                       callback(star);
                                   } catch (const sol::error& e) {
                                       qDebug() << "Lua callback error:" << e.what();
                                   }
                               }
                           });
        }
    );
    
    // Add convenience function to create star chart
    lua->set_function("create_star_chart", [this](const std::string& title) -> StarChartWidget* {
        StarChartWidget* chart = new StarChartWidget();
        chart->setWindowTitle(QString::fromStdString(title));
        chart->setAttribute(Qt::WA_DeleteOnClose);
        return chart;
    });
    
    // Add convenience function to create star chart with data
    lua->set_function("create_star_chart_with_data", [this](const QVector<CatalogStar>& stars, const std::string& title) -> StarChartWidget* {
        StarChartWidget* chart = new StarChartWidget();
        chart->setWindowTitle(QString::fromStdString(title));
        chart->setStarData(stars);
        chart->setAttribute(Qt::WA_DeleteOnClose);
        chart->show();
        return chart;
    });
}

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

    windowFactory->setupWindowMenu();
    
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
	"closeAllWindows", &LuaWindowFactory::closeAllWindows,
        "showAllWindows", &LuaWindowFactory::showAllWindows,
        "hideAllWindows", &LuaWindowFactory::hideAllWindows,
        "getWindowTitles", &LuaWindowFactory::getWindowTitles,
        "findWindowByTitle", &LuaWindowFactory::findWindowByTitle,
        "findWindowById", &LuaWindowFactory::findWindowById
					
    );

    // Bind chart widget
    lua->new_usertype<LuaChartWidget>("ChartWidget",
	"setFrequency", &LuaChartWidget::setFrequency,
	"setAmplitude", &LuaChartWidget::setAmplitude,
	"setPhase", &LuaChartWidget::setPhase,
	"getFrequency", &LuaChartWidget::getFrequency,
	"getAmplitude", &LuaChartWidget::getAmplitude,
	"getPhase", &LuaChartWidget::getPhase,
	"addPoint", &LuaChartWidget::addPoint,
	"clearSeries", &LuaChartWidget::clearSeries,
	"plotFunction", &LuaChartWidget::plotFunction,
	"setTitle", &LuaChartWidget::setTitle,
	"setAxisLabels", &LuaChartWidget::setAxisLabels
    );

    // Add chart creation to window factory
    lua->set_function("create_chart_window", [this](const std::string& title) {
	LuaChartWidget* chartWidget = new LuaChartWidget();
	chartWidget->setWindowTitle(QString::fromStdString(title));
	chartWidget->show();
	return chartWidget;
    });

    // Bind basic matrix class
    lua->new_usertype<LuaMatrix>("Matrix",
        // Constructors
        sol::constructors<LuaMatrix(size_t, size_t)>(),
        
        // Basic operations
        "get", &LuaMatrix::get,
        "set", &LuaMatrix::set,
        "getRows", &LuaMatrix::getRows,
        "getCols", &LuaMatrix::getCols,
        
        // Matrix operations
        "multiply", &LuaMatrix::multiply,
        "add", &LuaMatrix::add,
        "subtract", &LuaMatrix::subtract,
        "transpose", &LuaMatrix::transpose,
        "scale", &LuaMatrix::scale,
        "determinant", &LuaMatrix::determinant,
        
        // Utility functions
        "fillRandom", sol::overload(
            [](LuaMatrix& m) { m.fillRandom(); },
            [](LuaMatrix& m, double min, double max) { m.fillRandom(min, max); }
        ),
        "fillIdentity", &LuaMatrix::fillIdentity,
        "toString", &LuaMatrix::toString,
        "getRow", &LuaMatrix::getRow,
        "getCol", &LuaMatrix::getCol
    );
    
    // Matrix creation helpers
    lua->set_function("create_matrix", [](size_t rows, size_t cols) {
        return LuaMatrix(rows, cols);
    });
    
    lua->set_function("create_identity", [](size_t size) {
        LuaMatrix m(size, size);
        m.fillIdentity();
        return m;
    });
    
    lua->set_function("create_random_matrix", [](size_t rows, size_t cols, double min, double max) {
        LuaMatrix m(rows, cols);
        m.fillRandom(min, max);
        return m;
    });
    
    // Add this to your Lua bindings
    lua->set_function("show_labeled_matrix", [this](
	const AcceleratedMatrix& matrix, 
	const std::string& title,
	const sol::optional<sol::table>& row_labels,
	const sol::optional<sol::table>& col_labels
    ) {
	GenericDataTableWidget* table = new GenericDataTableWidget(
	    QString::fromStdString("Labeled Matrix: " + title)
	);
	table->setLuaState(lua);

	// Set custom headers if provided
	if (col_labels) {
	    QStringList headers;
	    for (const auto& pair : *col_labels) {
		headers << QString::fromStdString(pair.second.as<std::string>());
	    }
	    table->setCustomHeaders(headers);
	}

	// Set custom row labels if provided
	if (row_labels) {
	    QStringList rowLabels;
	    for (const auto& pair : *row_labels) {
		rowLabels << QString::fromStdString(pair.second.as<std::string>());
	    }
	    table->setCustomRowLabels(rowLabels);
	}

	// Convert and display matrix data
	sol::table matrixTable = convertMatrixToTable(matrix);
	table->displayData(matrixTable, QString::fromStdString(title));
	table->show();

	return table;
    });    
    // Vector creation functions
    lua->set_function("create_vector", [](size_t size) {
        return std::vector<double>(size, 0.0);
    });
    
    lua->set_function("create_vector_from_table", [](const sol::table& t) {
        std::vector<double> vec;
        for (size_t i = 1; i <= t.size(); ++i) {  // Lua arrays are 1-indexed
            vec.push_back(t[i].get_or(0.0));
        }
        return vec;
    });
    
    // Vector access functions
    lua->set_function("vector_get", [](const std::vector<double>& vec, size_t index) -> double {
        return (index < vec.size()) ? vec[index] : 0.0;
    });
    
    lua->set_function("vector_set", [](std::vector<double>& vec, size_t index, double value) {
        if (index < vec.size()) {
            vec[index] = value;
        }
    });
    
    lua->set_function("vector_size", [](const std::vector<double>& vec) {
        return vec.size();
    });
    
    lua->set_function("vector_push", [](std::vector<double>& vec, double value) {
        vec.push_back(value);
    });
    
    lua->set_function("vector_clear", [](std::vector<double>& vec) {
        vec.clear();
    });
    
    lua->set_function("vector_to_table", [this](const std::vector<double>& vec) {
        sol::table result = lua->create_table();
        for (size_t i = 0; i < vec.size(); ++i) {
            result[i + 1] = vec[i];  // Lua uses 1-based indexing
        }
        return result;
    });
    
    lua->set_function("table_to_vector", [](const sol::table& t) {
        std::vector<double> vec;
        vec.reserve(t.size());
        for (size_t i = 1; i <= t.size(); ++i) {
            vec.push_back(t[i].get_or(0.0));
        }
        return vec;
    });

    // CORRECTED Vector math functions
    lua->set_function("vector_norm", [](const std::vector<double>& v) -> double {
        double sum = 0.0;
        for (const double& x : v) {
            sum += x * x;
        }
        return std::sqrt(sum);
    });
    
    lua->set_function("vector_normalize", [](std::vector<double>& v) -> double {
        double norm = 0.0;
        for (const double& x : v) {
            norm += x * x;
        }
        norm = std::sqrt(norm);
        
        if (norm > 1e-15) {  // Avoid division by zero
            for (double& x : v) {
                x /= norm;
            }
        }
        return norm;  // Return the original norm
    });
    
    lua->set_function("dot_product", [](const std::vector<double>& a, const std::vector<double>& b) -> double {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be same size for dot product");
        }
        double result = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    });
    
    lua->set_function("vector_add", [](const std::vector<double>& a, const std::vector<double>& b) -> std::vector<double> {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be same size for addition");
        }
        std::vector<double> result;
        result.reserve(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result.push_back(a[i] + b[i]);
        }
        return result;
    });
    
    lua->set_function("vector_subtract", [](const std::vector<double>& a, const std::vector<double>& b) -> std::vector<double> {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be same size for subtraction");
        }
        std::vector<double> result;
        result.reserve(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result.push_back(a[i] - b[i]);
        }
        return result;
    });
    
    lua->set_function("vector_scale", [](const std::vector<double>& v, double factor) -> std::vector<double> {
        std::vector<double> result;
        result.reserve(v.size());
        for (const double& x : v) {
            result.push_back(x * factor);
        }
        return result;
    });
    
    // DEBUGGING: Add a simple test function
    lua->set_function("test_vector_norm_debug", []() -> double {
        std::vector<double> test_vec = {3.0, 4.0, 0.0};
        double sum = 0.0;
        for (const double& x : test_vec) {
            sum += x * x;
        }
        return std::sqrt(sum);  // Should return 5.0
    });
    
    // DEBUGGING: Add verbose vector norm
    lua->set_function("vector_norm_verbose", [this](const std::vector<double>& v) -> double {
        outputDisplay->append("Debug: vector_norm_verbose called");
        outputDisplay->append("Vector size: " + QString::number(v.size()));
        
        double sum = 0.0;
        for (size_t i = 0; i < v.size(); ++i) {
            outputDisplay->append("v[" + QString::number(i) + "] = " + QString::number(v[i]));
            sum += v[i] * v[i];
        }
        
        double norm = std::sqrt(sum);
        outputDisplay->append("Calculated norm: " + QString::number(norm));
        return norm;
    });
    
    // DEBUGGING: Test if vector contains expected values
    lua->set_function("debug_vector_contents", [this](const std::vector<double>& v) {
        outputDisplay->append("=== Debug Vector Contents ===");
        outputDisplay->append("Size: " + QString::number(v.size()));
        for (size_t i = 0; i < v.size(); ++i) {
            outputDisplay->append("  [" + QString::number(i) + "] = " + QString::number(v[i]));
        }
        outputDisplay->append("=== End Debug ===");
    });
    
    lua->set_function("vector_cross_product", [](const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != 3 || b.size() != 3) {
            throw std::invalid_argument("Cross product only defined for 3D vectors");
        }
        std::vector<double> result(3);
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
        return result;
    });
    
    // Statistics functions for vectors
    lua->set_function("vector_mean", [](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        double sum = 0.0;
        for (double x : v) sum += x;
        return sum / v.size();
    });
    
    lua->set_function("vector_std", [](const std::vector<double>& v) {
        if (v.size() < 2) return 0.0;
        
        double mean = 0.0;
        for (double x : v) mean += x;
        mean /= v.size();
        
        double variance = 0.0;
        for (double x : v) {
            double diff = x - mean;
            variance += diff * diff;
        }
        variance /= (v.size() - 1);  // Sample standard deviation
        
        return std::sqrt(variance);
    });
    
    lua->set_function("vector_min", [](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        return *std::min_element(v.begin(), v.end());
    });
    
    lua->set_function("vector_max", [](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        return *std::max_element(v.begin(), v.end());
    });
    
    lua->set_function("vector_sum", [](const std::vector<double>& v) {
        double sum = 0.0;
        for (double x : v) sum += x;
        return sum;
    });
    
    // Enhanced matrix operations that work with the fixed vector system
    lua->set_function("matrix_vector_multiply", [](const LuaMatrix& matrix, const std::vector<double>& vec) {
        if (matrix.getCols() != vec.size()) {
            throw std::invalid_argument("Matrix columns must equal vector size");
        }
        
        std::vector<double> result(matrix.getRows(), 0.0);
        for (size_t i = 0; i < matrix.getRows(); ++i) {
            for (size_t j = 0; j < matrix.getCols(); ++j) {
                result[i] += matrix.get(i, j) * vec[j];
            }
        }
        return result;
    });
    
    lua->set_function("matrix_get_row_vector", [](const LuaMatrix& matrix, size_t row) {
        if (row >= matrix.getRows()) throw std::out_of_range("Row index out of range");
        std::vector<double> result(matrix.getCols());
        for (size_t j = 0; j < matrix.getCols(); ++j) {
            result[j] = matrix.get(row, j);
        }
        return result;
    });
    
    lua->set_function("matrix_get_col_vector", [](const LuaMatrix& matrix, size_t col) {
        if (col >= matrix.getCols()) throw std::out_of_range("Column index out of range");
        std::vector<double> result(matrix.getRows());
        for (size_t i = 0; i < matrix.getRows(); ++i) {
            result[i] = matrix.get(i, col);
        }
        return result;
    });
    
    lua->set_function("matrix_set_row_vector", [](LuaMatrix& matrix, size_t row, const std::vector<double>& vec) {
        if (row >= matrix.getRows()) throw std::out_of_range("Row index out of range");
        if (vec.size() != matrix.getCols()) throw std::invalid_argument("Vector size must match matrix columns");
        
        for (size_t j = 0; j < matrix.getCols(); ++j) {
            matrix.set(row, j, vec[j]);
        }
    });
    
    lua->set_function("matrix_set_col_vector", [](LuaMatrix& matrix, size_t col, const std::vector<double>& vec) {
        if (col >= matrix.getCols()) throw std::out_of_range("Column index out of range");
        if (vec.size() != matrix.getRows()) throw std::invalid_argument("Vector size must match matrix rows");
        
        for (size_t i = 0; i < matrix.getRows(); ++i) {
            matrix.set(i, col, vec[i]);
        }
    });
    
    // Vector math functions
    lua->set_function("dot_product", [](const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) throw std::invalid_argument("Vectors must be same size");
        double result = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    });
    
    lua->set_function("vector_norm", [](const std::vector<double>& v) {
        double sum = 0.0;
        for (double x : v) sum += x * x;
        return std::sqrt(sum);
    });
    
    lua->set_function("vector_normalize", [](std::vector<double>& v) {
        double norm = 0.0;
        for (double x : v) norm += x * x;
        norm = std::sqrt(norm);
        if (norm > 0) {
            for (double& x : v) x /= norm;
        }
    });
    
    // Performance timing utilities
    lua->set_function("get_time_ms", []() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    });

    // Bind AcceleratedMatrix class for high-performance linear algebra
    lua->new_usertype<AcceleratedMatrix>("AcceleratedMatrix",
        // Constructors
        sol::constructors<AcceleratedMatrix(size_t, size_t)>(),
        
        // Basic operations
        "get", &AcceleratedMatrix::get,
        "set", &AcceleratedMatrix::set,
        "getRows", &AcceleratedMatrix::getRows,
        "getCols", &AcceleratedMatrix::getCols,
        
        // Standard matrix operations
        "multiply", &AcceleratedMatrix::multiply,
        "add", &AcceleratedMatrix::add,
        "subtract", &AcceleratedMatrix::subtract,
        "transpose", &AcceleratedMatrix::transpose,
        "scale", &AcceleratedMatrix::scale,
        "determinant", &AcceleratedMatrix::determinant,
        "norm", &AcceleratedMatrix::norm,
        
#ifdef __APPLE__
        // High-performance Accelerate operations
        "multiplyAccelerate", &AcceleratedMatrix::multiplyAccelerate,

    // Replace the multiplyVector binding with this version:
    "multiplyVector", [this](const AcceleratedMatrix& matrix, const sol::table& vec_table) -> sol::table {
	try {
	    // Convert Lua table to std::vector<double>
	    std::vector<double> input_vector;
	    input_vector.reserve(vec_table.size());

	    for (size_t i = 1; i <= vec_table.size(); ++i) {  // Lua 1-based indexing
		input_vector.push_back(vec_table[i].get_or(0.0));
	    }

	    auto result_vector = matrix.multiplyVector(input_vector);

	    // Convert result back to Lua table
	    sol::table result = lua->create_table();
	    for (size_t i = 0; i < result_vector.size(); ++i) {
		result[i + 1] = result_vector[i];  // Lua 1-based indexing
	    }

	    return result;

	} catch (const std::exception& e) {
	    outputDisplay->append("Matrix-vector multiply error: " + QString::fromStdString(e.what()));
	    return lua->create_table();  // Return empty table on error
	}
    },
        
        // LAPACK operations
        "inverse", &AcceleratedMatrix::inverse,
    // Fixed eigenvalue binding that returns proper Lua table
    "eigenvalues", [this](const AcceleratedMatrix& matrix) -> sol::table {
        try {
            auto eigenvalue_pair = matrix.eigenvalues();
            
            sol::table result = lua->create_table();
            sol::table real_parts = lua->create_table();
            sol::table imag_parts = lua->create_table();
            
            const auto& real_vals = eigenvalue_pair.first;
            const auto& imag_vals = eigenvalue_pair.second;
            
            // Fill real parts
            for (size_t i = 0; i < real_vals.size(); ++i) {
                real_parts[i + 1] = real_vals[i];  // Lua 1-based indexing
            }
            
            // Fill imaginary parts  
            for (size_t i = 0; i < imag_vals.size(); ++i) {
                imag_parts[i + 1] = imag_vals[i];
            }
            
            result["real"] = real_parts;
            result["imag"] = imag_parts;
            result[1] = real_parts;  // For backward compatibility
            result[2] = imag_parts;
            
            return result;
            
        } catch (const std::exception& e) {
            outputDisplay->append("Eigenvalue error: " + QString::fromStdString(e.what()));
            return lua->create_table();  // Return empty table on error
        }
    },
    
// Quick fix for linear solve binding in initializeSol2()

// Replace the solve binding with this version that handles Lua tables:

    "solve", [this](const AcceleratedMatrix& matrix, const sol::table& b_table) -> sol::table {
	try {
	    // Convert Lua table to std::vector<double>
	    std::vector<double> b_vector;
	    b_vector.reserve(b_table.size());

	    for (size_t i = 1; i <= b_table.size(); ++i) {  // Lua 1-based indexing
		b_vector.push_back(b_table[i].get_or(0.0));
	    }

	    auto solution = matrix.solve(b_vector);

	    sol::table result = lua->create_table();
	    for (size_t i = 0; i < solution.size(); ++i) {
		result[i + 1] = solution[i];  // Lua 1-based indexing
	    }

	    return result;

	} catch (const std::exception& e) {
	    outputDisplay->append("Linear solve error: " + QString::fromStdString(e.what()));
	    return lua->create_table();  // Return empty table on error
	}
    },    
    // Enhanced QR decomposition binding
    "qrDecomposition", [this](const AcceleratedMatrix& matrix) -> sol::table {
        try {
            auto qr_result = matrix.qrDecomposition();
            
            sol::table result = lua->create_table();
            result["Q"] = qr_result.first;
            result["R"] = qr_result.second;
            result[1] = qr_result.first;   // For backward compatibility
            result[2] = qr_result.second;
            
            return result;
            
        } catch (const std::exception& e) {
            outputDisplay->append("QR decomposition error: " + QString::fromStdString(e.what()));
            return lua->create_table();
        }
    },
    
    // Enhanced SVD binding
    "svd", [this](const AcceleratedMatrix& matrix) -> sol::table {
        try {
	    // SVD using Accelerate LAPACK
	    AcceleratedMatrix a_copy = matrix;

	    int m = static_cast<int>(matrix.getRows());
	    int n = static_cast<int>(matrix.getCols());
	    int lda = m;
	    int min_mn = std::min(m, n);

	    char jobu = 'A', jobvt = 'A';  // Compute full U and VT

	    AcceleratedMatrix U(m, m);
	    AcceleratedMatrix VT(n, n);
	    std::vector<double> S(min_mn);

	    int ldu = m, ldvt = n;

	    // Query optimal workspace size
	    double work_query;
	    int lwork = -1;
	    int info;

	    dgesvd_(&jobu, &jobvt, &m, &n, a_copy.getData(), &lda,
		    S.data(), U.getData(), &ldu, VT.getData(), &ldvt,
		    &work_query, &lwork, &info);

	    // Perform SVD
	    lwork = static_cast<int>(work_query);
	    std::vector<double> work(lwork);

	    dgesvd_(&jobu, &jobvt, &m, &n, a_copy.getData(), &lda,
		    S.data(), U.getData(), &ldu, VT.getData(), &ldvt,
		    work.data(), &lwork, &info);

	    if (info < 0) {
		throw std::runtime_error("LAPACK dgesvd: illegal parameter at position " + std::to_string(-info));
	    } else if (info > 0) {
		throw std::runtime_error("SVD failed to converge");
	    }
            
            sol::table result = lua->create_table();
            result["U"] = U;
            result["S"] = lua->create_table();
            result["VT"] = VT;
            
            // Fill singular values table
            sol::table S_table = result["S"];
            for (size_t i = 0; i < S.size(); ++i) {
                S_table[i + 1] = S[i];
            }
            
            return result;
            
        } catch (const std::exception& e) {
            outputDisplay->append("SVD error: " + QString::fromStdString(e.what()));
            return lua->create_table();
        }
    },
        "luFactorization", &AcceleratedMatrix::luFactorization,
        "qrDecomposition", &AcceleratedMatrix::qrDecomposition,
//        "svd", &AcceleratedMatrix::svd,
#endif
        // Utility functions
        "fillRandom", sol::overload(
            [](AcceleratedMatrix& m) { m.fillRandom(); },
            [](AcceleratedMatrix& m, double min, double max) { m.fillRandom(min, max); }
        ),
	"fillIdentity", &AcceleratedMatrix::fillIdentity,
        "toString", &AcceleratedMatrix::toString
    );
    
    // Factory functions for accelerated matrices
    lua->set_function("create_accelerated_matrix", [](size_t rows, size_t cols) {
        return AcceleratedMatrix(rows, cols);
    });
    
    lua->set_function("create_accelerated_identity", [](size_t size) {
        AcceleratedMatrix m(size, size);
        m.fillIdentity();
        return m;
    });

    lua->set_function("create_accelerated_random", [](size_t rows, size_t cols, double min, double max) {
        AcceleratedMatrix m(rows, cols);
        m.fillRandom(min, max);
        return m;
    });

    // Performance timing utilities
    lua->set_function("benchmark_matrix_multiply", [](size_t size, int iterations) {
        AcceleratedMatrix a(size, size);
        AcceleratedMatrix b(size, size);
        a.fillRandom(-1, 1);
        b.fillRandom(-1, 1);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
#ifdef __APPLE__
            auto result = a.multiplyAccelerate(b);
#else
            auto result = a.multiply(b);
#endif
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return duration.count();
    });

    // Memory usage estimation
    lua->set_function("estimate_matrix_memory", [](size_t rows, size_t cols) {
        double bytes = rows * cols * sizeof(double);
        double mb = bytes / (1024.0 * 1024.0);
        return mb;
    });
    
    // System information
    lua->set_function("get_accelerate_info", []() -> std::string {
#ifdef __APPLE__
        return "macOS Accelerate Framework available\n"
               "- Optimized BLAS/LAPACK routines\n"
               "- Multi-threaded operations\n"
               "- SIMD vectorization\n"
               "- Apple Silicon / Intel optimized";
#else
        return "Accelerate Framework not available\n"
               "Using basic C++ implementations";
#endif
    });
    
    // FLOPS calculation utilities
    lua->set_function("calculate_gflops", [](size_t matrix_size, double time_ms) {
        double flops = 2.0 * matrix_size * matrix_size * matrix_size;  // Matrix multiply FLOPs
        double gflops = flops / (time_ms * 1e6);  // Convert to GFLOPS
        return gflops;
    });

    // Condition number estimation
    lua->set_function("estimate_condition_number", [](const AcceleratedMatrix& matrix) -> double {
#ifdef __APPLE__
        try {
	    // SVD using Accelerate LAPACK
	    AcceleratedMatrix a_copy = matrix;

	    int m = static_cast<int>(matrix.getRows());
	    int n = static_cast<int>(matrix.getCols());
	    int lda = m;
	    int min_mn = std::min(m, n);

	    char jobu = 'A', jobvt = 'A';  // Compute full U and VT

	    AcceleratedMatrix U(m, m);
	    AcceleratedMatrix VT(n, n);
	    std::vector<double> S(min_mn);

	    int ldu = m, ldvt = n;

	    // Query optimal workspace size
	    double work_query;
	    int lwork = -1;
	    int info;

	    dgesvd_(&jobu, &jobvt, &m, &n, a_copy.getData(), &lda,
		    S.data(), U.getData(), &ldu, VT.getData(), &ldvt,
		    &work_query, &lwork, &info);

	    // Perform SVD
	    lwork = static_cast<int>(work_query);
	    std::vector<double> work(lwork);

	    dgesvd_(&jobu, &jobvt, &m, &n, a_copy.getData(), &lda,
		    S.data(), U.getData(), &ldu, VT.getData(), &ldvt,
		    work.data(), &lwork, &info);

	    if (info < 0) {
		throw std::runtime_error("LAPACK dgesvd: illegal parameter at position " + std::to_string(-info));
	    } else if (info > 0) {
		throw std::runtime_error("SVD failed to converge");
	    }
        
            const auto& singular_values = S;
            if (singular_values.empty()) return -1.0;
            
            double max_sv = *std::max_element(singular_values.begin(), singular_values.end());
            double min_sv = *std::min_element(singular_values.begin(), singular_values.end());
            
            if (min_sv < 1e-15) return std::numeric_limits<double>::infinity();
            
            return max_sv / min_sv;
        } catch (...) {
            return -1.0;  // Error in computation
        }
#else
        // Basic estimation using determinant (not accurate but gives some indication)
        try {
            double det = matrix.determinant();
            return std::abs(det) < 1e-12 ? 1e12 : 1.0 / std::abs(det);
        } catch (...) {
            return -1.0;
        }
#endif
    });

    // Matrix comparison utilities
    lua->set_function("matrix_difference_norm", [](const AcceleratedMatrix& a, const AcceleratedMatrix& b) {
        if (a.getRows() != b.getRows() || a.getCols() != b.getCols()) {
            throw std::invalid_argument("Matrices must have same dimensions");
        }
        
        auto diff = a.subtract(b);
        return diff.norm();
    });
    
    // Advanced matrix generators
    lua->set_function("create_hilbert_matrix", [](size_t n) {
        AcceleratedMatrix hilbert(n, n);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                hilbert.set(i, j, 1.0 / (i + j + 1.0));
            }
        }
        return hilbert;
    });
    
    lua->set_function("create_vandermonde_matrix", [](const std::vector<double>& x_vals, size_t degree) {
        size_t n = x_vals.size();
        AcceleratedMatrix vander(n, degree + 1);
        
        for (size_t i = 0; i < n; ++i) {
            double x = x_vals[i];
            double x_power = 1.0;
            
            for (size_t j = 0; j <= degree; ++j) {
                vander.set(i, j, x_power);
                x_power *= x;
            }
        }
        
        return vander;
    });
    
    lua->set_function("create_toeplitz_matrix", [](const std::vector<double>& first_row, 
                                                   const std::vector<double>& first_col) {
        size_t n = first_row.size();
        if (first_col.size() != n) {
            throw std::invalid_argument("First row and column must have same length");
        }
        
        AcceleratedMatrix toeplitz(n, n);
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (j >= i) {
                    toeplitz.set(i, j, first_row[j - i]);
                } else {
                    toeplitz.set(i, j, first_col[i - j]);
                }
            }
        }
        
        return toeplitz;
    });
    
    // Specialized linear algebra functions
    lua->set_function("solve_least_squares", [](const AcceleratedMatrix& A, const std::vector<double>& b) {
#ifdef __APPLE__
        // Use QR decomposition for least squares: A = QR, then solve Rx = Q^T b
        try {
            auto [Q, R] = A.qrDecomposition();
            auto QT_b = Q.transpose().multiplyVector(b);
            
            // Back-substitution for upper triangular R
            size_t n = R.getCols();
            std::vector<double> x(n, 0.0);
            
            for (int i = n - 1; i >= 0; --i) {
                double sum = QT_b[i];
                for (size_t j = i + 1; j < n; ++j) {
                    sum -= R.get(i, j) * x[j];
                }
                x[i] = sum / R.get(i, i);
            }
            
            return x;
        } catch (...) {
            // Fallback to normal equations
            auto AT = A.transpose();
            auto ATA = AT.multiplyAccelerate(A);
            auto ATb = AT.multiplyVector(b);
            return ATA.solve(ATb);
        }
#else
        // Normal equations: (A^T A) x = A^T b
        auto AT = A.transpose();
        auto ATA = AT.multiply(A);
        auto ATb = AT.multiplyVector(b);
        return ATA.solve(ATb);
#endif
    });
    
    // ... rest of existing initializeSol2() code ...
    
    outputDisplay->append("Accelerated matrix processing ready!");
    outputDisplay->append("macOS Accelerate Framework: " + 
#ifdef __APPLE__
                         QString("Available")
#else
                         QString("Not Available")
#endif
    );

    // High-resolution timing functions
    lua->set_function("get_time_us", []() -> long long {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    });
    
    lua->set_function("get_time_ns", []() -> long long {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    });
    
    // Performance timing helper
    lua->set_function("time_operation", [this](const sol::function& operation, int iterations) -> sol::table {
        std::vector<double> times;
        times.reserve(iterations);
        
        // Warm up
        for (int i = 0; i < std::min(3, iterations); ++i) {
            operation();
        }
        
        // Measure
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            operation();
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            times.push_back(static_cast<double>(duration_us));
        }
        
        // Calculate statistics
        std::sort(times.begin(), times.end());
        
        double sum = std::accumulate(times.begin(), times.end(), 0.0);
        double mean = sum / times.size();
        double median = times[times.size() / 2];
        double min_time = times.front();
        double max_time = times.back();
        
        // Calculate standard deviation
        double variance = 0.0;
        for (double time : times) {
            variance += (time - mean) * (time - mean);
        }
        double std_dev = std::sqrt(variance / times.size());
        
        sol::table result = lua->create_table();
        result["mean_us"] = mean;
        result["median_us"] = median;
        result["min_us"] = min_time;
        result["max_us"] = max_time;
        result["std_dev_us"] = std_dev;
        result["iterations"] = iterations;
        
        return result;
    });
    
    // Accurate GFLOPS calculation
    lua->set_function("calculate_gflops_precise", [](double operations, double time_us) -> double {
        return operations / (time_us * 1e-6) / 1e9;  // GFLOPS
    });
    
    // Bind GenericDataTableWidget
    lua->new_usertype<GenericDataTableWidget>("DataTable",
        sol::constructors<GenericDataTableWidget(const QString&, QWidget*)>(),
        
        "displayData", [this](GenericDataTableWidget& table, const sol::object& data, const std::string& name) {
            table.setLuaState(lua);
            table.displayData(data, QString::fromStdString(name));
        },
        
        "show", &GenericDataTableWidget::show,
        "clearTable", &GenericDataTableWidget::clearTable,
        "refreshDisplay", &GenericDataTableWidget::refreshDisplay
    );
    
    // Factory function for data tables
    lua->set_function("create_data_table", [](const std::string& title) {
        GenericDataTableWidget* table = new GenericDataTableWidget(QString::fromStdString(title));
        return table;
    });
    
    // Convenience function to display any data
    lua->set_function("show_data", [this](const sol::object& data, const std::string& name) {
        GenericDataTableWidget* table = new GenericDataTableWidget(QString::fromStdString("Data Viewer: " + name));
        table->setLuaState(lua);
        table->displayData(data, QString::fromStdString(name));
        table->show();
        return table;
    });
    
    // Matrix display helper
    lua->set_function("show_matrix", [this](const LuaMatrix& matrix, const std::string& name) {
        GenericDataTableWidget* table = new GenericDataTableWidget(QString::fromStdString("Matrix: " + name));
        table->setLuaState(lua);
        
        // Convert matrix to Lua table format
        sol::table matrixTable = lua->create_table();
        
        for (size_t i = 0; i < matrix.getRows(); ++i) {
            sol::table row = lua->create_table();
            for (size_t j = 0; j < matrix.getCols(); ++j) {
                row[j + 1] = matrix.get(i, j);  // Lua 1-based indexing
            }
            matrixTable[i + 1] = row;
        }
        
        table->displayData(matrixTable, QString::fromStdString(name));
        table->show();
        return table;
    });
    
    // Accelerated matrix display helper
    lua->set_function("show_accelerated_matrix", [this](const AcceleratedMatrix& matrix, const std::string& name) {
        GenericDataTableWidget* table = new GenericDataTableWidget(QString::fromStdString("Accelerated Matrix: " + name));
        table->setLuaState(lua);
        
        // Convert matrix to Lua table format
        sol::table matrixTable = lua->create_table();
        
        for (size_t i = 0; i < matrix.getRows(); ++i) {
            sol::table row = lua->create_table();
            for (size_t j = 0; j < matrix.getCols(); ++j) {
                row[j + 1] = matrix.get(i, j);  // Lua 1-based indexing
            }
            matrixTable[i + 1] = row;
        }
        
        table->displayData(matrixTable, QString::fromStdString(name));
        table->show();
        return table;
    });
    
    // Vector display helper
    lua->set_function("show_vector", [this](const std::vector<double>& vec, const std::string& name) {
        GenericDataTableWidget* table = new GenericDataTableWidget(QString::fromStdString("Vector: " + name));
        table->setLuaState(lua);
        
        // Convert vector to Lua table
        sol::table vecTable = lua->create_table();
        for (size_t i = 0; i < vec.size(); ++i) {
            vecTable[i + 1] = vec[i];
        }
        
        table->displayData(vecTable, QString::fromStdString(name));
        table->show();
        return table;
    });
    
    // Performance data helper
    lua->set_function("show_performance_data", [this](const sol::table& perfData, const std::string& title) {
        GenericDataTableWidget* table = new GenericDataTableWidget(QString::fromStdString("Performance: " + title));
        table->setLuaState(lua);
        table->displayData(perfData, QString::fromStdString(title));
        table->show();
        return table;
    });

    setupImageDisplayBindings();
    setupStarChartBindings();
        
    // Make objects available to Lua
    (*lua)["window_factory"] = windowFactory;
    (*lua)["main_window"] = this;
    
    outputDisplay->append("Sol2 Lua integration initialized with script loading!");
    outputDisplay->append("Available objects: main_window, window_factory");
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
