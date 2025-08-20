-- simple_vector_test.lua - Minimal test to debug vector functions

print("=== Simple Vector Test ===")

-- Test 1: Basic vector creation
print("\n1. Testing vector creation:")
local vec = create_vector(3)
if vec then
    print("✓ create_vector(3) succeeded")
    local size = vector_size(vec)
    print("✓ vector_size returned: " .. tostring(size))
else
    print("✗ create_vector(3) failed - returned nil")
    return "FAILED: Vector creation"
end

-- Test 2: Setting and getting values
print("\n2. Testing set/get:")
vector_set(vec, 0, 3.0)
vector_set(vec, 1, 4.0)  
vector_set(vec, 2, 0.0)

local val0 = vector_get(vec, 0)
local val1 = vector_get(vec, 1)
local val2 = vector_get(vec, 2)

print("Set: [3.0, 4.0, 0.0]")
print("Got: [" .. tostring(val0) .. ", " .. tostring(val1) .. ", " .. tostring(val2) .. "]")

-- Test 3: Simple norm calculation (should be 5.0 for vector [3,4,0])
print("\n3. Testing vector_norm:")
print("Vector: [3, 4, 0] - expected norm = 5.0")

local norm_result = vector_norm(vec)
print("vector_norm returned: " .. tostring(norm_result))
print("Type of result: " .. type(norm_result))

if norm_result == nil then
    print("✗ vector_norm returned nil!")
    
    -- Let's try to debug by checking if the function exists
    if vector_norm then
        print("  vector_norm function exists")
        
        -- Try calling it in a pcall to catch errors
        local success, result = pcall(vector_norm, vec)
        print("  pcall success: " .. tostring(success))
        print("  pcall result: " .. tostring(result))
        
        if not success then
            print("  Error message: " .. tostring(result))
        end
    else
        print("  vector_norm function does not exist!")
    end
else
    print("✓ vector_norm returned: " .. norm_result)
    if math.abs(norm_result - 5.0) < 0.001 then
        print("✓ Correct value!")
    else
        print("✗ Expected 5.0, got " .. norm_result)
    end
end

-- Test 4: Manual norm calculation to verify
print("\n4. Manual norm calculation:")
local manual_norm = math.sqrt(val0*val0 + val1*val1 + val2*val2)
print("Manual calculation: sqrt(" .. val0 .. "² + " .. val1 .. "² + " .. val2 .. "²) = " .. manual_norm)

-- Test 5: Simple table conversion
print("\n5. Testing table conversion:")
local test_table = {1, 2, 3}
print("Original table: {1, 2, 3}")

local vec_from_table = table_to_vector(test_table)
if vec_from_table then
    print("✓ table_to_vector succeeded")
    print("  Size: " .. vector_size(vec_from_table))
    print("  Values: [" .. vector_get(vec_from_table, 0) .. ", " .. 
          vector_get(vec_from_table, 1) .. ", " .. vector_get(vec_from_table, 2) .. "]")
else
    print("✗ table_to_vector failed")
end

-- Test 6: Check available functions
print("\n6. Function availability check:")
local functions = {
    "create_vector", "vector_size", "vector_get", "vector_set", 
    "vector_norm", "table_to_vector", "vector_to_table", "dot_product"
}

for _, func_name in ipairs(functions) do
    local func = _G[func_name]
    if func then
        print("✓ " .. func_name .. " available (type: " .. type(func) .. ")")
    else
        print("✗ " .. func_name .. " NOT available")
    end
end

print("\n=== Simple Vector Test Complete ===")

return "Simple test completed"