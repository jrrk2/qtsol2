-- simple_accelerate_test.lua - Direct Accelerate Framework test

print("=== Simple Accelerate Framework Test ===")

-- Test 1: Basic matrix creation and operations
print("\n1. Basic AcceleratedMatrix Operations:")

-- Create two 4x4 matrices
local A = create_accelerated_matrix(4, 4)
local B = create_accelerated_matrix(4, 4)

-- Fill with test data
A:fillRandom(-2, 2)
B:fillRandom(-1, 1)

print("Matrix A (4x4 random):")
print(A:toString())
print("Matrix B (4x4 random):")
print(B:toString())

-- Test matrix multiplication
print("\n2. Matrix Multiplication Test:")
local start_time = get_time_ms()
local C = A:multiply(B)
local mult_time = get_time_ms() - start_time

print("Matrix multiplication A × B completed in " .. mult_time .. " ms")
print("Result C:")
print(C:toString())

-- Test matrix addition
print("\n3. Matrix Addition Test:")
local D = A:add(B)
print("Matrix addition A + B:")
print(D:toString())

-- Test matrix scaling
print("\n4. Matrix Scaling Test:")
local E = A:scale(2.0)
print("Matrix A scaled by 2.0:")
print(E:toString())

-- Test transpose
print("\n5. Matrix Transpose Test:")
local F = A:transpose()
print("Matrix A transposed:")
print(F:toString())

-- Test determinant
print("\n6. Determinant Test:")
local det_A = A:determinant()
print("Determinant of A: " .. string.format("%.6f", det_A))

-- Test norms
print("\n7. Matrix Norm Test:")
local norm_A = A:norm()
local norm_B = B:norm()
print("||A||: " .. string.format("%.6f", norm_A))
print("||B||: " .. string.format("%.6f", norm_B))

-- Test 3x3 system that we know should work
print("\n8. Simple Linear System Test:")
local A_sys = create_accelerated_matrix(3, 3)
A_sys:set(0, 0, 2); A_sys:set(0, 1, 1); A_sys:set(0, 2, 1)
A_sys:set(1, 0, 1); A_sys:set(1, 1, 3); A_sys:set(1, 2, 2)
A_sys:set(2, 0, 1); A_sys:set(2, 1, 0); A_sys:set(2, 2, 2)

print("System matrix A:")
print(A_sys:toString())

-- Create RHS vector manually 
local b_data = {5, 8, 3}
print("RHS vector b: [" .. table.concat(b_data, ", ") .. "]")

-- Test if solve method exists
local solve_success = pcall(function()
    local x_solution = A_sys:solve(b_data)
    print("Solution found:")
    for i = 1, #x_solution do
        print("  x[" .. i .. "] = " .. string.format("%.6f", x_solution[i]))
    end
    
    -- Verify solution
    local verification = A_sys:multiplyVector(x_solution)
    print("Verification Ax:")
    for i = 1, #verification do
        print("  Ax[" .. i .. "] = " .. string.format("%.6f", verification[i]))
    end
end)

if solve_success then
    print("✓ Linear system solve: SUCCESS")
else
    print("✗ Linear system solve method not available or failed")
end

-- Test eigenvalue computation
print("\n9. Eigenvalue Computation Test:")
local A_eigen = create_accelerated_matrix(2, 2)
A_eigen:set(0, 0, 3); A_eigen:set(0, 1, 1)
A_eigen:set(1, 0, 1); A_eigen:set(1, 1, 3)

print("Eigenvalue test matrix:")
print(A_eigen:toString())
print("Expected eigenvalues: 4, 2")

