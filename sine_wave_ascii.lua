-- sine_wave_ascii.lua - ASCII Sine Wave Plotting with Current System
-- This works with your existing Sol2 + Qt setup without modifications

print("Creating ASCII Sine Wave Plotter...")

local plotter = window_factory:createWindow("ASCII Sine Wave Plotter", 600, 500)

plotter:addLabel("ASCII Sine Wave Plotter", "title")
plotter:addLabel("Frequency:", "freq_label")
plotter:addSlider(1, 10, 2, "frequency_slider")
plotter:addLabel("Amplitude:", "amp_label") 
plotter:addSlider(1, 20, 10, "amplitude_slider")
plotter:addLabel("Phase:", "phase_label")
plotter:addSlider(0, 360, 0, "phase_slider")
plotter:addButton("Generate Wave", "generate_btn")
plotter:addButton("Animate", "animate_btn")
plotter:addTextEdit("", "wave_display")

-- Current wave parameters
local wave_params = {
    frequency = 2,
    amplitude = 10,
    phase = 0,
    width = 80,    -- Character width of plot
    height = 20    -- Character height of plot
}

-- Function to create ASCII sine wave
function create_ascii_sine_wave()
    local freq = plotter:getSliderValue("frequency_slider")
    local amp = plotter:getSliderValue("amplitude_slider") 
    local phase_deg = plotter:getSliderValue("phase_slider")
    local phase_rad = phase_deg * math.pi / 180
    
    wave_params.frequency = freq
    wave_params.amplitude = amp
    wave_params.phase = phase_rad
    
    -- Update labels
    plotter:setWidgetText("freq_label", "Frequency: " .. freq .. " Hz")
    plotter:setWidgetText("amp_label", "Amplitude: " .. amp)
    plotter:setWidgetText("phase_label", "Phase: " .. phase_deg .. "°")
    
    local chart = "=== Sine Wave: A=" .. amp .. ", f=" .. freq .. "Hz, φ=" .. phase_deg .. "° ===\n\n"
    
    -- Create the ASCII plot
    local width = wave_params.width
    local height = wave_params.height
    local mid_line = math.floor(height / 2)
    
    -- Create 2D grid for plotting
    local grid = {}
    for y = 1, height do
        grid[y] = {}
        for x = 1, width do
            grid[y][x] = " "
        end
    end
    
    -- Plot the sine wave
    for x = 1, width do
        local t = (x - 1) / width * 4 * math.pi  -- 4π range for multiple cycles
        local y_val = amp * math.sin(freq * t + phase_rad)
        
        -- Scale to grid coordinates
        local y_pos = mid_line - math.floor(y_val * (height - 1) / (2 * amp))
        y_pos = math.max(1, math.min(height, y_pos))
        
        grid[y_pos][x] = "●"
    end
    
    -- Add axis markers
    for x = 1, width do
        grid[mid_line][x] = grid[mid_line][x] == "●" and "●" or "─"
    end
    
    -- Convert grid to string
    for y = 1, height do
        local line = ""
        for x = 1, width do
            line = line .. grid[y][x]
        end
        chart = chart .. line .. "\n"
    end
    
    -- Add scale information
    chart = chart .. "\nScale: X-axis = 4π radians (" .. (4 * math.pi) .. "), Y-axis = ±" .. amp .. "\n"
    chart = chart .. "Time points: " .. width .. " samples\n"
    
    -- Add numerical data
    chart = chart .. "\n=== Sample Values ===\n"
    for i = 0, 10 do
        local t = i * 4 * math.pi / 10
        local y = amp * math.sin(freq * t + phase_rad)
        chart = chart .. string.format("t=%.2f: y=%.3f\n", t, y)
    end
    
    plotter:setWidgetText("wave_display", chart)
    
    print("Sine wave generated: A=" .. amp .. ", f=" .. freq .. "Hz, φ=" .. phase_deg .. "°")
end

-- Animation state
local animation_active = false
local animation_step = 0

