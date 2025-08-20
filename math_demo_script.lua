print("=== Floating Point Math Demonstration ===")

-- Test basic floating point operations
print("\n--- Basic Floating Point Operations ---")
local a = 3.14159
local b = 2.71828
print("a = " .. a)
print("b = " .. b)
print("a + b = " .. (a + b))
print("a * b = " .. (a * b))
print("a / b = " .. (a / b))
print("a ^ b = " .. (a ^ b))

-- Test all available math functions
print("\n--- Math Library Functions ---")

-- Trigonometric functions (input in radians)
local angle_rad = math.pi / 4  -- 45 degrees
local angle_deg = 45

print("Angle: " .. angle_deg .. "° (" .. angle_rad .. " radians)")
print("sin(" .. angle_deg .. "°) = " .. math.sin(angle_rad))
print("cos(" .. angle_deg .. "°) = " .. math.cos(angle_rad))
print("tan(" .. angle_deg .. "°) = " .. math.tan(angle_rad))

-- Inverse trigonometric functions
print("asin(0.5) = " .. math.asin(0.5) .. " radians (" .. (math.asin(0.5) * 180 / math.pi) .. "°)")
print("acos(0.5) = " .. math.acos(0.5) .. " radians (" .. (math.acos(0.5) * 180 / math.pi) .. "°)")
print("atan(1) = " .. math.atan(1) .. " radians (" .. (math.atan(1) * 180 / math.pi) .. "°)")
print("atan2(1, 1) = " .. math.atan2(1, 1) .. " radians (" .. (math.atan2(1, 1) * 180 / math.pi) .. "°)")

-- Exponential and logarithmic functions
print("\n--- Exponential and Logarithmic Functions ---")
print("exp(1) = " .. math.exp(1))  -- e^1
print("exp(2) = " .. math.exp(2))  -- e^2
print("log(math.e) = " .. math.log(math.exp(1)))  -- natural log
print("log10(100) = " .. math.log10(100))  -- base 10 log
print("log(8, 2) = " .. math.log(8, 2))  -- log base 2 of 8

-- Power and root functions
print("\n--- Power and Root Functions ---")
print("sqrt(16) = " .. math.sqrt(16))
print("sqrt(2) = " .. math.sqrt(2))
print("pow(2, 8) = " .. math.pow(2, 8))  -- Same as 2^8
print("2^0.5 = " .. (2^0.5))  -- Square root using exponentiation

-- Rounding and ceiling/floor functions
print("\n--- Rounding Functions ---")
local test_num = 3.7
print("Number: " .. test_num)
print("floor(" .. test_num .. ") = " .. math.floor(test_num))
print("ceil(" .. test_num .. ") = " .. math.ceil(test_num))
print("abs(-" .. test_num .. ") = " .. math.abs(-test_num))

-- Min/Max functions
print("\n--- Min/Max Functions ---")
print("min(5.2, 3.8, 7.1) = " .. math.min(5.2, 3.8, 7.1))
print("max(5.2, 3.8, 7.1) = " .. math.max(5.2, 3.8, 7.1))

-- Random number generation
print("\n--- Random Number Generation ---")
math.randomseed(os.time())  -- Seed with current time
print("random() = " .. math.random())  -- Random float 0-1
print("random(10) = " .. math.random(10))  -- Random int 1-10
print("random(5, 15) = " .. math.random(5, 15))  -- Random int 5-15

-- Constants
print("\n--- Mathematical Constants ---")
print("π (pi) = " .. math.pi)
print("e (math.exp(1)) = " .. math.exp(1))

-- Create interactive math calculator window
local calc_window = window_factory:createWindow("Advanced Math Calculator", 450, 500)

calc_window:addLabel("Advanced Math Calculator", "title")
calc_window:addLineEdit("Enter expression (e.g., sin(math.pi/2))", "expression_input")
calc_window:addButton("Calculate", "calc_btn")
calc_window:addButton("Clear", "clear_btn")
calc_window:addTextEdit("Results will appear here...", "results_display")

-- Function examples
calc_window:addLabel("Function Examples:", "examples_label")
calc_window:addButton("sin(π/2)", "sin_example")
calc_window:addButton("√2", "sqrt_example")
calc_window:addButton("e^2", "exp_example")
calc_window:addButton("log₁₀(100)", "log_example")
calc_window:addButton("Random", "random_example")

-- Angle converter
calc_window:addLabel("Angle Converter:", "angle_label")
calc_window:addLineEdit("90", "degrees_input")
calc_window:addButton("Deg→Rad", "deg_to_rad")
calc_window:addButton("Rad→Deg", "rad_to_deg")

-- Scientific notation helper
calc_window:addLabel("Scientific Notation:", "sci_label")
calc_window:addLineEdit("1.5e-3", "sci_input")
calc_window:addButton("Evaluate", "sci_eval")

-- Store calculation history
local calc_history = {}

-- Function to safely evaluate mathematical expressions
function safe_eval(expression)
    -- Create a safe environment with math functions
    local safe_env = {
        math = math,
        sin = math.sin,
        cos = math.cos,
        tan = math.tan,
        asin = math.asin,
        acos = math.acos,
        atan = math.atan,
        atan2 = math.atan2,
        exp = math.exp,
        log = math.log,
        log10 = math.log10,
        sqrt = math.sqrt,
        pow = math.pow,
        floor = math.floor,
        ceil = math.ceil,
        abs = math.abs,
        min = math.min,
        max = math.max,
        random = math.random,
        pi = math.pi,
        e = math.exp(1),
        -- Allow basic arithmetic
        tonumber = tonumber,
        tostring = tostring
    }
    
    -- Compile the expression
    local func, err = load("return " .. expression, "calc", "t", safe_env)
    
    if not func then
        return nil, "Syntax error: " .. (err or "Unknown error")
    end
    
    -- Execute the expression
    local success, result = pcall(func)
    
    if success then
        return result, nil
    else
        return nil, "Runtime error: " .. (result or "Unknown error")
    end
