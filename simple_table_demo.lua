-- generic_table_demo.lua - Demonstrate the versatile data table widget

print("=== Simple Data Table Widget Demo ===")

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

show_data(mixed_array, "Mixed Data Types")
