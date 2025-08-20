-- generic_table_demo.lua - Demonstrate the versatile data table widget

print("=== Generic Data Table Widget Demo ===")

-- Test 1: Display matrices
print("\n1. Matrix Display Test:")

-- Create a test matrix
local test_matrix = create_accelerated_matrix(4, 4)
test_matrix:fillRandom(-5, 5)

print("Displaying 4x4 random matrix...")
show_accelerated_matrix(test_matrix, "Random 4x4 Matrix")

-- Create identity matrix
local identity = create_accelerated_identity(3)
print("Displaying 3x3 identity matrix...")
show_accelerated_matrix(identity, "3x3 Identity Matrix")

-- Test 2: Display vectors
print("\n2. Vector Display Test:")

-- Create test vector
local test_vector = table_to_vector({1.5, 2.8, -3.2, 4.7, -1.1})
print("Displaying test vector...")
show_vector(test_vector, "Test Vector")

-- Test 3: Display eigenvalue results
print("\n3. Eigenvalue Results Display:")

local eigen_matrix = create_accelerated_matrix(3, 3)
eigen_matrix:set(0, 0, 4); eigen_matrix:set(0, 1, 2); eigen_matrix:set(0, 2, 1)
eigen_matrix:set(1, 0, 2); eigen_matrix:set(1, 1, 5); eigen_matrix:set(1, 2, 3)
eigen_matrix:set(2, 0, 1); eigen_matrix:set(2, 1, 3); eigen_matrix:set(2, 2, 6)

local eigenvals = eigen_matrix:eigenvalues()
print("Displaying eigenvalue results...")
show_data(eigenvals, "Eigenvalue Results")

-- Test 4: Display performance data
print("\n4. Performance Data Display:")

-- Create performance benchmark data
local perf_data = {
    {size = "50x50", basic_time = 125.3, accel_time = 4.2, speedup = 29.8, gflops = 59.5},
    {size = "100x100", basic_time = 892.1, accel_time = 15.6, speedup = 57.2, gflops = 128.2},
    {size = "200x200", basic_time = 7234.5, accel_time = 62.8, speedup = 115.2, gflops = 254.8},
    {size = "300x300", basic_time = 24567.2, accel_time = 150.3, speedup = 163.5, gflops = 359.4}
}

print("Displaying performance benchmark data...")
show_data(perf_data, "Matrix Multiplication Performance")

-- Test 5: Display nested table structures
print("\n5. Nested Table Structures:")

local nested_data = {
    experiment = {
        name = "Linear Algebra Benchmark",
        date = "2025-08-20",
        parameters = {
            iterations = 100,
            precision = "double",
            algorithm = "BLAS DGEMM"
        }
    },
    results = {
        {matrix_size = 50, time_us = 45.2, memory_mb = 0.02},
        {matrix_size = 100, time_us = 156.8, memory_mb = 0.08},
        {matrix_size = 200, time_us = 625.4, memory_mb = 0.32}
    },
    statistics = {
        mean_speedup = 89.5,
        max_gflops = 359.4,
        efficiency = 0.85
    }
}

print("Displaying nested experimental data...")
show_data(nested_data, "Experimental Results")

-- Test 6: Display arrays with different data types
print("\n6. Mixed Data Type Arrays:")

local mixed_array = {
    "String value",
    42.7,
    true,
    {nested = "table"},
    math.pi,
    "Another string",
    false,
    1.23e-15
}

print("Displaying mixed data type array...")
show_data(mixed_array, "Mixed Data Types")

-- Test 7: Display mathematical results
print("\n7. Mathematical Results Display:")

-- Create QR decomposition results
local qr_matrix = create_accelerated_matrix(3, 3)
qr_matrix:set(0, 0, 1); qr_matrix:set(0, 1, 2); qr_matrix:set(0, 2, 3)
qr_matrix:set(1, 0, 4); qr_matrix:set(1, 1, 5); qr_matrix:set(1, 2, 6)
qr_matrix:set(2, 0, 7); qr_matrix:set(2, 1, 8); qr_matrix:set(2, 2, 10)

local qr_result = qr_matrix:qrDecomposition()
if qr_result and qr_result.Q then
    print("Displaying Q matrix from QR decomposition...")
    show_accelerated_matrix(qr_result.Q, "Q Matrix (QR Decomposition)")
    
    print("Displaying R matrix from QR decomposition...")
    show_accelerated_matrix(qr_result.R, "R Matrix (QR Decomposition)")
end

-- Test 8: Display statistical analysis
print("\n8. Statistical Analysis Display:")

local stats_data = {
    dataset = "Random Matrix Performance",
    sample_size = 1000,
    measurements = {
        mean = 156.78,
        median = 142.35,
        std_dev = 23.45,
        min_value = 98.21,
        max_value = 287.65,
        percentiles = {
            p25 = 134.56,
            p50 = 142.35,
            p75 = 178.92,
            p90 = 201.45,
            p95 = 234.78,
            p99 = 275.32
        }
    },
    distribution = {
        skewness = 0.234,
        kurtosis = -0.567,
        normality_test = "Anderson-Darling",
        p_value = 0.0234
    }
}

print("Displaying statistical analysis...")
show_data(stats_data, "Statistical Analysis")

-- Test 9: Display comparison tables
print("\n9. Algorithm Comparison Display:")

