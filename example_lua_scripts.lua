-- Example 1: simple_calculator.lua
-- A basic calculator application

print("Loading Simple Calculator...")

calculator = window_factory:createWindow("Simple Calculator", 300, 250)

calculator:addLineEdit("0", "display")
calculator:addButton("7", "btn_7")
calculator:addButton("8", "btn_8")
calculator:addButton("9", "btn_9")
calculator:addButton("+", "btn_add")
calculator:addButton("4", "btn_4")
calculator:addButton("5", "btn_5")
calculator:addButton("6", "btn_6")
calculator:addButton("-", "btn_sub")
calculator:addButton("1", "btn_1")
calculator:addButton("2", "btn_2")
calculator:addButton("3", "btn_3")
calculator:addButton("*", "btn_mul")
calculator:addButton("0", "btn_0")
calculator:addButton("C", "btn_clear")
calculator:addButton("=", "btn_equals")
calculator:addButton("/", "btn_div")

-- Calculator state
calc_state = {
    current = 0,
    operator = nil,
    operand = 0,
    new_number = true
}

function update_display()
    calculator:setWidgetText("display", tostring(calc_state.current))
end

function handle_number(num)
    if calc_state.new_number then
        calc_state.current = num
        calc_state.new_number = false
    else
        calc_state.current = calc_state.current * 10 + num
    end
    update_display()
end

function handle_operator(op)
    if calc_state.operator and not calc_state.new_number then
        calculate()
    end
    calc_state.operator = op
    calc_state.operand = calc_state.current
    calc_state.new_number = true
end

function calculate()
    if calc_state.operator then
        if calc_state.operator == "+" then
            calc_state.current = calc_state.operand + calc_state.current
        elseif calc_state.operator == "-" then
            calc_state.current = calc_state.operand - calc_state.current
        elseif calc_state.operator == "*" then
            calc_state.current = calc_state.operand * calc_state.current
        elseif calc_state.operator == "/" then
            if calc_state.current ~= 0 then
                calc_state.current = calc_state.operand / calc_state.current
            else
                calc_state.current = 0
                print("Error: Division by zero")
            end
        end
        calc_state.operator = nil
        calc_state.new_number = true
        update_display()
    end
end

calculator:setOnButtonClick(function(name)
    if name:match("btn_%d") then
        local num = tonumber(name:sub(5))
        handle_number(num)
    elseif name == "btn_add" then
        handle_operator("+")
    elseif name == "btn_sub" then
        handle_operator("-")
    elseif name == "btn_mul" then
        handle_operator("*")
    elseif name == "btn_div" then
        handle_operator("/")
    elseif name == "btn_equals" then
        calculate()
    elseif name == "btn_clear" then
        calc_state.current = 0
        calc_state.operator = nil
        calc_state.operand = 0
        calc_state.new_number = true
        update_display()
    end
end)

calculator:show()
print("Simple Calculator loaded and ready!")

-- Save this as: simple_calculator.lua

-- ============================================================================

-- Example 2: data_manager.lua
-- A data management and visualization tool

print("Loading Data Manager...")

data_manager = window_factory:createWindow("Data Manager", 450, 350)

data_manager:addLabel("Data Management Tool", "title")
data_manager:addLineEdit("Enter item name", "item_name")
data_manager:addLineEdit("Enter value", "item_value")
data_manager:addButton("Add Item", "add_btn")
data_manager:addButton("Clear All", "clear_btn")
data_manager:addButton("Save to File", "save_btn")
data_manager:addButton("Load from File", "load_btn")
data_manager:addTextEdit("Data will appear here...", "data_display")
data_manager:addLabel("Total Items: 0", "count_label")

-- Data storage
local data_items = {}

