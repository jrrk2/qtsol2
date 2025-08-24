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

namespace fs = std::filesystem;

struct FieldInfo {
    std::string name;
    std::string type;
    std::string defaultValue;
};

struct MethodInfo {
    std::string name;
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> parameters; // (type, name)
    bool isConst = false;
    bool isStatic = false;
    bool needsLambda = false;
    std::string signature; // For overload disambiguation
};

struct ConstructorInfo {
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string signature;
};

struct TypeInfo {
    std::string name;
    std::string fileName;
    bool isStruct = false;
    bool hasQObject = false;
    std::vector<FieldInfo> fields;
    std::vector<MethodInfo> methods;
    std::vector<ConstructorInfo> constructors;
};

class BindingGenerator {
private:
    std::unordered_set<std::string> systemTypes;
    std::unordered_set<std::string> skipPrefixes;
    std::string targetHeader;
    std::string projectPath;
    std::vector<std::string> pclPaths;  // NEW: Custom PCL paths
    bool verbose = false;
    
    void initializeFilters() {
        // System types to skip
        systemTypes = {
            "type", "pair", "numeric_limits", "sigevent", "sigaction", "sigvec",
            "sigstack", "timeval", "rusage", "timespec", "tm", "exception",
            "bad_exception", "hash", "ratio", "duration", "atomic", "mutex",
            "vector", "array", "string", "locale", "ios_base", "streambuf",
            "basic_string", "basic_ostream", "basic_istream", "fpos", "locale",
            "collate", "ctype_base", "codecvt_base", "error_category", "error_code",
            "error_condition", "system_error", "shared_ptr", "unique_ptr", "weak_ptr",
            "function", "bind", "tuple", "variant", "optional", "any",
            // Qt internal types
            "QMetaObject", "QObject", "QPrivateSignal", "QObjectData", "QFlags",
            "QFlag", "QAtomicInt", "QAtomicPointer", "QMutex", "QBasicAtomicInteger",
            // PCL internal types - expanded list
            "PCL_AssertScalarSizes", "PCL_AssertComplexSize", "PCL_AssertTimePointSize",
            "PCL_AssertVariantSize", "GenericVector", "GenericMatrix", "GenericImage",
            "Array", "SortedArray", "ReferenceArray", "IndirectArray",
            "PCLBase", "PCLException", "PCLPointCloud2", "IndicesPtr",
            "PointIndicesPtr", "ModelCoefficientsPtr"
        };
    
        // Prefixes to skip
        skipPrefixes = {
            "std::", "__", "_", "std_", "boost::", "detail::", "internal::",
            "pcl::detail::", "pcl::io::", "pcl::console::"  // PCL internal namespaces
        };
    }
    
