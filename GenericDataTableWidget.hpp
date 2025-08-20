// GenericDataTableWidget.hpp - Corrected version

#ifndef GENERICDATATABLEWIDGET_HPP
#define GENERICDATATABLEWIDGET_HPP

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSplitter>
#include <sol/sol.hpp>

class GenericDataTableWidget : public QWidget
{
    Q_OBJECT

private:
    QTableWidget* table;
    QLabel* titleLabel;
    QLabel* infoLabel;
    QPushButton* copyButton;
    QPushButton* saveButton;
    QPushButton* clearButton;
    QPushButton* refreshButton;
    QComboBox* formatCombo;
    QSpinBox* precisionSpin;
    QCheckBox* showIndicesCheck;
    QLineEdit* filterEdit;
    QStringList customHeaders;
    QStringList customRowLabels;
    QString matrixTitle;
    QString matrixDescription;
        
    sol::state* luaState;
    sol::object currentData;

public:
    explicit GenericDataTableWidget(const QString& title = "Data Table", QWidget* parent = nullptr)
        : QWidget(parent), luaState(nullptr)
    {
        setWindowTitle(title);
        setupUI(title);
        setAttribute(Qt::WA_DeleteOnClose);
    }

    void setupUI(const QString& title)
    {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        // Title and info
        titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px; color: #2c3e50;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        infoLabel = new QLabel("No data loaded");
        infoLabel->setStyleSheet("color: #7f8c8d; margin: 5px;");
        infoLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(infoLabel);
        
        // Control panel
        QWidget* controlPanel = new QWidget();
        QHBoxLayout* controlLayout = new QHBoxLayout(controlPanel);
        
        // Format options
        controlLayout->addWidget(new QLabel("Format:"));
        formatCombo = new QComboBox();
        formatCombo->addItems({"Auto", "Scientific", "Fixed", "General", "Hex"});
        connect(formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &GenericDataTableWidget::refreshDisplay);
        controlLayout->addWidget(formatCombo);
        
        // Precision
        controlLayout->addWidget(new QLabel("Precision:"));
        precisionSpin = new QSpinBox();
        precisionSpin->setRange(0, 15);
        precisionSpin->setValue(6);
        connect(precisionSpin, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &GenericDataTableWidget::refreshDisplay);
        controlLayout->addWidget(precisionSpin);
        
        // Show indices
        showIndicesCheck = new QCheckBox("Show Indices");
        showIndicesCheck->setChecked(true);
        connect(showIndicesCheck, &QCheckBox::toggled, this, &GenericDataTableWidget::refreshDisplay);
        controlLayout->addWidget(showIndicesCheck);
        
        // Filter
        controlLayout->addWidget(new QLabel("Filter:"));
        filterEdit = new QLineEdit();
        filterEdit->setPlaceholderText("Search...");
        connect(filterEdit, &QLineEdit::textChanged, this, &GenericDataTableWidget::applyFilter);
        controlLayout->addWidget(filterEdit);
        
        controlLayout->addStretch();
        mainLayout->addWidget(controlPanel);
        
        // Table
        table = new QTableWidget(this);
        table->setAlternatingRowColors(true);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSortingEnabled(true);
        table->setStyleSheet(
            "QTableWidget {"
            "    gridline-color: #bdc3c7;"
            "    background-color: #ffffff;"
            "    selection-background-color: #3498db;"
            "}"
            "QTableWidget::item {"
            "    padding: 6px;"
            "    border-bottom: 1px solid #ecf0f1;"
            "}"
            "QHeaderView::section {"
            "    background-color: #34495e;"
            "    color: white;"
            "    padding: 8px;"
            "    border: none;"
            "    font-weight: bold;"
            "}"
        );
        
        mainLayout->addWidget(table, 1);
        
        // Action buttons
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        
        refreshButton = new QPushButton("Refresh");
        refreshButton->setStyleSheet("QPushButton { background-color: #9b59b6; color: white; padding: 8px 16px; border: none; border-radius: 4px; }");
        connect(refreshButton, &QPushButton::clicked, this, &GenericDataTableWidget::refreshDisplay);
        
        copyButton = new QPushButton("Copy to Clipboard");
        copyButton->setStyleSheet("QPushButton { background-color: #3498db; color: white; padding: 8px 16px; border: none; border-radius: 4px; }");
        connect(copyButton, &QPushButton::clicked, this, &GenericDataTableWidget::copyToClipboard);
        
        saveButton = new QPushButton("Save as CSV");
        saveButton->setStyleSheet("QPushButton { background-color: #27ae60; color: white; padding: 8px 16px; border: none; border-radius: 4px; }");
        connect(saveButton, &QPushButton::clicked, this, &GenericDataTableWidget::saveAsCSV);
        
        clearButton = new QPushButton("Clear");
        clearButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; padding: 8px 16px; border: none; border-radius: 4px; }");
        connect(clearButton, &QPushButton::clicked, this, &GenericDataTableWidget::clearTable);
        
