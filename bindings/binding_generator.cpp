#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cctype>
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
    
    // JSON deserialization
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
    
    // Classification flags
    bool isGetter = false;
    bool isSetter = false;
    bool isValidator = false;
    bool isConverter = false;
    bool isMetamethod = false;
    bool isToString = false;
    std::string luaMetamethod;
    
    // JSON deserialization
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
        
        // Parse parameters array
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
    
    // JSON deserialization
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
    
    // Classification flags
    bool isPCLClass = false;
    bool isQtClass = false;
    bool hasQObject = false;
    bool isContainer = false;
    bool isValueType = false;
    bool isMetadata = false;
    bool isAlgorithm = false;
    
    // JSON deserialization
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
        
        // Parse arrays
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

// Binding generator
class BindingGenerator {
private:
    bool verbose = false;
    
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
        
        // Parse each class
        for (const auto& [className, classJson] : jsonData["classes"].items()) {
            ClassInfo classInfo;
            classInfo.from_json(classJson);
            classes[className] = classInfo;
            
            if (verbose && className.find("AstrometricMetadata") != std::string::npos) {
                std::cout << "Loaded " << className << ":" << std::endl;
                std::cout << "  Methods: " << classInfo.methods.size() << std::endl;
                std::cout << "  Constructors: " << classInfo.constructors.size() << std::endl;
                std::cout << "  Fields: " << classInfo.fields.size() << std::endl;
                if (!classInfo.methods.empty()) {
                    std::cout << "  Sample methods: ";
                    for (size_t i = 0; i < std::min(size_t(5), classInfo.methods.size()); ++i) {
                        if (i > 0) std::cout << ", ";
                        std::cout << classInfo.methods[i].name;
                    }
                    std::cout << std::endl;
                }
            }
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
        
        // Generate bindings for each class - but be selective
        for (const auto& [name, classInfo] : classes) {
            // Only bind classes that make sense for Lua scripting
            bool shouldBind = false;
            
            if (classInfo.isPCLClass && classInfo.isMetadata && name.find("AstrometricMetadata") != std::string::npos) {
                // Always bind AstrometricMetadata
                shouldBind = true;
                output << generateAstrometricMetadataBinding(classInfo) << "\n\n";
                boundClasses++;
            } else if (classInfo.isStruct && !classInfo.fields.empty() && isBindableStruct(classInfo)) {
                // Only bind structs with useful public fields
                shouldBind = true;
                output << generateStructBinding(classInfo) << "\n\n";
                boundClasses++;
            } else if (!classInfo.isStruct && isBindableClass(classInfo)) {
                // Only bind classes with useful public methods
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
    bool isBindableStruct(const ClassInfo& classInfo) const {
        // Only bind structs that are useful for Lua scripting
        if (classInfo.name == "DescriptionItems") return true;
        if (classInfo.name.find("Options") != std::string::npos && !classInfo.fields.empty()) return true;
        
        // Skip internal/nested structs
        if (classInfo.name.find("::") != std::string::npos) return false;
        if (classInfo.name.find("Data") != std::string::npos) return false;
        if (classInfo.name.find("Thread") != std::string::npos) return false;
        
        return classInfo.fields.size() >= 2; // Must have some useful fields
    }
    
    bool isBindableClass(const ClassInfo& classInfo) const {
        // Skip classes with no useful methods
        if (classInfo.methods.empty() && classInfo.constructors.empty()) return false;
        
        // Skip Sol2 binding infrastructure
        if (classInfo.name.find("sol") == 0 || classInfo.qualifiedName.find("sol::") != std::string::npos) {
            return false;
        }
        
        // Skip iterator types
        if (classInfo.name.find("iterator") != std::string::npos) return false;
        
        // Skip thread types
        if (classInfo.name.find("Thread") != std::string::npos) return false;
        
        // Skip nested types that aren't useful for scripting
        if (classInfo.name.find("::") != std::string::npos) {
            // Allow some useful nested types
            if (classInfo.name.find("DescriptionItems") != std::string::npos) return true;
            return false;
        }
        
        // Skip internal/low-level classes
        std::vector<std::string> skipPatterns = {
            "PixelTraits", "Allocator", "AutoReentrancy", "AutoLock",
            "ReferenceCounter", "SharedPixelData", "CharTraits",
            "sol_", "detail", "impl"  // Additional Sol2 and implementation details
        };
        
        for (const auto& pattern : skipPatterns) {
            if (classInfo.name.find(pattern) != std::string::npos) return false;
        }
        
        // Skip classes from Sol2 headers
        if (classInfo.fileName.find("sol") != std::string::npos || 
            classInfo.fileName.find("lua") != std::string::npos) {
            return false;
        }
        
        // Only bind classes that have substantial public interface
        int publicMethods = 0;
        for (const auto& method : classInfo.methods) {
            if (method.visibility == "public" && !shouldSkipMethod(method)) {
                publicMethods++;
            }
        }
        
        return publicMethods >= 3; // Must have at least 3 useful public methods
    }
    std::string generateStructBinding(const ClassInfo& classInfo) const {
        std::ostringstream output;
        
        size_t lastColon = classInfo.name.find_last_of(':');
        std::string simpleName = (lastColon != std::string::npos) ? classInfo.name.substr(lastColon + 1) : classInfo.name;
        
        output << "    // Bind " << classInfo.qualifiedName << " structure\n";
        output << "    lua->new_usertype<" << classInfo.qualifiedName << ">(\"" << simpleName << "\",\n";
        
        std::vector<std::string> bindings;
        
        // Add public fields
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
        
        // Add default constructor
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
        
        // Priority methods for AstrometricMetadata
        std::vector<std::string> priorityMethods = {
            "Build", "IsValid", "Width", "Height", "Bounds",
            "ImageToCelestial", "CelestialToImage", "ImageCenterToCelestial", 
            "RawImageToCelestial", "Resolution", "SearchRadius", 
            "ResolutionFromFocal", "FocalFromResolution", "ResolutionAt",
            "ObservationStartTime", "ObservationEndTime", "ObservationMiddleTime",
            "LocationLongitude", "LocationLatitude", "LocationHeight",
            "SetLocationLongitude", "SetLocationLatitude", "SetLocationHeight",
            "SetObservationStartTime", "SetObservationEndTime", 
            "PixelSize", "SetPixelSize", "ReferenceSystem", "SetReferenceSystem",
            "HasSplineWorldTransformation", "EnsureSplineGridInterpolationsInitialized",
            "Undistorted", "Validate", "Verify", "ToProperties", "Summary",
            "Projection", "WorldTransform", "Rotation", "CreationTime",
            "Catalog", "CreatorApplication", "CreatorModule", "CreatorOS"
        };
        
        // Add priority methods first
        int foundPriorityMethods = 0;
        for (const std::string& priorityName : priorityMethods) {
            auto it = std::find_if(classInfo.methods.begin(), classInfo.methods.end(),
                [&priorityName](const Method& m) { return m.name == priorityName; });
            
            if (it != classInfo.methods.end()) {
                foundPriorityMethods++;
                std::string luaName = toLuaName(it->name);
                
                if (needsSpecialBinding(*it)) {
                    bindings.push_back(generateSpecialMethodBinding(*it, classInfo, luaName));
                } else {
                    bindings.push_back("        \"" + luaName + "\", &" + classInfo.qualifiedName + "::" + it->name);
                }
            }
        }
        
        if (verbose) {
            std::cout << "Found " << foundPriorityMethods << " priority methods for AstrometricMetadata" << std::endl;
        }
        
        // Add metamethods
        for (const auto& method : classInfo.methods) {
            if (method.isMetamethod && !method.luaMetamethod.empty()) {
                if (method.name == "operator=" && method.parameters.size() == 1) {
                    continue; // Skip assignment operator
                }
                
                if (method.isToString || method.name == "Summary") {
                    bindings.push_back(generateStringConversionBinding(method, classInfo));
                } else {
                    bindings.push_back("        " + method.luaMetamethod + ", &" + 
                                     classInfo.qualifiedName + "::" + method.name);
                }
            }
        }
        
        // Add remaining important methods
        for (const auto& method : classInfo.methods) {
            auto it = std::find(priorityMethods.begin(), priorityMethods.end(), method.name);
            if (it != priorityMethods.end() || method.isMetamethod || shouldSkipMethod(method)) {
                continue;
            }
            
            std::string luaName = toLuaName(method.name);
            
            if (needsSpecialBinding(method)) {
                bindings.push_back(generateSpecialMethodBinding(method, classInfo, luaName));
            } else {
                bindings.push_back("        \"" + luaName + "\", &" + classInfo.qualifiedName + "::" + method.name);
            }
        }
        
        // Output all bindings
        if (!bindings.empty()) {
            output << bindings[0];
            for (size_t i = 1; i < bindings.size(); ++i) {
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
        
        // Add constructors
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
        
        // Group methods by name for overload handling
        std::unordered_map<std::string, std::vector<Method>> methodGroups;
        for (const auto& method : classInfo.methods) {
            if (!shouldSkipMethod(method)) {
                methodGroups[method.name].push_back(method);
            }
        }
        
        // Add method bindings
        for (const auto& [methodName, overloads] : methodGroups) {
            std::string luaName = toLuaName(methodName);
            
            // Check if any overload is a metamethod
            bool isMetamethod = false;
            std::string metamethodName;
            const Method* metamethod = nullptr;
            
            for (const auto& method : overloads) {
                if (method.isMetamethod && !method.luaMetamethod.empty()) {
                    isMetamethod = true;
                    metamethodName = method.luaMetamethod;
                    metamethod = &method;
                    break;
                }
            }
            
            if (isMetamethod && metamethod) {
                // Handle metamethods specially
                if (metamethod->isToString || metamethod->name == "Summary") {
                    bindings.push_back(generateStringConversionBinding(*metamethod, classInfo));
                } else if (metamethod->name == "operator=" && metamethod->parameters.size() == 1) {
                    continue; // Skip assignment operator
                } else {
                    bindings.push_back("        " + metamethodName + ", &" + 
                                     classInfo.qualifiedName + "::" + metamethod->name);
                }
            } else if (overloads.size() == 1) {
                // Single overload
                const auto& method = overloads[0];
                if (needsSpecialBinding(method)) {
                    bindings.push_back(generateSpecialMethodBinding(method, classInfo, luaName));
                } else {
                    bindings.push_back("        \"" + luaName + "\", &" + classInfo.qualifiedName + "::" + methodName);
                }
            } else {
                // Multiple overloads - bind the simplest one
                auto simpleMethod = std::min_element(overloads.begin(), overloads.end(),
                    [](const Method& a, const Method& b) {
                        return a.parameters.size() < b.parameters.size();
                    });
                
                if (needsSpecialBinding(*simpleMethod)) {
                    bindings.push_back(generateSpecialMethodBinding(*simpleMethod, classInfo, luaName));
                } else {
                    bindings.push_back("        \"" + luaName + "\", &" + classInfo.qualifiedName + "::" + methodName);
                }
            }
        }
        
        // Add public fields
        for (const auto& field : classInfo.fields) {
            if (field.visibility == "public") {
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
    
    bool needsSpecialBinding(const Method& method) const {
        if (method.isStatic) return true;
        
        for (const auto& param : method.parameters) {
            if (param.type.find("PropertyArray") != std::string::npos ||
                param.type.find("FITSKeywordArray") != std::string::npos ||
                param.type.find("QString") != std::string::npos ||
                param.type.find("pcl::String") != std::string::npos ||
                param.isOptional) {
                return true;
            }
        }
        
        if (method.returnType.find("Optional<") != std::string::npos ||
            method.returnType.find("QString") != std::string::npos ||
            method.returnType.find("pcl::String") != std::string::npos) {
            return true;
        }
        
        return false;
    }
    
    std::string generateSpecialMethodBinding(const Method& method, const ClassInfo& classInfo, const std::string& luaName) const {
        std::ostringstream output;
        
        output << "        \"" << luaName << "\", [](";
        
        if (!method.isStatic) {
            output << classInfo.qualifiedName << "& obj";
            if (!method.parameters.empty()) {
                output << ", ";
            }
        }
        
        for (size_t i = 0; i < method.parameters.size(); ++i) {
            if (i > 0) output << ", ";
            const auto& param = method.parameters[i];
            
            if (param.type.find("PropertyArray") != std::string::npos) {
                output << "sol::table " << param.name;
            } else if (param.type.find("FITSKeywordArray") != std::string::npos) {
                output << "sol::table " << param.name;
            } else if (param.type.find("QString") != std::string::npos) {
                output << "const std::string& " << param.name;
            } else if (param.type.find("pcl::String") != std::string::npos) {
                output << "const std::string& " << param.name;
            } else {
                output << param.type << " " << param.name;
            }
        }
        
        output << ") {\n";
        
        // Check for unsupported parameters
        bool hasUnsupportedParams = false;
        for (const auto& param : method.parameters) {
            if (param.type.find("PropertyArray") != std::string::npos ||
                param.type.find("FITSKeywordArray") != std::string::npos) {
                hasUnsupportedParams = true;
                break;
            }
        }
        
        if (hasUnsupportedParams) {
            output << "            throw std::runtime_error(\"Method not yet implemented: requires complex type conversion\");\n";
        } else {
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
            
            for (size_t i = 0; i < method.parameters.size(); ++i) {
                if (i > 0) output << ", ";
                const auto& param = method.parameters[i];
                
                if (param.type.find("QString") != std::string::npos) {
                    output << "QString::fromStdString(" << param.name << ")";
                } else if (param.type.find("pcl::String") != std::string::npos) {
                    output << "pcl::String(" << param.name << ")";
                } else {
                    output << param.name;
                }
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
                } else if (method.returnType.find("pcl::String") != std::string::npos) {
                    output << "            return std::string(result.c_str());\n";
                } else {
                    output << "            return result;\n";
                }
            }
        }
        
        output << "        }";
        return output.str();
    }
    
    std::string generateStringConversionBinding(const Method& method, const ClassInfo& classInfo) const {
        std::ostringstream output;
        
        output << "        sol::meta_function::to_string, [](const " << classInfo.qualifiedName << "& obj) {\n";
        
        if (method.returnType.find("QString") != std::string::npos) {
            output << "            return obj." << method.name << "().toStdString();\n";
        } else if (method.returnType.find("pcl::String") != std::string::npos) {
            output << "            return std::string(obj." << method.name << "().c_str());\n";
        } else if (method.returnType.find("std::string") != std::string::npos) {
            output << "            return obj." << method.name << "();\n";
        } else {
            output << "            std::ostringstream oss;\n";
            output << "            oss << obj." << method.name << "();\n";
            output << "            return oss.str();\n";
        }
        
        output << "        }";
        return output.str();
    }
    
    bool shouldSkipMethod(const Method& method) const {
        if (method.visibility != "public") return true;
        
        if (method.name.empty() || method.name[0] == '~') return true;
        
        // Skip problematic operators but allow useful ones
        if (method.name.find("operator") == 0) {
            if (method.name == "operator==" || method.name == "operator!=" ||
                method.name == "operator<" || method.name == "operator<=" ||
                method.name == "operator>" || method.name == "operator>=" ||
                method.name == "operator+" || method.name == "operator-" ||
                method.name == "operator*" || method.name == "operator/" ||
                method.name == "operator[]" || method.name == "operator()") {
                return false;  // Don't skip these
            }
            if (method.name == "operator=" && method.parameters.size() == 1) {
                return true;   // Skip assignment - Sol2 handles automatically
            }
            return true;  // Skip other operators by default
        }
        
        // Skip Qt meta methods
        if (method.name == "metaObject" || method.name == "qt_metacast" || 
            method.name == "qt_metacall" || method.name == "staticMetaObject") {
            return true;
        }
        
        // Skip methods starting with underscore
        if (method.name[0] == '_') return true;
        
        return false;
    }
    
    std::string toLuaName(const std::string& cppName) const {
        // Convert CamelCase to snake_case for Lua
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
    
    // Parse options
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v") {
            verbose = true;
        }
    }
    
    if (verbose) {
        std::cout << "Stage 2: Binding Generation with nlohmann/json\n";
        std::cout << "Input JSON: " << inputFile << "\n";
        std::cout << "Output file: " << outputFile << "\n";
    }
    
    // Create binding generator
    BindingGenerator generator;
    generator.setVerbose(verbose);
    
    // Load classes from JSON
    auto classes = generator.loadFromJson(inputFile);
    
    if (classes.empty()) {
        std::cout << "No classes loaded from JSON file!\n";
        return 1;
    }
    
    std::cout << "Loaded " << classes.size() << " classes from JSON\n";
    
    // Show class summary
    for (const auto& [name, classInfo] : classes) {
        std::cout << "  " << name;
        if (classInfo.isPCLClass) std::cout << " [PCL]";
        if (classInfo.isMetadata) std::cout << " [METADATA]";
        if (classInfo.isAlgorithm) std::cout << " [ALGORITHM]";
        std::cout << " (" << classInfo.methods.size() << " methods)\n";
    }
    
    // Generate bindings
    std::string bindings = generator.generateBindings(classes);
    
    // Write output file
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        return 1;
    }
    
    out << bindings;
    out.close();
    
    std::cout << "Sol2 Lua bindings written to " << outputFile << std::endl;
    
    return 0;
}