    bool shouldSkipType(const std::string& name, const std::string& fileName) const {
        if (name.empty() || name.length() <= 1 || name.length() > 50) {
            return true;
        }
        
        // Skip prefixes
        for (const auto& prefix : skipPrefixes) {
            if (name.length() >= prefix.length() && 
                name.substr(0, prefix.length()) == prefix) {
                return true;
            }
        }
        
        // Check system types
        if (systemTypes.count(name)) {
            return true;
        }
        
        // Skip template instantiations with system types
        if (name.find('<') != std::string::npos) {
            for (const auto& prefix : {"std::", "boost::", "detail::"}) {
                if (name.find(prefix) != std::string::npos) {
                    return true;
                }
            }
        }
        
        // Skip system headers
        std::vector<std::string> systemPaths = {
            "/usr/", "/opt/", "/Applications/Xcode.app/", "/Library/", "/System/"
        };
        
        for (const auto& sysPath : systemPaths) {
            if (fileName.length() >= sysPath.length() && 
                fileName.substr(0, sysPath.length()) == sysPath) {
                return true;
            }
        }
        
        // Skip Qt framework internal headers
        if (fileName.find("/Qt") != std::string::npos) {
            if (fileName.find("private") != std::string::npos ||
                fileName.find("_p.h") != std::string::npos ||
                fileName.find("internal") != std::string::npos ||
                fileName.find(".framework/Versions") != std::string::npos) {
                return true;
            }
        }
        
        // NEW: Enhanced PCL path handling
        if (fileName.find("/pcl/") != std::string::npos || fileName.find("\\pcl\\") != std::string::npos) {
            // Skip PCL internal/detail headers
            if (fileName.find("/detail/") != std::string::npos ||
                fileName.find("/impl/") != std::string::npos ||
                fileName.find("/internal/") != std::string::npos ||
                fileName.find("_impl.h") != std::string::npos ||
                fileName.find("_detail.h") != std::string::npos) {
                return true;
            }
            
            // If we have custom PCL paths, only allow files from those paths
            if (!pclPaths.empty()) {
                bool foundInCustomPaths = false;
                std::string canonicalFileName;
                
                try {
                    canonicalFileName = fs::canonical(fileName).string();
                } catch (...) {
                    canonicalFileName = fileName;
                }
                
                for (const auto& pclPath : pclPaths) {
                    if (canonicalFileName.find(pclPath) == 0) {
                        foundInCustomPaths = true;
                        break;
                    }
                }
                
                if (!foundInCustomPaths) {
                    if (verbose) {
                        std::cout << "Skipping PCL file not in custom paths: " << fileName << std::endl;
                    }
                    return true;
                }
            }
        }
        
        // If we have a target header, prefer types from that file or related files
        if (!targetHeader.empty()) {
            fs::path targetPath(targetHeader);
            fs::path filePath(fileName);
            
            std::string targetName = targetPath.filename().string();
            std::string currentName = filePath.filename().string();
            
            // Remove extension for comparison
            auto dotPos = targetName.find_last_of('.');
            if (dotPos != std::string::npos) {
                targetName = targetName.substr(0, dotPos);
            }
            
            if (fileName != targetHeader && 
                currentName.find(targetName) == std::string::npos) {
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
        
        // Clean up type string
        result = std::regex_replace(result, std::regex(R"(\s*\bstruct\b\s*)"), "");
        result = std::regex_replace(result, std::regex(R"(\s*\bclass\b\s*)"), "");
        
        // Ensure PCL types are fully qualified
        if (result.find("AstrometricMetadata") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bAstrometricMetadata\b)"), "pcl::AstrometricMetadata");
        }
        if (result.find("ProjectionBase") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bProjectionBase\b)"), "pcl::ProjectionBase");
        }
        if (result.find("WorldTransformation") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bWorldTransformation\b)"), "pcl::WorldTransformation");
        }
        if (result.find("ImageWindow") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bImageWindow\b)"), "pcl::ImageWindow");
        }
        if (result.find("XISFReader") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bXISFReader\b)"), "pcl::XISFReader");
        }
        if (result.find("XISFWriter") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bXISFWriter\b)"), "pcl::XISFWriter");
        }
        if (result.find("WCSKeywords") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bWCSKeywords\b)"), "pcl::WCSKeywords");
        }
        if (result.find("PropertyArray") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bPropertyArray\b)"), "pcl::PropertyArray");
        }
        if (result.find("FITSKeywordArray") != std::string::npos && 
            result.find("pcl::") == std::string::npos) {
            result = std::regex_replace(result, std::regex(R"(\bFITSKeywordArray\b)"), "pcl::FITSKeywordArray");
        }
        
        // Trim whitespace
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
    
    bool isPublicAccess(CXCursor cursor) const {
        CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);
        return access == CX_CXXPublic;
    }
    
    std::string generateSignature(const std::vector<std::pair<std::string, std::string>>& params) const {
        std::string sig = "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) sig += ", ";
            sig += params[i].first;
        }
        sig += ")";
        return sig;
    }
    
    MethodInfo parseMethod(CXCursor cursor) const {
        MethodInfo method;
        method.name = getCursorSpelling(cursor);
        
        CXType functionType = clang_getCursorType(cursor);
        CXType returnType = clang_getResultType(functionType);
        method.returnType = getTypeSpelling(returnType);
        
        // Check if const
        method.isConst = clang_CXXMethod_isConst(cursor);
        method.isStatic = clang_CXXMethod_isStatic(cursor);
        
        // Parse parameters
        int numArgs = clang_Cursor_getNumArguments(cursor);
        for (int i = 0; i < numArgs; ++i) {
            CXCursor arg = clang_Cursor_getArgument(cursor, i);
            std::string paramName = getCursorSpelling(arg);
            std::string paramType = getTypeSpelling(clang_getCursorType(arg));
            method.parameters.emplace_back(paramType, paramName);
        }
        
        // Generate signature for overload resolution
        method.signature = generateSignature(method.parameters);
        if (method.isConst) method.signature += " const";
        
        // Mark if this method needs special lambda treatment
        method.needsLambda = needsSpecialLambda(method);
        
        return method;
    }
    
    bool needsSpecialLambda(const MethodInfo& method) const {
        // Static methods always need lambdas (can't use member function pointers)
        if (method.isStatic) {
            return true;
        }
        
        // Check for methods that need Qt type conversions
        for (const auto& [paramType, paramName] : method.parameters) {
            // QVector parameters need Lua table conversion
            if (paramType.find("QVector") != std::string::npos) {
                return true;
            }
            // QString parameters need std::string conversion
            if (paramType.find("QString") != std::string::npos) {
                return true;
            }
            // Qt container types
            if (paramType.find("QList") != std::string::npos ||
                paramType.find("QStringList") != std::string::npos) {
                return true;
            }
            // Qt geometry types might need conversion
            if (paramType.find("QPoint") != std::string::npos || 
                paramType.find("QSize") != std::string::npos ||
                paramType.find("QRect") != std::string::npos ||
                paramType.find("QColor") != std::string::npos) {
                return true;
            }
            // sol::table or sol::function parameters
            if (paramType.find("sol::") != std::string::npos) {
                return true;
            }
            // PCL specific types that need conversion
            if (paramType.find("PointCloud") != std::string::npos ||
                paramType.find("IndicesPtr") != std::string::npos ||
                paramType.find("ModelCoefficients") != std::string::npos) {
                return true;
            }
            // PCL String needs special handling
            if (paramType.find("pcl::String") != std::string::npos) {
                return true;
            }
            // PropertyArray needs special handling
            if (paramType.find("PropertyArray") != std::string::npos) {
                return true;
            }
        }
        
        // QString return types need conversion
        if (method.returnType.find("QString") != std::string::npos) {
            return true;
        }
        
        // PCL String return types need conversion
        if (method.returnType.find("pcl::String") != std::string::npos) {
            return true;
        }
        
        // Methods that typically need special handling
        if (method.name.find("setStarOverlay") != std::string::npos ||
            method.name.find("setWindowTitle") != std::string::npos ||
            method.name.find("invokeMethod") != std::string::npos ||
            method.name.find("zoom") != std::string::npos ||
            method.name.find("setInputCloud") != std::string::npos ||
            method.name.find("setIndices") != std::string::npos ||
            method.name.find("filter") != std::string::npos ||
            method.name.find("segment") != std::string::npos ||
            method.name.find("extract") != std::string::npos) {
            return true;
        }
        
        return false;
    }
    
    ConstructorInfo parseConstructor(CXCursor cursor) const {
        ConstructorInfo ctor;
        
        int numArgs = clang_Cursor_getNumArguments(cursor);
        for (int i = 0; i < numArgs; ++i) {
            CXCursor arg = clang_Cursor_getArgument(cursor, i);
            std::string paramName = getCursorSpelling(arg);
            std::string paramType = getTypeSpelling(clang_getCursorType(arg));
            ctor.parameters.emplace_back(paramType, paramName);
        }
        
        ctor.signature = generateSignature(ctor.parameters);
        
        return ctor;
    }
    
    FieldInfo parseField(CXCursor cursor) const {
        FieldInfo field;
        field.name = getCursorSpelling(cursor);
        field.type = getTypeSpelling(clang_getCursorType(cursor));
        // TODO: Extract default values if needed
        return field;
    }
    
