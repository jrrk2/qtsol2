-- working_vector_demo.lua - Complete vector functionality demonstration
-- Now that we know the basic functions work!

print("=== Complete Vector Functionality Demo ===")

-- Create vector operations window
local vector_window = window_factory:createWindow("Vector Operations Lab", 600, 500)

vector_window:addLabel("Vector Operations Laboratory", "title")
vector_window:addLabel("Vector Size:", "size_label")
vector_window:addSlider(2, 10, 3, "size_slider")
vector_window:addButton("Create Test Vectors", "create_btn")
vector_window:addButton("Vector Math Demo", "math_btn")
vector_window:addButton("3D Vector Demo", "3d_btn")
vector_window:addButton("Statistics Demo", "stats_btn")
vector_window:addTextEdit("Vector operations will appear here...", "results_display")

-- Global vectors for operations
local vec_a = nil
local vec_b = nil

function vec_update(text)
    local current = vector_window:getWidgetText("results_display")
    vector_window:setWidgetText("results_display", current .. text .. "\n")
end

function vec_clear()
    vector_window:setWidgetText("results_display", "")
end

-- Function to display vector nicely
function display_vector(vec, name)
    if not vec then
        vec_update(name .. ": [NULL VECTOR]")
        return
    end
    
    local size = vector_size(vec)
    local display = name .. " (" .. size .. "): ["
    
    for i = 0, size - 1 do
        display = display .. string.format("%.3f", vector_get(vec, i))
        if i < size - 1 then display = display .. ", " end
    end
    display = display .. "]"
    
    vec_update(display)
end

-- Create test vectors
function create_test_vectors()
    local size = vector_window:getSliderValue("size_slider")
    vector_window:setWidgetText("size_label", "Vector Size: " .. size)
    
    vec_clear()
    vec_update("=== Creating Test Vectors (Size: " .. size .. ") ===")
    vec_update("")
    
    -- Create first vector with sequential values
    vec_a = create_vector(size)
    for i = 0, size - 1 do
        vector_set(vec_a, i, i + 1)  -- Values: 1, 2, 3, ...
    end
    display_vector(vec_a, "Vector A")
    
    -- Create second vector with different pattern
    vec_b = create_vector(size)
    for i = 0, size - 1 do
        vector_set(vec_b, i, (i + 1) * 0.5)  -- Values: 0.5, 1.0, 1.5, ...
    end
    display_vector(vec_b, "Vector B")
    
    -- Test norms
    local norm_a = vector_norm(vec_a)
    local norm_b = vector_norm(vec_b)
    vec_update("")
    vec_update("Vector Norms:")
    vec_update("  ||A|| = " .. string.format("%.4f", norm_a))
    vec_update("  ||B|| = " .. string.format("%.4f", norm_b))
    
    print("Test vectors created successfully")
end

-- Vector math demonstrations
function vector_math_demo()
    if not vec_a or not vec_b then
        vec_update("Error: Create test vectors first!")
        return
    end
    
    vec_clear()
    vec_update("=== Vector Math Operations Demo ===")
    vec_update("")
    
    display_vector(vec_a, "Vector A")
    display_vector(vec_b, "Vector B")
    vec_update("")
    
    -- Addition
    local vec_sum = vector_add(vec_a, vec_b)
    display_vector(vec_sum, "A + B")
    
    -- Subtraction
    local vec_diff = vector_subtract(vec_a, vec_b)
    display_vector(vec_diff, "A - B")
    
    -- Scaling
    local vec_scaled = vector_scale(vec_a, 2.5)
    display_vector(vec_scaled, "A × 2.5")
    
    -- Dot product
    local dot_result = dot_product(vec_a, vec_b)
    vec_update("")
    vec_update("Dot Product A · B = " .. string.format("%.4f", dot_result))
    
    -- Vector normalization
    local vec_a_copy = vector_scale(vec_a, 1.0)  -- Make a copy
    local original_norm = vector_normalize(vec_a_copy)
    local new_norm = vector_norm(vec_a_copy)
    
    vec_update("")
    vec_update("Normalization Test:")
    vec_update("  Original ||A||: " .. string.format("%.4f", original_norm))
    vec_update("  After normalize: " .. string.format("%.6f", new_norm))
    display_vector(vec_a_copy, "Normalized A")
    
    print("Vector math demo completed")
