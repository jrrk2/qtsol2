-- self_editor.lua - A Lua script that can edit itself!
-- This script creates a window that allows you to modify and reload its own code

print("Loading Self-Editing Script...")

-- Get the current script filename
local script_filename = "self_editor.lua"

-- Create the self-editor window
local editor_window = window_factory:createWindow("Self-Editing Script", 600, 500)

-- Add UI elements
editor_window:addLabel("Self-Editing Lua Script - iteration2", "title")
editor_window:addLabel("Filename: " .. script_filename, "filename_label")
editor_window:addTextEdit("", "code_editor")
editor_window:addButton("Load Current Script", "load_btn")
editor_window:addButton("Save Changes", "save_btn")
editor_window:addButton("Save & Reload", "save_reload_btn")
editor_window:addButton("Backup Script", "backup_btn")
editor_window:addButton("Reset to Original", "reset_btn")
editor_window:addLabel("Status: Ready", "status_label")

-- Store original script content for reset functionality
local original_script = ""

-- Function to load the current script into the editor
function load_script_content()
    if file_exists(script_filename) then
        local content = read_file(script_filename)
        editor_window:setWidgetText("code_editor", content)
        editor_window:setWidgetText("status_label", "Status: Script loaded successfully")
        
        -- Store original content on first load
        if original_script == "" then
            original_script = content
        end
        
        print("Script content loaded into editor")
        return content
    else
        editor_window:setWidgetText("status_label", "Status: Error - Script file not found!")
        print("Error: Script file not found: " .. script_filename)
        return nil
    end
end

-- Function to save the edited script
function save_script_content()
    local new_content = editor_window:getWidgetText("code_editor")
    
    if new_content and new_content ~= "" then
        local success = write_file(script_filename, new_content)
        
        if success then
            editor_window:setWidgetText("status_label", "Status: Script saved successfully!")
            print("Script saved: " .. script_filename)
            return true
        else
            editor_window:setWidgetText("status_label", "Status: Error saving script!")
            print("Error: Could not save script")
            return false
        end
    else
        editor_window:setWidgetText("status_label", "Status: Error - No content to save!")
        return false
    end
end

-- Function to create a backup of the current script
function create_backup()
    if file_exists(script_filename) then
        local timestamp = os.date("%Y%m%d_%H%M%S")
        local backup_filename = script_filename:gsub("%.lua$", "_backup_" .. timestamp .. ".lua")
        
        local original_content = read_file(script_filename)
        local success = write_file(backup_filename, original_content)
        
        if success then
            editor_window:setWidgetText("status_label", "Status: Backup created: " .. backup_filename)
            print("Backup created: " .. backup_filename)
            return backup_filename
        else
            editor_window:setWidgetText("status_label", "Status: Error creating backup!")
            return nil
        end
    end
end

-- Function to reload the script (dangerous but cool!)
function reload_script()
    editor_window:setWidgetText("status_label", "Status: Reloading script...")
    print("Reloading script: " .. script_filename)
    
    -- Close current window before reloading to avoid conflicts
    editor_window:close()
    
    -- Small delay to ensure window closes
    -- Then reload the script
    local success, result = pcall(dofile, script_filename)
    
    if success then
        print("Script reloaded successfully!")
        if result then
            print("Script returned: " .. tostring(result))
        end
    else
        print("Error reloading script: " .. tostring(result))
        -- If reload fails, we need to create a new window to show the error
        local error_window = window_factory:createWindow("Script Error", 400, 200)
        error_window:addLabel("Script Reload Error", "error_title")
        error_window:addTextEdit("Error: " .. tostring(result), "error_text")
        error_window:addButton("Close", "close_btn")
        
        error_window:setOnButtonClick(function(name)
            if name == "close_btn" then
                error_window:close()
            end
        end)
        
        error_window:show()
    end
end

-- Syntax highlighting helper (basic)
function check_syntax()
    local content = editor_window:getWidgetText("code_editor")
    
    -- Basic Lua syntax checking by trying to load the string
    local func, err = load(content)
    
    if func then
        editor_window:setWidgetText("status_label", "Status: Syntax OK ✓")
        editor_window:setBackgroundColor("#e8f5e8")  -- Light green
        return true
    else
        editor_window:setWidgetText("status_label", "Status: Syntax Error - " .. (err or "Unknown error"))
        editor_window:setBackgroundColor("#ffe8e8")  -- Light red
        print("Syntax error: " .. (err or "Unknown error"))
        return false
    end
