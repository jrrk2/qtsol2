-- matrix_demo.lua - Comprehensive matrix processing demonstration
-- Shows current capabilities and performance comparisons

print("=== Matrix Processing Acceleration Demo ===")

-- Create matrix performance testing window
local matrix_window = window_factory:createWindow("Matrix Performance Lab", 600, 500)

matrix_window:addLabel("Matrix Performance Laboratory", "title")
matrix_window:addLabel("Matrix Size:", "size_label")
matrix_window:addSlider(2, 20, 5, "size_slider")
matrix_window:addButton("Create Random Matrices", "create_btn")
matrix_window:addButton("Multiply Matrices", "multiply_btn")
matrix_window:addButton("Performance Test", "perf_btn")
matrix_window:addButton("Linear Algebra Demo", "algebra_btn")
matrix_window:addTextEdit("Matrix operations will appear here...", "results_display")

-- Global matrices for operations
local matrix_a = nil
local matrix_b = nil
local result_matrix = nil

-- Function to update display with matrix info
function update_display(text)
    local current = matrix_window:getWidgetText("results_display")
    matrix_window:setWidgetText("results_display", current .. text .. "\n")
end

-- Function to create test matrices
function create_test_matrices()
    local size = matrix_window:getSliderValue("size_slider")
    matrix_window:setWidgetText("size_label", "Matrix Size: " .. size .. "x" .. size)
    
    update_display("=== Creating " .. size .. "x" .. size .. " Matrices ===")
    
    -- Create random matrices
    matrix_a = create_random_matrix(size, size, -10, 10)
    matrix_b = create_random_matrix(size, size, -10, 10)
    
    update_display("Matrix A:")
    update_display(matrix_a:toString())
    update_display("Matrix B:")
    update_display(matrix_b:toString())
    
    print("Created " .. size .. "x" .. size .. " random matrices")
end

-- Function to perform matrix multiplication
function multiply_matrices()
    if not matrix_a or not matrix_b then
        update_display("Error: Create matrices first!")
        return
    end
    
    update_display("=== Matrix Multiplication: A × B ===")
    
    local start_time = get_time_ms()
    result_matrix = matrix_a:multiply(matrix_b)
    local end_time = get_time_ms()
    
    local elapsed = end_time - start_time
    
    update_display("Result Matrix (A × B):")
    update_display(result_matrix:toString())
    update_display("Multiplication completed in " .. elapsed .. " ms")
    
    print("Matrix multiplication completed in " .. elapsed .. " ms")
end

-- Performance comparison function
function performance_test()
    update_display("=== Performance Test: Matrix Sizes 2x2 to 10x10 ===")
    
    local sizes = {2, 3, 4, 5, 6, 7, 8, 9, 10}
    local times = {}
    
    for _, size in ipairs(sizes) do
        local a = create_random_matrix(size, size, -5, 5)
        local b = create_random_matrix(size, size, -5, 5)
        
        local start_time = get_time_ms()
        local result = a:multiply(b)
        local end_time = get_time_ms()
        
        local elapsed = end_time - start_time
        times[size] = elapsed
        
        update_display(string.format("%dx%d: %d ms", size, size, elapsed))
    end
    
    -- Find the inflection point where performance starts to matter
    local slow_threshold = 10  -- ms
    for size, time in pairs(times) do
        if time > slow_threshold then
            update_display("Performance note: " .. size .. "x" .. size .. 
                          " and larger matrices may benefit from BLAS acceleration")
            break
        end
    end
    
    print("Performance test completed")
end