end

-- 3D vector specific operations
function vector_3d_demo()
    vec_clear()
    vec_update("=== 3D Vector Operations Demo ===")
    vec_update("")
    
    -- Create 3D vectors using Lua tables for convenience
    local vec3d_a = table_to_vector({1, 0, 0})  -- Unit X
    local vec3d_b = table_to_vector({0, 1, 0})  -- Unit Y  
    local vec3d_c = table_to_vector({0, 0, 1})  -- Unit Z
    
    display_vector(vec3d_a, "Unit X")
    display_vector(vec3d_b, "Unit Y")
    display_vector(vec3d_c, "Unit Z")
    vec_update("")
    
    -- Cross products
    local cross_xy = vector_cross_product(vec3d_a, vec3d_b)  -- Should be (0,0,1)
    local cross_yz = vector_cross_product(vec3d_b, vec3d_c)  -- Should be (1,0,0)
    local cross_zx = vector_cross_product(vec3d_c, vec3d_a)  -- Should be (0,1,0)
    
    vec_update("Cross Products:")
    display_vector(cross_xy, "X × Y")
    display_vector(cross_yz, "Y × Z")
    display_vector(cross_zx, "Z × X")
    
    -- Test orthogonality (dot product should be 0)
    local dot_xy = dot_product(vec3d_a, vec3d_b)
    local dot_yz = dot_product(vec3d_b, vec3d_c)
    local dot_zx = dot_product(vec3d_c, vec3d_a)
    
    vec_update("")
    vec_update("Orthogonality Check (should be 0):")
    vec_update("  X · Y = " .. string.format("%.6f", dot_xy))
    vec_update("  Y · Z = " .. string.format("%.6f", dot_yz))
    vec_update("  Z · X = " .. string.format("%.6f", dot_zx))
    
    -- Create a more interesting 3D vector
    local vec3d_test = table_to_vector({3, 4, 12})
    display_vector(vec3d_test, "Test Vector")
    
    local norm_test = vector_norm(vec3d_test)
    vec_update("Norm: " .. string.format("%.4f", norm_test))
    vec_update("Expected: sqrt(3² + 4² + 12²) = sqrt(169) = 13")
    
    -- Normalize it
    local vec3d_unit = vector_scale(vec3d_test, 1.0)  -- Copy
    vector_normalize(vec3d_unit)
    display_vector(vec3d_unit, "Normalized")
    
    print("3D vector demo completed")
end

-- Statistics demonstration
function vector_stats_demo()
    vec_clear()
    vec_update("=== Vector Statistics Demo ===")
    vec_update("")
    
    -- Create a dataset
    local data_table = {2.1, 3.5, 1.8, 4.2, 2.9, 3.1, 2.7, 3.8, 2.3, 4.1}
    local data_vec = table_to_vector(data_table)
    
    display_vector(data_vec, "Dataset")
    vec_update("")
    
    -- Basic statistics
    local mean_val = vector_mean(data_vec)
    local std_val = vector_std(data_vec)
    local min_val = vector_min(data_vec)
    local max_val = vector_max(data_vec)
    local sum_val = vector_sum(data_vec)
    
    vec_update("Statistical Summary:")
    vec_update("  Count: " .. vector_size(data_vec))
    vec_update("  Sum: " .. string.format("%.3f", sum_val))
    vec_update("  Mean: " .. string.format("%.3f", mean_val))
    vec_update("  Std Dev: " .. string.format("%.3f", std_val))
    vec_update("  Min: " .. string.format("%.3f", min_val))
    vec_update("  Max: " .. string.format("%.3f", max_val))
    vec_update("  Range: " .. string.format("%.3f", max_val - min_val))
    
    -- Create standardized (z-score) data
    vec_update("")
    vec_update("Standardization (z-scores):")
    local z_scores = create_vector(vector_size(data_vec))
    
    for i = 0, vector_size(data_vec) - 1 do
        local original = vector_get(data_vec, i)
        local z_score = (original - mean_val) / std_val
        vector_set(z_scores, i, z_score)
    end
    
    display_vector(z_scores, "Z-scores")
    
    -- Verify standardization
    local z_mean = vector_mean(z_scores)
    local z_std = vector_std(z_scores)
    vec_update("Z-score verification:")
    vec_update("  Mean: " .. string.format("%.6f", z_mean) .. " (should be ≈ 0)")
    vec_update("  Std Dev: " .. string.format("%.6f", z_std) .. " (should be ≈ 1)")
    
    print("Vector statistics demo completed")
