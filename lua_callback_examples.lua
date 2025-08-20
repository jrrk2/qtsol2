-- Event Callback Examples for Sol2 + Qt6 Integration

print("=== Creating Interactive Windows with Lua Callbacks ===")

-- Example 1: Simple Calculator Window
calculator = window_factory:createWindow("Calculator", 300, 200)

calculator:addLineEdit("Enter first number", "num1")
calculator:addLineEdit("Enter second number", "num2")
calculator:addButton("Add", "add_btn")
calculator:addButton("Multiply", "mul_btn")
calculator:addLabel("Result: ", "result")

-- Calculator button click handler
calculator:setOnButtonClick(function(buttonName)
    local num1 = tonumber(calculator:getWidgetText("num1"))
    local num2 = tonumber(calculator:getWidgetText("num2"))
    
    if num1 and num2 then
        local result = 0
        local operation = ""
        
        if buttonName == "add_btn" then
            result = num1 + num2
            operation = "+"
        elseif buttonName == "mul_btn" then
            result = num1 * num2
            operation = "*"
        end
        
        calculator:setWidgetText("result", string.format("Result: %.1f %s %.1f = %.2f", 
                                                        num1, operation, num2, result))
        print(string.format("Calculation: %.1f %s %.1f = %.2f", num1, operation, num2, result))
    else
        calculator:setWidgetText("result", "Result: Please enter valid numbers!")
    end
end)

calculator:show()

-- Example 2: Dynamic Color Mixer
mixer = window_factory:createWindow("Color Mixer", 250, 300)

mixer:addLabel("Red: 0", "red_label")
mixer:addSlider(0, 255, 128, "red_slider")
mixer:addLabel("Green: 0", "green_label")
mixer:addSlider(0, 255, 128, "green_slider")
mixer:addLabel("Blue: 0", "blue_label")
mixer:addSlider(0, 255, 128, "blue_slider")
mixer:addLabel("Preview", "color_preview")

-- Color update function
function updateColor()
    local r = mixer:getSliderValue("red_slider")
    local g = mixer:getSliderValue("green_slider")
    local b = mixer:getSliderValue("blue_slider")
    
    mixer:setWidgetText("red_label", "Red: " .. r)
    mixer:setWidgetText("green_label", "Green: " .. g)
    mixer:setWidgetText("blue_label", "Blue: " .. b)
    
    local color = string.format("rgb(%d, %d, %d)", r, g, b)
    mixer:setBackgroundColor(color)
    
    local hex = string.format("#%02X%02X%02X", r, g, b)
    mixer:setWidgetText("color_preview", "Color: " .. hex)
    
    print("Color updated: " .. hex)
end

-- Set slider callback
mixer:setOnSliderChange(function(sliderName, value)
    updateColor()
end)

-- Initialize color on show
mixer:setOnWindowShow(function()
    updateColor()
    print("Color mixer window opened")
end)

mixer:show()

-- Example 3: Chat/Log Window with Text Input
chat = window_factory:createWindow("Chat Log", 400, 350)

chat:addTextEdit("", "chat_log")
chat:addLineEdit("Type a message...", "message_input")
chat:addButton("Send", "send_btn")
chat:addButton("Clear", "clear_btn")

-- Chat messages storage
chat_messages = {}

-- Function to add message to chat
function addChatMessage(message)
    local timestamp = os.date("%H:%M:%S")
    local formatted = string.format("[%s] %s", timestamp, message)
    table.insert(chat_messages, formatted)
    
    -- Update chat display
    local full_text = table.concat(chat_messages, "\n")
    chat:setWidgetText("chat_log", full_text)
    
    print("Chat: " .. formatted)
end

-- Chat button handler
chat:setOnButtonClick(function(buttonName)
    if buttonName == "send_btn" then
        local message = chat:getWidgetText("message_input")
        if message and message ~= "" then
            addChatMessage(message)
            chat:setWidgetText("message_input", "")
        end
    elseif buttonName == "clear_btn" then
        chat_messages = {}
        chat:setWidgetText("chat_log", "")
        print("Chat cleared")
    end
end)

-- Handle text changes in message input (could trigger auto-save, etc.)
chat:setOnTextChange(function(widgetName, text)
    if widgetName == "message_input" then
        -- Could implement typing indicators, auto-complete, etc.
        if string.len(text) > 100 then
            print("Warning: Message is getting long (" .. string.len(text) .. " chars)")
        end
    end
end)

-- Welcome message
chat:setOnWindowShow(function()
    addChatMessage("Welcome to the chat! Type a message below.")
end)

chat:show()

