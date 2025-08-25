#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <regex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Data structures matching Stage 1
struct Parameter {
    std::string type;
    std::string name;
    std::string defaultValue;
    bool isConst = false;
    bool isReference = false;
    bool isPointer = false;
    bool isOptional = false;
    
    void from_json(const json& j) {
        type = j.value("type", "");
        name = j.value("name", "");
        defaultValue = j.value("defaultValue", "");
        isConst = j.value("isConst", false);
        isReference = j.value("isReference", false);
        isPointer = j.value("isPointer", false);
        isOptional = j.value("isOptional", false);
    }
};

struct Method {
    std::string name;
    std::string returnType;
    std::vector<Parameter> parameters;
    bool isConst = false;
    bool isStatic = false;
    bool isVirtual = false;
    std::string visibility = "public";
    bool isConstructor = false;
    bool isDestructor = false;
    
    bool isGetter = false;
    bool isSetter = false;
    bool isValidator = false;
    bool isConverter = false;
    bool isMetamethod = false;
    bool isToString = false;
    std::string luaMetamethod;
    
    void from_json(const json& j) {
        name = j.value("name", "");
        returnType = j.value("returnType", "");
        isConst = j.value("isConst", false);
        isStatic = j.value("isStatic", false);
        isVirtual = j.value("isVirtual", false);
        visibility = j.value("visibility", "public");
        isConstructor = j.value("isConstructor", false);
        isDestructor = j.value("isDestructor", false);
        isGetter = j.value("isGetter", false);
        isSetter = j.value("isSetter", false);
        isValidator = j.value("isValidator", false);
        isConverter = j.value("isConverter", false);
        isMetamethod = j.value("isMetamethod", false);
        isToString = j.value("isToString", false);
        luaMetamethod = j.value("luaMetamethod", "");
        
        if (j.contains("parameters") && j["parameters"].is_array()) {
            for (const auto& paramJson : j["parameters"]) {
                Parameter param;
                param.from_json(paramJson);
                parameters.push_back(param);
            }
        }
    }
};

struct Field {
    std::string name;
    std::string type;
    std::string visibility = "public";
    bool isStatic = false;
    bool isMutable = false;
    
    void from_json(const json& j) {
        name = j.value("name", "");
        type = j.value("type", "");
        visibility = j.value("visibility", "public");
        isStatic = j.value("isStatic", false);
        isMutable = j.value("isMutable", false);
    }
};

struct ClassInfo {
    std::string name;
    std::string qualifiedName;
    std::string fileName;
    bool isStruct = false;
    bool isTemplate = false;
    
    std::vector<std::string> baseClasses;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Method> constructors;
    
    bool isPCLClass = false;
    bool isQtClass = false;
    bool hasQObject = false;
    bool isContainer = false;
    bool isValueType = false;
    bool isMetadata = false;
    bool isAlgorithm = false;
    
    void from_json(const json& j) {
        name = j.value("name", "");
        qualifiedName = j.value("qualifiedName", "");
        fileName = j.value("fileName", "");
        isStruct = j.value("isStruct", false);
        isTemplate = j.value("isTemplate", false);
        isPCLClass = j.value("isPCLClass", false);
        isQtClass = j.value("isQtClass", false);
        hasQObject = j.value("hasQObject", false);
        isContainer = j.value("isContainer", false);
        isValueType = j.value("isValueType", false);
        isMetadata = j.value("isMetadata", false);
        isAlgorithm = j.value("isAlgorithm", false);
        
        if (j.contains("baseClasses") && j["baseClasses"].is_array()) {
            baseClasses = j["baseClasses"].get<std::vector<std::string>>();
        }
        
        if (j.contains("fields") && j["fields"].is_array()) {
            for (const auto& fieldJson : j["fields"]) {
                Field field;
                field.from_json(fieldJson);
                fields.push_back(field);
            }
        }
        
        if (j.contains("methods") && j["methods"].is_array()) {
            for (const auto& methodJson : j["methods"]) {
                Method method;
                method.from_json(methodJson);
                methods.push_back(method);
            }
        }
        
        if (j.contains("constructors") && j["constructors"].is_array()) {
            for (const auto& ctorJson : j["constructors"]) {
                Method ctor;
                ctor.from_json(ctorJson);
                constructors.push_back(ctor);
            }
        }
    }
};

class BindingGenerator {
private:
    bool verbose = false;
    
    // Known problematic classes/methods
    std::unordered_set<std::string> problematicClasses = {
        "pcl::UIObject"  // Has protected constructors
    };
    
