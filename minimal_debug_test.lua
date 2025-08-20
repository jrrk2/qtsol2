-- minimal_debug_test.lua - Ultra-simple debugging test

print("=== Minimal Debug Test ===")

-- Test 1: Check if the debug function works
print("\n1. Testing debug function:")
if test_vector_norm_debug then
    local result = test_vector_norm_debug()
    print("test_vector_norm_debug() returned: " .. tostring(result))
    print("Expected: 5.0")
else
    print("test_vector_norm_debug function not available")
end

-- Test 2: Create a simple vector and debug it
print("\n2. Creating and debugging vector:")
local vec = create_vector(3)
if vec then
    print("Vector created successfully")
    
    -- Set simple values
    vector_set(vec, 0, 3.0)
    vector_set(vec, 1, 4.0)
    vector_set(vec, 2, 0.0)
    
    print("Values set: [3.0, 4.0, 0.0]")
    
    -- Debug the vector contents
    if debug_vector_contents then
        debug_vector_contents(vec)
    end
    
    -- Try the verbose norm function
    if vector_norm_verbose then
        print("Calling vector_norm_verbose:")
        local norm_result = vector_norm_verbose(vec)
        print("Result: " .. tostring(norm_result))
    end
    
    -- Try the regular norm function
    print("Calling regular vector_norm:")
    local success, norm_or_error = pcall(vector_norm, vec)
    print("Success: " .. tostring(success))
    print("Result: " .. tostring(norm_or_error))
    
else
    print("Failed to create vector")
end

-- Test 3: Check basic Lua math
print("\n3. Basic Lua math test:")
local test_sqrt = math.sqrt(25)
print("math.sqrt(25) = " .. test_sqrt)

-- Test 4: Manual calculation
print("\n4. Manual norm calculation:")
local a, b, c = 3.0, 4.0, 0.0
local manual_norm = math.sqrt(a*a + b*b + c*c)
print("Manual: sqrt(3² + 4² + 0²) = " .. manual_norm)

print("\n=== End Minimal Debug Test ===")

return "Debug test completed"