-- Example 4: Settings Window with Close Confirmation
settings = window_factory:createWindow("Settings", 300, 250)

settings:addLabel("Application Settings", "title")
settings:addLineEdit("Username", "username")
settings:addSlider(1, 10, 5, "volume_slider")
settings:addLabel("Volume: 5", "volume_label")
settings:addButton("Save Settings", "save_btn")
settings:addButton("Reset", "reset_btn")

-- Settings data
app_settings = {
    username = "User",
    volume = 5
}

-- Update volume display
settings:setOnSliderChange(function(sliderName, value)
    if sliderName == "volume_slider" then
        settings:setWidgetText("volume_label", "Volume: " .. value)
        app_settings.volume = value
    end
end)

-- Settings button handler
settings:setOnButtonClick(function(buttonName)
    if buttonName == "save_btn" then
        app_settings.username = settings:getWidgetText("username")
        app_settings.volume = settings:getSliderValue("volume_slider")
        
        print("Settings saved:")
        print("  Username: " .. app_settings.username)
        print("  Volume: " .. app_settings.volume)
        
        settings:setWidgetText("title", "Settings Saved!")
        
    elseif buttonName == "reset_btn" then
        settings:setWidgetText("username", "User")
        settings:setSliderValue("volume_slider", 5)
        settings:setWidgetText("volume_label", "Volume: 5")
        settings:setWidgetText("title", "Settings Reset!")
        print("Settings reset to defaults")
    end
end)

-- Close confirmation
settings:setOnWindowClose(function()
    local current_username = settings:getWidgetText("username")
    local current_volume = settings:getSliderValue("volume_slider")
    
    if current_username ~= app_settings.username or current_volume ~= app_settings.volume then
        print("Warning: You have unsaved changes!")
        -- In a real app, you might show a confirmation dialog here
        -- For now, we'll just warn and allow closing
    end
    
    print("Settings window closing")
    -- Return true to allow closing, false to prevent
    return true
end)

settings:show()

-- Example 5: Advanced Progress Monitor
monitor = window_factory:createWindow("Task Monitor", 350, 200)

monitor:addLabel("Task Progress", "task_title")
monitor:addSlider(0, 100, 0, "progress_slider")
monitor:addLabel("Progress: 0%", "progress_label")
monitor:addButton("Start Task", "start_btn")
monitor:addButton("Pause", "pause_btn")
monitor:addButton("Reset", "reset_btn")

-- Task state
task_state = {
    running = false,
    progress = 0,
    timer_active = false
}

-- Simulate task progress (this would normally be driven by actual work)
function simulateProgress()
    if task_state.running and task_state.progress < 100 then
        task_state.progress = task_state.progress + math.random(1, 5)
        if task_state.progress > 100 then
            task_state.progress = 100
            task_state.running = false
        end
        
        monitor:setSliderValue("progress_slider", task_state.progress)
        monitor:setWidgetText("progress_label", "Progress: " .. task_state.progress .. "%")
        
        if task_state.progress >= 100 then
            monitor:setWidgetText("task_title", "Task Completed!")
            print("Task completed successfully!")
        end
    end
end

-- Monitor button handler
monitor:setOnButtonClick(function(buttonName)
    if buttonName == "start_btn" then
        if not task_state.running then
            task_state.running = true
            monitor:setWidgetText("task_title", "Task Running...")
            print("Task started")
            
            -- Simulate work with a simple loop (in real app, this would be actual work)
            for i = 1, 20 do
                if task_state.running then
                    simulateProgress()
                end
            end
        end
        
    elseif buttonName == "pause_btn" then
        task_state.running = not task_state.running
        if task_state.running then
            monitor:setWidgetText("task_title", "Task Running...")
            print("Task resumed")
        else
            monitor:setWidgetText("task_title", "Task Paused")
            print("Task paused")
        end
        
    elseif buttonName == "reset_btn" then
        task_state.running = false
        task_state.progress = 0
        monitor:setSliderValue("progress_slider", 0)
        monitor:setWidgetText("progress_label", "Progress: 0%")
        monitor:setWidgetText("task_title", "Task Progress")
        print("Task reset")
    end
end)

monitor:show()

print("=== All Example Windows Created ===")
print("Windows created:")
print("1. Calculator - Basic math operations")
print("2. Color Mixer - Dynamic RGB color mixing")
print("3. Chat Log - Message logging with timestamps")
print("4. Settings - Configuration with save/reset")
print("5. Task Monitor - Progress tracking simulation")
print("")
print("Total windows: " .. window_factory:getWindowCount())