-- Animation function
function animate_wave()
    if animation_active then
        animation_step = animation_step + 0.1
        local current_phase = (animation_step * 57.3) % 360  -- Convert to degrees
        plotter:setSliderValue("phase_slider", math.floor(current_phase))
        create_ascii_sine_wave()
        
        -- Continue animation (in real app, you'd use a timer)
        print("Animation step: " .. animation_step .. ", phase: " .. current_phase .. "°")
    end
end

-- Event handlers
plotter:setOnSliderChange(function(sliderName, value)
    create_ascii_sine_wave()
end)

plotter:setOnButtonClick(function(buttonName)
    if buttonName == "generate_btn" then
        create_ascii_sine_wave()
        
    elseif buttonName == "animate_btn" then
        animation_active = not animation_active
        if animation_active then
            plotter:setWidgetText("title", "ASCII Sine Wave Plotter [ANIMATING]")
            print("Animation started - phase will change continuously")
            -- Simple animation loop
            for i = 1, 20 do
                if animation_active then
                    animate_wave()
                end
            end
            animation_active = false
            plotter:setWidgetText("title", "ASCII Sine Wave Plotter")
        else
            plotter:setWidgetText("title", "ASCII Sine Wave Plotter")
            print("Animation stopped")
        end
    end
end)

plotter:show()

-- Generate initial wave
create_ascii_sine_wave()

-- Create a 3D surface plot (ASCII)
local surface_plotter = window_factory:createWindow("3D Wave Surface", 500, 400)

surface_plotter:addLabel("3D Wave Surface Plot", "surface_title")
surface_plotter:addLabel("Wave Type:", "wave_type_label")
surface_plotter:addSlider(1, 4, 1, "wave_type_slider")
surface_plotter:addButton("Generate 3D", "gen_3d_btn")
surface_plotter:addTextEdit("", "surface_display")

function create_3d_surface()
    local wave_type = surface_plotter:getSliderValue("wave_type_slider")
    local wave_names = {"Sine", "Cosine", "Sine*Cos", "Ripple"}
    
    surface_plotter:setWidgetText("wave_type_label", "Wave Type: " .. wave_names[wave_type])
    
    local surface = "=== 3D " .. wave_names[wave_type] .. " Wave Surface ===\n\n"
    
    local width, height = 40, 20
    
    for y = 1, height do
        local line = ""
        for x = 1, width do
            local x_norm = (x - 1) / (width - 1) * 4 * math.pi
            local y_norm = (y - 1) / (height - 1) * 4 * math.pi
            
            local z = 0
            if wave_type == 1 then
                z = math.sin(x_norm) * math.sin(y_norm)
            elseif wave_type == 2 then
                z = math.cos(x_norm) * math.cos(y_norm)
            elseif wave_type == 3 then
                z = math.sin(x_norm) * math.cos(y_norm)
            else
                local r = math.sqrt(x_norm^2 + y_norm^2)
                z = math.sin(r) / (r + 0.1)
            end
            
            -- Convert z value to ASCII character
            local chars = " .:-=+*#%@"
            local char_index = math.floor((z + 1) * (#chars - 1) / 2) + 1
            char_index = math.max(1, math.min(#chars, char_index))
            
            line = line .. string.sub(chars, char_index, char_index)
        end
        surface = surface .. line .. "\n"
    end
    
    surface = surface .. "\nLegend: " .. " .:-=+*#%@" .. " (low to high)\n"
    surface = surface .. "Range: X,Y ∈ [0, 4π], Z ∈ [-1, 1]\n"
    
    surface_plotter:setWidgetText("surface_display", surface)
    print("3D surface generated: " .. wave_names[wave_type])
end

surface_plotter:setOnSliderChange(function(sliderName, value)
    create_3d_surface()
end)

surface_plotter:setOnButtonClick(function(buttonName)
    if buttonName == "gen_3d_btn" then
        create_3d_surface()
    end
end)

surface_plotter:show()
create_3d_surface()

-- Fourier series visualization
local fourier_window = window_factory:createWindow("Fourier Series", 550, 450)

fourier_window:addLabel("Fourier Series Approximation", "fourier_title")
fourier_window:addLabel("Harmonics:", "harmonics_label")
fourier_window:addSlider(1, 10, 3, "harmonics_slider")
fourier_window:addLabel("Function:", "function_label")
fourier_window:addSlider(1, 3, 1, "function_type_slider")
fourier_window:addButton("Generate Fourier", "fourier_btn")
fourier_window:addTextEdit("", "fourier_display")

function create_fourier_series()
    local harmonics = fourier_window:getSliderValue("harmonics_slider")
    local func_type = fourier_window:getSliderValue("function_type_slider")
    local func_names = {"Square Wave", "Sawtooth Wave", "Triangle Wave"}
    
    fourier_window:setWidgetText("harmonics_label", "Harmonics: " .. harmonics)
    fourier_window:setWidgetText("function_label", "Function: " .. func_names[func_type])
    
    local chart = "=== Fourier Series: " .. func_names[func_type] .. " (" .. harmonics .. " harmonics) ===\n\n"
    
    local width = 80
    local height = 15
    local grid = {}
    
    -- Initialize grid
    for y = 1, height do
        grid[y] = {}
        for x = 1, width do
            grid[y][x] = " "
        end
    end
    
    -- Generate Fourier series
    for x = 1, width do
        local t = (x - 1) / width * 4 * math.pi
        local y_fourier = 0
        
        -- Calculate Fourier series based on function type
        for n = 1, harmonics do
            if func_type == 1 then  -- Square wave
                if n % 2 == 1 then  -- Odd harmonics only
                    y_fourier = y_fourier + (4 / math.pi) * math.sin(n * t) / n
                end
            elseif func_type == 2 then  -- Sawtooth wave
                y_fourier = y_fourier + (2 / math.pi) * math.pow(-1, n + 1) * math.sin(n * t) / n
            else  -- Triangle wave
                if n % 2 == 1 then  -- Odd harmonics only
                    y_fourier = y_fourier + (8 / (math.pi^2)) * math.pow(-1, (n-1)/2) * math.sin(n * t) / (n^2)
                end
            end
        end
        
        -- Scale and plot
        local y_pos = math.floor(height/2 - y_fourier * height/4)
        y_pos = math.max(1, math.min(height, y_pos))
        grid[y_pos][x] = "●"
    end
    
    -- Convert to string
    for y = 1, height do
        local line = ""
        for x = 1, width do
            line = line .. grid[y][x]
        end
        chart = chart .. line .. "\n"
    end
    
    chart = chart .. "\nFourier coefficients used: " .. harmonics .. "\n"
    chart = chart .. "Convergence improves with more harmonics\n"
    
    fourier_window:setWidgetText("fourier_display", chart)
    print("Fourier series generated for " .. func_names[func_type] .. " with " .. harmonics .. " harmonics")
end

fourier_window:setOnSliderChange(function(sliderName, value)
    create_fourier_series()
end)

fourier_window:setOnButtonClick(function(buttonName)
    if buttonName == "fourier_btn" then
        create_fourier_series()
    end
end)

fourier_window:show()
create_fourier_series()

print("=== ASCII Plotting System Created ===")
print("Created 3 visualization tools:")
print("1. ASCII Sine Wave Plotter - Interactive sine wave with controls")
print("2. 3D Wave Surface - Multiple wave types in ASCII 3D")
print("3. Fourier Series - Mathematical series approximations")
print("")
print("While these are ASCII-based, they demonstrate:")
print("• Real-time parameter control")
print("• Mathematical accuracy") 
print("• Interactive visualization")
print("• Multiple wave types")
print("")
print("For true graphical charts, you'll need Qt Charts integration!")

return "ASCII plotting system ready with " .. window_factory:getWindowCount() .. " visualization tools"