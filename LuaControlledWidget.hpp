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

// LuaControlledWidget class definition
class LuaControlledWidget : public QObject
{
    Q_OBJECT

private:
    QLabel* statusLabel;
    QProgressBar* progressBar;
    QSlider* slider;
    QTimer* timer;
    int counter;

public:
    LuaControlledWidget(QWidget* parent = nullptr) 
        : QObject(parent), counter(0)
    {
        statusLabel = new QLabel("Ready", qobject_cast<QWidget*>(parent));
        progressBar = new QProgressBar(qobject_cast<QWidget*>(parent));
        slider = new QSlider(Qt::Horizontal, qobject_cast<QWidget*>(parent));
        
        slider->setRange(0, 100);
        slider->setValue(50);
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &LuaControlledWidget::onTimerTick);
    }

    QLabel* getStatusLabel() const { return statusLabel; }
    QProgressBar* getProgressBar() const { return progressBar; }
    QSlider* getSlider() const { return slider; }

public slots:
    void setStatus(const QString& text) {
        statusLabel->setText(text);
        emit statusChanged(text);
    }
    
    void setProgress(int value) {
        progressBar->setValue(qBound(0, value, 100));
        emit progressChanged(value);
    }
    
    void startAnimation() {
        counter = 0;
        timer->start(100);
        emit animationStarted();
    }
    
    void stopAnimation() {
        timer->stop();
        emit animationStopped();
    }
    
    int getSliderValue() const {
        return slider->value();
    }
    
    void animateProgress(int target) {
        int current = progressBar->value();
        if (current < target) {
            setProgress(current + 1);
            QTimer::singleShot(50, this, [this, target]() {
                if (progressBar->value() < target) {
                    animateProgress(target);
                }
            });
        }
    }

    // Sol2-friendly methods
    std::string getStatusText() const {
        return statusLabel->text().toStdString();
    }
    
    void setStatusString(const std::string& text) {
        setStatus(QString::fromStdString(text));
    }

signals:
    void statusChanged(const QString& text);
    void progressChanged(int value);
    void animationStarted();
    void animationStopped();

private slots:
    void onTimerTick() {
        counter = (counter + 5) % 101;
        setProgress(counter);
    }
};