-- Advanced Example 6: Data Visualization Dashboard
dashboard = window_factory:createWindow("Data Dashboard", 500, 400)

dashboard:addLabel("Sales Data Dashboard", "dash_title")
dashboard:addSlider(1, 12, 6, "month_slider")
dashboard:addLabel("Month: June", "month_label")
dashboard:addLabel("Sales: $0", "sales_label")
dashboard:addLabel("Growth: 0%", "growth_label")
dashboard:addTextEdit("", "chart_display")
dashboard:addButton("Generate Report", "report_btn")
dashboard:addButton("Export Data", "export_btn")

-- Sample sales data
sales_data = {
    {month = "January", sales = 45000, growth = 5.2},
    {month = "February", sales = 52000, growth = 15.6},
    {month = "March", sales = 48000, growth = -7.7},
    {month = "April", sales = 61000, growth = 27.1},
    {month = "May", sales = 58000, growth = -4.9},
    {month = "June", sales = 67000, growth = 15.5},
    {month = "July", sales = 72000, growth = 7.5},
    {month = "August", sales = 69000, growth = -4.2},
    {month = "September", sales = 75000, growth = 8.7},
    {month = "October", sales = 82000, growth = 9.3},
    {month = "November", sales = 89000, growth = 8.5},
    {month = "December", sales = 95000, growth = 6.7}
}

-- Function to create ASCII chart
function createChart(data, current_month)
    local chart = "Sales Chart (Simplified):\n\n"
    local max_sales = 0
    
    -- Find max for scaling
    for i = 1, current_month do
        if data[i].sales > max_sales then
            max_sales = data[i].sales
        end
    end
    
    -- Create simple bar chart
    for i = 1, current_month do
        local bar_length = math.floor((data[i].sales / max_sales) * 30)
        local bar = string.rep("█", bar_length)
        chart = chart .. string.format("%s: %s $%dk\n", 
            string.sub(data[i].month, 1, 3), bar, data[i].sales / 1000)
    end
    
    return chart
end

-- Dashboard update function
function updateDashboard()
    local month_idx = dashboard:getSliderValue("month_slider")
    local current_data = sales_data[month_idx]
    
    dashboard:setWidgetText("month_label", "Month: " .. current_data.month)
    dashboard:setWidgetText("sales_label", string.format("Sales: $%d", current_data.sales))
    dashboard:setWidgetText("growth_label", string.format("Growth: %.1f%%", current_data.growth))
    
    -- Update chart
    local chart = createChart(sales_data, month_idx)
    dashboard:setWidgetText("chart_display", chart)
    
    -- Color code based on growth
    if current_data.growth > 0 then
        dashboard:setBackgroundColor("#e8f5e8")  -- Light green
    else
        dashboard:setBackgroundColor("#ffe8e8")  -- Light red
    end
end

-- Dashboard slider handler
dashboard:setOnSliderChange(function(sliderName, value)
    if sliderName == "month_slider" then
        updateDashboard()
        print("Dashboard updated for month: " .. sales_data[value].month)
    end
end)

-- Dashboard button handler
dashboard:setOnButtonClick(function(buttonName)
    if buttonName == "report_btn" then
        local month_idx = dashboard:getSliderValue("month_slider")
        local data = sales_data[month_idx]
        
        print("=== Monthly Report ===")
        print("Month: " .. data.month)
        print("Sales: $" .. data.sales)
        print("Growth: " .. data.growth .. "%")
        
        -- Calculate year-to-date
        local ytd_sales = 0
        for i = 1, month_idx do
            ytd_sales = ytd_sales + sales_data[i].sales
        end
        print("YTD Sales: $" .. ytd_sales)
        print("Average: $" .. math.floor(ytd_sales / month_idx))
        
    elseif buttonName == "export_btn" then
        print("=== Exported Data ===")
        for i = 1, dashboard:getSliderValue("month_slider") do
            local data = sales_data[i]
            print(string.format("%s,%d,%.1f", data.month, data.sales, data.growth))
        end
        print("Data exported to console (CSV format)")
    end
end)

-- Initialize dashboard
dashboard:setOnWindowShow(function()
    updateDashboard()
    print("Dashboard initialized")
end)

dashboard:show()

-- Example 7: Interactive Game/Quiz Window
quiz = window_factory:createWindow("Math Quiz", 350, 300)

quiz:addLabel("Math Quiz Game", "quiz_title")
quiz:addLabel("Question: ", "question_label")
quiz:addLineEdit("Your answer", "answer_input")
quiz:addButton("Submit", "submit_btn")
quiz:addButton("New Question", "new_btn")
quiz:addButton("Show Score", "score_btn")
quiz:addLabel("Score: 0/0", "score_label")
quiz:addLabel("", "feedback_label")

