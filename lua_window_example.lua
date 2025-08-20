-- Sol2 + Qt6 Window Creation Example
print("=== Creating new Qt windows from Lua ===")

-- Create a simple window
win1 = window_factory:createWindow("My Lua Window", 300, 200)
win1:addLabel("Hello from Lua!")
win1:addButton("Click me!")
win1:addSlider(0, 100, 25)
win1:show()

-- Create a more complex window
win2 = window_factory:createWindow("Advanced Window", 400, 350)
win2:setBackgroundColor("#f0f8ff")
win2:addLabel("This window was created entirely from Lua")
win2:addTextEdit("Type something here...")
win2:addButton("Save")
win2:addButton("Cancel")
win2:addSlider(0, 200, 100)
win2:show()

-- Create a calculator-style window
calc = window_factory:createWindow("Lua Calculator", 250, 300)
calc:addLabel("Simple Calculator")
calc:addTextEdit("Enter numbers...")
calc:addButton("Add")
calc:addButton("Subtract") 
calc:addButton("Multiply")
calc:addButton("Clear")
calc:show()

print("Created " .. window_factory:getWindowCount() .. " windows")
print("Windows will send events to main console")

-- You can also close all windows:
-- window_factory:closeAllWindows()

return "Successfully created multiple Qt windows from Lua!"