end

-- Set up event handlers
editor_window:setOnButtonClick(function(buttonName)
    if buttonName == "load_btn" then
        load_script_content()
        
    elseif buttonName == "save_btn" then
        -- Check syntax before saving
        if check_syntax() then
            save_script_content()
        else
            editor_window:setWidgetText("status_label", "Status: Cannot save - syntax errors detected!")
        end
        
    elseif buttonName == "save_reload_btn" then
        -- Check syntax first
        if check_syntax() then
            -- Create backup before making changes
            local backup_file = create_backup()
            
            if backup_file and save_script_content() then
                editor_window:setWidgetText("status_label", "Status: Saved! Reloading in 2 seconds...")
                print("Script saved successfully. Reloading...")
                
                -- Give user time to see the message
                -- In a real implementation, you'd use a timer here
                reload_script()
            end
        else
            editor_window:setWidgetText("status_label", "Status: Cannot reload - syntax errors detected!")
        end
        
    elseif buttonName == "backup_btn" then
        create_backup()
        
    elseif buttonName == "reset_btn" then
        if original_script ~= "" then
            editor_window:setWidgetText("code_editor", original_script)
            editor_window:setWidgetText("status_label", "Status: Reset to original script")
            editor_window:setBackgroundColor("#ffffff")  -- Reset background
            print("Script reset to original content")
        else
            editor_window:setWidgetText("status_label", "Status: No original content to restore")
        end
    end
end)

-- Real-time syntax checking on text changes
editor_window:setOnTextChange(function(widgetName, text)
    if widgetName == "code_editor" then
        -- Only check syntax if there's substantial content
        if string.len(text) > 10 then
            -- Add a small delay effect by only checking every few characters
            if string.len(text) % 50 == 0 then
                check_syntax()
            end
        end
    end
end)

-- Window close confirmation
editor_window:setOnWindowClose(function()
    local current_content = editor_window:getWidgetText("code_editor")
    
    -- Check if content has been modified
    if current_content ~= original_script and current_content ~= "" then
        print("Warning: You have unsaved changes!")
        editor_window:setWidgetText("status_label", "Status: Closing with unsaved changes!")
        -- In a real implementation, you might want to prevent closing here
        -- return false  -- Uncomment to prevent closing
    end
    
    print("Self-editor window closing")
    return true
end)

-- Show the window and load initial content
editor_window:show()

-- Auto-load the current script content
load_script_content()

-- Add some helpful instructions to the status
editor_window:setWidgetText("status_label", "Status: Ready - Edit the code above and click 'Save & Reload' to see changes!")

print("Self-editing script loaded successfully!")
print("INSTRUCTIONS:")
print("1. Click 'Load Current Script' to see this script's source code")
print("2. Edit the code in the text area")
print("3. Click 'Save & Reload' to apply changes and restart the script")
print("4. Use 'Backup Script' before making major changes")
print("5. Use 'Reset to Original' to undo all changes")
print("")
print("⚠️  WARNING: Be careful when editing! Syntax errors may prevent reloading.")
print("💡 TIP: Always create a backup before making significant changes.")

-- Example of self-modification: Add a counter that increments each time the script runs
local run_count_marker = "-- RUN_COUNT: "
local current_content = read_file(script_filename)

-- Look for existing run count
local count = 1
local count_line = current_content:match(run_count_marker .. "(%d+)")
if count_line then
    count = tonumber(count_line) + 1
    
    -- Update the count in the file
    local updated_content = current_content:gsub(run_count_marker .. "%d+", run_count_marker .. count)
    write_file(script_filename, updated_content)
    
    print("This script has been run " .. count .. " times!")
else
    -- Add run count to the file
    local updated_content = current_content .. "\n" .. run_count_marker .. count
    write_file(script_filename, updated_content)
    print("First time running this script!")
end

-- Update the filename label to show run count
editor_window:setWidgetText("filename_label", "Filename: " .. script_filename .. " (Run #" .. count .. ")")

-- Return a success message
return "Self-editing script initialized successfully! (Run #" .. count .. ")"

-- RUN_COUNT: 2