        buttonLayout->addWidget(refreshButton);
        buttonLayout->addWidget(copyButton);
        buttonLayout->addWidget(saveButton);
        buttonLayout->addWidget(clearButton);
        buttonLayout->addStretch();
        
        mainLayout->addLayout(buttonLayout);
        
        resize(900, 700);
    }

    void setLuaState(sol::state* lua) {
        luaState = lua;
    }

    // Display any Lua data structure
    void displayData(const sol::object& data, const QString& dataName = "Data")
    {
        currentData = data;
        
        if (!data.valid()) {
            setInfo("Invalid data");
            return;
        }
        
        sol::type dataType = data.get_type();
        
        switch (dataType) {
        case sol::type::table:
            displayTable(data.as<sol::table>(), dataName);
            break;
        case sol::type::number:
            displayNumberScalar(data.as<double>(), dataName);
            break;
        case sol::type::string:
            displayStringScalar(data.as<std::string>(), dataName);
            break;
        case sol::type::boolean:
            displayBooleanScalar(data.as<bool>(), dataName);
            break;
        case sol::type::userdata:
            displayUserData(data, dataName);
            break;
        default:
            setInfo("Unsupported data type: " + QString::fromStdString(sol::type_name(luaState->lua_state(), dataType)));
            break;
        }
    }

    void setCustomHeaders(const QStringList& headers) {
        customHeaders = headers;
        if (table) {
            table->setHorizontalHeaderLabels(headers);
        }
    }
    
    void setCustomRowLabels(const QStringList& rowLabels) {
        customRowLabels = rowLabels;
        if (table && table->rowCount() == rowLabels.size()) {
            table->setVerticalHeaderLabels(rowLabels);
        }
    }
    
    void setMatrixMetadata(const QString& title, const QString& description) {
        matrixTitle = title;
        matrixDescription = description;
        updateInfoDisplay();
    }
  