function update_display()
    local display_text = "=== Data Items ===\n"
    local total_value = 0
    
    for i, item in ipairs(data_items) do
        display_text = display_text .. string.format("%d. %s: %s\n", i, item.name, item.value)
        if tonumber(item.value) then
            total_value = total_value + tonumber(item.value)
        end
    end
    
    display_text = display_text .. "\n=== Statistics ===\n"
    display_text = display_text .. "Total Items: " .. #data_items .. "\n"
    display_text = display_text .. "Numeric Total: " .. total_value .. "\n"
    
    if #data_items > 0 then
        display_text = display_text .. "Average: " .. (total_value / #data_items) .. "\n"
    end
    
    data_manager:setWidgetText("data_display", display_text)
    data_manager:setWidgetText("count_label", "Total Items: " .. #data_items)
end

data_manager:setOnButtonClick(function(name)
    if name == "add_btn" then
        local item_name = data_manager:getWidgetText("item_name")
        local item_value = data_manager:getWidgetText("item_value")
        
        if item_name ~= "" and item_value ~= "" then
            table.insert(data_items, {name = item_name, value = item_value})
            data_manager:setWidgetText("item_name", "")
            data_manager:setWidgetText("item_value", "")
            update_display()
            print("Added item: " .. item_name .. " = " .. item_value)
        else
            print("Please enter both name and value")
        end
        
    elseif name == "clear_btn" then
        data_items = {}
        update_display()
        print("All data cleared")
        
    elseif name == "save_btn" then
        local content = "-- Data Manager Export\nlocal data = {\n"
        for _, item in ipairs(data_items) do
            content = content .. string.format('  {name = "%s", value = "%s"},\n', item.name, item.value)
        end
        content = content .. "}\nreturn data"
        
        if write_file("data_export.lua", content) then
            print("Data saved to data_export.lua")
        end
        
    elseif name == "load_btn" then
        if file_exists("data_export.lua") then
            local loaded_data = dofile("data_export.lua")
            if loaded_data then
                data_items = loaded_data
                update_display()
                print("Data loaded from data_export.lua")
            end
        else
            print("No data_export.lua file found")
        end
    end
end)

data_manager:show()
update_display()
print("Data Manager loaded successfully!")

-- Save this as: data_manager.lua

-- ============================================================================

-- Example 3: script_loader.lua
-- A script that demonstrates loading other scripts

print("Loading Script Loader Manager...")

loader = window_factory:createWindow("Script Loader", 400, 300)

loader:addLabel("Lua Script Loader", "title")
loader:addLineEdit("script_name.lua", "filename_input")
loader:addButton("Load Script", "load_btn")
loader:addButton("List .lua Files", "list_btn")
loader:addButton("Create Template", "template_btn")
loader:addTextEdit("Available scripts and results...", "results_display")

-- Get current directory files
function refresh_file_list()
    local files = list_files(".", "*.lua")
    local display_text = "=== Available Lua Scripts ===\n"
    
    for i, file in ipairs(files) do
        display_text = display_text .. i .. ". " .. file .. "\n"
    end
    
    if #files == 0 then
        display_text = display_text .. "No .lua files found in current directory\n"
    end
    
    display_text = display_text .. "\n=== Instructions ===\n"
    display_text = display_text .. "1. Enter filename and click 'Load Script'\n"
    display_text = display_text .. "2. Or click 'List .lua Files' to see available scripts\n"
    display_text = display_text .. "3. Use 'Create Template' to make a new script\n"
    
    loader:setWidgetText("results_display", display_text)
end

loader:setOnButtonClick(function(name)
    if name == "load_btn" then
        local filename = loader:getWidgetText("filename_input")
        
        if filename ~= "" then
            if file_exists(filename) then
                print("Loading script: " .. filename)
                
                -- Load the script
                local success, result = pcall(dofile, filename)
                
                if success then
                    local result_text = "Script loaded successfully: " .. filename .. "\n"
                    if result then
                        result_text = result_text .. "Result: " .. tostring(result) .. "\n"
                    end
                    
                    loader:setWidgetText("results_display", result_text)
                    print("Script executed: " .. filename)
                else
                    local error_text = "Error loading script: " .. filename .. "\n"
                    error_text = error_text .. "Error: " .. tostring(result) .. "\n"
                    loader:setWidgetText("results_display", error_text)
                    print("Script error: " .. tostring(result))
                end
            else
                loader:setWidgetText("results_display", "File not found: " .. filename)
                print("File not found: " .. filename)
            end
        else
            print("Please enter a filename")
        end
        
    elseif name == "list_btn" then
        refresh_file_list()
        
    elseif name == "template_btn" then
        local template = [[-- New Lua Script Template
print("Loading new script...")

-- Create a window
window = window_factory:createWindow("My Window", 300, 200)

-- Add some widgets
window:addLabel("Hello from new script!", "greeting")
window:addButton("Click Me", "my_button")
window:addLineEdit("Type here", "my_input")

-- Add event handler
window:setOnButtonClick(function(buttonName)
    if buttonName == "my_button" then
        local text = window:getWidgetText("my_input")
        window:setWidgetText("greeting", "You typed: " .. text)
        print("Button clicked! Input: " .. text)
    end
end)

-- Show the window
window:show()

print("New script loaded successfully!")
return "Template script completed"
]]
        
        local new_filename = "new_script_" .. os.date("%H%M%S") .. ".lua"
        
        if write_file(new_filename, template) then
            loader:setWidgetText("filename_input", new_filename)
            loader:setWidgetText("results_display", 
                "Template created: " .. new_filename .. "\n" ..
                "Click 'Load Script' to run it, or edit it first!")
            print("Template created: " .. new_filename)
        end
    end
end)

-- Initialize
loader:setOnWindowShow(function()
    refresh_file_list()
end)

loader:show()
print("Script Loader ready!")

-- Save this as: script_loader.lua

-- ============================================================================

-- Example 4: multi_script_demo.lua
-- Demonstrates loading multiple scripts and coordination

print("=== Multi-Script Demonstration ===")

-- This script shows how to load and coordinate multiple other scripts

-- First, let's create some helper functions
function load_script_safe(filename)
    if file_exists(filename) then
        print("Loading: " .. filename)
        local success, result = pcall(dofile, filename)
        if success then
            print("✓ Successfully loaded: " .. filename)
            return true, result
        else
            print("✗ Error in " .. filename .. ": " .. tostring(result))
            return false, result
        end
    else
        print("✗ File not found: " .. filename)
        return false, "File not found"
    end
end

-- Load multiple scripts in sequence
local scripts_to_load = {
    "simple_calculator.lua",
    "data_manager.lua", 
    "script_loader.lua"
}

print("Attempting to load " .. #scripts_to_load .. " scripts...")

local loaded_count = 0
for i, script in ipairs(scripts_to_load) do
    local success, result = load_script_safe(script)
    if success then
        loaded_count = loaded_count + 1
    end
end

print("=== Loading Summary ===")
print("Successfully loaded: " .. loaded_count .. "/" .. #scripts_to_load .. " scripts")
print("Total windows now: " .. window_factory:getWindowCount())

-- Create a control panel for all loaded scripts
control_panel = window_factory:createWindow("Multi-Script Control Panel", 350, 250)

control_panel:addLabel("Multi-Script Control Panel", "title")
control_panel:addLabel("Loaded Scripts: " .. loaded_count, "status")
control_panel:addButton("Show Window Count", "count_btn")
control_panel:addButton("Close All Windows", "close_all_btn")
control_panel:addButton("Reload All Scripts", "reload_btn")
control_panel:addTextEdit("Control panel ready...", "log")

control_panel:setOnButtonClick(function(name)
    if name == "count_btn" then
        local count = window_factory:getWindowCount()
        control_panel:setWidgetText("log", "Active windows: " .. count)
        print("Current window count: " .. count)
        
    elseif name == "close_all_btn" then
        local count = window_factory:getWindowCount()
        window_factory:closeAllWindows()
        control_panel:setWidgetText("log", "Closed " .. count .. " windows")
        print("All windows closed")
        
    elseif name == "reload_btn" then
        control_panel:setWidgetText("log", "Reloading scripts...")
        -- This would reload the current script
        load_script_safe("multi_script_demo.lua")
    end
end)

control_panel:show()

print("Multi-script demo complete!")
print("Use the control panel to manage loaded scripts")

-- Save this as: multi_script_demo.lua

return "Multi-script demonstration loaded with " .. loaded_count .. " scripts"