// Enhanced LuaWindowFactory.hpp with proper window cleanup

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
#include <QCloseEvent>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>

#include <sol/sol.hpp>
#include "sol2qtmainwindow.hpp"

class LuaWindow : public QWidget
{
    Q_OBJECT

private:
    QVBoxLayout* layout;
    std::vector<QWidget*> widgets;
    sol::state* luaState;
    
    // Lua callback functions
    sol::safe_function onButtonClickCallback;
    sol::safe_function onSliderChangeCallback;
    sol::safe_function onWindowCloseCallback;
    sol::safe_function onWindowShowCallback;
    sol::safe_function onTextChangeCallback;
    
    // Widget tracking for callbacks
    std::map<QWidget*, std::string> widgetNames;
    std::map<std::string, QWidget*> namedWidgets;
    static int windowCounter; // Static counter for unique IDs
    int windowId;
    std::string windowTitle;

public:
    explicit LuaWindow(const std::string& title, int width = 400, int height = 300, 
                      sol::state* lua = nullptr, QWidget* parent = nullptr)
        : QWidget(parent), luaState(lua)
    {
        windowId = ++windowCounter;
        windowTitle = title;  // Store the title
        setWindowTitle(QString::fromStdString(title));
        resize(width, height);
        layout = new QVBoxLayout(this);
        setAttribute(Qt::WA_DeleteOnClose);
    }

    // Add getters for menu system
    int getWindowId() const { return windowId; }
    std::string getWindowTitle() const { return windowTitle; }
    bool isVisible() const { return QWidget::isVisible(); }

    // Enhanced widget creation with optional names for callbacks
    void addLabel(const std::string& text, const std::string& name = "") {
        QLabel* label = new QLabel(QString::fromStdString(text), this);
        layout->addWidget(label);
        widgets.push_back(label);
        
        if (!name.empty()) {
            widgetNames[label] = name;
            namedWidgets[name] = label;
        }
    }

    void addButton(const std::string& text, const std::string& name = "") {
        QPushButton* button = new QPushButton(QString::fromStdString(text), this);
        layout->addWidget(button);
        widgets.push_back(button);
        
        std::string buttonName = name.empty() ? text : name;
        widgetNames[button] = buttonName;
        namedWidgets[buttonName] = button;
        
        connect(button, &QPushButton::clicked, this, [this, buttonName]() {
            handleButtonClick(buttonName);
        });
    }

    void addTextEdit(const std::string& placeholder = "", const std::string& name = "") {
        QTextEdit* textEdit = new QTextEdit(this);
        if (!placeholder.empty()) {
            textEdit->setPlaceholderText(QString::fromStdString(placeholder));
        }
        layout->addWidget(textEdit);
        widgets.push_back(textEdit);
        
        if (!name.empty()) {
            widgetNames[textEdit] = name;
            namedWidgets[name] = textEdit;
            
            // Connect text change signal
            connect(textEdit, &QTextEdit::textChanged, this, [this, name]() {
                handleTextChange(name);
            });
        }
    }

    void addSlider(int min = 0, int max = 100, int value = 50, const std::string& name = "") {
        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(min, max);
        slider->setValue(value);
        layout->addWidget(slider);
        widgets.push_back(slider);
        
        if (!name.empty()) {
            widgetNames[slider] = name;
            namedWidgets[name] = slider;
        }
        
        connect(slider, QOverload<int>::of(&QSlider::valueChanged), 
                this, [this, name](int val) {
            handleSliderChange(name.empty() ? "slider" : name, val);
        });
    }

    void addLineEdit(const std::string& placeholder = "", const std::string& name = "") {
        QLineEdit* lineEdit = new QLineEdit(this);
        if (!placeholder.empty()) {
            lineEdit->setPlaceholderText(QString::fromStdString(placeholder));
        }
        layout->addWidget(lineEdit);
        widgets.push_back(lineEdit);
        
        if (!name.empty()) {
            widgetNames[lineEdit] = name;
            namedWidgets[name] = lineEdit;
            
            connect(lineEdit, &QLineEdit::textChanged, this, [this, name](const QString& text) {
                handleLineEditChange(name, text.toStdString());
            });
        }
    }

    // Event callback setters
    void setOnButtonClick(const sol::safe_function& callback) {
        onButtonClickCallback = callback;
    }
    
