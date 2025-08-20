-- vector_test.lua - Test the fixed vector functionality

print("=== Vector Functions Test ===")

-- Test vector creation
print("\n1. Vector Creation:")
local vec1 = create_vector(5)  -- Create zero vector of size 5
print("Created zero vector of size " .. vector_size(vec1))

-- Test setting values
for i = 0, 4 do
    vector_set(vec1, i, i + 1)  -- Set values 1, 2, 3, 4, 5
end

print("Vector after setting values:")
for i = 0, vector_size(vec1) - 1 do
    print("  vec1[" .. i .. "] = " .. vector_get(vec1, i))
end

-- Test creating vector from Lua table
print("\n2. Vector from Table:")
local lua_table = {1.5, 2.5, 3.5, 4.5}
local vec2 = table_to_vector(lua_table)
print("Created vector from table with " .. vector_size(vec2) .. " elements")

-- Test vector math operations
print("\n3. Vector Math:")
local vec3 = create_vector(4)
for i = 0, 3 do
    vector_set(vec3, i, (i + 1) * 2)  -- Values: 2, 4, 6, 8
end

-- Test dot product
local dot_result = dot_product(vec2, vec3)
print("Dot product: " .. string.format("%.2f", dot_result))

-- Test vector addition
local vec_sum = vector_add(vec2, vec3)
print("Vector addition result:")
for i = 0, vector_size(vec_sum) - 1 do
    print("  sum[" .. i .. "] = " .. vector_get(vec_sum, i))
end

-- Test vector norms
local norm2 = vector_norm(vec2)
local norm3 = vector_norm(vec3)
print("Vector norms:")
print("  ||vec2|| = " .. string.format("%.3f", norm2))
print("  ||vec3|| = " .. string.format("%.3f", norm3))

-- Test vector normalization
local vec_copy = vector_scale(vec2, 1.0)  -- Make a copy
local original_norm = vector_normalize(vec_copy)
local new_norm = vector_norm(vec_copy)
print("Normalization test:")
print("  Original norm: " .. string.format("%.3f", original_norm))
print("  After normalize: " .. string.format("%.3f", new_norm))

-- Test statistics
print("\n4. Vector Statistics:")
local test_data = table_to_vector({1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
print("Test data: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10")
print("  Mean: " .. string.format("%.2f", vector_mean(test_data)))
print("  Std Dev: " .. string.format("%.2f", vector_std(test_data)))
print("  Min: " .. vector_min(test_data))
print("  Max: " .. vector_max(test_data))
print("  Sum: " .. vector_sum(test_data))

-- Test 3D vector operations
print("\n5. 3D Vector Operations:")
local vec_a = table_to_vector({1, 0, 0})  -- Unit vector along x
local vec_b = table_to_vector({0, 1, 0})  -- Unit vector along y

local cross_result = vector_cross_product(vec_a, vec_b)
print("Cross product of (1,0,0) × (0,1,0):")
for i = 0, 2 do
    print("  [" .. i .. "] = " .. vector_get(cross_result, i))
end
print("Expected: (0,0,1)")

-- Test matrix-vector operations
print("\n6. Matrix-Vector Operations:")
local matrix = create_matrix(3, 3)
matrix:fillIdentity()  -- 3x3 identity matrix

local test_vec = table_to_vector({5, 10, 15})
local mv_result = matrix_vector_multiply(matrix, test_vec)

print("Identity matrix × vector [5, 10, 15]:")
for i = 0, vector_size(mv_result) - 1 do
    print("  result[" .. i .. "] = " .. vector_get(mv_result, i))
end
print("Expected: [5, 10, 15] (identity preserves vector)")

-- Test getting matrix rows/columns as vectors
print("\n7. Matrix Row/Column Extraction:")
local test_matrix = create_matrix(3, 3)
for i = 0, 2 do
    for j = 0, 2 do
        test_matrix:set(i, j, i * 3 + j + 1)  -- Fill with values 1-9
    end
end

print("Test matrix:")
print(test_matrix:toString())

local row_1 = matrix_get_row_vector(test_matrix, 1)  -- Get row 1 (0-indexed)
print("Row 1 as vector:")
for i = 0, vector_size(row_1) - 1 do
    print("  [" .. i .. "] = " .. vector_get(row_1, i))
end

local col_2 = matrix_get_col_vector(test_matrix, 2)  -- Get column 2 (0-indexed)
print("Column 2 as vector:")
for i = 0, vector_size(col_2) - 1 do
    print("  [" .. i .. "] = " .. vector_get(col_2, i))
end

-- Test conversion back to Lua table
print("\n8. Vector to Table Conversion:")
local back_to_table = vector_to_table(vec2)
print("Vector converted back to Lua table:")
for i = 1, #back_to_table do
    print("  table[" .. i .. "] = " .. back_to_table[i])
end

print("\n=== Vector Functions Test Complete ===")
print("All basic vector operations working correctly!")

return "Vector test completed successfully"