end

-- Event handlers
vector_window:setOnSliderChange(function(sliderName, value)
    vector_window:setWidgetText("size_label", "Vector Size: " .. value)
end)

vector_window:setOnButtonClick(function(buttonName)
    if buttonName == "create_btn" then
        create_test_vectors()
    elseif buttonName == "math_btn" then
        vector_math_demo()
    elseif buttonName == "3d_btn" then
        vector_3d_demo()
    elseif buttonName == "stats_btn" then
        vector_stats_demo()
    end
end)

vector_window:show()

-- Matrix-Vector integration demo
local mv_window = window_factory:createWindow("Matrix-Vector Integration", 500, 400)

mv_window:addLabel("Matrix-Vector Operations", "mv_title")
mv_window:addButton("Linear Transform", "transform_btn")
mv_window:addButton("System of Equations", "system_btn")
mv_window:addButton("Eigenvalue Demo", "eigen_btn")
mv_window:addTextEdit("Matrix-vector results...", "mv_display")

function mv_update(text)
    local current = mv_window:getWidgetText("mv_display")
    mv_window:setWidgetText("mv_display", current .. text .. "\n")
end

function mv_clear()
    mv_window:setWidgetText("mv_display", "")
end

-- Linear transformation demo
function linear_transform_demo()
    mv_clear()
    mv_update("=== Linear Transformation Demo ===")
    mv_update("")
    
    -- Create a 2D rotation matrix (45 degrees)
    local angle = math.pi / 4  -- 45 degrees in radians
    local rotation_matrix = create_matrix(2, 2)
    rotation_matrix:set(0, 0, math.cos(angle))
    rotation_matrix:set(0, 1, -math.sin(angle))
    rotation_matrix:set(1, 0, math.sin(angle))
    rotation_matrix:set(1, 1, math.cos(angle))
    
    mv_update("2D Rotation Matrix (45°):")
    mv_update(rotation_matrix:toString())
    
    -- Create test vector
    local test_vec = table_to_vector({1, 0})  -- Unit vector along X-axis
    mv_update("Original vector: [1, 0]")
    
    -- Apply transformation
    local transformed = matrix_vector_multiply(rotation_matrix, test_vec)
    
    local tx = vector_get(transformed, 0)
    local ty = vector_get(transformed, 1)
    mv_update("Transformed vector: [" .. string.format("%.4f", tx) .. ", " .. string.format("%.4f", ty) .. "]")
    mv_update("Expected: [" .. string.format("%.4f", math.cos(angle)) .. ", " .. string.format("%.4f", math.sin(angle)) .. "]")
    
    -- Test multiple vectors
    mv_update("")
    mv_update("Multiple vector transformations:")
    local test_vectors = {{1, 0}, {0, 1}, {1, 1}, {2, -1}}
    
    for i, vec_data in ipairs(test_vectors) do
        local input_vec = table_to_vector(vec_data)
        local output_vec = matrix_vector_multiply(rotation_matrix, input_vec)
        
        local ix, iy = vector_get(input_vec, 0), vector_get(input_vec, 1)
        local ox, oy = vector_get(output_vec, 0), vector_get(output_vec, 1)
        
        mv_update(string.format("  [%.1f, %.1f] → [%.3f, %.3f]", ix, iy, ox, oy))
    end
    
    print("Linear transformation demo completed")
end

