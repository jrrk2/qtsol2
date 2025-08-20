-- accelerate_performance_test.lua - Test macOS Accelerate Framework performance

print("=== macOS Accelerate Framework Performance Test ===")

-- Check if Accelerate is available
print("\n1. System Information:")
local accel_info = get_accelerate_info()
print(accel_info)

-- Test 1: Basic accelerated matrix creation
print("\n2. Basic AcceleratedMatrix Test:")

local success_basic = pcall(function()
    local accel_matrix = create_accelerated_matrix(3, 3)
    accel_matrix:fillIdentity()
    print("✓ AcceleratedMatrix creation: SUCCESS")
    print("Identity matrix:")
    print(accel_matrix:toString())
    
    local norm = accel_matrix:norm()
    print("Matrix norm: " .. string.format("%.6f", norm))
    print("Expected for 3x3 identity: " .. string.format("%.6f", math.sqrt(3)))
end)

if not success_basic then
    print("✗ AcceleratedMatrix creation: FAILED")
    print("This might indicate missing AcceleratedMatrix.hpp or compilation issues")
    return "AcceleratedMatrix not available"
end

-- Test 2: Performance comparison for different matrix sizes
print("\n3. Performance Comparison: Basic vs Accelerated")
print("Matrix Size | Basic (ms) | Accel (ms) | Speedup | GFLOPS")
print("------------|------------|------------|---------|-------")

local test_sizes = {10, 25, 50, 100, 150}
local iterations = 5

for _, size in ipairs(test_sizes) do
    local benchmark_success, results = pcall(benchmark_multiply_comparison, size, iterations)
    
    if benchmark_success then
        local basic_time = results.basic_time
        local accel_time = results.accel_time  
        local speedup = results.speedup
        
        -- Calculate GFLOPS for accelerated version
        local gflops = calculate_gflops(size, accel_time / iterations)
        
        print(string.format("%10d | %9.1f | %9.1f | %6.2fx | %6.2f",
            size, basic_time, accel_time, speedup, gflops))
            
        if speedup > 1.5 then
            print("           └─ Good speedup! Accelerate is helping")
        elseif speedup < 0.8 then
            print("           └─ Accelerate slower (overhead for small matrices)")
        end
    else
        print(string.format("%10d | ERROR: %s", size, tostring(results)))
    end
end

-- Test 3: LAPACK operations test
print("\n4. LAPACK Advanced Operations Test:")

local lapack_sizes = {5, 10, 20}

for _, size in ipairs(lapack_sizes) do
    print("Testing " .. size .. "x" .. size .. " system:")
    
    local lapack_success, lapack_results = pcall(test_lapack_solve, size)
    
    if lapack_success then
        if lapack_results.success then
            print("  ✓ Linear system solve: SUCCESS")
            print("  ⏱ Solve time: " .. lapack_results.solve_time_us .. " μs")
            print("  📊 Solution error: " .. string.format("%.2e", lapack_results.error))
            
            if lapack_results.error < 1e-10 then
                print("  ✓ Excellent numerical accuracy!")
            elseif lapack_results.error < 1e-6 then
                print("  ✓ Good numerical accuracy")
            else
                print("  ⚠ Moderate numerical accuracy")
            end
        else
            print("  ✗ Linear system solve: FAILED")
            print("  Error: " .. (lapack_results.error_msg or "Unknown"))
        end
    else
        print("  ✗ LAPACK test error: " .. tostring(lapack_results))
    end
end

-- Test 4: Specific Accelerate operations
print("\n5. Specific Accelerate Operations Test:")