private:
    void updateInfoDisplay() {
        QString info = QString("Matrix: %1").arg(matrixTitle);
        if (!matrixDescription.isEmpty()) {
            info += QString(" - %1").arg(matrixDescription);
        }
        setInfo(info);
    }
  
    void displayTable(const sol::table& luaTable, const QString& tableName)
    {
        // Analyze table structure
        bool isArray = true;
        bool isMatrix = true;
        size_t maxIndex = 0;
        size_t rowCount = 0;
        size_t colCount = 0;
        
        // First pass: determine structure
        for (const auto& pair : luaTable) {
            sol::object key = pair.first;
            sol::object value = pair.second;
            
            if (key.get_type() == sol::type::number) {
                size_t index = key.as<size_t>();
                maxIndex = std::max(maxIndex, index);
                
                if (value.get_type() == sol::type::table) {
                    // Potential matrix row
                    rowCount++;
                    sol::table row = value.as<sol::table>();
                    size_t currentRowSize = 0;
                    for (const auto& elem : row) {
                        currentRowSize++;
                    }
                    if (colCount == 0) colCount = currentRowSize;
                    else if (colCount != currentRowSize) isMatrix = false;
                } else {
                    isMatrix = false;
                }
            } else {
                isArray = false;
                isMatrix = false;
            }
        }
        
        if (isMatrix && rowCount > 0 && colCount > 0) {
            displayMatrixTable(luaTable, tableName, rowCount, colCount);
        } else if (isArray) {
            displayArrayTable(luaTable, tableName);
        } else {
            displayGenericTable(luaTable, tableName);
        }
    }
  
    void displayMatrixTable(const sol::table& luaTable, const QString& matrixName, size_t rows, size_t cols)
    {
	setInfo(QString("Matrix: %1 (%2×%3)").arg(matrixName).arg(rows).arg(cols));

	// Set up headers - USE CUSTOM HEADERS IF AVAILABLE
	QStringList headers;

	// Only add "Row" column if we DON'T have custom row labels AND user wants indices shown
	bool hasCustomRowLabels = (!customRowLabels.isEmpty() && customRowLabels.size() >= rows);
	bool showRowColumn = !hasCustomRowLabels && showIndicesCheck->isChecked();

	if (showRowColumn) {
	    headers << "Row";
	}

	// Use custom column headers if available, otherwise default
	if (!customHeaders.isEmpty() && customHeaders.size() >= cols) {
	    for (size_t j = 0; j < cols; ++j) {
		headers << customHeaders[j];
	    }
	} else {
	    for (size_t j = 1; j <= cols; ++j) {
		headers << QString("Col %1").arg(j);
	    }
	}

	table->setColumnCount(headers.size());
	table->setHorizontalHeaderLabels(headers);
	table->setRowCount(rows);

	// USE CUSTOM ROW LABELS IF AVAILABLE
	if (hasCustomRowLabels) {
	    table->setVerticalHeaderLabels(customRowLabels);
	}

	// Fill data
	for (size_t i = 1; i <= rows; ++i) {
	    sol::object rowObj = luaTable[i];
	    if (rowObj.get_type() == sol::type::table) {
		sol::table row = rowObj.as<sol::table>();

		int colOffset = 0;
		if (showRowColumn) {
		    table->setItem(i-1, 0, new QTableWidgetItem(QString::number(i)));
		    colOffset = 1;
		}

		for (size_t j = 1; j <= cols; ++j) {
		    sol::object cellObj = row[j];
		    QString cellText = formatValue(cellObj);

		    QTableWidgetItem* item = new QTableWidgetItem(cellText);
		    if (cellObj.get_type() == sol::type::number) {
			item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		    }
		    table->setItem(i-1, j-1+colOffset, item);
		}
	    }
	}

	autoResizeColumns();
    }  
    
    void displayArrayTable(const sol::table& luaTable, const QString& arrayName)
    {
        size_t arraySize = 0;
        for (const auto& pair : luaTable) {
            arraySize++;
        }
        
        setInfo(QString("Array: %1 (length: %2)").arg(arrayName).arg(arraySize));
        
        QStringList headers;
        if (showIndicesCheck->isChecked()) {
            headers << "Index";
        }
        headers << "Value" << "Type";
        
        table->setColumnCount(headers.size());
        table->setHorizontalHeaderLabels(headers);
        table->setRowCount(arraySize);
        
        int row = 0;
        for (const auto& pair : luaTable) {
            sol::object key = pair.first;
            sol::object value = pair.second;
            
            int colOffset = 0;
            if (showIndicesCheck->isChecked()) {
                table->setItem(row, 0, new QTableWidgetItem(formatValue(key)));
                colOffset = 1;
            }
            
            QString valueText = formatValue(value);
            QString typeText = QString::fromStdString(sol::type_name(luaState->lua_state(), value.get_type()));
            
            QTableWidgetItem* valueItem = new QTableWidgetItem(valueText);
            if (value.get_type() == sol::type::number) {
                valueItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            }
            
            table->setItem(row, 0+colOffset, valueItem);
            table->setItem(row, 1+colOffset, new QTableWidgetItem(typeText));
            
            row++;
        }
        
        autoResizeColumns();
    }
    
    void displayGenericTable(const sol::table& luaTable, const QString& tableName)
    {
        setInfo(QString("Table: %1").arg(tableName));
        
        QStringList headers = {"Key", "Value", "Key Type", "Value Type"};
        table->setColumnCount(headers.size());
        table->setHorizontalHeaderLabels(headers);
        
        // Count entries
        size_t entryCount = 0;
        for (const auto& pair : luaTable) {
            entryCount++;
        }
        table->setRowCount(entryCount);
        
        int row = 0;
        for (const auto& pair : luaTable) {
            sol::object key = pair.first;
            sol::object value = pair.second;
            
            QString keyText = formatValue(key);
            QString valueText = formatValue(value);
            QString keyType = QString::fromStdString(sol::type_name(luaState->lua_state(), key.get_type()));
            QString valueType = QString::fromStdString(sol::type_name(luaState->lua_state(), value.get_type()));
            
            table->setItem(row, 0, new QTableWidgetItem(keyText));
            
            QTableWidgetItem* valueItem = new QTableWidgetItem(valueText);
            if (value.get_type() == sol::type::number) {
                valueItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            }
            table->setItem(row, 1, valueItem);
            
            table->setItem(row, 2, new QTableWidgetItem(keyType));
            table->setItem(row, 3, new QTableWidgetItem(valueType));
            
            row++;
        }
        
        autoResizeColumns();
    }
    
    // Fixed scalar display methods - no templates to avoid compilation issues
    void displayNumberScalar(double value, const QString& name)
    {
        setInfo(QString("Scalar: %1").arg(name));
        
        QStringList headers = {"Property", "Value"};
        table->setColumnCount(headers.size());
        table->setHorizontalHeaderLabels(headers);
        table->setRowCount(2);
        
        table->setItem(0, 0, new QTableWidgetItem("Value"));
        table->setItem(0, 1, new QTableWidgetItem(QString::number(value)));
        
        table->setItem(1, 0, new QTableWidgetItem("Type"));
        table->setItem(1, 1, new QTableWidgetItem("number"));
        
        autoResizeColumns();
    }
    
    void displayStringScalar(const std::string& value, const QString& name)
    {
        setInfo(QString("Scalar: %1").arg(name));
        
        QStringList headers = {"Property", "Value"};
        table->setColumnCount(headers.size());
        table->setHorizontalHeaderLabels(headers);
        table->setRowCount(2);
        
        table->setItem(0, 0, new QTableWidgetItem("Value"));
        table->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(value)));
        
        table->setItem(1, 0, new QTableWidgetItem("Type"));
        table->setItem(1, 1, new QTableWidgetItem("string"));
        
        autoResizeColumns();
    }
    
    void displayBooleanScalar(bool value, const QString& name)
    {
        setInfo(QString("Scalar: %1").arg(name));
        
        QStringList headers = {"Property", "Value"};
        table->setColumnCount(headers.size());
        table->setHorizontalHeaderLabels(headers);
        table->setRowCount(2);
        
        table->setItem(0, 0, new QTableWidgetItem("Value"));
        table->setItem(0, 1, new QTableWidgetItem(value ? "true" : "false"));
        
        table->setItem(1, 0, new QTableWidgetItem("Type"));
        table->setItem(1, 1, new QTableWidgetItem("boolean"));
        
        autoResizeColumns();
    }
    
    void displayUserData(const sol::object& userData, const QString& name)
    {
        setInfo(QString("UserData: %1").arg(name));
        
        QStringList headers = {"Property", "Value"};
        table->setColumnCount(headers.size());
        table->setHorizontalHeaderLabels(headers);
        table->setRowCount(1);
        
        table->setItem(0, 0, new QTableWidgetItem("Type"));
        table->setItem(0, 1, new QTableWidgetItem("UserData (C++ object)"));
        
        autoResizeColumns();
    }
    
    QString formatValue(const sol::object& obj)
    {
        if (!obj.valid()) return "nil";
        
        sol::type objType = obj.get_type();
        QString format = formatCombo->currentText();
        int precision = precisionSpin->value();
        
        switch (objType) {
        case sol::type::number: {
            double value = obj.as<double>();
            
            if (format == "Scientific") {
                return QString::number(value, 'e', precision);
            } else if (format == "Fixed") {
                return QString::number(value, 'f', precision);
            } else if (format == "General") {
                return QString::number(value, 'g', precision);
            } else if (format == "Hex") {
                return QString("0x%1").arg(QString::number(static_cast<long long>(value), 16));
            } else { // Auto
                if (std::abs(value) > 1e6 || (std::abs(value) < 1e-3 && value != 0)) {
                    return QString::number(value, 'e', precision);
                } else {
                    return QString::number(value, 'f', precision);
                }
            }
        }
        case sol::type::string:
            return QString::fromStdString(obj.as<std::string>());
        case sol::type::boolean:
            return obj.as<bool>() ? "true" : "false";
        case sol::type::lua_nil:
            return "nil";
        case sol::type::table:
            return "[table]";
        case sol::type::userdata:
            return "[userdata]";
        case sol::type::function:
            return "[function]";
        default:
            return "[unknown]";
        }
    }
    
    void setInfo(const QString& info) {
        infoLabel->setText(info);
    }
    
    void autoResizeColumns() {
        table->resizeColumnsToContents();
        
        // Ensure minimum and maximum column widths
        for (int i = 0; i < table->columnCount(); ++i) {
            int currentWidth = table->columnWidth(i);
            table->setColumnWidth(i, std::max(80, std::min(300, currentWidth)));
        }
    }

