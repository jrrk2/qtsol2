// Enhanced LuaWindowFactory.hpp with Lua event callback support

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

public:
    explicit LuaWindow(const std::string& title, int width = 400, int height = 300, 
                      sol::state* lua = nullptr, QWidget* parent = nullptr)
        : QWidget(parent), luaState(lua)
    {
        setWindowTitle(QString::fromStdString(title));
        resize(width, height);
        layout = new QVBoxLayout(this);
        setAttribute(Qt::WA_DeleteOnClose);
    }

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
};

// Enhanced Window factory class
class LuaWindowFactory : public QObject
{
    Q_OBJECT

private:
    std::vector<LuaWindow*> windows;
    Sol2QtMainWindow* mainWindow;
    sol::state* luaState;

public:
    explicit LuaWindowFactory(Sol2QtMainWindow* parent = nullptr, sol::state* lua = nullptr) 
        : QObject(parent), mainWindow(parent), luaState(lua) {}

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
        
        connect(window, &LuaWindow::windowClosed, this, [this, window]() {
            // Remove from vector when closed
            auto it = std::find(windows.begin(), windows.end(), window);
            if (it != windows.end()) {
                windows.erase(it);
            }
            if (mainWindow) {
                mainWindow->appendToOutput("Window closed");
            }
        });
        
        return window;
    }

    int getWindowCount() const {
        return static_cast<int>(windows.size());
    }

    void closeAllWindows() {
        for (auto* window : windows) {
            if (window) {
                window->close();
            }
        }
        windows.clear();
    }
};