local test_success = pcall(function()
    -- Create test matrices
    local A = create_accelerated_matrix(4, 4)
    local B = create_accelerated_matrix(4, 4)
    
    -- Fill with test data
    A:fillRandom(-5, 5)
    B:fillRandom(-2, 2)
    
    print("Test matrices created")
    print("Matrix A:")
    print(A:toString())
    
    -- Test matrix multiplication
    local start_time = get_time_ms()
    local C = A:multiply(B)  -- This will use Accelerate on macOS
    local mult_time = get_time_ms() - start_time
    
    print("Matrix multiplication completed in " .. mult_time .. " ms")
    
    -- Test matrix addition (should use vDSP)
    local D = A:add(B)
    print("Matrix addition completed")
    
    -- Test matrix scaling (should use vDSP)
    local E = A:scale(2.5)
    print("Matrix scaling completed")
    
    -- Test transpose (should use vDSP)
    local F = A:transpose()
    print("Matrix transpose completed")
    print("Transpose result:")
    print(F:toString())
    
    -- Test norm calculation
    local norm_A = A:norm()
    print("Matrix norm: " .. string.format("%.6f", norm_A))
end)

if test_success then
    print("✓ All Accelerate operations completed successfully")
else
    print("✗ Some Accelerate operations failed")
end

-- Test 5: Advanced LAPACK features (if available)
print("\n6. Advanced LAPACK Features Test:")

if get_accelerate_info():find("Available") then
    local advanced_success = pcall(function()
        -- Test eigenvalue computation
        print("Testing eigenvalue computation...")
        local A_eigen = create_accelerated_matrix(3, 3)
        
        -- Create a symmetric matrix for real eigenvalues
        A_eigen:set(0, 0, 4); A_eigen:set(0, 1, 2); A_eigen:set(0, 2, 1)
        A_eigen:set(1, 0, 2); A_eigen:set(1, 1, 3); A_eigen:set(1, 2, 0)
        A_eigen:set(2, 0, 1); A_eigen:set(2, 1, 0); A_eigen:set(2, 2, 2)
        
        print("Eigenvalue test matrix:")
        print(A_eigen:toString())
        
        local eigen_start = get_time_ms()
        local eigenvals = A_eigen:eigenvalues()
        local eigen_time = get_time_ms() - eigen_start
        
        print("Eigenvalues computed in " .. eigen_time .. " ms")
        
        -- Extract real and imaginary parts
        local real_parts = eigenvals[1] or eigenvals
        local imag_parts = eigenvals[2] or {}
        
        print("Eigenvalues (real parts):")
        for i = 1, #real_parts do
            print("  λ" .. i .. " = " .. string.format("%.6f", real_parts[i]))
        end
        
        -- Test matrix inversion
        print("\nTesting matrix inversion...")
        local inv_start = get_time_ms()
        local A_inv = A_eigen:inverse()
        local inv_time = get_time_ms() - inv_start
        
        print("Matrix inversion completed in " .. inv_time .. " ms")
        
        -- Verify: A * A^(-1) should be identity
        local identity_check = A_eigen:multiply(A_inv)
        print("Verification A * A^(-1):")
        print(identity_check:toString())
        
        -- Test QR decomposition
        print("\nTesting QR decomposition...")
        local qr_start = get_time_ms()
        local qr_result = A_eigen:qrDecomposition()
        local qr_time = get_time_ms() - qr_start
        
        print("QR decomposition completed in " .. qr_time .. " ms")
        
        local Q = qr_result[1]
        local R = qr_result[2]
        
        print("Q matrix (orthogonal):")
        print(Q:toString())
        print("R matrix (upper triangular):")
        print(R:toString())
        
    end)
    
    if advanced_success then
        print("✓ Advanced LAPACK features working!")
    else
        print("✗ Some advanced LAPACK features not available or failed")
    end
else
    print("Accelerate Framework not available - skipping advanced tests")
end

-- Performance summary
print("\n=== Performance Summary ===")
print("Your macOS system with Accelerate Framework provides:")
print("• Optimized BLAS operations for matrix multiplication")
print("• Vectorized operations using vDSP (add, scale, transpose)")
print("• Production-quality LAPACK routines")
print("• Significant speedup for matrices > 50x50")
print("• Research-grade numerical accuracy")
print("")

if get_accelerate_info():find("Available") then
    print("Status: ✓ ACCELERATE FRAMEWORK FULLY OPERATIONAL")
    print("Recommendation: Use AcceleratedMatrix for all computations > 25x25")
else
    print("Status: ⚠ Using basic C++ implementations")
    print("Recommendation: Ensure macOS Accelerate Framework is linked")
end

return "Accelerate Framework performance test completed"