    void setOnSliderChange(const sol::safe_function& callback) {
        onSliderChangeCallback = callback;
    }
    
    void setOnWindowClose(const sol::safe_function& callback) {
        onWindowCloseCallback = callback;
    }
    
    void setOnWindowShow(const sol::safe_function& callback) {
        onWindowShowCallback = callback;
    }
    
    void setOnTextChange(const sol::safe_function& callback) {
        onTextChangeCallback = callback;
    }

    // Widget value getters/setters for Lua
    std::string getWidgetText(const std::string& name) {
        auto it = namedWidgets.find(name);
        if (it != namedWidgets.end()) {
            if (auto* lineEdit = qobject_cast<QLineEdit*>(it->second)) {
                return lineEdit->text().toStdString();
            } else if (auto* textEdit = qobject_cast<QTextEdit*>(it->second)) {
                return textEdit->toPlainText().toStdString();
            } else if (auto* label = qobject_cast<QLabel*>(it->second)) {
                return label->text().toStdString();
            }
        }
        return "";
    }
    
    void setWidgetText(const std::string& name, const std::string& text) {
        auto it = namedWidgets.find(name);
        if (it != namedWidgets.end()) {
            if (auto* lineEdit = qobject_cast<QLineEdit*>(it->second)) {
                lineEdit->setText(QString::fromStdString(text));
            } else if (auto* textEdit = qobject_cast<QTextEdit*>(it->second)) {
                textEdit->setPlainText(QString::fromStdString(text));
            } else if (auto* label = qobject_cast<QLabel*>(it->second)) {
                label->setText(QString::fromStdString(text));
            }
        }
    }
    
    int getSliderValue(const std::string& name) {
        auto it = namedWidgets.find(name);
        if (it != namedWidgets.end()) {
            if (auto* slider = qobject_cast<QSlider*>(it->second)) {
                return slider->value();
            }
        }
        return 0;
    }
    
    void setSliderValue(const std::string& name, int value) {
        auto it = namedWidgets.find(name);
        if (it != namedWidgets.end()) {
            if (auto* slider = qobject_cast<QSlider*>(it->second)) {
                slider->setValue(value);
            }
        }
    }

    void show() {
        QWidget::show();
        raise();
        activateWindow();
        
        // Call Lua callback if set
        if (onWindowShowCallback.valid()) {
            try {
                onWindowShowCallback();
            } catch (const sol::error& e) {
                qDebug() << "Error in window show callback:" << e.what();
            }
        }
    }

    void setBackgroundColor(const std::string& color) {
        setStyleSheet(QString("background-color: %1;").arg(QString::fromStdString(color)));
    }

private:
    void handleButtonClick(const std::string& buttonName) {
        if (onButtonClickCallback.valid()) {
            try {
                onButtonClickCallback(buttonName);
            } catch (const sol::error& e) {
                qDebug() << "Error in button click callback:" << e.what();
            }
        }
        
        // Still emit the signal for backwards compatibility
        emit buttonClicked(QString::fromStdString(buttonName));
    }
    
    void handleSliderChange(const std::string& sliderName, int value) {
        if (onSliderChangeCallback.valid()) {
            try {
                onSliderChangeCallback(sliderName, value);
            } catch (const sol::error& e) {
                qDebug() << "Error in slider change callback:" << e.what();
            }
        }
        
        emit sliderChanged(value);
    }
    
    void handleTextChange(const std::string& widgetName) {
        if (onTextChangeCallback.valid()) {
            try {
                std::string currentText = getWidgetText(widgetName);
                onTextChangeCallback(widgetName, currentText);
            } catch (const sol::error& e) {
                qDebug() << "Error in text change callback:" << e.what();
            }
        }
    }
    
