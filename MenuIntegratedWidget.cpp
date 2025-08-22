// ===== CORRECTED MenuIntegratedWidget.cpp =====

#include "MenuIntegratedWidget.hpp"
#include "LuaWindowFactory.hpp"
#include "sol2qtmainwindow.hpp"

// Static member definition
int MenuIntegratedWidget::globalWindowCounter = 0;

MenuIntegratedWidget::MenuIntegratedWidget(QWidget* parent)
    : QWidget(parent)
    , windowFactory(nullptr)
    , isRegisteredWithMenu(false)
    , isUsingAutoTitle(true)
{
    // Always generate auto-title using virtual method
    QString autoTitle = generateAutoTitle();
    menuTitle = autoTitle;
    QWidget::setWindowTitle(autoTitle);  // Call base class method explicitly
    
    // Always auto-register since we always have a meaningful title
    autoRegisterWithGlobalFactory(autoTitle);
}

MenuIntegratedWidget::MenuIntegratedWidget(const QString& explicitTitle, QWidget* parent)
    : QWidget(parent)
    , windowFactory(nullptr)
    , isRegisteredWithMenu(false)
    , isUsingAutoTitle(false)
{
    menuTitle = explicitTitle;
    QWidget::setWindowTitle(explicitTitle);
    autoRegisterWithGlobalFactory(explicitTitle);
}

MenuIntegratedWidget::~MenuIntegratedWidget()
{
    unregisterFromMenu();
}

QString MenuIntegratedWidget::generateAutoTitle()
{
    return QString("Window %1").arg(++globalWindowCounter);
}

QString MenuIntegratedWidget::getNextAutoTitle() const
{
    return QString("Window %1").arg(globalWindowCounter + 1);
}

void MenuIntegratedWidget::autoRegisterWithGlobalFactory(const QString& title)
{
    if (auto* globalFactory = Sol2QtMainWindow::getGlobalWindowFactory()) {
        windowFactory = globalFactory;
        registerWithMenu(title);
    }
}

void MenuIntegratedWidget::registerWithMenu(const QString& title)
{
    if (!windowFactory) {
        windowFactory = Sol2QtMainWindow::getGlobalWindowFactory();
    }
    
    if (windowFactory && !isRegisteredWithMenu) {
        QString finalTitle = title.isEmpty() ? menuTitle : title;
        windowFactory->registerWidget(this, finalTitle);
        isRegisteredWithMenu = true;
    }
}

void MenuIntegratedWidget::unregisterFromMenu()
{
    if (windowFactory && isRegisteredWithMenu) {
        windowFactory->unregisterWidget(this);
        isRegisteredWithMenu = false;
    }
}

void MenuIntegratedWidget::setWindowTitle(const QString& title)
{
    QWidget::setWindowTitle(title);  // Call base class method
    
    // If user sets a custom title, we're no longer using auto-title
    if (isUsingAutoTitle && title != menuTitle) {
        isUsingAutoTitle = false;
    }
    
    menuTitle = title;
    
    if (isRegisteredWithMenu) {
        emit titleChanged(title);
        if (windowFactory) {
            windowFactory->updateWidgetTitle(this, title);
        }
    }
}

void MenuIntegratedWidget::setCustomTitle(const QString& title)
{
    isUsingAutoTitle = false;
    setWindowTitle(title);
}

void MenuIntegratedWidget::resetToAutoTitle()
{
    QString newAutoTitle = generateAutoTitle();
    isUsingAutoTitle = true;
    setWindowTitle(newAutoTitle);
}

void MenuIntegratedWidget::show()
{
    QWidget::show();  // Call base class method
    emit visibilityChanged(true);
}

void MenuIntegratedWidget::hide()
{
    QWidget::hide();  // Call base class method
    emit visibilityChanged(false);
}

void MenuIntegratedWidget::setVisible(bool visible)
{
    QWidget::setVisible(visible);  // Call base class method
    emit visibilityChanged(visible);
}

bool MenuIntegratedWidget::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::WindowStateChange: {
        QWindowStateChangeEvent* stateEvent = static_cast<QWindowStateChangeEvent*>(event);
        Qt::WindowStates oldState = stateEvent->oldState();
        Qt::WindowStates newState = windowState();
        
        if ((newState & Qt::WindowMinimized) != (oldState & Qt::WindowMinimized)) {
            emit visibilityChanged(!(newState & Qt::WindowMinimized) && isVisible());
        }
        break;
    }
    default:
        break;
    }
    
    return QWidget::event(event);
}

// ===== MISSING: LuaWindowFactory updateWidgetTitle METHOD =====
// You'll also need to add this method to your LuaWindowFactory class:

/*
// Add this to LuaWindowFactory.hpp:
void updateWidgetTitle(QWidget* widget, const QString& newTitle);

// Add this to LuaWindowFactory.cpp:
void LuaWindowFactory::updateWidgetTitle(QWidget* widget, const QString& newTitle)
{
    auto actionIt = widgetActions.find(widget);
    if (actionIt != widgetActions.end()) {
        QString actionText = QString("Chart: %1").arg(newTitle);
        actionIt->second->setText(actionText);
    }
}
*/