public:
    BindingGenerator(const std::string& target = "", const std::string& project = "") 
        : targetHeader(target), projectPath(project) {
        initializeFilters();
    }
    
    void setVerbose(bool v) { verbose = v; }
    
    // NEW: Method to add custom PCL paths
    void addPCLPath(const std::string& path) {
        if (fs::exists(path)) {
            pclPaths.push_back(fs::canonical(path).string());
            if (verbose) {
                std::cout << "Added PCL path: " << pclPaths.back() << std::endl;
            }
        } else if (verbose) {
            std::cout << "Warning: PCL path does not exist: " << path << std::endl;
        }
    }
    
    // NEW: Method to add multiple PCL paths
    void addPCLPaths(const std::vector<std::string>& paths) {
        for (const auto& path : paths) {
            addPCLPath(path);
        }
    }
    
    // NEW: Auto-detect common PCL installation paths
    void autoDetectPCLPaths() {
        std::vector<std::string> commonPCLPaths = {
            "/usr/local/include/pcl-1.12",
            "/usr/local/include/pcl-1.13",
            "/usr/local/include/pcl-1.14",
            "/opt/homebrew/include/pcl-1.12",
            "/opt/homebrew/include/pcl-1.13",
            "/opt/homebrew/include/pcl-1.14",
            "/usr/include/pcl-1.12",
            "/usr/include/pcl-1.13",
            "/usr/include/pcl-1.14",
            // Windows paths
            "C:/Program Files/PCL 1.12.1/include/pcl-1.12",
            "C:/Program Files/PCL 1.13.1/include/pcl-1.13",
            // Custom build paths
            "/opt/pcl/include",
            "/usr/local/pcl/include"
        };
        
        for (const auto& path : commonPCLPaths) {
            if (fs::exists(path)) {
                addPCLPath(path);
            }
        }
    }
    
    std::unordered_map<std::string, TypeInfo> parseHeader(const std::string& headerFile, 
                                                         const std::vector<std::string>& clangArgs) {
        
        std::unordered_map<std::string, TypeInfo> types;
        
        CXIndex index = clang_createIndex(0, 0);
        if (!index) {
            std::cerr << "Failed to create clang index\n";
            return types;
        }
        
        // Convert string args to char* array
        std::vector<const char*> args;
        for (const auto& arg : clangArgs) {
            args.push_back(arg.c_str());
        }
        
        if (verbose) {
            std::cout << "Clang arguments (" << args.size() << " total):\n";
            for (const auto& arg : clangArgs) {
                std::cout << "  " << arg << "\n";
            }
            std::cout << "Parsing file: " << headerFile << "\n";
            
            std::cout << "\n=== MANUAL CLANG COMMAND ===\n";
            std::cout << "You can test this manually with:\n\n";
            std::cout << "# For .h files, force C++ mode:\n";
            std::cout << "clang -fsyntax-only -x c++";
            for (const auto& arg : clangArgs) {
                // Skip problematic flags and -x c++ if already present
                if (arg != "-x" && arg != "c++" && 
                    arg.find("-arch") != 0 && arg.find("-stdlib=") != 0) {
                    std::cout << " " << arg;
                }
            }
            std::cout << " " << headerFile << "\n\n";
            
            // Also provide a simple test command
            std::cout << "Or test basic parsing with:\n";
            std::cout << "clang -fsyntax-only -x c++ -std=c++17 -I/Users/jonathan/PCL/include ";
            std::cout << "-I/opt/homebrew/include/eigen3 -I/opt/homebrew/include ";
            std::cout << "-DPCL_NO_PRECOMPILE -D__PCL_MACOSX ";
            std::cout << "-D__PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES ";
            std::cout << "-DACCELERATE_NEW_LAPACK -Wno-dangling-else " << headerFile << "\n";
            std::cout << "================================\n\n";
        }
        
        CXTranslationUnit unit = clang_parseTranslationUnit(
            index, headerFile.c_str(), args.data(), args.size(),
            nullptr, 0, CXTranslationUnit_DetailedPreprocessingRecord
        );
        
        if (!unit) {
            std::cerr << "Failed to parse translation unit\n";
            
            // Try with more lenient parsing options
            if (verbose) {
                std::cout << "Retrying with lenient parsing options...\n";
            }
            
            unit = clang_parseTranslationUnit(
                index, headerFile.c_str(), args.data(), args.size(),
                nullptr, 0, 
                CXTranslationUnit_SkipFunctionBodies | 
                CXTranslationUnit_Incomplete |
                CXTranslationUnit_IgnoreNonErrorsFromIncludedFiles
            );
            
            if (!unit) {
                // Try with even more basic options
                if (verbose) {
                    std::cout << "Retrying with basic parsing options...\n";
                }
                
                unit = clang_parseTranslationUnit(
                    index, headerFile.c_str(), args.data(), args.size(),
                    nullptr, 0, CXTranslationUnit_SkipFunctionBodies
                );
            }
            
            if (!unit) {
                std::cerr << "Failed to parse translation unit even with lenient options\n";
                
                if (verbose) {
                    std::cout << "\n=== DEBUGGING SUGGESTIONS ===\n";
                    std::cout << "1. Try the manual clang command shown above\n";
                    std::cout << "2. Check if the header exists: ls -la " << headerFile << "\n";
                    std::cout << "3. Verify dependencies:\n";
                    std::cout << "   - Eigen3: ls /opt/homebrew/include/eigen3/Eigen/Core\n";
                    std::cout << "   - Boost: ls /opt/homebrew/include/boost/version.hpp\n";
                    std::cout << "   - PCL: ls /Users/jonathan/PCL/include/pcl/\n";
                    std::cout << "4. Try a simpler header first\n";
                    std::cout << "5. Check compile_commands.json format\n";
                    std::cout << "==============================\n";
                }
                
                clang_disposeIndex(index);
                return types;
            }
        }
        
        // Get diagnostics
        unsigned numDiagnostics = clang_getNumDiagnostics(unit);
        if (numDiagnostics > 0) {
            std::cout << "Clang diagnostics (" << numDiagnostics << " issues):\n";
            for (unsigned i = 0; i < numDiagnostics; ++i) {
                CXDiagnostic diag = clang_getDiagnostic(unit, i);
                CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diag);
                CXString diagStr = clang_formatDiagnostic(diag, CXDiagnostic_DisplaySourceLocation);
                
                const char* severityStr = "Unknown";
                switch (severity) {
                    case CXDiagnostic_Ignored: severityStr = "Ignored"; break;
                    case CXDiagnostic_Note: severityStr = "Note"; break;
                    case CXDiagnostic_Warning: severityStr = "Warning"; break;
                    case CXDiagnostic_Error: severityStr = "Error"; break;
                    case CXDiagnostic_Fatal: severityStr = "Fatal"; break;
                }
                
                std::cout << "  [" << severityStr << "] " << clang_getCString(diagStr) << "\n";
                clang_disposeString(diagStr);
                clang_disposeDiagnostic(diag);
            }
        }
        
        CXCursor cursor = clang_getTranslationUnitCursor(unit);
        
        // Visitor function
        auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData clientData) -> CXChildVisitResult {
            auto* generator = static_cast<BindingGenerator*>(clientData);
            return generator->visitCursor(cursor, parent);
        };
        
        // Store types reference for visitor
        this->currentTypes = &types;
        
        clang_visitChildren(cursor, visitor, this);
        
        clang_disposeTranslationUnit(unit);
        clang_disposeIndex(index);
        
        return types;
    }
    