local algo_comparison = {
    {
        algorithm = "Basic C++",
        complexity = "O(n³)",
        memory = "3n² doubles",
        parallelism = "None",
        performance_50 = 2.1,
        performance_100 = 16.8,
        performance_200 = 134.5
    },
    {
        algorithm = "Accelerate BLAS",
        complexity = "O(n³)",
        memory = "3n² doubles + workspace",
        parallelism = "Multi-threaded",
        performance_50 = 58.9,
        performance_100 = 235.6,
        performance_200 = 942.8
    },
    {
        algorithm = "Theoretical Peak",
        complexity = "O(n³)",
        memory = "Optimal",
        parallelism = "Perfect",
        performance_50 = 80.0,
        performance_100 = 320.0,
        performance_200 = 1280.0
    }
}

print("Displaying algorithm comparison...")
show_data(algo_comparison, "Algorithm Performance Comparison")

-- Test 10: Display scientific notation and precision formatting
print("\n10. Scientific Data Display:")

local scientific_data = {
    physical_constants = {
        speed_of_light = 2.99792458e8,
        planck_constant = 6.62607015e-34,
        elementary_charge = 1.602176634e-19,
        avogadro_number = 6.02214076e23
    },
    measurements = {
        {quantity = "Force", value = 1.23e-12, unit = "N", uncertainty = 2.1e-14},
        {quantity = "Energy", value = 4.56e-18, unit = "J", uncertainty = 1.2e-19},
        {quantity = "Time", value = 7.89e-9, unit = "s", uncertainty = 3.4e-11}
    },
    errors = {
        absolute_error = 2.34e-15,
        relative_error = 1.23e-13,
        systematic_error = 5.67e-14,
        statistical_error = 8.91e-15
    }
}

print("Displaying scientific data with various magnitudes...")
show_data(scientific_data, "Scientific Measurements")

-- Test 11: Create a custom data table for interactive use
print("\n11. Custom Interactive Data Table:")

local custom_table = create_data_table("Interactive Experiment Data")

-- Create some experimental data
local experiment_results = {}
for i = 1, 10 do
    local size = i * 25
    local time = math.random() * 1000 + size * 0.1
    local gflops = (2 * size^3) / (time * 1000)
    
    table.insert(experiment_results, {
        trial = i,
        matrix_size = size,
        execution_time = time,
        gflops = gflops,
        efficiency = gflops / 100,  -- Assume 100 GFLOPS peak
        status = gflops > 50 and "Excellent" or (gflops > 25 and "Good" or "Poor")
    })
end

custom_table:displayData(experiment_results, "Experiment Results")
custom_table:show()

print("Created interactive custom table with filtering and export capabilities")

-- Test 12: Display matrix comparison
print("\n12. Matrix Before/After Comparison:")

-- Create original matrix
local original = create_accelerated_matrix(3, 3)
original:fillRandom(-2, 2)

-- Create processed matrix (e.g., after some operation)
local processed = original:scale(2.0)
processed = processed:add(create_accelerated_identity(3))

print("Displaying original matrix...")
show_accelerated_matrix(original, "Original Matrix")

print("Displaying processed matrix (scaled + identity)...")
show_accelerated_matrix(processed, "Processed Matrix")

-- Test 13: Display solver verification data
print("\n13. Linear System Solver Verification:")

-- Create system
local A = create_accelerated_matrix(4, 4)
A:fillRandom(-3, 3)

-- Make well-conditioned
for i = 0, 3 do
    A:set(i, i, A:get(i, i) + 5)
end

local b = {10, 15, 8, 12}
local solution = A:solve(b)

-- Create verification data
local verification_data = {
    system_info = {
        matrix_size = "4x4",
        condition_estimate = "Good",
        solver = "LAPACK dgesv"
    },
    solution = {},
    verification = {},
    errors = {}
}

-- Fill solution data
for i = 1, #solution do
    verification_data.solution[i] = {
        variable = "x" .. i,
        value = solution[i]
    }
end

-- Manual verification
for i = 0, 3 do
    local computed = 0
    for j = 0, 3 do
        computed = computed + A:get(i, j) * solution[j + 1]
    end
    table.insert(verification_data.verification, {
        equation = i + 1,
        computed = computed,
        expected = b[i + 1],
        error = math.abs(computed - b[i + 1])
    })
end

print("Displaying linear system solver verification...")
show_data(verification_data, "Linear System Verification")

print("\n=== Generic Data Table Demo Complete ===")
print("✅ Matrix display (basic and accelerated)")
print("✅ Vector display") 
print("✅ Eigenvalue results")
print("✅ Performance benchmarks")
print("✅ Nested table structures")
print("✅ Mixed data types")
print("✅ Mathematical results (QR decomposition)")
print("✅ Statistical analysis")
print("✅ Algorithm comparisons")
print("✅ Scientific notation data")
print("✅ Interactive custom tables")
print("✅ Matrix comparisons")
print("✅ Solver verification")
print("")
print("The generic data table can display ANY Lua data structure!")
print("Features:")
print("• Automatic format detection (matrices, arrays, tables)")
print("• Customizable number formatting (scientific, fixed, auto)")
print("• Filtering and searching")
print("• Copy to clipboard and CSV export")
print("• Professional Qt styling")
print("• Sortable columns")
print("• Index display toggle")

return "Generic data table demonstration completed successfully!"