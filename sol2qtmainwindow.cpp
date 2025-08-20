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

void Sol2QtMainWindow::setupUI() 
{
    setWindowTitle("Sol2 + Qt6 Lua Integration");
    setMinimumSize(1000, 700);
    
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
    
    // Button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    runButton = new QPushButton("Run Sol2 Script");
    runButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    
    loadExampleButton = new QPushButton("Load Advanced Example");
    loadExampleButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; }");
    
    clearButton = new QPushButton("Clear Output");
    clearButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");
    
    buttonLayout->addWidget(runButton);
    buttonLayout->addWidget(loadExampleButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();
    
    editorLayout->addLayout(buttonLayout);
    splitter->addWidget(editorWidget);
    
    // Output section
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
    consoleInput->setPlaceholderText("Enter Lua command (try: math.sqrt(16), widget:getSliderValue(), x = 42)");
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

void Sol2QtMainWindow::initializeSol2() 
{
    // Create Sol2 state
    lua = new sol::state();
    
    // Open standard Lua libraries
    lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table);
    
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
        "appendToOutput", &Sol2QtMainWindow::appendToOutput
    );
    
    // Make objects available to Lua
    (*lua)["app_widget"] = controlWidget;

    // Create window factory
    windowFactory = new LuaWindowFactory(this);

    // Bind LuaWindow class
    lua->new_usertype<LuaWindow>("LuaWindow",
        "addLabel", &LuaWindow::addLabel,
        "addButton", &LuaWindow::addButton,
        "addTextEdit", &LuaWindow::addTextEdit,
        "addSlider", &LuaWindow::addSlider,
        "show", &LuaWindow::show,
        "setBackgroundColor", &LuaWindow::setBackgroundColor,
        "close", &QWidget::close
    );

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

    // Make factory available to Lua
    (*lua)["window_factory"] = windowFactory;
    (*lua)["main_window"] = this;
    
    outputDisplay->append("Sol2 Lua integration initialized!");
    outputDisplay->append("Available objects: app_widget, main_window");
    outputDisplay->append("Sol2 provides type-safe C++/Lua binding");
    outputDisplay->append("");
    outputDisplay->append("=== Interactive Console Ready ===");
    outputDisplay->append("Try these commands:");
    outputDisplay->append("  math.sqrt(16)");
    outputDisplay->append("  x = 42");
    outputDisplay->append("  x * 2");
    outputDisplay->append("  app_widget:getSliderValue()");
    outputDisplay->append("  app_widget:setProgress(75)");
    outputDisplay->append("Use ↑/↓ for command history\n");

}