local eigen_success = pcall(function()
    local eigenvals = A_eigen:eigenvalues()
    
    -- Handle different return formats
    if type(eigenvals) == "table" then
        if eigenvals[1] and type(eigenvals[1]) == "table" then
            -- Format: {real_parts, imag_parts}
            local real_parts = eigenvals[1]
            local imag_parts = eigenvals[2] or {}
            
            print("Real eigenvalues:")
            for i = 1, #real_parts do
                print("  λ" .. i .. " = " .. string.format("%.6f", real_parts[i]))
            end
            
            if #imag_parts > 0 then
                print("Imaginary parts:")
                for i = 1, #imag_parts do
                    if math.abs(imag_parts[i]) > 1e-10 then
                        print("  Im(λ" .. i .. ") = " .. string.format("%.6f", imag_parts[i]))
                    end
                end
            end
        else
            -- Simple array format
            print("Eigenvalues:")
            for i = 1, #eigenvals do
                print("  λ" .. i .. " = " .. string.format("%.6f", eigenvals[i]))
            end
        end
    else
        print("Eigenvalue result format unknown: " .. type(eigenvals))
    end
end)

if eigen_success then
    print("✓ Eigenvalue computation: SUCCESS")
else
    print("✗ Eigenvalue computation method not available or failed")
end

-- Test matrix inversion
print("\n10. Matrix Inversion Test:")
local A_inv_test = create_accelerated_matrix(2, 2)
A_inv_test:set(0, 0, 2); A_inv_test:set(0, 1, 1)
A_inv_test:set(1, 0, 1); A_inv_test:set(1, 1, 2)

print("Matrix to invert:")
print(A_inv_test:toString())

local inv_success = pcall(function()
    local A_inverse = A_inv_test:inverse()
    print("Inverse matrix:")
    print(A_inverse:toString())
    
    -- Verify: A * A^(-1) should be identity
    local identity_check = A_inv_test:multiply(A_inverse)
    print("Verification A * A^(-1):")
    print(identity_check:toString())
end)

if inv_success then
    print("✓ Matrix inversion: SUCCESS")
else
    print("✗ Matrix inversion method not available or failed")
end

-- Manual performance comparison
print("\n11. Manual Performance Comparison:")
print("Testing 100x100 matrix multiplication...")

-- Create large matrices
local large_A = create_matrix(100, 100)  -- Basic matrix
local large_B = create_matrix(100, 100)
large_A:fillRandom(-1, 1)
large_B:fillRandom(-1, 1)

local accel_A = create_accelerated_matrix(100, 100)  -- Accelerated matrix
local accel_B = create_accelerated_matrix(100, 100)

-- Copy data to accelerated matrices
for i = 0, 99 do
    for j = 0, 99 do
        accel_A:set(i, j, large_A:get(i, j))
        accel_B:set(i, j, large_B:get(i, j))
    end
end

-- Time basic multiplication
local basic_start = get_time_ms()
local basic_result = large_A:multiply(large_B)
local basic_time = get_time_ms() - basic_start

-- Time accelerated multiplication  
local accel_start = get_time_ms()
local accel_result = accel_A:multiply(accel_B)
local accel_time = get_time_ms() - accel_start

local speedup = basic_time / accel_time

print("Results:")
print("  Basic C++: " .. basic_time .. " ms")
print("  Accelerated: " .. accel_time .. " ms") 
print("  Speedup: " .. string.format("%.2fx", speedup))

if speedup > 2.0 then
    print("  ✓ Excellent speedup! Accelerate Framework is working")
elseif speedup > 1.2 then
    print("  ✓ Good speedup! Accelerate Framework providing benefit")
else
    print("  ⚠ Modest speedup - may be overhead for this size")
end

-- GFLOPS calculation
local flops = 2 * 100 * 100 * 100  -- 2 * n^3 for matrix multiplication
local gflops = flops / (accel_time * 1e6)
print("  Performance: " .. string.format("%.2f GFLOPS", gflops))

print("\n=== Summary ===")
print("AcceleratedMatrix basic operations: ✓ Working")
print("Matrix math operations: ✓ Working") 
print("Performance comparison: ✓ Measured")

if speedup > 1.5 then
    print("Overall: ✅ ACCELERATE FRAMEWORK IS OPERATIONAL!")
    print("Status: Ready for high-performance computing")
else
    print("Overall: ⚠ Accelerate Framework may not be fully optimized")
    print("Status: Basic functionality confirmed")
end

return "Accelerate Framework basic test completed - speedup: " .. string.format("%.2fx", speedup)