    std::unordered_set<std::string> bitFieldStructs = {
        "pcl::ISO8601ConversionOptions",
        "pcl::SexagesimalConversionOptions"
    };
    
    std::unordered_set<std::string> problematicMethods = {
        "Null", "UTF16ToUTF32", "UTF8ToUTF16", "UTF16ToUTF8"
    };
    
public:
    void setVerbose(bool v) { verbose = v; }
    
    std::unordered_map<std::string, ClassInfo> loadFromJson(const std::string& filename) {
        std::unordered_map<std::string, ClassInfo> classes;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open JSON file: " << filename << std::endl;
            return classes;
        }
        
        json jsonData;
        try {
            file >> jsonData;
        } catch (const json::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            return classes;
        }
        
        if (!jsonData.contains("classes") || !jsonData["classes"].is_object()) {
            std::cerr << "Invalid JSON format: missing 'classes' object" << std::endl;
            return classes;
        }
        
        for (const auto& [className, classJson] : jsonData["classes"].items()) {
            ClassInfo classInfo;
            classInfo.from_json(classJson);
            classes[className] = classInfo;
        }
        
        if (verbose) {
            std::cout << "Loaded " << classes.size() << " classes from " << filename << std::endl;
        }
        
        return classes;
    }
    
    std::string generateBindings(const std::unordered_map<std::string, ClassInfo>& classes) {
        std::ostringstream output;
        
        output << "// Generated Sol2 Lua bindings\n";
        output << "// Total classes: " << classes.size() << "\n\n";
        
        int boundClasses = 0;
        
        for (const auto& [name, classInfo] : classes) {
            if (shouldSkipClass(classInfo)) {
                if (verbose) {
                    std::cout << "Skipping problematic class: " << name << std::endl;
                }
                continue;
            }
            
            bool shouldBind = false;
            
            if (classInfo.isPCLClass && classInfo.isMetadata && name.find("AstrometricMetadata") != std::string::npos) {
                shouldBind = true;
                output << generateAstrometricMetadataBinding(classInfo) << "\n\n";
                boundClasses++;
            } else if (classInfo.isStruct && !classInfo.fields.empty() && isBindableStruct(classInfo)) {
                shouldBind = true;
                output << generateStructBinding(classInfo) << "\n\n";
                boundClasses++;
            } else if (!classInfo.isStruct && isBindableClass(classInfo)) {
                shouldBind = true;
                output << generateClassBinding(classInfo) << "\n\n";
                boundClasses++;
            }
            
            if (verbose && shouldBind) {
                std::cout << "Generated binding for: " << name << std::endl;
            }
        }
        
        output << "// Generated bindings for " << boundClasses << " classes\n";
        
        return output.str();
    }
    
