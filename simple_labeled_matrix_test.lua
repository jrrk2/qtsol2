-- simple_labeled_matrix_test.lua - Test the actual show_labeled_matrix function

print("=== Testing show_labeled_matrix function ===")

-- Create a tiny 2x3 matrix with simple data
local test_matrix = create_accelerated_matrix(2, 3)

-- Fill with simple numbers
test_matrix:set(0, 0, 10)   
test_matrix:set(0, 1, 20)   
test_matrix:set(0, 2, 30)   
test_matrix:set(1, 0, 40)   
test_matrix:set(1, 1, 50)   
test_matrix:set(1, 2, 60)   

-- Create labels as Lua tables (not arrays)
local row_labels = {}
row_labels[1] = "Test_A"
row_labels[2] = "Test_B"

local col_labels = {}
col_labels[1] = "Size"
col_labels[2] = "Time" 
col_labels[3] = "Score"

print("Matrix created with data:")
print("Row 1: 10, 20, 30")
print("Row 2: 40, 50, 60")

print("\nRow labels:", row_labels[1], row_labels[2])
print("Column labels:", col_labels[1], col_labels[2], col_labels[3])

print("\nCalling show_labeled_matrix...")

-- Test the actual function
local success, result = pcall(function()
    return show_labeled_matrix(test_matrix, "Simple Test Matrix", row_labels, col_labels)
end)

if success then
    print("✅ show_labeled_matrix worked!")
    print("Result:", result)
else
    print("❌ show_labeled_matrix failed:", result)
    
    -- Fallback: show without labels
    print("Fallback - showing matrix without labels:")
    show_accelerated_matrix(test_matrix, "Fallback Matrix Display")
end

return "Labeled matrix test completed"