-- Advanced linear algebra demonstrations
function linear_algebra_demo()
    update_display("=== Advanced Linear Algebra Operations ===")
    
    -- Create a 3x3 test matrix
    local test_matrix = create_matrix(3, 3)
    test_matrix:set(0, 0, 4)  -- Row 0, Col 0
    test_matrix:set(0, 1, 2)
    test_matrix:set(0, 2, 1)
    test_matrix:set(1, 0, 2)
    test_matrix:set(1, 1, 3)
    test_matrix:set(1, 2, 1)
    test_matrix:set(2, 0, 1)
    test_matrix:set(2, 1, 1)
    test_matrix:set(2, 2, 2)
    
    update_display("Test Matrix:")
    update_display(test_matrix:toString())
    
    -- Transpose
    local transposed = test_matrix:transpose()
    update_display("Transpose:")
    update_display(transposed:toString())
    
    -- Determinant
    local det = test_matrix:determinant()
    update_display("Determinant: " .. string.format("%.6f", det))
    
    -- Scalar multiplication
    local scaled = test_matrix:scale(2.5)
    update_display("Scaled by 2.5:")
    update_display(scaled:toString())
    
    -- Identity matrix
    local identity = create_identity(3)
    update_display("3x3 Identity Matrix:")
    update_display(identity:toString())
    
    -- Matrix addition
    local sum_matrix = test_matrix:add(identity)
    update_display("Matrix + Identity:")
    update_display(sum_matrix:toString())
    
    -- Vector operations demonstration
    update_display("\n=== Vector Operations ===")
    
    -- Create test vectors
    local vec_a = {3, 4, 5}
    local vec_b = {1, 2, 2}
    
    update_display("Vector A: [" .. table.concat(vec_a, ", ") .. "]")
    update_display("Vector B: [" .. table.concat(vec_b, ", ") .. "]")
    
    -- Dot product
    local dot_prod = dot_product(vec_a, vec_b)
    update_display("Dot Product A·B: " .. string.format("%.3f", dot_prod))
    
    -- Vector norms
    local norm_a = vector_norm(vec_a)
    local norm_b = vector_norm(vec_b)
    update_display("||A||: " .. string.format("%.3f", norm_a))
    update_display("||B||: " .. string.format("%.3f", norm_b))
    
    -- Normalize vectors
    local vec_a_copy = {vec_a[1], vec_a[2], vec_a[3]}  -- Copy
    vector_normalize(vec_a_copy)
    update_display("Normalized A: [" .. string.format("%.3f, %.3f, %.3f", 
                   vec_a_copy[1], vec_a_copy[2], vec_a_copy[3]) .. "]")
    
    print("Linear algebra demonstration completed")
end