private:
    bool shouldSkipClass(const ClassInfo& classInfo) const {
        return problematicClasses.count(classInfo.qualifiedName) > 0;
    }
    
    bool isBindableStruct(const ClassInfo& classInfo) const {
        if (bitFieldStructs.count(classInfo.qualifiedName) > 0) {
            return false; // Skip structs with bit fields
        }
        
        if (classInfo.name == "DescriptionItems") return true;
        if (classInfo.name.find("Options") != std::string::npos && !classInfo.fields.empty()) return true;
        
        if (classInfo.name.find("::") != std::string::npos) return false;
        if (classInfo.name.find("Data") != std::string::npos) return false;
        if (classInfo.name.find("Thread") != std::string::npos) return false;
        
        return classInfo.fields.size() >= 2;
    }
    
    bool isBindableClass(const ClassInfo& classInfo) const {
        if (classInfo.methods.empty() && classInfo.constructors.empty()) return false;
        
        if (classInfo.name.find("sol") == 0 || classInfo.qualifiedName.find("sol::") != std::string::npos) {
            return false;
        }
        
        if (classInfo.name.find("iterator") != std::string::npos) return false;
        if (classInfo.name.find("Thread") != std::string::npos) return false;
        
        if (classInfo.name.find("::") != std::string::npos) {
            if (classInfo.name.find("DescriptionItems") != std::string::npos) return true;
            return false;
        }
        
        std::vector<std::string> skipPatterns = {
            "PixelTraits", "Allocator", "AutoReentrancy", "AutoLock",
            "ReferenceCounter", "SharedPixelData", "CharTraits",
            "sol_", "detail", "impl"
        };
        
        for (const auto& pattern : skipPatterns) {
            if (classInfo.name.find(pattern) != std::string::npos) return false;
        }
        
        if (classInfo.fileName.find("sol") != std::string::npos || 
            classInfo.fileName.find("lua") != std::string::npos) {
            return false;
        }
        
        int publicMethods = 0;
        for (const auto& method : classInfo.methods) {
            if (method.visibility == "public" && !shouldSkipMethod(method)) {
                publicMethods++;
            }
        }
        
        return publicMethods >= 3;
    }
    
    std::string generateStructBinding(const ClassInfo& classInfo) const {
        std::ostringstream output;
        
        size_t lastColon = classInfo.name.find_last_of(':');
        std::string simpleName = (lastColon != std::string::npos) ? classInfo.name.substr(lastColon + 1) : classInfo.name;
        
        output << "    // Bind " << classInfo.qualifiedName << " structure\n";
        output << "    lua->new_usertype<" << classInfo.qualifiedName << ">(\"" << simpleName << "\",\n";
        
        std::vector<std::string> bindings;
        
        for (const auto& field : classInfo.fields) {
            if (field.visibility == "public" && !field.type.empty()) {
                bindings.push_back("        \"" + toLuaName(field.name) + "\", &" + classInfo.qualifiedName + "::" + field.name);
            }
        }
        
        if (!bindings.empty()) {
            output << bindings[0];
            for (size_t i = 1; i < bindings.size(); ++i) {
                output << ",\n" << bindings[i];
            }
        }
        
        output << "\n    );";
        return output.str();
    }
    
    std::string generateAstrometricMetadataBinding(const ClassInfo& classInfo) const {
        std::ostringstream output;
        
        output << "    // Specialized binding for " << classInfo.qualifiedName << "\n";
        output << "    lua->new_usertype<" << classInfo.qualifiedName << ">(\"AstrometricMetadata\",\n";
        
        std::vector<std::string> bindings;
        
        // Add default constructor if available
        bool hasDefaultConstructor = false;
        for (const auto& ctor : classInfo.constructors) {
            if (ctor.parameters.empty()) {
                hasDefaultConstructor = true;
                break;
            }
        }
        
        if (hasDefaultConstructor) {
            bindings.push_back("        sol::constructors<" + classInfo.qualifiedName + "()>()");
        }
        
        // Priority methods for AstrometricMetadata - limit to most important
        std::vector<std::string> priorityMethods = {
            "Build", "IsValid", "Width", "Height", "Bounds",
            "ImageToCelestial", "CelestialToImage", "Resolution", 
            "ObservationStartTime", "ObservationEndTime", 
            "LocationLongitude", "LocationLatitude", "LocationHeight",
            "SetLocationLongitude", "SetLocationLatitude", "SetLocationHeight",
            "PixelSize", "SetPixelSize", "ReferenceSystem", "SetReferenceSystem",
            "Validate", "ToProperties", "Summary"
        };
        
        // Add priority methods with limits to avoid Sol2 template issues
        int addedMethods = 0;
        const int MAX_BINDINGS = 25; // Limit to avoid template recursion
        
        for (const std::string& priorityName : priorityMethods) {
            if (addedMethods >= MAX_BINDINGS) break;
            
            auto it = std::find_if(classInfo.methods.begin(), classInfo.methods.end(),
                [&priorityName](const Method& m) { return m.name == priorityName; });
            
            if (it != classInfo.methods.end()) {
                std::string luaName = toLuaName(it->name);
                
                if (needsSpecialBinding(*it)) {
                    bindings.push_back(generateSpecialMethodBinding(*it, classInfo, luaName));
                } else {
                    bindings.push_back("        \"" + luaName + "\", &" + classInfo.qualifiedName + "::" + it->name);
                }
                addedMethods++;
            }
        }
        
        // Add metamethods
        for (const auto& method : classInfo.methods) {
            if (addedMethods >= MAX_BINDINGS) break;
            
            if (method.isMetamethod && !method.luaMetamethod.empty()) {
                if (method.name == "operator=" && method.parameters.size() == 1) {
                    continue; // Skip assignment operator
                }
                
                bindings.push_back("        " + method.luaMetamethod + ", &" + 
                                 classInfo.qualifiedName + "::" + method.name);
                addedMethods++;
            }
        }
        
        if (!bindings.empty()) {
            output << bindings[0];
            for (size_t i = 1; i < bindings.size() && i < MAX_BINDINGS; ++i) {
                output << ",\n" << bindings[i];
            }
        }
        
        output << "\n    );";
        return output.str();
    }
    
    std::string generateClassBinding(const ClassInfo& classInfo) const {
        std::ostringstream output;
        
        size_t lastColon = classInfo.name.find_last_of(':');
        std::string simpleName = (lastColon != std::string::npos) ? classInfo.name.substr(lastColon + 1) : classInfo.name;
        
        output << "    // Bind " << classInfo.qualifiedName << " class\n";
        output << "    lua->new_usertype<" << classInfo.qualifiedName << ">(\"" << simpleName << "\",\n";
        
        std::vector<std::string> bindings;
        const int MAX_BINDINGS = 20; // Limit to avoid Sol2 template issues
        
        // Add default constructor if available
        bool hasDefaultConstructor = false;
        for (const auto& ctor : classInfo.constructors) {
            if (ctor.parameters.empty()) {
                hasDefaultConstructor = true;
                break;
            }
        }
        
        if (hasDefaultConstructor) {
            bindings.push_back("        sol::constructors<" + classInfo.qualifiedName + "()>()");
        }
        
        // Add methods with limit
        int addedMethods = 0;
        for (const auto& method : classInfo.methods) {
            if (addedMethods >= MAX_BINDINGS || shouldSkipMethod(method)) {
                continue;
            }
            
            std::string luaName = toLuaName(method.name);
            
            if (method.isMetamethod && !method.luaMetamethod.empty()) {
                if (method.name == "operator=" && method.parameters.size() == 1) {
                    continue;
                }
                bindings.push_back("        " + method.luaMetamethod + ", &" + 
                                 classInfo.qualifiedName + "::" + method.name);
            } else if (needsSpecialBinding(method)) {
                bindings.push_back(generateSpecialMethodBinding(method, classInfo, luaName));
            } else {
                bindings.push_back("        \"" + luaName + "\", &" + classInfo.qualifiedName + "::" + method.name);
            }
            addedMethods++;
        }
        
        // Add public fields
        for (const auto& field : classInfo.fields) {
            if (addedMethods >= MAX_BINDINGS) break;
            if (field.visibility == "public") {
                bindings.push_back("        \"" + toLuaName(field.name) + "\", &" + classInfo.qualifiedName + "::" + field.name);
                addedMethods++;
            }
        }
        
        if (!bindings.empty()) {
            output << bindings[0];
            for (size_t i = 1; i < bindings.size() && i < MAX_BINDINGS; ++i) {
                output << ",\n" << bindings[i];
            }
        }
        
        output << "\n    );";
        return output.str();
    }
    
    bool needsSpecialBinding(const Method& method) const {
        if (method.isStatic) return true;
        if (problematicMethods.count(method.name) > 0) return true;
        
        for (const auto& param : method.parameters) {
            if (param.type.find("PropertyArray") != std::string::npos ||
                param.type.find("FITSKeywordArray") != std::string::npos ||
                param.type.find("QString") != std::string::npos ||
                param.isOptional) {
                return true;
            }
        }
        
        if (method.returnType.find("Optional<") != std::string::npos ||
            method.returnType.find("QString") != std::string::npos) {
            return true;
        }
        
        return false;
    }
    
    std::string generateSpecialMethodBinding(const Method& method, const ClassInfo& classInfo, const std::string& luaName) const {
        std::ostringstream output;
        
        // Skip methods we know are problematic
        if (problematicMethods.count(method.name) > 0) {
            output << "        // SKIPPED: " << method.name << " (known problematic)";
            return output.str();
        }
        
        output << "        \"" << luaName << "\", [](";
        
        if (!method.isStatic) {
            output << classInfo.qualifiedName << "& obj";
            if (!method.parameters.empty()) {
                output << ", ";
            }
        }
        
        // Generate safe parameter list
        std::vector<std::string> paramDecls;
        std::vector<std::string> paramCalls;
        
        for (const auto& param : method.parameters) {
            if (param.type.find("PropertyArray") != std::string::npos ||
                param.type.find("FITSKeywordArray") != std::string::npos) {
                // Skip complex types for now
                continue;
            } else if (param.type.find("QString") != std::string::npos) {
                paramDecls.push_back("const std::string& " + param.name);
                paramCalls.push_back("QString::fromStdString(" + param.name + ")");
            } else {
                std::string paramType = qualifyPCLType(param.type);
                paramDecls.push_back(paramType + " " + param.name);
                paramCalls.push_back(param.name);
            }
        }
        
        // Output parameter declarations
        for (size_t i = 0; i < paramDecls.size(); ++i) {
            if (i > 0) output << ", ";
            output << paramDecls[i];
        }
        
        output << ") {\n";
        
        // Generate the method call
        if (method.returnType != "void") {
            output << "            auto result = ";
        } else {
            output << "            ";
        }
        
        if (method.isStatic) {
            output << classInfo.qualifiedName << "::" << method.name << "(";
        } else {
            output << "obj." << method.name << "(";
        }
        
        for (size_t i = 0; i < paramCalls.size(); ++i) {
            if (i > 0) output << ", ";
            output << paramCalls[i];
        }
        
        output << ");\n";
        
        // Handle return value conversion
        if (method.returnType != "void") {
            if (method.returnType.find("Optional<") != std::string::npos) {
                output << "            if (result.IsDefined()) {\n";
                output << "                return result();\n";
                output << "            } else {\n";
                output << "                return sol::nil;\n";
                output << "            }\n";
            } else if (method.returnType.find("QString") != std::string::npos) {
                output << "            return result.toStdString();\n";
            } else {
                output << "            return result;\n";
            }
        }
        
        output << "        }";
        return output.str();
    }
    
    std::string qualifyPCLType(const std::string& type) const {
        std::string result = type;
        
        std::vector<std::pair<std::string, std::string>> pclTypes = {
            {"RandomizationOptions", "pcl::RandomizationOptions"},
            {"ISO8601ConversionOptions", "pcl::ISO8601ConversionOptions"},
            {"SexagesimalConversionOptions", "pcl::SexagesimalConversionOptions"},
            {"IsoString", "pcl::IsoString"},
            {"String", "pcl::String"},
            {"DPoint", "pcl::DPoint"},
            {"FPoint", "pcl::FPoint"},
            {"TimePoint", "pcl::TimePoint"}
        };
        
        for (const auto& [shortName, fullName] : pclTypes) {
            if (result.find("pcl::" + shortName) == std::string::npos) {
                std::regex pattern("\\b" + shortName + "\\b");
                result = std::regex_replace(result, pattern, fullName);
            }
        }
        
        return result;
    }
    
    bool shouldSkipMethod(const Method& method) const {
        if (method.visibility != "public") return true;
        if (method.name.empty() || method.name[0] == '~') return true;
        if (problematicMethods.count(method.name) > 0) return true;
        
        // Skip problematic operators but allow useful ones
        if (method.name.find("operator") == 0) {
            if (method.name == "operator==" || method.name == "operator!=" ||
                method.name == "operator<" || method.name == "operator<=" ||
                method.name == "operator>" || method.name == "operator>=" ||
                method.name == "operator+" || method.name == "operator-" ||
                method.name == "operator*" || method.name == "operator/" ||
                method.name == "operator[]" || method.name == "operator()") {
                return false;
            }
            if (method.name == "operator=" && method.parameters.size() == 1) {
                return true;
            }
            return true;
        }
        
        // Skip Qt meta methods
        if (method.name == "metaObject" || method.name == "qt_metacast" || 
            method.name == "qt_metacall" || method.name == "staticMetaObject") {
            return true;
        }
        
        if (method.name[0] == '_') return true;
        
        return false;
    }
    
    std::string toLuaName(const std::string& cppName) const {
        std::string result;
        for (size_t i = 0; i < cppName.length(); ++i) {
            if (i > 0 && std::isupper(cppName[i]) && std::islower(cppName[i-1])) {
                result += '_';
            }
            result += std::tolower(cppName[i]);
        }
        return result;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input.json> <output.h> [options]\n";
        std::cout << "Options:\n";
        std::cout << "  -v    Verbose output\n";
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    bool verbose = false;
    
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v") {
            verbose = true;
        }
    }
    
    if (verbose) {
        std::cout << "Stage 2: Fixed Binding Generation\n";
        std::cout << "Input JSON: " << inputFile << "\n";
        std::cout << "Output file: " << outputFile << "\n";
    }
    
    BindingGenerator generator;
    generator.setVerbose(verbose);
    
    auto classes = generator.loadFromJson(inputFile);
    
    if (classes.empty()) {
        std::cout << "No classes loaded from JSON file!\n";
        return 1;
    }
    
    std::cout << "Loaded " << classes.size() << " classes from JSON\n";
    
    std::string bindings = generator.generateBindings(classes);
    
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        return 1;
    }
    
    out << bindings;
    out.close();
    
    std::cout << "Fixed Sol2 Lua bindings written to " << outputFile << std::endl;
    
    return 0;
}