private:
    std::unordered_map<std::string, TypeInfo>* currentTypes = nullptr;
    
    CXChildVisitResult visitCursor(CXCursor cursor, CXCursor parent) {
        CXCursorKind kind = clang_getCursorKind(cursor);
        
        if (kind == CXCursor_StructDecl || kind == CXCursor_ClassDecl) {
            std::string name = getCursorSpelling(cursor);
            std::string fileName = getFileName(cursor);
            
            if (shouldSkipType(name, fileName)) {
                return CXChildVisit_Continue;
            }
            
            // Get the fully qualified name for nested types
            std::string qualifiedName = getFullyQualifiedName(cursor);
            
            if (verbose) {
                std::cout << "Processing type: " << name << " (qualified: " << qualifiedName << ") from " << fileName;
                if (!clang_isCursorDefinition(cursor)) {
                    std::cout << " (declaration only - skipping)";
                }
                std::cout << "\n";
            }
            
            // Check if this is a complete definition
            if (!clang_isCursorDefinition(cursor)) {
                return CXChildVisit_Continue;
            }
            
            TypeInfo typeInfo;
            typeInfo.name = qualifiedName;  // Use qualified name instead of simple name
            typeInfo.fileName = fileName;
            typeInfo.isStruct = (kind == CXCursor_StructDecl);
            
            // Parse members
            visitTypeMembers(cursor, typeInfo);
            
            // Only add if it has bindable content
            if (!typeInfo.fields.empty() || !typeInfo.methods.empty() || !typeInfo.constructors.empty()) {
                if (verbose) {
                    std::cout << "  -> Added " << qualifiedName << " (fields:" << typeInfo.fields.size() 
                              << ", methods:" << typeInfo.methods.size() 
                              << ", constructors:" << typeInfo.constructors.size() << ")\n";
                }
                (*currentTypes)[qualifiedName] = std::move(typeInfo);
            } else if (verbose) {
                std::cout << "  -> Skipped " << qualifiedName << " (no bindable content)\n";
            }
        }
        
        return CXChildVisit_Recurse;
    }
    
    // NEW: Get fully qualified name for nested types
    std::string getFullyQualifiedName(CXCursor cursor) const {
        std::vector<std::string> nameParts;
        
        CXCursor current = cursor;
        while (!clang_Cursor_isNull(current)) {
            CXCursorKind kind = clang_getCursorKind(current);
            
            // Add this level's name if it's a type or namespace
            if (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl || 
                kind == CXCursor_Namespace || kind == CXCursor_UnionDecl) {
                std::string name = getCursorSpelling(current);
                if (!name.empty()) {
                    nameParts.insert(nameParts.begin(), name);
                }
            }
            
            // Move to parent
            current = clang_getCursorSemanticParent(current);
            
            // Stop at translation unit
            if (clang_getCursorKind(current) == CXCursor_TranslationUnit) {
                break;
            }
        }
        
        // Join with "::"
        std::string result;
        for (size_t i = 0; i < nameParts.size(); ++i) {
            if (i > 0) result += "::";
            result += nameParts[i];
        }
        
        return result.empty() ? getCursorSpelling(cursor) : result;
    }
    
    void visitTypeMembers(CXCursor cursor, TypeInfo& typeInfo) {
        auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData clientData) -> CXChildVisitResult {
            auto* data = static_cast<std::pair<BindingGenerator*, TypeInfo*>*>(clientData);
            BindingGenerator* generator = data->first;
            TypeInfo* typeInfo = data->second;
            
            CXCursorKind kind = clang_getCursorKind(cursor);
            
            // Only process public members for classes (structs are public by default)
            if (!typeInfo->isStruct && !generator->isPublicAccess(cursor)) {
                return CXChildVisit_Continue;
            }
            
            switch (kind) {
                case CXCursor_FieldDecl: {
                    FieldInfo field = generator->parseField(cursor);
                    if (!field.name.empty() && field.name.substr(0, 1) != "_") {
                        typeInfo->fields.push_back(std::move(field));
                    }
                    break;
                }
                case CXCursor_CXXMethod: {
                    MethodInfo method = generator->parseMethod(cursor);
                    if (!method.name.empty() && method.name.substr(0, 1) != "_" &&
                        method.name != "metaObject" && method.name != "qt_metacast" &&
                        method.name != "qt_metacall" && method.name != "staticMetaObject") {
                        typeInfo->methods.push_back(std::move(method));
                    }
                    break;
                }
                case CXCursor_Constructor: {
                    ConstructorInfo ctor = generator->parseConstructor(cursor);
                    typeInfo->constructors.push_back(std::move(ctor));
                    break;
                }
                case CXCursor_CXXBaseSpecifier: {
                    std::string baseName = generator->getTypeSpelling(clang_getCursorType(cursor));
                    if (baseName.find("QObject") != std::string::npos) {
                        typeInfo->hasQObject = true;
                    }
                    break;
                }
                default:
                    // Ignore other cursor types
                    break;
            }
            
            return CXChildVisit_Continue;
        };
        
        std::pair<BindingGenerator*, TypeInfo*> data{this, &typeInfo};
        clang_visitChildren(cursor, visitor, &data);
    }
    
public:
    std::string generateBindings(const std::unordered_map<std::string, TypeInfo>& types) const {
        std::ostringstream output;
        
        output << "// Generated Sol2 Lua bindings\n";
        output << "// Generated types: ";
        bool first = true;
        for (const auto& [name, info] : types) {
            if (!first) output << ", ";
            // Use simple name for the comment
            size_t lastColon = name.find_last_of(':');
            std::string simpleName = (lastColon != std::string::npos) ? name.substr(lastColon + 1) : name;
            output << simpleName;
            first = false;
        }
        output << "\n\n";
        
        for (const auto& [name, typeInfo] : types) {
            if (typeInfo.isStruct && !typeInfo.fields.empty()) {
                output << generateStructBinding(typeInfo) << "\n\n";
            } else if (!typeInfo.isStruct && (!typeInfo.methods.empty() || !typeInfo.constructors.empty())) {
                output << generateClassBinding(typeInfo) << "\n\n";
            }
        }
        
        return output.str();
    }
    