-- Matrix visualization function
function visualize_matrix(matrix, title)
    if not matrix then return end
    
    local viz_text = "=== " .. title .. " ===\n"
    
    -- Create a visual representation using characters
    local rows = matrix:getRows()
    local cols = matrix:getCols()
    
    -- Find min/max values for scaling
    local min_val, max_val = math.huge, -math.huge
    for i = 0, rows-1 do
        for j = 0, cols-1 do
            local val = matrix:get(i, j)
            min_val = math.min(min_val, val)
            max_val = math.max(max_val, val)
        end
    end
    
    -- Create character-based visualization
    local chars = " .:-=+*#%@"
    for i = 0, rows-1 do
        local line = ""
        for j = 0, cols-1 do
            local val = matrix:get(i, j)
            local normalized = (val - min_val) / (max_val - min_val)
            local char_index = math.floor(normalized * (#chars - 1)) + 1
            char_index = math.max(1, math.min(#chars, char_index))
            line = line .. string.sub(chars, char_index, char_index)
        end
        viz_text = viz_text .. line .. "\n"
    end
    
    viz_text = viz_text .. "Range: [" .. string.format("%.2f", min_val) .. 
               ", " .. string.format("%.2f", max_val) .. "]\n"
    
    update_display(viz_text)
end

-- Event handlers
matrix_window:setOnSliderChange(function(sliderName, value)
    matrix_window:setWidgetText("size_label", "Matrix Size: " .. value .. "x" .. value)
end)

matrix_window:setOnButtonClick(function(buttonName)
    if buttonName == "create_btn" then
        create_test_matrices()
        
    elseif buttonName == "multiply_btn" then
        multiply_matrices()
        
    elseif buttonName == "perf_btn" then
        performance_test()
        
    elseif buttonName == "algebra_btn" then
        linear_algebra_demo()
    end
end)

matrix_window:show()

-- Create scientific computing demonstration
local science_window = window_factory:createWindow("Scientific Computing", 550, 400)

science_window:addLabel("Scientific Computing Applications", "sci_title")
science_window:addButton("Solve Linear System", "solve_btn")
science_window:addButton("Eigenvalue Demo", "eigen_btn")
science_window:addButton("Least Squares", "lsq_btn")
science_window:addButton("Matrix Decomposition", "decomp_btn")
science_window:addTextEdit("Scientific computing results...", "sci_display")

function sci_update(text)
    local current = science_window:getWidgetText("sci_display")
    science_window:setWidgetText("sci_display", current .. text .. "\n")
end

-- Solve linear system Ax = b
function solve_linear_system()
    sci_update("=== Solving Linear System Ax = b ===")
    
    -- Create a well-conditioned 3x3 system
    local A = create_matrix(3, 3)
    A:set(0, 0, 2); A:set(0, 1, 1); A:set(0, 2, 1)
    A:set(1, 0, 1); A:set(1, 1, 3); A:set(1, 2, 2)
    A:set(2, 0, 1); A:set(2, 1, 0); A:set(2, 2, 2)
    
    sci_update("Matrix A:")
    sci_update(A:toString())
    
    -- Right-hand side vector (as 3x1 matrix)
    local b = create_matrix(3, 1)
    b:set(0, 0, 5)  -- b = [5, 8, 3]^T
    b:set(1, 0, 8)
    b:set(2, 0, 3)
    
    sci_update("Vector b:")
    sci_update(b:toString())
    
    -- Note: For actual solving, you'd need matrix inversion or LU decomposition
    -- This demonstrates the setup for numerical methods
    
    local det_A = A:determinant()
    sci_update("det(A) = " .. string.format("%.6f", det_A))
    
    if math.abs(det_A) > 1e-10 then
        sci_update("System is solvable (det ≠ 0)")
        sci_update("Note: Actual solution requires matrix inversion")
        sci_update("Consider integrating LAPACK for production use")
    else
        sci_update("System is singular (det ≈ 0)")
    end
    
    print("Linear system analysis completed")
end

-- Demonstrate iterative methods
function eigenvalue_demo()
    sci_update("=== Eigenvalue Estimation (Power Method) ===")
    
    -- Create a symmetric matrix for real eigenvalues
    local A = create_matrix(3, 3)
    A:set(0, 0, 4); A:set(0, 1, 2); A:set(0, 2, 1)
    A:set(1, 0, 2); A:set(1, 1, 3); A:set(1, 2, 0)
    A:set(2, 0, 1); A:set(2, 1, 0); A:set(2, 2, 2)
    
    sci_update("Matrix A:")
    sci_update(A:toString())
    
    -- Power method for dominant eigenvalue
    local x = create_matrix(3, 1)  -- Initial vector
    x:set(0, 0, 1)
    x:set(1, 0, 1)
    x:set(2, 0, 1)
    
    sci_update("Power Method Iterations:")
    
    for iter = 1, 5 do
        local Ax = A:multiply(x)
        
        -- Find the largest component for normalization
        local max_val = 0
        for i = 0, 2 do
            local val = math.abs(Ax:get(i, 0))
            if val > max_val then max_val = val end
        end
        
        -- Normalize
        local normalized = Ax:scale(1.0 / max_val)
        
        sci_update(string.format("Iteration %d: λ ≈ %.4f", iter, max_val))
        
        x = normalized
    end
    
    sci_update("Note: Use LAPACK dgeev() for accurate eigenvalues")
    print("Eigenvalue estimation completed")
end

-- Least squares demonstration
function least_squares_demo()
    sci_update("=== Least Squares Curve Fitting ===")
    
    -- Generate noisy data points for y = 2x + 1
    local data_points = {
        {0, 1.1}, {1, 2.9}, {2, 5.2}, {3, 6.8}, {4, 9.1}
    }
    
    sci_update("Data Points (x, y):")
    for i, point in ipairs(data_points) do
        sci_update(string.format("  (%.1f, %.1f)", point[1], point[2]))
    end
    
    -- Set up normal equations: A^T A x = A^T b
    -- For linear fit y = mx + c, A = [x_i, 1]
    
    local n = #data_points
    local AtA = create_matrix(2, 2)  -- [sum(x²), sum(x); sum(x), n]
    local Atb = create_matrix(2, 1)  -- [sum(xy), sum(y)]
    
    local sum_x, sum_y, sum_xx, sum_xy = 0, 0, 0, 0
    
    for _, point in ipairs(data_points) do
        local x, y = point[1], point[2]
        sum_x = sum_x + x
        sum_y = sum_y + y
        sum_xx = sum_xx + x * x
        sum_xy = sum_xy + x * y
    end
    
    AtA:set(0, 0, sum_xx); AtA:set(0, 1, sum_x)
    AtA:set(1, 0, sum_x);  AtA:set(1, 1, n)
    
    Atb:set(0, 0, sum_xy)
    Atb:set(1, 0, sum_y)
    
    sci_update("Normal equation matrices:")
    sci_update("A^T A:")
    sci_update(AtA:toString())
    sci_update("A^T b:")
    sci_update(Atb:toString())
    
    -- Calculate coefficients using Cramer's rule
    local det = AtA:determinant()
    if math.abs(det) > 1e-10 then
        local slope = (n * sum_xy - sum_x * sum_y) / det
        local intercept = (sum_xx * sum_y - sum_x * sum_xy) / det
        
        sci_update(string.format("Fitted line: y = %.3fx + %.3f", slope, intercept))
        
        -- Calculate R-squared
        local mean_y = sum_y / n
        local ss_tot, ss_res = 0, 0
        
        for _, point in ipairs(data_points) do
            local x, y = point[1], point[2]
            local predicted = slope * x + intercept
            ss_tot = ss_tot + (y - mean_y)^2
            ss_res = ss_res + (y - predicted)^2
        end
        
        local r_squared = 1 - (ss_res / ss_tot)
        sci_update(string.format("R² = %.4f", r_squared))
    else
        sci_update("Singular system - cannot solve")
    end
    
    print("Least squares fitting completed")
end

-- Matrix decomposition examples
function decomposition_demo()
    sci_update("=== Matrix Decomposition Examples ===")
    
    local A = create_matrix(3, 3)
    A:set(0, 0, 2); A:set(0, 1, 1); A:set(0, 2, 1)
    A:set(1, 0, 1); A:set(1, 1, 2); A:set(1, 2, 1)
    A:set(2, 0, 1); A:set(2, 1, 1); A:set(2, 2, 3)
    
    sci_update("Original Matrix A:")
    sci_update(A:toString())
    
    -- LU Decomposition simulation (conceptual)
    sci_update("LU Decomposition Concept:")
    sci_update("A = L × U (Lower × Upper triangular)")
    sci_update("Used for: Solving linear systems, matrix inversion")
    sci_update("Implementation requires pivoting for numerical stability")
    
    -- QR Decomposition concept
    sci_update("\nQR Decomposition Concept:")
    sci_update("A = Q × R (Orthogonal × Upper triangular)")
    sci_update("Used for: Least squares, eigenvalue algorithms")
    sci_update("Q has orthonormal columns, R is upper triangular")
    
    -- SVD concept
    sci_update("\nSingular Value Decomposition (SVD):")
    sci_update("A = U × Σ × V^T")
    sci_update("Most robust decomposition for numerical computation")
    sci_update("Used for: Pseudoinverse, principal component analysis")
    
    sci_update("\nNote: Production implementations require:")
    sci_update("- LAPACK for robust, optimized algorithms")
    sci_update("- Proper numerical pivoting and scaling")
    sci_update("- Error handling for ill-conditioned matrices")
    
    print("Decomposition concepts demonstrated")
end

science_window:setOnButtonClick(function(buttonName)
    if buttonName == "solve_btn" then
        solve_linear_system()
    elseif buttonName == "eigen_btn" then
        eigenvalue_demo()
    elseif buttonName == "lsq_btn" then
        least_squares_demo()
    elseif buttonName == "decomp_btn" then
        decomposition_demo()
    end
end)

science_window:show()

print("=== Matrix Processing System Ready ===")
print("Current capabilities:")
print("✓ Basic matrix operations (add, multiply, transpose)")
print("✓ Determinant calculation")
print("✓ Vector operations (dot product, norms)")
print("✓ Performance timing")
print("✓ Random matrix generation")
print("")
print("For high-performance computing, consider adding:")
print("• BLAS/LAPACK integration for large matrices")
print("• Eigen library for advanced algorithms")
print("• OpenMP for parallel processing")
print("• GPU acceleration with CUDA/OpenCL")
print("")
print("Performance threshold: ~10x10 matrices benefit from acceleration")
print("Current system suitable for educational and small-scale computation")

return "Matrix processing demonstration ready with " .. window_factory:getWindowCount() .. " tools"