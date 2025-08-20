-- fixed_system_solver.lua - Correct linear system solving

print("=== Fixed Linear System Solver ===")

-- Let's solve the system correctly using Gaussian elimination
function solve_system_gaussian(A, b)
    local n = A:getRows()
    
    -- Create augmented matrix [A|b]
    local augmented = create_matrix(n, n + 1)
    
    -- Copy A and b into augmented matrix
    for i = 0, n - 1 do
        for j = 0, n - 1 do
            augmented:set(i, j, A:get(i, j))
        end
        augmented:set(i, n, vector_get(b, i))  -- Last column is b
    end
    
    print("Augmented matrix [A|b]:")
    print(augmented:toString())
    
    -- Forward elimination
    for k = 0, n - 2 do  -- For each pivot row
        -- Find pivot (largest element in column k)
        local max_val = math.abs(augmented:get(k, k))
        local max_row = k
        
        for i = k + 1, n - 1 do
            local val = math.abs(augmented:get(i, k))
            if val > max_val then
                max_val = val
                max_row = i
            end
        end
        
        -- Swap rows if needed
        if max_row ~= k then
            for j = 0, n do
                local temp = augmented:get(k, j)
                augmented:set(k, j, augmented:get(max_row, j))
                augmented:set(max_row, j, temp)
            end
            print("Swapped rows " .. k .. " and " .. max_row)
        end
        
        -- Check for zero pivot
        if math.abs(augmented:get(k, k)) < 1e-12 then
            return nil, "Matrix is singular"
        end
        
        -- Eliminate column k in rows below pivot
        for i = k + 1, n - 1 do
            local factor = augmented:get(i, k) / augmented:get(k, k)
            
            for j = k, n do
                local new_val = augmented:get(i, j) - factor * augmented:get(k, j)
                augmented:set(i, j, new_val)
            end
        end
    end
    
    print("After forward elimination:")
    print(augmented:toString())
    
    -- Back substitution
    local solution = create_vector(n)
    
    for i = n - 1, 0, -1 do  -- From last row to first
        local sum = augmented:get(i, n)  -- Right-hand side
        
        for j = i + 1, n - 1 do
            sum = sum - augmented:get(i, j) * vector_get(solution, j)
        end
        
        local x_i = sum / augmented:get(i, i)
        vector_set(solution, i, x_i)
    end
    
    return solution, "Success"
end

-- Create the system from the demo
local A = create_matrix(3, 3)
A:set(0, 0, 2); A:set(0, 1, 1); A:set(0, 2, 1)
A:set(1, 0, 1); A:set(1, 1, 3); A:set(1, 2, 2)
A:set(2, 0, 1); A:set(2, 1, 0); A:set(2, 2, 2)

local b = table_to_vector({5, 8, 3})

print("\nOriginal system:")
print("Matrix A:")
print(A:toString())
print("Vector b: [5, 8, 3]")
print("")

-- Solve the system
local solution, status = solve_system_gaussian(A, b)

if solution then
    print("Solution found:")
    local x0 = vector_get(solution, 0)
    local x1 = vector_get(solution, 1)
    local x2 = vector_get(solution, 2)
    
    print(string.format("x = [%.6f, %.6f, %.6f]", x0, x1, x2))
    
    -- Verify the solution
    print("\nVerification:")
    local Ax = matrix_vector_multiply(A, solution)
    
    local ax0 = vector_get(Ax, 0)
    local ax1 = vector_get(Ax, 1)
    local ax2 = vector_get(Ax, 2)
    
    print("Ax = [" .. string.format("%.6f", ax0) .. ", " .. 
          string.format("%.6f", ax1) .. ", " .. string.format("%.6f", ax2) .. "]")
    print("b  = [5.000000, 8.000000, 3.000000]")
    
    -- Calculate residual
    local residual = 0
    residual = residual + (ax0 - 5)^2
    residual = residual + (ax1 - 8)^2  
    residual = residual + (ax2 - 3)^2
    residual = math.sqrt(residual)
    
    print("Residual ||Ax - b||: " .. string.format("%.2e", residual))
    
    if residual < 1e-10 then
        print("✓ Solution verified!")
    else
        print("Warning: Large residual")
    end
else
    print("Error: " .. status)
end

-- Let's also solve it manually to double-check
print("\n=== Manual Solution Check ===")
print("System of equations:")
print("2x₁ + 1x₂ + 1x₃ = 5")
print("1x₁ + 3x₂ + 2x₃ = 8") 
print("1x₁ + 0x₂ + 2x₃ = 3")
print("")

-- From the third equation: x₁ + 2x₃ = 3, so x₁ = 3 - 2x₃
-- Substitute into first equation: 2(3 - 2x₃) + x₂ + x₃ = 5
-- 6 - 4x₃ + x₂ + x₃ = 5
-- x₂ - 3x₃ = -1, so x₂ = 3x₃ - 1

-- Substitute into second equation: (3 - 2x₃) + 3(3x₃ - 1) + 2x₃ = 8
-- 3 - 2x₃ + 9x₃ - 3 + 2x₃ = 8
-- 9x₃ = 8
-- x₃ = 8/9

local x3_manual = 8/9
local x1_manual = 3 - 2 * x3_manual  -- = 3 - 16/9 = 11/9
local x2_manual = 3 * x3_manual - 1  -- = 24/9 - 1 = 15/9

print("Manual calculation:")
print(string.format("x₁ = %.6f", x1_manual))
print(string.format("x₂ = %.6f", x2_manual)) 
print(string.format("x₃ = %.6f", x3_manual))
print("")

-- Verify manual solution
print("Manual verification:")
local eq1 = 2*x1_manual + 1*x2_manual + 1*x3_manual
local eq2 = 1*x1_manual + 3*x2_manual + 2*x3_manual
local eq3 = 1*x1_manual + 0*x2_manual + 2*x3_manual

print(string.format("Equation 1: %.6f (should be 5)", eq1))
print(string.format("Equation 2: %.6f (should be 8)", eq2))
print(string.format("Equation 3: %.6f (should be 3)", eq3))

-- The original test solution [1, 2, 1] was indeed wrong!
print("\n=== Why [1, 2, 1] was wrong ===")
print("Testing x = [1, 2, 1]:")
print("Equation 1: 2(1) + 1(2) + 1(1) = 2 + 2 + 1 = 5 ✓")
print("Equation 2: 1(1) + 3(2) + 2(1) = 1 + 6 + 2 = 9 ≠ 8 ✗")  
print("Equation 3: 1(1) + 0(2) + 2(1) = 1 + 0 + 2 = 3 ✓")
print("")
print("The error was in equation 2: got 9 instead of 8")
print("That's why the total error was |5-5| + |9-8| + |3-3| = 1")

return "Linear system solved correctly!"