end

-- Function to update results display
function update_results(new_result)
    table.insert(calc_history, new_result)
    
    -- Keep only last 10 results
    while #calc_history > 10 do
        table.remove(calc_history, 1)
    end
    
    local display_text = "=== Calculation History ===\n"
    for i, result in ipairs(calc_history) do
        display_text = display_text .. i .. ". " .. result .. "\n"
    end
    
    calc_window:setWidgetText("results_display", display_text)
end

-- Button event handlers
calc_window:setOnButtonClick(function(buttonName)
    if buttonName == "calc_btn" then
        local expression = calc_window:getWidgetText("expression_input")
        
        if expression and expression ~= "" then
            local result, error_msg = safe_eval(expression)
            
            if result ~= nil then
                local result_text = expression .. " = " .. tostring(result)
                update_results(result_text)
                print("Calculated: " .. result_text)
            else
                local error_text = expression .. " → ERROR: " .. error_msg
                update_results(error_text)
                print("Error: " .. error_msg)
            end
        end
        
    elseif buttonName == "clear_btn" then
        calc_history = {}
        calc_window:setWidgetText("results_display", "Results cleared.")
        calc_window:setWidgetText("expression_input", "")
        
    elseif buttonName == "sin_example" then
        calc_window:setWidgetText("expression_input", "sin(math.pi/2)")
        
    elseif buttonName == "sqrt_example" then
        calc_window:setWidgetText("expression_input", "sqrt(2)")
        
    elseif buttonName == "exp_example" then
        calc_window:setWidgetText("expression_input", "exp(2)")
        
    elseif buttonName == "log_example" then
        calc_window:setWidgetText("expression_input", "log10(100)")
        
    elseif buttonName == "random_example" then
        local rand_expr = "random() * 100"
        calc_window:setWidgetText("expression_input", rand_expr)
        
    elseif buttonName == "deg_to_rad" then
        local degrees = tonumber(calc_window:getWidgetText("degrees_input"))
        if degrees then
            local radians = degrees * math.pi / 180
            local result_text = degrees .. "° = " .. radians .. " radians"
            update_results(result_text)
        end
        
    elseif buttonName == "rad_to_deg" then
        local radians = tonumber(calc_window:getWidgetText("degrees_input"))
        if radians then
            local degrees = radians * 180 / math.pi
            local result_text = radians .. " rad = " .. degrees .. "°"
            update_results(result_text)
        end
        
    elseif buttonName == "sci_eval" then
        local sci_notation = calc_window:getWidgetText("sci_input")
        local number = tonumber(sci_notation)
        if number then
            local result_text = sci_notation .. " = " .. number
            update_results(result_text)
        else
            update_results("Invalid scientific notation: " .. sci_notation)
        end
    end
end)

calc_window:show()

-- Create a real-time function plotter
local plotter_window = window_factory:createWindow("Function Plotter", 400, 350)

plotter_window:addLabel("Function Plotter", "plotter_title")
plotter_window:addLineEdit("sin(x)", "function_input")
plotter_window:addLineEdit("-10", "x_min")
plotter_window:addLineEdit("10", "x_max")
plotter_window:addLineEdit("0.1", "step_size")
plotter_window:addButton("Plot Function", "plot_btn")
plotter_window:addTextEdit("Function values will appear here...", "plot_display")

plotter_window:setOnButtonClick(function(buttonName)
    if buttonName == "plot_btn" then
        local func_expr = plotter_window:getWidgetText("function_input")
        local x_min = tonumber(plotter_window:getWidgetText("x_min")) or -10
        local x_max = tonumber(plotter_window:getWidgetText("x_max")) or 10
        local step = tonumber(plotter_window:getWidgetText("step_size")) or 0.1
        
        local plot_text = "=== Function: " .. func_expr .. " ===\n"
        plot_text = plot_text .. "Range: " .. x_min .. " to " .. x_max .. " (step: " .. step .. ")\n\n"
        
        local x = x_min
        local point_count = 0
        
        while x <= x_max and point_count < 50 do  -- Limit points to prevent overflow
            -- Replace 'x' in the expression with the current value
            local current_expr = func_expr:gsub("x", tostring(x))
            
            local result, error_msg = safe_eval(current_expr)
            
            if result ~= nil then
                plot_text = plot_text .. string.format("x=%.2f, y=%.6f\n", x, result)
            else
                plot_text = plot_text .. string.format("x=%.2f, ERROR: %s\n", x, error_msg)
            end
            
            x = x + step
            point_count = point_count + 1
        end
        
        if point_count >= 50 then
            plot_text = plot_text .. "\n(Limited to 50 points)"
        end
        
        plotter_window:setWidgetText("plot_display", plot_text)
    end
end)

plotter_window:show()

print("\n=== Interactive Math Tools Created ===")
print("1. Advanced Math Calculator - Evaluate complex expressions")
print("2. Function Plotter - Plot mathematical functions")
print("")
print("Available math functions in expressions:")
print("• Trigonometric: sin, cos, tan, asin, acos, atan, atan2")
print("• Exponential: exp, log, log10, pow, sqrt")
print("• Rounding: floor, ceil, abs")
print("• Comparison: min, max")
print("• Random: random")
print("• Constants: math.pi, e")
print("")
print("Example expressions to try:")
print("• sin(math.pi/2)")
print("• sqrt(2) + sqrt(3)")
print("• exp(1) * log(10)")
print("• pow(2, 8)")
print("• abs(-3.14)")

return "Math demonstration complete with " .. window_factory:getWindowCount() .. " interactive tools"