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
#include "sol2qtmainwindow.hpp"

class LuaWindow : public QWidget
{
    Q_OBJECT

private:
    QVBoxLayout* layout;
    std::vector<QWidget*> widgets;

public:
    explicit LuaWindow(const std::string& title, int width = 400, int height = 300, QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setWindowTitle(QString::fromStdString(title));
        resize(width, height);
        layout = new QVBoxLayout(this);
        setAttribute(Qt::WA_DeleteOnClose); // Auto-delete when closed
    }

    // Add various widget types
    void addLabel(const std::string& text) {
        QLabel* label = new QLabel(QString::fromStdString(text), this);
        layout->addWidget(label);
        widgets.push_back(label);
    }

    void addButton(const std::string& text) {
        QPushButton* button = new QPushButton(QString::fromStdString(text), this);
        layout->addWidget(button);
        widgets.push_back(button);
        
        // Connect to a generic signal for Lua callbacks
        connect(button, &QPushButton::clicked, this, [this, text]() {
            emit buttonClicked(QString::fromStdString(text));
        });
    }

    void addTextEdit(const std::string& placeholder = "") {
        QTextEdit* textEdit = new QTextEdit(this);
        if (!placeholder.empty()) {
            textEdit->setPlaceholderText(QString::fromStdString(placeholder));
        }
        layout->addWidget(textEdit);
        widgets.push_back(textEdit);
    }

    void addSlider(int min = 0, int max = 100, int value = 50) {
        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(min, max);
        slider->setValue(value);
        layout->addWidget(slider);
        widgets.push_back(slider);
        
        connect(slider, QOverload<int>::of(&QSlider::valueChanged), 
                this, [this](int val) {
            emit sliderChanged(val);
        });
    }

    void show() {
        QWidget::show();
        raise();
        activateWindow();
    }

    void setBackgroundColor(const std::string& color) {
        setStyleSheet(QString("background-color: %1;").arg(QString::fromStdString(color)));
    }

signals:
    void buttonClicked(const QString& buttonText);
    void sliderChanged(int value);
    void windowClosed();

protected:
    void closeEvent(QCloseEvent* event) override {
        emit windowClosed();
        QWidget::closeEvent(event);
    }
};

// Window factory class
class LuaWindowFactory : public QObject
{
    Q_OBJECT

private:
    std::vector<LuaWindow*> windows;
    Sol2QtMainWindow* mainWindow;

public:
    explicit LuaWindowFactory(Sol2QtMainWindow* parent = nullptr) 
        : QObject(parent), mainWindow(parent) {}

    LuaWindow* createWindow(const std::string& title, int width = 400, int height = 300) {
        LuaWindow* window = new LuaWindow(title, width, height);
        windows.push_back(window);
        
        // Connect signals to main window output
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