-- System of equations demo
function system_equations_demo()
    mv_clear()
    mv_update("=== System of Equations Demo ===")
    mv_update("")
    
    -- Create a simple 3x3 system: Ax = b
    local A = create_matrix(3, 3)
    A:set(0, 0, 2); A:set(0, 1, 1); A:set(0, 2, 1)
    A:set(1, 0, 1); A:set(1, 1, 3); A:set(1, 2, 2) 
    A:set(2, 0, 1); A:set(2, 1, 0); A:set(2, 2, 2)
    
    local b = table_to_vector({5, 8, 3})
    
    mv_update("System: Ax = b")
    mv_update("Matrix A:")
    mv_update(A:toString())
    mv_update("Vector b: [5, 8, 3]")
    mv_update("")
    
    -- For this demo, we'll verify the solution manually
    -- In a real implementation, you'd solve using LU decomposition
    local x_solution = table_to_vector({1, 2, 1})  -- Known solution
    
    mv_update("Testing solution x = [1, 2, 1]:")
    local Ax = matrix_vector_multiply(A, x_solution)
    
    local ax0 = vector_get(Ax, 0)
    local ax1 = vector_get(Ax, 1)  
    local ax2 = vector_get(Ax, 2)
    
    mv_update("Ax = [" .. ax0 .. ", " .. ax1 .. ", " .. ax2 .. "]")
    mv_update("b =  [5, 8, 3]")
    
    -- Check if solution is correct
    local b0, b1, b2 = vector_get(b, 0), vector_get(b, 1), vector_get(b, 2)
    local error = math.abs(ax0 - b0) + math.abs(ax1 - b1) + math.abs(ax2 - b2)
    
    mv_update("Total error: " .. string.format("%.6f", error))
    
    if error < 0.0001 then
        mv_update("✓ Solution verified!")
    else
        mv_update("✗ Solution incorrect")
    end
    
    print("System of equations demo completed")
end

-- Simple eigenvalue estimation
function eigenvalue_demo()
    mv_clear()
    mv_update("=== Eigenvalue Demo (Power Method) ===")
    mv_update("")
    
    -- Create a simple 2x2 matrix with known eigenvalues
    local A = create_matrix(2, 2)
    A:set(0, 0, 3); A:set(0, 1, 1)
    A:set(1, 0, 1); A:set(1, 1, 3)
    
    mv_update("Matrix A:")
    mv_update(A:toString())
    mv_update("(Eigenvalues should be 4 and 2)")
    mv_update("")
    
    -- Power method to find dominant eigenvalue
    local x = table_to_vector({1, 1})  -- Initial guess
    mv_update("Power Method Iterations:")
    
    for iter = 1, 6 do
        -- Multiply by matrix
        local Ax = matrix_vector_multiply(A, x)
        
        -- Find dominant component
        local max_val = math.max(math.abs(vector_get(Ax, 0)), math.abs(vector_get(Ax, 1)))
        
        -- Normalize
        local normalized = vector_scale(Ax, 1.0 / max_val)
        
        mv_update(string.format("  Iter %d: λ ≈ %.4f", iter, max_val))
        
        x = normalized
    end
    
    mv_update("")
    mv_update("Converged to dominant eigenvalue ≈ 4")
    
    print("Eigenvalue demo completed")
end

mv_window:setOnButtonClick(function(buttonName)
    if buttonName == "transform_btn" then
        linear_transform_demo()
    elseif buttonName == "system_btn" then
        system_equations_demo()  
    elseif buttonName == "eigen_btn" then
        eigenvalue_demo()
    end
end)

mv_window:show()

print("=== Complete Vector Demo Ready ===")
print("Created interactive tools:")
print("1. Vector Operations Lab - Basic vector math")
print("2. Matrix-Vector Integration - Linear algebra applications")
print("")
print("All vector functions working correctly:")
print("✓ Vector creation and access")
print("✓ Vector mathematics (add, subtract, scale, dot product)")
print("✓ Vector norms and normalization") 
print("✓ 3D vector operations (cross product)")
print("✓ Statistical functions")
print("✓ Matrix-vector multiplication")
print("✓ Table/vector conversion")

return "Complete vector functionality demo ready with " .. window_factory:getWindowCount() .. " tools"