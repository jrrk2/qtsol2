// LuaChartWidget.hpp - New widget for Qt Charts integration
#ifndef LUACHARTWIDGET_HPP
#define LUACHARTWIDGET_HPP

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <vector>
#include <cmath>

// QT_CHARTS_USE_NAMESPACE

class LuaChartWidget : public QWidget
{
    Q_OBJECT

private:
    QChart* chart;
    QChartView* chartView;
    QLineSeries* series;
    QValueAxis* axisX;
    QValueAxis* axisY;
    
    // Control widgets
    QSlider* frequencySlider;
    QSlider* amplitudeSlider;
    QSlider* phaseSlider;
    QLabel* frequencyLabel;
    QLabel* amplitudeLabel;
    QLabel* phaseLabel;
    QPushButton* updateButton;

public:
    explicit LuaChartWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setupUI();
        setupChart();
        updateSineWave();
    }

    // Lua-accessible methods
    void setFrequency(double freq) {
        frequencySlider->setValue(static_cast<int>(freq * 10));
        updateSineWave();
    }
    
    void setAmplitude(double amp) {
        amplitudeSlider->setValue(static_cast<int>(amp * 10));
        updateSineWave();
    }
    
    void setPhase(double phase) {
        phaseSlider->setValue(static_cast<int>(phase));
        updateSineWave();
    }
    
    double getFrequency() const {
        return frequencySlider->value() / 10.0;
    }
    
    double getAmplitude() const {
        return amplitudeSlider->value() / 10.0;
    }
    
    double getPhase() const {
        return phaseSlider->value();
    }
    
    void addPoint(double x, double y) {
        series->append(x, y);
    }
    
    void clearSeries() {
        series->clear();
    }
    
    void plotFunction(const std::string& function_type, double xMin, double xMax, int points) {
        series->clear();
        
        double step = (xMax - xMin) / (points - 1);
        
        for (int i = 0; i < points; ++i) {
            double x = xMin + i * step;
            double y = 0;
            
            if (function_type == "sine") {
                y = getAmplitude() * std::sin(getFrequency() * x + getPhase() * M_PI / 180);
            } else if (function_type == "cosine") {
                y = getAmplitude() * std::cos(getFrequency() * x + getPhase() * M_PI / 180);
            } else if (function_type == "tangent") {
                y = getAmplitude() * std::tan(getFrequency() * x + getPhase() * M_PI / 180);
                // Clamp tangent to prevent extreme values
                y = std::max(-10.0, std::min(10.0, y));
            } else if (function_type == "square") {
                y = getAmplitude() * (std::sin(getFrequency() * x + getPhase() * M_PI / 180) >= 0 ? 1 : -1);
            }
            
            series->append(x, y);
        }
        
        // Auto-scale axes
        axisX->setRange(xMin, xMax);
        axisY->setRange(-getAmplitude() * 1.2, getAmplitude() * 1.2);
    }
    
    void setTitle(const std::string& title) {
        chart->setTitle(QString::fromStdString(title));
    }
    
    void setAxisLabels(const std::string& xLabel, const std::string& yLabel) {
        axisX->setTitleText(QString::fromStdString(xLabel));
        axisY->setTitleText(QString::fromStdString(yLabel));
    }

private slots:
    void updateSineWave() {
        double freq = frequencySlider->value() / 10.0;
        double amp = amplitudeSlider->value() / 10.0;
        double phase = phaseSlider->value();
        
        frequencyLabel->setText(QString("Frequency: %1 Hz").arg(freq, 0, 'f', 1));
        amplitudeLabel->setText(QString("Amplitude: %1").arg(amp, 0, 'f', 1));
        phaseLabel->setText(QString("Phase: %1°").arg(phase));
        
        plotFunction("sine", -4 * M_PI, 4 * M_PI, 1000);
        
        chart->setTitle(QString("Sine Wave: A=%1, f=%2Hz, φ=%3°")
                       .arg(amp, 0, 'f', 1)
                       .arg(freq, 0, 'f', 1)
                       .arg(phase));
    }

private:
    void setupUI() {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        // Create chart view
        chartView = new QChartView(this);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumSize(600, 400);
        mainLayout->addWidget(chartView);
        
        // Control panel
        QWidget* controlPanel = new QWidget();
        QHBoxLayout* controlLayout = new QHBoxLayout(controlPanel);
        
        // Frequency control
        QVBoxLayout* freqLayout = new QVBoxLayout();
        frequencyLabel = new QLabel("Frequency: 1.0 Hz");
        frequencySlider = new QSlider(Qt::Horizontal);
        frequencySlider->setRange(1, 50);  // 0.1 to 5.0 Hz
        frequencySlider->setValue(10);     // 1.0 Hz
        freqLayout->addWidget(frequencyLabel);
        freqLayout->addWidget(frequencySlider);
        controlLayout->addLayout(freqLayout);
        
        // Amplitude control
        QVBoxLayout* ampLayout = new QVBoxLayout();
        amplitudeLabel = new QLabel("Amplitude: 1.0");
        amplitudeSlider = new QSlider(Qt::Horizontal);
        amplitudeSlider->setRange(1, 50);  // 0.1 to 5.0
        amplitudeSlider->setValue(10);     // 1.0
        ampLayout->addWidget(amplitudeLabel);
        ampLayout->addWidget(amplitudeSlider);
        controlLayout->addLayout(ampLayout);
        
        // Phase control
        QVBoxLayout* phaseLayout = new QVBoxLayout();
        phaseLabel = new QLabel("Phase: 0°");
        phaseSlider = new QSlider(Qt::Horizontal);
        phaseSlider->setRange(0, 360);
        phaseSlider->setValue(0);
        phaseLayout->addWidget(phaseLabel);
        phaseLayout->addWidget(phaseSlider);
        controlLayout->addLayout(phaseLayout);
        
        // Update button
        updateButton = new QPushButton("Update");
        controlLayout->addWidget(updateButton);
        
        mainLayout->addWidget(controlPanel);
        
        // Connect signals
        connect(frequencySlider, &QSlider::valueChanged, this, &LuaChartWidget::updateSineWave);
        connect(amplitudeSlider, &QSlider::valueChanged, this, &LuaChartWidget::updateSineWave);
        connect(phaseSlider, &QSlider::valueChanged, this, &LuaChartWidget::updateSineWave);
        connect(updateButton, &QPushButton::clicked, this, &LuaChartWidget::updateSineWave);
    }
    
    void setupChart() {
        chart = new QChart();
        chartView->setChart(chart);
        
        // Create series
        series = new QLineSeries();
        series->setName("Sine Wave");
        chart->addSeries(series);
        
        // Create axes
        axisX = new QValueAxis();
        axisX->setTitleText("Time (radians)");
        axisX->setRange(-4 * M_PI, 4 * M_PI);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
        axisY = new QValueAxis();
        axisY->setTitleText("Amplitude");
        axisY->setRange(-2, 2);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        
        // Styling
        chart->setTheme(QChart::ChartThemeBlueIcy);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
    }
};

#endif // LUACHARTWIDGET_HPP