    void handleLineEditChange(const std::string& widgetName, const std::string& text) {
        if (onTextChangeCallback.valid()) {
            try {
                onTextChangeCallback(widgetName, text);
            } catch (const sol::error& e) {
                qDebug() << "Error in line edit change callback:" << e.what();
            }
        }
    }

protected:
    void closeEvent(QCloseEvent* event) override {
        // Call Lua callback before closing
        if (onWindowCloseCallback.valid()) {
            try {
                // Allow Lua to potentially prevent closing by returning false
                auto result = onWindowCloseCallback();
                if (result.valid() && result.get_type() == sol::type::boolean) {
                    bool shouldClose = result.get<bool>();
                    if (!shouldClose) {
                        event->ignore();
                        return;
                    }
                }
            } catch (const sol::error& e) {
                qDebug() << "Error in window close callback:" << e.what();
            }
        }
        
        emit windowClosed();
        QWidget::closeEvent(event);
    }

signals:
    void buttonClicked(const QString& buttonText);
    void sliderChanged(int value);
    void windowClosed();
    void windowShown();
    void windowHidden();

protected:
    void showEvent(QShowEvent* event) override {
        QWidget::showEvent(event);
        emit windowShown();
    }
    
    void hideEvent(QHideEvent* event) override {
        QWidget::hideEvent(event);
        emit windowHidden();
    }
};

// Enhanced Window factory class with FIXED cleanup
class LuaWindowFactory : public QObject
{
    Q_OBJECT

private:
    std::vector<LuaWindow*> windows;
    Sol2QtMainWindow* mainWindow;
    sol::state* luaState;
    
    // Menu system
    QMenu* windowMenu;
    QActionGroup* windowActionGroup;
    QAction* noWindowsAction;
    std::map<LuaWindow*, QAction*> windowActions;

public:
    explicit LuaWindowFactory(Sol2QtMainWindow* parent = nullptr, sol::state* lua = nullptr) 
        : QObject(parent), mainWindow(parent), luaState(lua) {}

    void setupWindowMenu() {
        if (!mainWindow) return;
        
        // Create Window menu in main window's menu bar
        QMenuBar* menuBar = mainWindow->menuBar();
        windowMenu = menuBar->addMenu("&Windows");
        
        // Create action group for window management
        windowActionGroup = new QActionGroup(this);
        
        // Add "No windows open" placeholder
        noWindowsAction = windowMenu->addAction("No windows open");
        noWindowsAction->setEnabled(false);
        
        windowMenu->addSeparator();
        
        // Add window management actions
        QAction* closeAllAction = windowMenu->addAction("Close &All Windows");
        closeAllAction->setShortcut(QKeySequence("Ctrl+Shift+W"));
        connect(closeAllAction, &QAction::triggered, this, &LuaWindowFactory::closeAllWindows);
        
        QAction* showAllAction = windowMenu->addAction("&Show All Windows");
        connect(showAllAction, &QAction::triggered, this, &LuaWindowFactory::showAllWindows);
        
        QAction* hideAllAction = windowMenu->addAction("&Hide All Windows");
        connect(hideAllAction, &QAction::triggered, this, &LuaWindowFactory::hideAllWindows);
        
        windowMenu->addSeparator();
        
        // Add window count display
        updateWindowMenu();
    }

    LuaWindow* createWindow(const std::string& title, int width = 400, int height = 300) {
        LuaWindow* window = new LuaWindow(title, width, height, luaState);
        windows.push_back(window);
        
        // Connect default signals to main window output (for backwards compatibility)
        connect(window, &LuaWindow::buttonClicked, this, [this](const QString& text) {
            if (mainWindow) {
                mainWindow->appendToOutput("Button clicked: " + text.toStdString());
            }
        });
        
        connect(window, &LuaWindow::sliderChanged, this, [this](int value) {
            if (mainWindow) {
                mainWindow->appendToOutput("Slider changed: " + std::to_string(value));
            }
        });
        
        // FIXED: Properly clean up window from both vector AND menu when closed
        connect(window, &LuaWindow::windowClosed, this, [this, window]() {
            this->removeWindow(window);  // Use helper method for complete cleanup
            if (mainWindow) {
                mainWindow->appendToOutput("Window closed: " + window->getWindowTitle());
            }
        });
        
        connect(window, &LuaWindow::windowShown, this, [this, window]() {
            updateWindowActionState(window);
        });
        
        connect(window, &LuaWindow::windowHidden, this, [this, window]() {
            updateWindowActionState(window);
        });
        
        // Add to menu
        addWindowToMenu(window);
        updateWindowMenu();
        
        return window;
    }

private:
    // NEW: Helper method for complete window removal
    void removeWindow(LuaWindow* window) {
        // Remove from vector
        auto it = std::find(windows.begin(), windows.end(), window);
        if (it != windows.end()) {
            windows.erase(it);
        }
        
        // Remove from menu
        removeWindowFromMenu(window);
        
        // Update menu display
        updateWindowMenu();
    }

public:
    void addWindowToMenu(LuaWindow* window) {
        if (!windowMenu) return;
        
        QString actionText = QString("Window %1: %2")
            .arg(window->getWindowId())
            .arg(QString::fromStdString(window->getWindowTitle()));
            
        QAction* windowAction = new QAction(actionText, this);
        windowAction->setCheckable(true);
        windowAction->setChecked(window->isVisible());
        
        // Add keyboard shortcut for first 9 windows
        if (windowActions.size() < 9) {
            windowAction->setShortcut(QKeySequence(QString("Ctrl+%1").arg(windowActions.size() + 1)));
        }
        
        connect(windowAction, &QAction::triggered, this, [this, window](bool checked) {
            if (checked) {
                // Always show, raise, and activate when selected from menu
                window->show();
                window->raise();
                window->activateWindow();
                window->setWindowState(window->windowState() & ~Qt::WindowMinimized);
            } else if (!checked && window->isVisible()) {
                window->hide();
            }
        });
        
        windowActions[window] = windowAction;
        windowActionGroup->addAction(windowAction);
        windowMenu->addAction(windowAction);
    }
    
