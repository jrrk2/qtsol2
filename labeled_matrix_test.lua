-- simple_labeled_matrix_test.lua - Actually use the labels!

print("=== Simple Labeled Matrix Test ===")

-- Create a tiny 2x3 matrix with simple data
local test_matrix = create_accelerated_matrix(2, 3)

-- Fill with simple numbers
test_matrix:set(0, 0, 10)   -- Row 1, Col 1
test_matrix:set(0, 1, 20)   -- Row 1, Col 2  
test_matrix:set(0, 2, 30)   -- Row 1, Col 3

test_matrix:set(1, 0, 40)   -- Row 2, Col 1
test_matrix:set(1, 1, 50)   -- Row 2, Col 2
test_matrix:set(1, 2, 60)   -- Row 2, Col 3

-- Simple labels
local row_labels = {"Test_A", "Test_B"}
local col_labels = {"Size", "Time", "Score"}

print("Matrix created. Displaying WITHOUT labels:")
show_accelerated_matrix(test_matrix, "Raw Matrix Data")

print("\nNow creating a labeled data structure that actually USES the labels:")

-- Create a proper table structure that uses the labels
local labeled_data = {}

for i = 1, #row_labels do
    local row_data = {
        test_case = row_labels[i]  -- Use the row label
    }
    
    -- Add each column with its label as the key
    for j = 1, #col_labels do
        row_data[col_labels[j]] = test_matrix:get(i-1, j-1)
    end
    
    table.insert(labeled_data, row_data)
end

print("Displaying the LABELED data structure:")
show_data(labeled_data, "Properly Labeled Matrix Data")

print("\nThis should show a table with:")
print("- test_case column showing: Test_A, Test_B")
print("- Size column showing: 10, 40") 
print("- Time column showing: 20, 50")
print("- Score column showing: 30, 60")

return "Actually used the labels this time!"