// Add this to your Sol2 binding initialization code
// (typically in initializeSol2() or similar function)

// First, we need to bind pcl::FITSKeywordArray if not already bound
lua->new_usertype<pcl::FITSKeywordArray>("FITSKeywordArray",
    // Constructor
    sol::constructors<pcl::FITSKeywordArray()>(),
    
    // Methods
    "Add", sol::overload(
        // For numeric keywords
        [](pcl::FITSKeywordArray& keywords, const std::string& name, double value, const std::string& comment) {
            keywords.Add(pcl::FITSHeaderKeyword(
                pcl::IsoString(name.c_str()), 
                value, 
                pcl::IsoString(comment.c_str())
            ));
        },
        // For string keywords  
        [](pcl::FITSKeywordArray& keywords, const std::string& name, const std::string& value, const std::string& comment) {
            keywords.Add(pcl::FITSHeaderKeyword(
                pcl::IsoString(name.c_str()), 
                pcl::IsoString(value.c_str()), 
                pcl::IsoString(comment.c_str())
            ));
        }
    ),
    
    "Length", &pcl::FITSKeywordArray::Length,
    "Clear", &pcl::FITSKeywordArray::Clear,
    
    // Helper method to add from Lua table
    "addFromTable", [](pcl::FITSKeywordArray& keywords, const sol::table& keywordTable) {
        for (const auto& pair : keywordTable) {
            if (pair.second.get_type() == sol::type::table) {
                sol::table keywordData = pair.second;
                std::string name = keywordData["name"].get_or<std::string>("");
                std::string comment = keywordData["comment"].get_or<std::string>("");
                
                // Check if value is numeric or string
                auto value = keywordData["value"];
                if (value.get_type() == sol::type::number) {
                    double numValue = value.get<double>();
                    keywords.Add(pcl::FITSHeaderKeyword(
                        pcl::IsoString(name.c_str()), 
                        numValue, 
                        pcl::IsoString(comment.c_str())
                    ));
                } else if (value.get_type() == sol::type::string) {
                    std::string strValue = value.get<std::string>();
                    keywords.Add(pcl::FITSHeaderKeyword(
                        pcl::IsoString(name.c_str()), 
                        pcl::IsoString(strValue.c_str()), 
                        pcl::IsoString(comment.c_str())
                    ));
                }
            }
        }
    }
);

// Now bind the StarCatalogValidator::setWCSFromSolver method
// Add this to your existing StarCatalogValidator binding:
lua->new_usertype<StarCatalogValidator>("StarCatalogValidator",
    // ... existing bindings ...
    
    // Add the setWCSFromSolver binding
    "setWCSFromSolver", sol::overload(
        // Direct version with FITSKeywordArray
        [](StarCatalogValidator& validator, const pcl::FITSKeywordArray& keywords, int width, int height) -> bool {
            return validator.setWCSFromSolver(keywords, width, height);
        },
        
        // Lua table version - converts table to FITSKeywordArray
        [](StarCatalogValidator& validator, const sol::table& keywordTable, int width, int height) -> bool {
            pcl::FITSKeywordArray keywords;
            
            // Convert Lua table to FITSKeywordArray
            for (const auto& pair : keywordTable) {
                if (pair.second.get_type() == sol::type::table) {
                    sol::table keywordData = pair.second;
                    std::string name = keywordData["name"].get_or<std::string>("");
                    std::string comment = keywordData["comment"].get_or<std::string>("");
                    
                    // Check if value is numeric or string
                    auto value = keywordData["value"];
                    if (value.get_type() == sol::type::number) {
                        double numValue = value.get<double>();
                        keywords.Add(pcl::FITSHeaderKeyword(
                            pcl::IsoString(name.c_str()), 
                            numValue, 
                            pcl::IsoString(comment.c_str())
                        ));
                    } else if (value.get_type() == sol::type::string) {
                        std::string strValue = value.get<std::string>();
                        keywords.Add(pcl::FITSHeaderKeyword(
                            pcl::IsoString(name.c_str()), 
                            pcl::IsoString(strValue.c_str()), 
                            pcl::IsoString(comment.c_str())
                        ));
                    }
                }
            }
            
            return validator.setWCSFromSolver(keywords, width, height);
        }
    )
    
    // ... rest of existing bindings ...
);

// Helper function to create FITS keywords from common WCS parameters
lua->set_function("createWCSKeywords", [](double crval1, double crval2, double crpix1, double crpix2,
                                         double cd1_1, double cd1_2, double cd2_1, double cd2_2) -> pcl::FITSKeywordArray {
    pcl::FITSKeywordArray keywords;
    
    // Add essential WCS keywords
    keywords.Add(pcl::FITSHeaderKeyword("CRVAL1", crval1, "Reference RA"));
    keywords.Add(pcl::FITSHeaderKeyword("CRVAL2", crval2, "Reference Dec"));
    keywords.Add(pcl::FITSHeaderKeyword("CRPIX1", crpix1, "Reference pixel X"));
    keywords.Add(pcl::FITSHeaderKeyword("CRPIX2", crpix2, "Reference pixel Y"));
    keywords.Add(pcl::FITSHeaderKeyword("CD1_1", cd1_1, "Transformation matrix"));
    keywords.Add(pcl::FITSHeaderKeyword("CD1_2", cd1_2, "Transformation matrix"));
    keywords.Add(pcl::FITSHeaderKeyword("CD2_1", cd2_1, "Transformation matrix"));
    keywords.Add(pcl::FITSHeaderKeyword("CD2_2", cd2_2, "Transformation matrix"));
    
    // Add coordinate system defaults
    keywords.Add(pcl::FITSHeaderKeyword("CTYPE1", pcl::IsoString("'RA---TAN'"), "Coordinate type"));
    keywords.Add(pcl::FITSHeaderKeyword("CTYPE2", pcl::IsoString("'DEC--TAN'"), "Coordinate type"));
    keywords.Add(pcl::FITSHeaderKeyword("RADESYS", pcl::IsoString("'ICRS'"), "Coordinate reference system"));
    
    return keywords;
});