-- Quiz state
quiz_state = {
    current_question = {},
    score = 0,
    total = 0,
    questions = {}
}

-- Generate random math question
function generateQuestion()
    local operations = {"+", "-", "*"}
    local op = operations[math.random(1, 3)]
    local a = math.random(1, 20)
    local b = math.random(1, 20)
    
    local question_text = string.format("%d %s %d = ?", a, op, b)
    local answer = 0
    
    if op == "+" then
        answer = a + b
    elseif op == "-" then
        answer = a - b
    else
        answer = a * b
    end
    
    return {
        text = question_text,
        answer = answer,
        a = a,
        b = b,
        op = op
    }
end

-- Start new question
function newQuestion()
    quiz_state.current_question = generateQuestion()
    quiz:setWidgetText("question_label", "Question: " .. quiz_state.current_question.text)
    quiz:setWidgetText("answer_input", "")
    quiz:setWidgetText("feedback_label", "")
    print("New question: " .. quiz_state.current_question.text)
end

-- Check answer
function checkAnswer()
    local user_answer = tonumber(quiz:getWidgetText("answer_input"))
    if not user_answer then
        quiz:setWidgetText("feedback_label", "Please enter a valid number!")
        return
    end
    
    quiz_state.total = quiz_state.total + 1
    
    if user_answer == quiz_state.current_question.answer then
        quiz_state.score = quiz_state.score + 1
        quiz:setWidgetText("feedback_label", "Correct! Well done!")
        quiz:setBackgroundColor("#e8f5e8")  -- Light green
        print("Correct answer!")
    else
        quiz:setWidgetText("feedback_label", 
            string.format("Wrong! The answer was %d", quiz_state.current_question.answer))
        quiz:setBackgroundColor("#ffe8e8")  -- Light red
        print("Incorrect answer. Correct was: " .. quiz_state.current_question.answer)
    end
    
    quiz:setWidgetText("score_label", 
        string.format("Score: %d/%d (%.1f%%)", 
            quiz_state.score, quiz_state.total, 
            (quiz_state.score / quiz_state.total) * 100))
end

-- Quiz button handler
quiz:setOnButtonClick(function(buttonName)
    if buttonName == "submit_btn" then
        if quiz_state.current_question.text then
            checkAnswer()
        else
            quiz:setWidgetText("feedback_label", "Generate a question first!")
        end
        
    elseif buttonName == "new_btn" then
        newQuestion()
        quiz:setBackgroundColor("#ffffff")  -- Reset background
        
    elseif buttonName == "score_btn" then
        if quiz_state.total > 0 then
            local percentage = (quiz_state.score / quiz_state.total) * 100
            print("=== Quiz Statistics ===")
            print("Correct: " .. quiz_state.score)
            print("Total: " .. quiz_state.total)
            print("Percentage: " .. string.format("%.1f%%", percentage))
            
            local grade = ""
            if percentage >= 90 then grade = "A"
            elseif percentage >= 80 then grade = "B"
            elseif percentage >= 70 then grade = "C"
            elseif percentage >= 60 then grade = "D"
            else grade = "F" end
            
            print("Grade: " .. grade)
            quiz:setWidgetText("feedback_label", "Grade: " .. grade)
        else
            print("No questions answered yet!")
        end
    end
end)

-- Handle Enter key in answer input
quiz:setOnTextChange(function(widgetName, text)
    -- Could implement real-time validation here
    if widgetName == "answer_input" and string.len(text) > 10 then
        quiz:setWidgetText("feedback_label", "Answer seems too long...")
    end
end)

-- Initialize quiz
quiz:setOnWindowShow(function()
    newQuestion()
    print("Math quiz started!")
end)

quiz:show()

print("")
print("=== Event Callback System Demonstration Complete ===")
print("Total active windows: " .. window_factory:getWindowCount())
print("")
print("Try interacting with the windows to see the callbacks in action!")
print("Each window demonstrates different event handling patterns:")
print("- Button clicks with custom logic")
print("- Slider changes with real-time updates")
print("- Text input with validation")
print("- Window close confirmations")
print("- Dynamic content generation")
print("")
print("You can also create your own windows with custom callbacks!")
print("Example:")
print("  win = window_factory:createWindow('My Window')")
print("  win:addButton('Test', 'test_btn')")
print("  win:setOnButtonClick(function(name) print('Hello from ' .. name) end)")
print("  win:show()")

-- Return summary
return "Event callback system initialized with " .. window_factory:getWindowCount() .. " example windows"