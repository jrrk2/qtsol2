make -C bindings/build
bindings/build/ast_extractor sol2qtmainwindow.cpp -c ./build/compile_commands.json --pcl-path /Users/jonathan/PCL/include -o ast_data.json 
bindings/build/binding_generator ast_data.json bindings_auto.h