private:
    std::string generateStructBinding(const TypeInfo& typeInfo) const {
        std::ostringstream output;
        
        // Extract simple name for Lua binding name
        size_t lastColon = typeInfo.name.find_last_of(':');
        std::string simpleName = (lastColon != std::string::npos) ? typeInfo.name.substr(lastColon + 1) : typeInfo.name;
        
        output << "    // Bind " << typeInfo.name << " structure\n";
        
        // For PCL structs, disable automagic features that cause issues
        bool isPCLStruct = typeInfo.name.find("pcl::") == 0;
        if (isPCLStruct) {
            output << "    lua->new_usertype<" << typeInfo.name << ">(\"" << simpleName << "\",\n";
            output << "        sol::no_constructor,\n";  // Disable automatic constructors
            
            // Add field bindings
            for (const auto& field : typeInfo.fields) {
                // Skip pcl::String fields as they cause Sol2 issues
                if (field.type.find("pcl::String") != std::string::npos) {
                    continue;
                }
                output << "        \"" << field.name << "\", &" << typeInfo.name << "::" << field.name;
                if (&field != &typeInfo.fields.back()) {
                    output << ",\n";
                }
            }
        } else {
            output << "    lua->new_usertype<" << typeInfo.name << ">(\"" << simpleName << "\",\n";
            
            std::vector<std::string> bindings;
            
            // Add field bindings
            for (const auto& field : typeInfo.fields) {
                bindings.push_back("        \"" + field.name + "\", &" + typeInfo.name + "::" + field.name);
            }
            
            if (!bindings.empty()) {
                output << bindings[0];
                for (size_t i = 1; i < bindings.size(); ++i) {
                    output << ",\n" << bindings[i];
                }
            }
        }
        
        output << "\n    );";
        
        return output.str();
    }
    
    std::string generateClassBinding(const TypeInfo& typeInfo) const {
        std::ostringstream output;
        
        // Extract simple name for Lua binding name
        size_t lastColon = typeInfo.name.find_last_of(':');
        std::string simpleName = (lastColon != std::string::npos) ? typeInfo.name.substr(lastColon + 1) : typeInfo.name;
        
        output << "    // Bind " << typeInfo.name << " class\n";
        output << "    lua->new_usertype<" << typeInfo.name << ">(\"" << simpleName << "\",\n";
        
        std::vector<std::string> bindings;
        
        // Add constructors (simplified approach)
        if (!typeInfo.constructors.empty()) {
            // Just use sol::constructors with no parameters for default constructor
            bool hasDefaultConstructor = false;
            for (const auto& ctor : typeInfo.constructors) {
                if (ctor.parameters.empty()) {
                    hasDefaultConstructor = true;
                    break;
                }
            }
            
            if (hasDefaultConstructor) {
                bindings.push_back("        sol::constructors<" + typeInfo.name + "()>()");
            }
        }
        
        // Group methods by name to handle overloads
        std::unordered_map<std::string, std::vector<MethodInfo>> methodGroups;
        for (const auto& method : typeInfo.methods) {
            methodGroups[method.name].push_back(method);
        }
        
        // Add method bindings
        for (const auto& [methodName, overloads] : methodGroups) {
            std::string luaName = methodName;
            
            // Convert camelCase to snake_case
            std::string snakeName;
            for (size_t i = 0; i < luaName.length(); ++i) {
                if (i > 0 && std::isupper(luaName[i]) && std::islower(luaName[i-1])) {
                    snakeName += '_';
                }
                snakeName += std::tolower(luaName[i]);
            }
            
            if (overloads.size() == 1) {
                // Single overload
                const auto& method = overloads[0];
                if (method.needsLambda || method.isStatic) {
                    // Generate lambda for complex conversions or static methods
                    bindings.push_back(generateMethodLambda(typeInfo.name, method, snakeName));
                } else {
                    // Simple member function binding
                    bindings.push_back("        \"" + snakeName + "\", &" + typeInfo.name + "::" + methodName);
                }
            } else {
                // Multiple overloads - only bind non-static ones with sol::overload
                std::vector<std::string> overloadBindings;
                bool hasLambdas = false;
                
                for (const auto& method : overloads) {
                    if (method.isStatic || method.needsLambda) {
                        hasLambdas = true;
                        break;
                    }
                }
                
                if (hasLambdas) {
                    // If any overload needs lambdas, generate separate bindings with different names
                    for (size_t i = 0; i < overloads.size(); ++i) {
                        const auto& method = overloads[i];
                        std::string uniqueName = snakeName;
                        if (i > 0) {
                            uniqueName += "_" + std::to_string(i);
                        }
                        bindings.push_back(generateMethodLambda(typeInfo.name, method, uniqueName));
                    }
                } else {
                    // All are simple member functions - use sol::overload
                    for (const auto& method : overloads) {
                        // Cast to specific overload
                        std::string cast = "static_cast<" + method.returnType + "(" + typeInfo.name + "::*)";
                        cast += "(";
                        for (size_t i = 0; i < method.parameters.size(); ++i) {
                            if (i > 0) cast += ", ";
                            cast += method.parameters[i].first;
                        }
                        cast += ")";
                        if (method.isConst) cast += " const";
                        cast += ">(&" + typeInfo.name + "::" + methodName + ")";
                        overloadBindings.push_back(cast);
                    }
                    
                    if (!overloadBindings.empty()) {
                        std::string overloadBinding = "        \"" + snakeName + "\", sol::overload(";
                        for (size_t i = 0; i < overloadBindings.size(); ++i) {
                            if (i > 0) overloadBinding += ", ";
                            overloadBinding += overloadBindings[i];
                        }
                        overloadBinding += ")";
                        bindings.push_back(overloadBinding);
                    }
                }
            }
        }
        
        // Add field bindings for classes too
        for (const auto& field : typeInfo.fields) {
            bindings.push_back("        \"" + field.name + "\", &" + typeInfo.name + "::" + field.name);
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
    
    std::string generateMethodLambda(const std::string& className, const MethodInfo& method, const std::string& luaName) const {
        std::ostringstream output;
        
        output << "        \"" << luaName << "\", [](";
        
        // For static methods, don't include instance parameter
        if (!method.isStatic) {
            output << className << "& obj";
            if (!method.parameters.empty()) {
                output << ", ";
            }
        }
        
        // Add parameters with conversions
        for (size_t i = 0; i < method.parameters.size(); ++i) {
            if (i > 0) output << ", ";
            const auto& [paramType, paramName] = method.parameters[i];
            
            if (paramType.find("QString") != std::string::npos) {
                output << "const std::string& " << paramName;
            } else if (paramType.find("QVector") != std::string::npos) {
                output << "const sol::table& " << paramName;
            } else if (paramType.find("pcl::String") != std::string::npos) {
                output << "const std::string& " << paramName;
            } else if (paramType.find("pcl::PropertyArray") != std::string::npos) {
                // PropertyArray parameters - skip these methods or handle differently
                output << "/* PropertyArray not supported */ sol::object " << paramName;
            } else {
                output << paramType << " " << paramName;
            }
        }
        
        output << ") {\n";
        
        // Skip methods with PropertyArray parameters for now
        bool hasUnsupportedParams = false;
        for (const auto& [paramType, paramName] : method.parameters) {
            if (paramType.find("pcl::PropertyArray") != std::string::npos) {
                hasUnsupportedParams = true;
                break;
            }
        }
        
        if (hasUnsupportedParams) {
            output << "            // Method with PropertyArray parameters not supported\n";
            output << "            throw std::runtime_error(\"Method not supported: requires PropertyArray\");\n";
        } else {
            // Generate conversions
            for (const auto& [paramType, paramName] : method.parameters) {
                if (paramType.find("QString") != std::string::npos) {
                    output << "            QString q" << paramName << " = QString::fromStdString(" << paramName << ");\n";
                } else if (paramType.find("pcl::String") != std::string::npos) {
                    output << "            pcl::String pcl_" << paramName << "(" << paramName << ");\n";
                }
            }
            
            // Generate the call
            if (method.returnType != "void") {
                if (method.returnType.find("QString") != std::string::npos) {
                    output << "            QString result = ";
                } else if (method.returnType.find("pcl::String") != std::string::npos) {
                    output << "            pcl::String result = ";
                } else {
                    output << "            auto result = ";
                }
            } else {
                output << "            ";
            }
            
            if (method.isStatic) {
                output << className << "::" << method.name << "(";
            } else {
                output << "obj." << method.name << "(";
            }
            
            // Add call parameters
            for (size_t i = 0; i < method.parameters.size(); ++i) {
                if (i > 0) output << ", ";
                const auto& [paramType, paramName] = method.parameters[i];
                if (paramType.find("QString") != std::string::npos) {
                    output << "q" << paramName;
                } else if (paramType.find("pcl::String") != std::string::npos) {
                    output << "pcl_" << paramName;
                } else {
                    output << paramName;
                }
            }
            
            output << ");\n";
            
            // Handle return value conversion
            if (method.returnType != "void") {
                if (method.returnType.find("QString") != std::string::npos) {
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
};

// Enhanced compile flags extraction with PCL support
std::vector<std::string> extractCompileFlags(const std::string& compileCommandsPath, 
                                           const std::string& headerFile, 
                                           const std::vector<std::string>& additionalPCLPaths = {},
                                           bool verbose = false) {
    std::vector<std::string> flags;
    
    std::ifstream file(compileCommandsPath);
    if (!file.is_open()) {
        if (verbose) {
            std::cerr << "Could not open compile_commands.json: " << compileCommandsPath << "\n";
        }
        // Continue with default flags even if compile_commands.json is missing
    } else {
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        
        // Find the first command entry - this is a simple approach
        // For production use, you'd want a proper JSON parser
        size_t commandPos = content.find("\"command\":");
        if (commandPos != std::string::npos) {
            // Extract the command string
            size_t startQuote = content.find('"', commandPos + 10);
            size_t endQuote = content.find('"', startQuote + 1);
            
            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                std::string command = content.substr(startQuote + 1, endQuote - startQuote - 1);
                
                // Parse the command for relevant flags
                std::regex flagRegex(R"((-I\S+|-isystem\s+\S+|-iframework\s+\S+|-F\S+|-D\S+|-std=\S+|-arch\s+\S+))");
                std::smatch match;
                
                auto searchStart = command.cbegin();
                while (std::regex_search(searchStart, command.cend(), match, flagRegex)) {
                    std::string flag = match[0];
                    
                    // Handle space-separated flags and filter problematic ones
                    if (flag.find("-isystem ") == 0) {
                        flags.push_back("-isystem");
                        flags.push_back(flag.substr(9));
                    } else if (flag.find("-iframework ") == 0) {
                        flags.push_back("-iframework");
                        flags.push_back(flag.substr(12));
                    } else if (flag.find("-arch ") == 0) {
                        // Skip -arch flags for libclang (they cause issues)
                        if (verbose) {
                            std::cout << "Skipping problematic flag: " << flag << std::endl;
                        }
                    } else if (flag.find("-stdlib=") == 0) {
                        // Skip -stdlib for libclang parsing (can cause issues)
                        if (verbose) {
                            std::cout << "Skipping stdlib flag: " << flag << std::endl;
                        }
                    } else if (flag == "-std=gnu++17") {
                        // Convert to standard C++17
                        flags.push_back("-std=c++17");
                    } else {
                        flags.push_back(flag);
                    }
                    
                    searchStart = match.suffix().first;
                }
            }
        }
    }
    
    // Add essential C++ flags if not present
    bool hasStd = false;
    bool hasXCpp = false;
    for (const auto& flag : flags) {
        if (flag.find("-std=") == 0) {
            hasStd = true;
        }
        if (flag == "-x" && flags.size() > 1) {
            // Check if next flag is c++
            auto it = std::find(flags.begin(), flags.end(), flag);
            if (it != flags.end() && std::next(it) != flags.end() && 
                std::next(it)->find("c++") != std::string::npos) {
                hasXCpp = true;
            }
        }
    }
    
    if (!hasStd) {
        flags.push_back("-std=c++17");
    }
    
    // Force C++ mode for .h files (critical!)
    if (!hasXCpp) {
        flags.insert(flags.begin(), "-x");
        flags.insert(flags.begin() + 1, "c++");
    }
    
    // Add PCL paths first (higher priority)
    std::vector<std::string> pclPaths = additionalPCLPaths;
    
    // Auto-detect PCL installations and add PCL paths
    std::vector<std::string> commonPCLPaths = {
        "/usr/local/include/pcl-1.12",
        "/usr/local/include/pcl-1.13", 
        "/usr/local/include/pcl-1.14",
        "/opt/homebrew/include/pcl-1.12",
        "/opt/homebrew/include/pcl-1.13",
        "/opt/homebrew/include/pcl-1.14",
        "/usr/include/pcl-1.12",
        "/usr/include/pcl-1.13",
        "/usr/include/pcl-1.14"
    };
    
    for (const auto& pclPath : commonPCLPaths) {
        if (fs::exists(pclPath)) {
            pclPaths.push_back(pclPath);
            if (verbose) {
                std::cout << "Found PCL installation: " << pclPath << std::endl;
            }
        }
    }
    
    // Add PCL include paths to compile flags (at beginning for higher priority)
    for (const auto& pclPath : pclPaths) {
        if (fs::exists(pclPath)) {
            bool found = false;
            for (const auto& existing : flags) {
                if (existing.find(pclPath) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                flags.insert(flags.begin(), "-I" + pclPath);
                if (verbose) {
                    std::cout << "Added PCL path: " << pclPath << std::endl;
                }
            }
        }
    }
    
    // IMPORTANT: Always add the custom PCL paths (like /Users/jonathan/PCL/include)
    // even if they weren't auto-detected
    for (const auto& customPath : additionalPCLPaths) {
        if (fs::exists(customPath)) {
            bool found = false;
            for (const auto& existing : flags) {
                if (existing.find(customPath) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                flags.insert(flags.begin(), "-I" + customPath);
                if (verbose) {
                    std::cout << "Added custom PCL path: " << customPath << std::endl;
                }
            }
        } else if (verbose) {
            std::cout << "Warning: Custom PCL path does not exist: " << customPath << std::endl;
        }
    }
    
    // Add Eigen paths (PCL dependency) - critical for PCL
    std::vector<std::string> eigenPaths = {
        "/opt/homebrew/include/eigen3", 
        "/usr/local/include/eigen3",
        "/usr/include/eigen3"
    };
    
    for (const auto& eigenPath : eigenPaths) {
        if (fs::exists(eigenPath)) {
            flags.insert(flags.begin(), "-I" + eigenPath);
            if (verbose) {
                std::cout << "Added Eigen path: " << eigenPath << std::endl;
            }
            break; // Only add one Eigen path
        }
    }
    
    // Add Boost paths (PCL dependency)
    std::vector<std::string> boostPaths = {
        "/opt/homebrew/include",
        "/usr/local/include",
        "/usr/include"
    };
    
    for (const auto& boostPath : boostPaths) {
        std::string boostDir = boostPath + "/boost";
        if (fs::exists(boostDir)) {
            flags.insert(flags.begin(), "-I" + boostPath);
            if (verbose) {
                std::cout << "Added Boost path: " << boostPath << std::endl;
            }
            break;
        }
    }
    
    // Add essential PCL defines
    flags.push_back("-DPCL_NO_PRECOMPILE");
    flags.push_back("-DEIGEN_USE_NEW_STDVECTOR");
    flags.push_back("-DEIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET");
    
    // Add macOS specific flags for PCL (the ones that actually work!)
    #ifdef __APPLE__
    flags.push_back("-D__APPLE__");
    flags.push_back("-D__PCL_MACOSX");
    flags.push_back("-D__PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES");
    flags.push_back("-DACCELERATE_NEW_LAPACK");
    
    // Suppress the specific warnings that PCL generates on macOS
    flags.push_back("-Wno-dangling-else");
    flags.push_back("-Wno-extern-c-compat");
    flags.push_back("-Wno-asm-operand-widths");
    
    // Note: -stdlib=libc++ can cause issues with libclang, so we skip it
    #endif
    
    // Auto-detect Qt installations and add common Qt paths if not already present
    std::vector<std::string> qtPaths;
    
    // Try to find Qt 6 in homebrew
    std::vector<std::string> qtBasePaths = {
        "/opt/homebrew/Cellar/qt/6.9.1/lib",
        "/opt/homebrew/Cellar/qt/6.8.0/lib",
        "/opt/homebrew/Cellar/qt/6.7.2/lib",
        "/opt/homebrew/lib",
        "/usr/local/lib"
    };
    
    std::vector<std::string> qtFrameworks = {
        "QtCore", "QtWidgets", "QtGui", "QtNetwork", 
        "QtOpenGL", "QtCharts", "QtOpenGLWidgets"
    };
    
    for (const auto& basePath : qtBasePaths) {
        for (const auto& framework : qtFrameworks) {
            // Try framework style first
            std::string frameworkPath = basePath + "/" + framework + ".framework/Versions/A/Headers";
            if (fs::exists(frameworkPath)) {
                qtPaths.push_back(frameworkPath);
                
                // Also add the framework's parent directory for framework-style includes
                std::string frameworkParent = basePath + "/" + framework + ".framework/Versions/A";
                qtPaths.push_back(frameworkParent);
            }
            
            // Try include style
            std::string includePath = basePath + "/../include/" + framework;
            if (fs::exists(includePath)) {
                qtPaths.push_back(includePath);
            }
        }
        
        // Add the base lib path for framework resolution
        if (fs::exists(basePath)) {
            qtPaths.push_back(basePath);
        }
        
        // Also check if this base path exists at all
        if (fs::exists(basePath) && verbose) {
            // Reduce Qt verbosity to avoid clutter
            static bool qtPathLogged = false;
            if (!qtPathLogged) {
                std::cout << "Found Qt installation: " << basePath << std::endl;
                qtPathLogged = true;
            }
        }
    }
    
    for (const auto& qtPath : qtPaths) {
        if (fs::exists(qtPath)) {
            bool found = false;
            for (const auto& existing : flags) {
                if (existing.find(qtPath) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                flags.push_back("-I" + qtPath);
            }
        }
    }
    
    // Add framework search paths for Qt
    std::vector<std::string> frameworkPaths = {
        "/opt/homebrew/Cellar/qt/6.9.1/lib",
        "/opt/homebrew/lib"
    };
    
    for (const auto& fwPath : frameworkPaths) {
        if (fs::exists(fwPath)) {
            bool found = false;
            for (const auto& existing : flags) {
                if (existing == "-F" + fwPath || existing == "-F " + fwPath) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                flags.push_back("-F" + fwPath);
            }
        }
    }
    
    // Add standard library headers (needed for type_traits, etc.)
    std::vector<std::string> stdPaths = {
        "/opt/homebrew/Cellar/llvm/20.1.8/include/c++/v1",
        "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1",
        "/usr/include/c++/v1",
        "/opt/homebrew/opt/llvm/include/c++/v1"
    };
    
    for (const auto& stdPath : stdPaths) {
        if (fs::exists(stdPath)) {
            bool found = false;
            for (const auto& existing : flags) {
                if (existing.find(stdPath) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                flags.push_back("-isystem" + stdPath);
                break; // Only add one standard library path
            }
        }
    }
    
    // Add system include path for clang built-ins
    std::vector<std::string> clangPaths = {
        "/opt/homebrew/Cellar/llvm/20.1.8/lib/clang/20/include",
        "/opt/homebrew/opt/llvm/lib/clang/*/include"
    };
    
    for (const auto& clangPath : clangPaths) {
        if (fs::exists(clangPath)) {
            flags.push_back("-isystem" + clangPath);
            break;
        }
    }
    
    if (verbose) {
        std::cout << "Final compile flags (" << flags.size() << " total):\n";
        for (const auto& flag : flags) {
            std::cout << "  " << flag << "\n";
        }
    }
    
    return flags;
}

// Enhanced main function with PCL path support
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <header_file> [options]\n";
        std::cout << "Options:\n";
        std::cout << "  -c <compile_commands.json>  Use compile commands\n";
        std::cout << "  -o <output_file>           Output file (default: stdout)\n";
        std::cout << "  -v                         Verbose output\n";
        std::cout << "  --project-path <path>      Project path for filtering\n";
        std::cout << "  --pcl-path <path>          Add custom PCL include path\n";
        std::cout << "  --auto-pcl                 Auto-detect PCL installations\n";
        std::cout << "  --dump-command             Just dump the clang command and exit\n";
        return 1;
    }
    
    std::string headerFile = argv[1];
    std::string compileCommandsPath;
    std::string outputFile;
    std::string projectPath;
    std::vector<std::string> customPCLPaths;
    bool verbose = false;
    bool autoPCL = false;
    bool dumpCommand = false;
    
    // Parse arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-c" && i + 1 < argc) {
            compileCommandsPath = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-v") {
            verbose = true;
        } else if (arg == "--project-path" && i + 1 < argc) {
            projectPath = argv[++i];
        } else if (arg == "--pcl-path" && i + 1 < argc) {
            customPCLPaths.push_back(argv[++i]);
        } else if (arg == "--auto-pcl") {
            autoPCL = true;
        } else if (arg == "--dump-command") {
            dumpCommand = true;
            verbose = true;  // Force verbose for command dumping
        }
    }
    
    // Extract compile flags with PCL support
    std::vector<std::string> clangArgs;
    if (!compileCommandsPath.empty()) {
        clangArgs = extractCompileFlags(compileCommandsPath, headerFile, customPCLPaths, verbose);
    } else {
        // Default flags
        clangArgs = extractCompileFlags("", headerFile, customPCLPaths, verbose);
    }
    
    // If just dumping command, do that and exit
    if (dumpCommand) {
        std::cout << "=== CLANG COMMAND FOR MANUAL TESTING ===\n\n";
        std::cout << "# Standard clang command:\n";
        std::cout << "clang -fsyntax-only";
        for (const auto& arg : clangArgs) {
            std::cout << " \\\n  " << arg;
        }
        std::cout << " \\\n  " << headerFile << "\n\n";
        
        std::cout << "# For .h files, you need to force C++ mode:\n";
        std::cout << "clang -fsyntax-only -x c++";
        for (const auto& arg : clangArgs) {
            // Skip -x c++ if already present, and problematic flags
            if (arg != "-x" && arg != "c++" && 
                arg.find("-arch") != 0 && arg.find("-stdlib=") != 0) {
                std::cout << " \\\n  " << arg;
            }
        }
        std::cout << " \\\n  " << headerFile << "\n\n";
        
        std::cout << "=== SIMPLIFIED TEST COMMAND ===\n";
        std::cout << "# Working PCL parsing command:\n";
        std::cout << "clang -fsyntax-only -x c++ -std=c++17 \\\n";
        std::cout << "  -I/Users/jonathan/PCL/include \\\n";
        std::cout << "  -I/opt/homebrew/include/eigen3 \\\n";
        std::cout << "  -I/opt/homebrew/include \\\n";
        std::cout << "  -DPCL_NO_PRECOMPILE \\\n";
        std::cout << "  -DEIGEN_USE_NEW_STDVECTOR \\\n";
        std::cout << "  -D__APPLE__ -D__PCL_MACOSX \\\n";
        std::cout << "  -D__PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES \\\n";
        std::cout << "  -DACCELERATE_NEW_LAPACK \\\n";
        std::cout << "  -Wno-dangling-else -Wno-extern-c-compat \\\n";
        std::cout << "  -Wno-asm-operand-widths \\\n";
        std::cout << "  " << headerFile << "\n\n";
        
        std::cout << "=== VERIFY PCL PATHS ===\n";
        std::cout << "Check that these paths exist:\n";
        for (const auto& path : customPCLPaths) {
            std::cout << "  ls -la " << path << "\n";
        }
        std::cout << "  ls -la /opt/homebrew/include/eigen3/\n";
        std::cout << "  ls -la " << headerFile << "\n";
        
        std::cout << "\n=== PCL ISSUES DETECTED ===\n";
        std::cout << "PCL has known issues on Apple Silicon (ARM64):\n";
        std::cout << "1. Architecture detection problems\n";
        std::cout << "2. Intel intrinsics (_mm_malloc/_mm_free) not available\n";
        std::cout << "3. x86 assembly code incompatible with ARM64\n";
        std::cout << "\nThe command above includes workarounds for these issues.\n";
        
        return 0;
    }
    
    // Create generator with PCL support
    BindingGenerator generator(headerFile, projectPath);
    generator.setVerbose(verbose);
    
    // Add custom PCL paths
    generator.addPCLPaths(customPCLPaths);
    
    // Auto-detect PCL if requested
    if (autoPCL) {
        generator.autoDetectPCLPaths();
    }
    
    if (verbose) {
        std::cout << "Parsing " << headerFile << " with PCL support...\n";
    }
    
    // Parse and generate
    auto types = generator.parseHeader(headerFile, clangArgs);
    
    std::cout << "Found " << types.size() << " bindable types: ";
    bool first = true;
    for (const auto& [name, info] : types) {
        if (!first) std::cout << ", ";
        std::cout << name;
        first = false;
    }
    std::cout << "\n";
    
    if (types.empty()) {
        std::cout << "No bindable types found. This might be due to:\n";
        std::cout << "1. Types defined in system headers\n";
        std::cout << "2. Types without public members\n";
        std::cout << "3. Missing PCL paths (try --auto-pcl or --pcl-path)\n";
        std::cout << "4. Parsing errors (try -v for diagnostics)\n";
        return 1;
    }
    
    std::string bindings = generator.generateBindings(types);
    
    if (!outputFile.empty()) {
        std::ofstream out(outputFile);
        out << bindings;
        std::cout << "Bindings written to " << outputFile << "\n";
    } else {
        std::cout << bindings;
    }
    
    return 0;
}
