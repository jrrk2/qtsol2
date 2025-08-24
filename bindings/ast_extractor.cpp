#include <clang-c/Index.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <regex>
#include <cctype>

namespace fs = std::filesystem;

// JSON-serializable AST structures
struct JsonParameter {
    std::string type;
    std::string name;
    std::string defaultValue;
    bool isConst = false;
    bool isReference = false;
    bool isPointer = false;
    bool isOptional = false;
    
    std::string toJson() const {
        std::ostringstream json;
        json << "{\n";
        json << "    \"type\": \"" << escapeJson(type) << "\",\n";
        json << "    \"name\": \"" << escapeJson(name) << "\",\n";
        json << "    \"defaultValue\": \"" << escapeJson(defaultValue) << "\",\n";
        json << "    \"isConst\": " << (isConst ? "true" : "false") << ",\n";
        json << "    \"isReference\": " << (isReference ? "true" : "false") << ",\n";
        json << "    \"isPointer\": " << (isPointer ? "true" : "false") << ",\n";
        json << "    \"isOptional\": " << (isOptional ? "true" : "false") << "\n";
        json << "  }";
        return json.str();
    }
    
private:
    std::string escapeJson(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

struct JsonMethod {
    std::string name;
    std::string returnType;
    std::vector<JsonParameter> parameters;
    bool isConst = false;
    bool isStatic = false;
    bool isVirtual = false;
    std::string visibility = "public";
    bool isConstructor = false;
    bool isDestructor = false;
    
    // Classification flags (will be added by pattern analyzer)
    bool isGetter = false;
    bool isSetter = false;
    bool isValidator = false;
    bool isConverter = false;
    bool isMetamethod = false;
    bool isToString = false;
    std::string luaMetamethod;
    
    std::string toJson() const {
        std::ostringstream json;
        json << "{\n";
        json << "    \"name\": \"" << escapeJson(name) << "\",\n";
        json << "    \"returnType\": \"" << escapeJson(returnType) << "\",\n";
        json << "    \"parameters\": [\n";
        
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) json << ",\n";
            json << "      " << parameters[i].toJson();
        }
        json << "\n    ],\n";
        
        json << "    \"isConst\": " << (isConst ? "true" : "false") << ",\n";
        json << "    \"isStatic\": " << (isStatic ? "true" : "false") << ",\n";
        json << "    \"isVirtual\": " << (isVirtual ? "true" : "false") << ",\n";
        json << "    \"visibility\": \"" << visibility << "\",\n";
        json << "    \"isConstructor\": " << (isConstructor ? "true" : "false") << ",\n";
        json << "    \"isDestructor\": " << (isDestructor ? "true" : "false") << ",\n";
        json << "    \"isGetter\": " << (isGetter ? "true" : "false") << ",\n";
        json << "    \"isSetter\": " << (isSetter ? "true" : "false") << ",\n";
        json << "    \"isValidator\": " << (isValidator ? "true" : "false") << ",\n";
        json << "    \"isConverter\": " << (isConverter ? "true" : "false") << ",\n";
        json << "    \"isMetamethod\": " << (isMetamethod ? "true" : "false") << ",\n";
        json << "    \"isToString\": " << (isToString ? "true" : "false") << ",\n";
        json << "    \"luaMetamethod\": \"" << escapeJson(luaMetamethod) << "\"\n";
        json << "  }";
        return json.str();
    }
    
private:
    std::string escapeJson(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

struct JsonField {
    std::string name;
    std::string type;
    std::string visibility = "public";
    bool isStatic = false;
    bool isMutable = false;
    
    std::string toJson() const {
        std::ostringstream json;
        json << "{\n";
        json << "    \"name\": \"" << escapeJson(name) << "\",\n";
        json << "    \"type\": \"" << escapeJson(type) << "\",\n";
        json << "    \"visibility\": \"" << visibility << "\",\n";
        json << "    \"isStatic\": " << (isStatic ? "true" : "false") << ",\n";
        json << "    \"isMutable\": " << (isMutable ? "true" : "false") << "\n";
        json << "  }";
        return json.str();
    }
    
private:
    std::string escapeJson(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

struct JsonClassInfo {
    std::string name;
    std::string qualifiedName;
    std::string fileName;
    bool isStruct = false;
    bool isTemplate = false;
    
    std::vector<std::string> baseClasses;
    std::vector<JsonField> fields;
    std::vector<JsonMethod> methods;
    std::vector<JsonMethod> constructors;
    
    // Classification flags
    bool isPCLClass = false;
    bool isQtClass = false;
    bool hasQObject = false;
    bool isContainer = false;
    bool isValueType = false;
    bool isMetadata = false;
    bool isAlgorithm = false;
    
    std::string toJson() const {
        std::ostringstream json;
        json << "{\n";
        json << "  \"name\": \"" << escapeJson(name) << "\",\n";
        json << "  \"qualifiedName\": \"" << escapeJson(qualifiedName) << "\",\n";
        json << "  \"fileName\": \"" << escapeJson(fileName) << "\",\n";
        json << "  \"isStruct\": " << (isStruct ? "true" : "false") << ",\n";
        json << "  \"isTemplate\": " << (isTemplate ? "true" : "false") << ",\n";
        
        json << "  \"baseClasses\": [";
        for (size_t i = 0; i < baseClasses.size(); ++i) {
            if (i > 0) json << ", ";
            json << "\"" << escapeJson(baseClasses[i]) << "\"";
        }
        json << "],\n";
        
        json << "  \"fields\": [\n";
        for (size_t i = 0; i < fields.size(); ++i) {
            if (i > 0) json << ",\n";
            json << "    " << fields[i].toJson();
        }
        json << "\n  ],\n";
        
        json << "  \"methods\": [\n";
        for (size_t i = 0; i < methods.size(); ++i) {
            if (i > 0) json << ",\n";
            json << "    " << methods[i].toJson();
        }
        json << "\n  ],\n";
        
        json << "  \"constructors\": [\n";
        for (size_t i = 0; i < constructors.size(); ++i) {
            if (i > 0) json << ",\n";
            json << "    " << constructors[i].toJson();
        }
        json << "\n  ],\n";
        
        json << "  \"isPCLClass\": " << (isPCLClass ? "true" : "false") << ",\n";
        json << "  \"isQtClass\": " << (isQtClass ? "true" : "false") << ",\n";
        json << "  \"hasQObject\": " << (hasQObject ? "true" : "false") << ",\n";
        json << "  \"isContainer\": " << (isContainer ? "true" : "false") << ",\n";
        json << "  \"isValueType\": " << (isValueType ? "true" : "false") << ",\n";
        json << "  \"isMetadata\": " << (isMetadata ? "true" : "false") << ",\n";
        json << "  \"isAlgorithm\": " << (isAlgorithm ? "true" : "false") << "\n";
        json << "}";
        return json.str();
    }
    
private:
    std::string escapeJson(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// Pattern analyzer for classification
class PatternAnalyzer {
private:
    std::unordered_set<std::string> pclMetadataPatterns = {
        "metadata", "header", "keywords", "properties", "build", "parse", "serialize"
    };
    
    std::unordered_set<std::string> pclAlgorithmPatterns = {
        "filter", "segment", "extract", "compute", "estimate", "detect", "match"
    };
    
    std::unordered_set<std::string> getterPatterns = {
        "get", "is", "has", "can", "width", "height", "size", "length", "count"
    };
    
    std::unordered_set<std::string> setterPatterns = {
        "set", "add", "remove", "clear", "reset", "update", "enable", "disable"
    };
    
public:
    void analyzeClass(JsonClassInfo& classInfo) {
        classifyClass(classInfo);
        
        for (auto& method : classInfo.methods) {
            analyzeMethod(method, classInfo);
        }
        
        for (auto& ctor : classInfo.constructors) {
            analyzeConstructor(ctor, classInfo);
        }
    }
    
private:
    void classifyClass(JsonClassInfo& classInfo) {
        std::string lowerName = toLower(classInfo.name);
        
        classInfo.isPCLClass = classInfo.qualifiedName.find("pcl::") == 0;
        classInfo.isQtClass = classInfo.name.find("Q") == 0;
        
        classInfo.isMetadata = (lowerName.find("metadata") != std::string::npos ||
                               lowerName.find("header") != std::string::npos ||
                               lowerName.find("properties") != std::string::npos);
        
        classInfo.isAlgorithm = hasPatternInName(lowerName, pclAlgorithmPatterns);
        classInfo.isContainer = (lowerName.find("vector") != std::string::npos ||
                                lowerName.find("array") != std::string::npos ||
                                lowerName.find("list") != std::string::npos);
        
        classInfo.isValueType = (classInfo.isStruct || 
                                lowerName.find("point") != std::string::npos ||
                                lowerName.find("size") != std::string::npos ||
                                lowerName.find("rect") != std::string::npos);
    }
    
    void analyzeMethod(JsonMethod& method, const JsonClassInfo& classInfo) {
        std::string lowerName = toLower(method.name);
        
        method.isMetamethod = isOperatorMethod(method.name);
        if (method.isMetamethod) {
            method.luaMetamethod = getLuaMetamethod(method.name);
        }
        
        method.isGetter = (hasPatternInName(lowerName, getterPatterns) ||
                          (method.parameters.empty() && method.isConst && method.returnType != "void"));
        
        method.isSetter = (hasPatternInName(lowerName, setterPatterns) ||
                          (lowerName.find("set") == 0 && method.returnType == "void"));
        
        method.isValidator = (lowerName.find("valid") != std::string::npos ||
                             lowerName.find("verify") != std::string::npos ||
                             lowerName.find("check") != std::string::npos);
        
        method.isConverter = (lowerName.find("to") == 0 ||
                             lowerName.find("from") != std::string::npos ||
                             lowerName.find("convert") != std::string::npos ||
                             lowerName.find("transform") != std::string::npos);
        
        method.isToString = (method.name == "toString" || method.name == "to_string" ||
                            (method.name == "Summary" && method.returnType.find("String") != std::string::npos));
        
        for (auto& param : method.parameters) {
            analyzeParameter(param);
        }
    }
    
    void analyzeConstructor(JsonMethod& ctor, const JsonClassInfo& classInfo) {
        ctor.isConstructor = true;
        for (auto& param : ctor.parameters) {
            analyzeParameter(param);
        }
    }
    
    void analyzeParameter(JsonParameter& param) {
        param.isOptional = param.type.find("Optional<") == 0 || param.type.find("optional<") == 0;
        param.isConst = param.type.find("const") != std::string::npos;
        param.isReference = param.type.find("&") != std::string::npos;
        param.isPointer = param.type.find("*") != std::string::npos;
    }
    
    bool isOperatorMethod(const std::string& methodName) const {
        return methodName.find("operator") == 0;
    }
    
    std::string getLuaMetamethod(const std::string& operatorName) const {
        static std::unordered_map<std::string, std::string> operatorMap = {
            {"operator==", "sol::meta_function::equal_to"},
            {"operator!=", "sol::meta_function::not_equal_to"},
            {"operator<", "sol::meta_function::less_than"},
            {"operator<=", "sol::meta_function::less_than_or_equal_to"},
            {"operator>", "sol::meta_function::greater_than"},
            {"operator>=", "sol::meta_function::greater_than_or_equal_to"},
            {"operator+", "sol::meta_function::addition"},
            {"operator-", "sol::meta_function::subtraction"},
            {"operator*", "sol::meta_function::multiplication"},
            {"operator/", "sol::meta_function::division"},
            {"operator%", "sol::meta_function::modulus"},
            {"operator^", "sol::meta_function::power_of"},
            {"operator[]", "sol::meta_function::index"},
            {"operator()", "sol::meta_function::call"},
            {"to_string", "sol::meta_function::to_string"},
            {"toString", "sol::meta_function::to_string"},
            {"size", "sol::meta_function::length"},
            {"length", "sol::meta_function::length"}
        };
        
        auto it = operatorMap.find(operatorName);
        return (it != operatorMap.end()) ? it->second : "";
    }
    
    bool hasPatternInName(const std::string& name, const std::unordered_set<std::string>& patterns) {
        for (const auto& pattern : patterns) {
            if (name.find(pattern) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
};

// AST Extractor class
class ASTExtractor {
private:
    std::unordered_set<std::string> systemTypes;
    std::unordered_set<std::string> skipPrefixes;
    std::vector<std::string> pclPaths;
    bool verbose = false;
    PatternAnalyzer analyzer;
    
    void initializeFilters() {
        systemTypes = {
            "type", "pair", "numeric_limits", "vector", "array", "string", "locale"
        };
        
        skipPrefixes = {
            "std::", "__", "_", "boost::", "detail::", "internal::"
        };
    }
    
    bool shouldSkipType(const std::string& name, const std::string& fileName) const {
        if (name.empty() || name.length() <= 1 || name.length() > 50) {
            return true;
        }
        
        for (const auto& prefix : skipPrefixes) {
            if (name.length() >= prefix.length() && 
                name.substr(0, prefix.length()) == prefix) {
                return true;
            }
        }
        
        if (systemTypes.count(name)) {
            return true;
        }
        
        std::vector<std::string> systemPaths = {
            "/usr/", "/opt/", "/Applications/Xcode.app/", "/Library/", "/System/"
        };
        
        for (const auto& sysPath : systemPaths) {
            if (fileName.length() >= sysPath.length() && 
                fileName.substr(0, sysPath.length()) == sysPath) {
                return true;
            }
        }
        
        return false;
    }
    
    std::string getCursorSpelling(CXCursor cursor) const {
        CXString cxStr = clang_getCursorSpelling(cursor);
        std::string result = clang_getCString(cxStr);
        clang_disposeString(cxStr);
        return result;
    }
    
    std::string getTypeSpelling(CXType type) const {
        CXString cxStr = clang_getTypeSpelling(type);
        std::string result = clang_getCString(cxStr);
        clang_disposeString(cxStr);
        
        result = std::regex_replace(result, std::regex(R"(\s*\bstruct\b\s*)"), "");
        result = std::regex_replace(result, std::regex(R"(\s*\bclass\b\s*)"), "");
        
        result.erase(0, result.find_first_not_of(" \t"));
        result.erase(result.find_last_not_of(" \t") + 1);
        
        return result;
    }
    
    std::string getFileName(CXCursor cursor) const {
        CXSourceLocation location = clang_getCursorLocation(cursor);
        CXFile file;
        unsigned line, column, offset;
        clang_getExpansionLocation(location, &file, &line, &column, &offset);
        
        if (file) {
            CXString fileName = clang_getFileName(file);
            std::string result = clang_getCString(fileName);
            clang_disposeString(fileName);
            return result;
        }
        return "";
    }
    
    std::string getFullyQualifiedName(CXCursor cursor) const {
        std::vector<std::string> nameParts;
        
        CXCursor current = cursor;
        while (!clang_Cursor_isNull(current)) {
            CXCursorKind kind = clang_getCursorKind(current);
            
            if (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl || 
                kind == CXCursor_Namespace || kind == CXCursor_UnionDecl) {
                std::string name = getCursorSpelling(current);
                if (!name.empty()) {
                    nameParts.insert(nameParts.begin(), name);
                }
            }
            
            current = clang_getCursorSemanticParent(current);
            
            if (clang_getCursorKind(current) == CXCursor_TranslationUnit) {
                break;
            }
        }
        
        std::string result;
        for (size_t i = 0; i < nameParts.size(); ++i) {
            if (i > 0) result += "::";
            result += nameParts[i];
        }
        
        return result.empty() ? getCursorSpelling(cursor) : result;
    }
    
    JsonParameter parseParameter(CXCursor cursor) const {
        JsonParameter param;
        param.name = getCursorSpelling(cursor);
        param.type = getTypeSpelling(clang_getCursorType(cursor));
        return param;
    }
    
    JsonMethod parseMethod(CXCursor cursor) const {
        JsonMethod method;
        method.name = getCursorSpelling(cursor);
        
        CXType functionType = clang_getCursorType(cursor);
        CXType returnType = clang_getResultType(functionType);
        method.returnType = getTypeSpelling(returnType);
        
        method.isConst = clang_CXXMethod_isConst(cursor);
        method.isStatic = clang_CXXMethod_isStatic(cursor);
        method.isVirtual = clang_CXXMethod_isVirtual(cursor);
        
        CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);
        switch (access) {
            case CX_CXXPublic: method.visibility = "public"; break;
            case CX_CXXProtected: method.visibility = "protected"; break;
            case CX_CXXPrivate: method.visibility = "private"; break;
            default: method.visibility = "public"; break;
        }
        
        int numArgs = clang_Cursor_getNumArguments(cursor);
        for (int i = 0; i < numArgs; ++i) {
            CXCursor arg = clang_Cursor_getArgument(cursor, i);
            method.parameters.push_back(parseParameter(arg));
        }
        
        return method;
    }
    
public:
    ASTExtractor() {
        initializeFilters();
    }
    
    void setVerbose(bool v) { verbose = v; }
    
    void addPCLPath(const std::string& path) { 
        if (fs::exists(path)) {
            pclPaths.push_back(fs::canonical(path).string());
        }
    }
    
    std::unordered_map<std::string, JsonClassInfo> extractAST(const std::string& headerFile, 
                                                              const std::vector<std::string>& clangArgs) {
        
        std::unordered_map<std::string, JsonClassInfo> classes;
        
        CXIndex index = clang_createIndex(0, 0);
        if (!index) {
            std::cerr << "Failed to create clang index\n";
            return classes;
        }
        
        std::vector<const char*> args;
        for (const auto& arg : clangArgs) {
            args.push_back(arg.c_str());
        }
        
        if (verbose) {
            std::cout << "Parsing " << headerFile << " with " << args.size() << " arguments\n";
        }
        
        CXTranslationUnit unit = clang_parseTranslationUnit(
            index, headerFile.c_str(), args.data(), args.size(),
            nullptr, 0, CXTranslationUnit_DetailedPreprocessingRecord
        );
        
        if (!unit) {
            std::cerr << "Failed to parse translation unit\n";
            clang_disposeIndex(index);
            return classes;
        }
        
        CXCursor cursor = clang_getTranslationUnitCursor(unit);
        this->currentClasses = &classes;
        
        auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData clientData) -> CXChildVisitResult {
            auto* extractor = static_cast<ASTExtractor*>(clientData);
            return extractor->visitCursor(cursor, parent);
        };
        
        clang_visitChildren(cursor, visitor, this);
        
        // Apply pattern analysis
        for (auto& [name, classInfo] : classes) {
            analyzer.analyzeClass(classInfo);
        }
        
        clang_disposeTranslationUnit(unit);
        clang_disposeIndex(index);
        
        return classes;
    }
    
    void writeJsonFile(const std::string& filename, const std::unordered_map<std::string, JsonClassInfo>& classes) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open output file: " << filename << std::endl;
            return;
        }
        
        file << "{\n";
        file << "  \"classes\": {\n";
        
        bool first = true;
        for (const auto& [name, classInfo] : classes) {
            if (!first) file << ",\n";
            file << "    \"" << classInfo.qualifiedName << "\": " << classInfo.toJson();
            first = false;
        }
        
        file << "\n  }\n";
        file << "}\n";
        
        file.close();
        
        if (verbose) {
            std::cout << "Written " << classes.size() << " classes to " << filename << std::endl;
        }
    }
    
private:
    std::unordered_map<std::string, JsonClassInfo>* currentClasses = nullptr;
    
    CXChildVisitResult visitCursor(CXCursor cursor, CXCursor parent) {
        CXCursorKind kind = clang_getCursorKind(cursor);
        
        if (kind == CXCursor_StructDecl || kind == CXCursor_ClassDecl) {
            std::string name = getCursorSpelling(cursor);
            std::string fileName = getFileName(cursor);
            
            if (shouldSkipType(name, fileName)) {
                if (verbose && (name.find("sol") != std::string::npos || name.find("usertype") != std::string::npos)) {
                    std::cout << "SKIPPING Sol2 type: " << name << " from " << fileName << std::endl;
                }
                return CXChildVisit_Continue;
            }
            
            if (!clang_isCursorDefinition(cursor)) {
                return CXChildVisit_Continue;
            }
            
            std::string qualifiedName = getFullyQualifiedName(cursor);
            
            // Additional Sol2 filtering by qualified name
            if (qualifiedName.find("sol::") != std::string::npos || 
                qualifiedName.find("lua") != std::string::npos) {
                if (verbose) {
                    std::cout << "SKIPPING Sol2 qualified type: " << qualifiedName << std::endl;
                }
                return CXChildVisit_Continue;
            }
            
            JsonClassInfo classInfo;
            classInfo.name = name;
            classInfo.qualifiedName = qualifiedName;
            classInfo.fileName = fileName;
            classInfo.isStruct = (kind == CXCursor_StructDecl);
            
            visitClassMembers(cursor, classInfo);
            
            if (!classInfo.methods.empty() || !classInfo.fields.empty() || !classInfo.constructors.empty()) {
                if (verbose) {
                    std::cout << "Parsed class: " << qualifiedName << " (methods: " 
                              << classInfo.methods.size() << ", fields: " << classInfo.fields.size() << ")\n";
                }
                (*currentClasses)[qualifiedName] = std::move(classInfo);
            }
        }
        
        return CXChildVisit_Recurse;
    }
    
    void visitClassMembers(CXCursor cursor, JsonClassInfo& classInfo) {
        auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData clientData) -> CXChildVisitResult {
            auto* data = static_cast<std::pair<ASTExtractor*, JsonClassInfo*>*>(clientData);
            ASTExtractor* extractor = data->first;
            JsonClassInfo* classInfo = data->second;
            
            CXCursorKind kind = clang_getCursorKind(cursor);
            
            switch (kind) {
                case CXCursor_FieldDecl: {
                    JsonField field;
                    field.name = extractor->getCursorSpelling(cursor);
                    field.type = extractor->getTypeSpelling(clang_getCursorType(cursor));
                    field.isStatic = clang_CXXMethod_isStatic(cursor);
                    
                    CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);
                    switch (access) {
                        case CX_CXXPublic: field.visibility = "public"; break;
                        case CX_CXXProtected: field.visibility = "protected"; break;
                        case CX_CXXPrivate: field.visibility = "private"; break;
                        default: field.visibility = classInfo->isStruct ? "public" : "private"; break;
                    }
                    
                    if (field.visibility == "public") {
                        classInfo->fields.push_back(std::move(field));
                    }
                    break;
                }
                case CXCursor_CXXMethod: {
                    JsonMethod method = extractor->parseMethod(cursor);
                    if (method.visibility == "public" && !method.name.empty()) {
                        classInfo->methods.push_back(std::move(method));
                    }
                    break;
                }
                case CXCursor_Constructor: {
                    JsonMethod ctor = extractor->parseMethod(cursor);
                    ctor.isConstructor = true;
                    if (ctor.visibility == "public") {
                        classInfo->constructors.push_back(std::move(ctor));
                    }
                    break;
                }
                case CXCursor_CXXBaseSpecifier: {
                    std::string baseName = extractor->getTypeSpelling(clang_getCursorType(cursor));
                    classInfo->baseClasses.push_back(baseName);
                    if (baseName.find("QObject") != std::string::npos) {
                        classInfo->hasQObject = true;
                    }
                    break;
                }
                default:
                    break;
            }
            
            return CXChildVisit_Continue;
        };
        
        std::pair<ASTExtractor*, JsonClassInfo*> data{this, &classInfo};
        clang_visitChildren(cursor, visitor, &data);
    }
};

// Enhanced compile flags extraction
std::vector<std::string> extractCompileFlags(const std::string& compileCommandsPath, 
                                           const std::string& headerFile, 
                                           const std::vector<std::string>& additionalPCLPaths = {},
                                           bool verbose = false) {
    std::vector<std::string> flags;
    
    if (!compileCommandsPath.empty() && fs::exists(compileCommandsPath)) {
        std::ifstream file(compileCommandsPath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            
            std::regex flagRegex(R"((-I\S+|-isystem\s+\S+|-D\S+|-std=\S+))");
            std::smatch match;
            
            auto searchStart = content.cbegin();
            while (std::regex_search(searchStart, content.cend(), match, flagRegex)) {
                std::string flag = match[0];
                if (flag.find("-isystem ") == 0) {
                    flags.push_back("-isystem");
                    flags.push_back(flag.substr(9));
                } else if (flag == "-std=gnu++17") {
                    flags.push_back("-std=c++17");
                } else {
                    flags.push_back(flag);
                }
                searchStart = match.suffix().first;
            }
        }
    }
    
    // Ensure essential flags
    bool hasStd = false, hasXCpp = false;
    for (const auto& flag : flags) {
        if (flag.find("-std=") == 0) hasStd = true;
        if (flag == "-x") hasXCpp = true;
    }
    
    if (!hasStd) flags.push_back("-std=c++17");
    if (!hasXCpp) {
        flags.insert(flags.begin(), "-x");
        flags.insert(flags.begin() + 1, "c++");
    }
    
    // Add custom PCL paths first
    for (const auto& pclPath : additionalPCLPaths) {
        if (fs::exists(pclPath)) {
            flags.insert(flags.begin(), "-I" + pclPath);
            if (verbose) {
                std::cout << "Added custom PCL path: " << pclPath << std::endl;
            }
        }
    }
    
    // Auto-detect PCL paths
    std::vector<std::string> commonPCLPaths = {
        "/usr/local/include/pcl-1.14",
        "/usr/local/include/pcl-1.13",
        "/usr/local/include/pcl-1.12",
        "/opt/homebrew/include/pcl-1.14",
        "/opt/homebrew/include/pcl-1.13", 
        "/opt/homebrew/include/pcl-1.12"
    };
    
    for (const auto& pclPath : commonPCLPaths) {
        if (fs::exists(pclPath)) {
            bool alreadyAdded = false;
            for (const auto& existing : flags) {
                if (existing.find(pclPath) != std::string::npos) {
                    alreadyAdded = true;
                    break;
                }
            }
            if (!alreadyAdded) {
                flags.insert(flags.begin(), "-I" + pclPath);
                if (verbose) {
                    std::cout << "Auto-detected PCL: " << pclPath << std::endl;
                }
                break;
            }
        }
    }
    
    // Add Eigen
    std::vector<std::string> eigenPaths = {
        "/opt/homebrew/include/eigen3",
        "/usr/local/include/eigen3"
    };
    
    for (const auto& eigenPath : eigenPaths) {
        if (fs::exists(eigenPath)) {
            flags.insert(flags.begin(), "-I" + eigenPath);
            if (verbose) {
                std::cout << "Added Eigen: " << eigenPath << std::endl;
            }
            break;
        }
    }
    
    // Add Boost
    std::vector<std::string> boostPaths = {
        "/opt/homebrew/include",
        "/usr/local/include"
    };
    
    for (const auto& boostPath : boostPaths) {
        if (fs::exists(boostPath + "/boost")) {
            flags.insert(flags.begin(), "-I" + boostPath);
            break;
        }
    }
    
    // PCL-specific defines
    flags.push_back("-DPCL_NO_PRECOMPILE");
    flags.push_back("-DEIGEN_USE_NEW_STDVECTOR");
    
    #ifdef __APPLE__
    flags.push_back("-D__APPLE__");
    flags.push_back("-D__PCL_MACOSX");
    flags.push_back("-D__PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES");
    flags.push_back("-DACCELERATE_NEW_LAPACK");
    flags.push_back("-Wno-dangling-else");
    #endif
    
    return flags;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <header_file> [options]\n";
        std::cout << "Options:\n";
        std::cout << "  -c <compile_commands.json>  Use compile commands\n";
        std::cout << "  -o <output_file.json>      JSON output file (required)\n";
        std::cout << "  -v                         Verbose output\n";
        std::cout << "  --pcl-path <path>          Add custom PCL include path\n";
        return 1;
    }
    
    std::string headerFile = argv[1];
    std::string compileCommandsPath;
    std::string outputFile;
    std::vector<std::string> customPCLPaths;
    bool verbose = false;
    
    // Parse arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-c" && i + 1 < argc) {
            compileCommandsPath = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-v") {
            verbose = true;
        } else if (arg == "--pcl-path" && i + 1 < argc) {
            customPCLPaths.push_back(argv[++i]);
        }
    }
    