public slots:
    void refreshDisplay() {
        if (currentData.valid()) {
            displayData(currentData, titleLabel->text());
        }
    }
    
    void applyFilter() {
        QString filterText = filterEdit->text().toLower();
        
        for (int row = 0; row < table->rowCount(); ++row) {
            bool showRow = filterText.isEmpty();
            
            if (!showRow) {
                for (int col = 0; col < table->columnCount(); ++col) {
                    QTableWidgetItem* item = table->item(row, col);
                    if (item && item->text().toLower().contains(filterText)) {
                        showRow = true;
                        break;
                    }
                }
            }
            
            table->setRowHidden(row, !showRow);
        }
    }
    
    void copyToClipboard() {
        QString clipboardText;
        
        // Headers
        QStringList headers;
        for (int col = 0; col < table->columnCount(); ++col) {
            headers << table->horizontalHeaderItem(col)->text();
        }
        clipboardText += headers.join("\t") + "\n";
        
        // Data (only visible rows)
        for (int row = 0; row < table->rowCount(); ++row) {
            if (!table->isRowHidden(row)) {
                QStringList rowData;
                for (int col = 0; col < table->columnCount(); ++col) {
                    QTableWidgetItem* item = table->item(row, col);
                    rowData << (item ? item->text() : "");
                }
                clipboardText += rowData.join("\t") + "\n";
            }
        }
        
        QApplication::clipboard()->setText(clipboardText);
        QMessageBox::information(this, "Copied", "Data copied to clipboard!");
    }
    
    void saveAsCSV() {
	// Enhanced CSV export with labels
	QString fileName = QFileDialog::getSaveFileName(this, "Save as CSV", "", "CSV Files (*.csv)");
	if (fileName.isEmpty()) return;

	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
	    QTextStream out(&file);

	    // Write metadata header
	    if (!matrixTitle.isEmpty()) {
		out << "# " << matrixTitle << "\n";
	    }
	    if (!matrixDescription.isEmpty()) {
		out << "# " << matrixDescription << "\n";
	    }
	    out << "# Generated: " << QDateTime::currentDateTime().toString() << "\n\n";

	    // Write column headers (with custom labels if available)
	    if (!customHeaders.isEmpty()) {
		out << "Row Label,";
		for (int i = 0; i < customHeaders.size(); ++i) {
		    out << customHeaders[i];
		    if (i < customHeaders.size() - 1) out << ",";
		}
		out << "\n";
	    }

	    // Write data rows with custom row labels
	    for (int row = 0; row < table->rowCount(); ++row) {
		if (!customRowLabels.isEmpty() && row < customRowLabels.size()) {
		    out << customRowLabels[row] << ",";
		}

		for (int col = 0; col < table->columnCount(); ++col) {
		    QTableWidgetItem* item = table->item(row, col);
		    if (item) {
			out << item->text();
		    }
		    if (col < table->columnCount() - 1) out << ",";
		}
		out << "\n";
	    }
	}
    }
    void clearTable() {
        table->setRowCount(0);
        table->setColumnCount(0);
        setInfo("No data loaded");
	//        currentData = sol::lua_nil;
    }
};

#endif // GENERICDATATABLEWIDGET_HPP