    void removeWindowFromMenu(LuaWindow* window) {
        auto it = windowActions.find(window);
        if (it != windowActions.end()) {
            windowMenu->removeAction(it->second);
            windowActionGroup->removeAction(it->second);
            it->second->deleteLater();
            windowActions.erase(it);
            
            // Reassign keyboard shortcuts for remaining windows
            updateKeyboardShortcuts();
        }
    }
    
    // NEW: Update keyboard shortcuts when windows are removed
    void updateKeyboardShortcuts() {
        int shortcutIndex = 1;
        for (auto& pair : windowActions) {
            if (shortcutIndex <= 9) {
                pair.second->setShortcut(QKeySequence(QString("Ctrl+%1").arg(shortcutIndex)));
                shortcutIndex++;
            } else {
                pair.second->setShortcut(QKeySequence());  // Remove shortcut for windows beyond 9
            }
        }
    }

    void updateWindowActionState(LuaWindow* window) {
        auto it = windowActions.find(window);
        if (it != windowActions.end()) {
            it->second->setChecked(window->isVisible());
        }
    }
    
    void updateWindowMenu() {
        if (!windowMenu) return;
        
        // Update "no windows" placeholder
        bool hasWindows = !windows.empty();
        noWindowsAction->setVisible(!hasWindows);
        
        // Update menu title with count
        if (hasWindows) {
            windowMenu->setTitle(QString("&Windows (%1)").arg(windows.size()));
        } else {
            windowMenu->setTitle("&Windows");
        }
    }

    int getWindowCount() const {
        return static_cast<int>(windows.size());
    }

    void closeAllWindows() {
        // Create a copy of the vector to avoid iterator invalidation
        auto windowsCopy = windows;
        for (auto* window : windowsCopy) {
            if (window) {
                window->close();  // This will trigger the cleanup automatically
            }
        }
        // Note: Individual windows will be removed by their close signals
        // so we don't need to clear the vector manually here
    }
    
    void showAllWindows() {
        for (auto* window : windows) {
            if (window) {
                window->show();
                updateWindowActionState(window);
            }
        }
    }
    
    void hideAllWindows() {
        for (auto* window : windows) {
            if (window) {
                window->hide();
                updateWindowActionState(window);
            }
        }
    }
    
    std::vector<std::string> getWindowTitles() const {
        std::vector<std::string> titles;
        for (const auto* window : windows) {
            if (window) {
                titles.push_back(window->getWindowTitle());
            }
        }
        return titles;
    }
    
    // Find window by title or ID
    LuaWindow* findWindowByTitle(const std::string& title) {
        for (auto* window : windows) {
            if (window && window->getWindowTitle() == title) {
                return window;
            }
        }
        return nullptr;
    }
    
    LuaWindow* findWindowById(int id) {
        for (auto* window : windows) {
            if (window && window->getWindowId() == id) {
                return window;
            }
        }
        return nullptr;
    }
};