    if (outputFile.empty()) {
        std::cerr << "Error: Output file (-o) is required\n";
        return 1;
    }
    
    // Create AST extractor
    ASTExtractor extractor;
    extractor.setVerbose(verbose);
    
    for (const auto& path : customPCLPaths) {
        extractor.addPCLPath(path);
    }
    
    // Extract compile flags
    std::vector<std::string> clangArgs = extractCompileFlags(compileCommandsPath, headerFile, customPCLPaths, verbose);
    
    if (verbose) {
        std::cout << "Stage 1: AST Extraction\n";
        std::cout << "Parsing " << headerFile << "...\n";
    }
    
    // Extract AST
    auto classes = extractor.extractAST(headerFile, clangArgs);
    
    if (classes.empty()) {
        std::cout << "No bindable classes found!\n";
        return 1;
    }
    
    std::cout << "Found " << classes.size() << " classes with patterns:\n";
    for (const auto& [name, classInfo] : classes) {
        std::cout << "  " << name;
        if (classInfo.isPCLClass) std::cout << " [PCL]";
        if (classInfo.isMetadata) std::cout << " [METADATA]";
        if (classInfo.isAlgorithm) std::cout << " [ALGORITHM]";
        if (classInfo.hasQObject) std::cout << " [QOBJECT]";
        std::cout << " (" << classInfo.methods.size() << " methods";
        
        // Count metamethods
        int metamethodCount = 0;
        for (const auto& method : classInfo.methods) {
            if (method.isMetamethod) metamethodCount++;
        }
        if (metamethodCount > 0) {
            std::cout << ", " << metamethodCount << " operators";
        }
        std::cout << ")\n";
        
        // Show important methods for AstrometricMetadata
        if (name.find("AstrometricMetadata") != std::string::npos && verbose) {
            std::vector<std::string> foundMethods;
            for (const auto& method : classInfo.methods) {
                if (method.name == "Build" || method.name == "IsValid" || 
                    method.name == "ImageToCelestial" || method.name == "Summary") {
                    foundMethods.push_back(method.name);
                }
            }
            if (!foundMethods.empty()) {
                std::cout << "    Key methods found: ";
                for (size_t i = 0; i < foundMethods.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << foundMethods[i];
                }
                std::cout << "\n";
            }
        }
    }
    
    // Write JSON file
    extractor.writeJsonFile(outputFile, classes);
    std::cout << "AST extracted to " << outputFile << std::endl;
    std::cout << "Use the binding generator to convert JSON to Lua bindings.\n";
    
    return 0;
}
                