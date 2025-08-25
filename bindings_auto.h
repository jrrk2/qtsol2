// Generated Sol2 Lua bindings
// Total classes: 198

    // Bind pcl::WorldTransformation class
    lua->new_usertype<pcl::WorldTransformation>("WorldTransformation",
        sol::constructors<pcl::WorldTransformation()>(),
        "is_empty", &pcl::WorldTransformation::IsEmpty,
        "clone", &pcl::WorldTransformation::Clone,
        "direct", &pcl::WorldTransformation::Direct,
        "inverse", &pcl::WorldTransformation::Inverse,
        "approximate_linear_transform", &pcl::WorldTransformation::ApproximateLinearTransform
    );

    // Bind pcl::ImageSelections structure
    lua->new_usertype<pcl::ImageSelections>("ImageSelections",
        "channel", &pcl::ImageSelections::channel,
        "last_channel", &pcl::ImageSelections::lastChannel,
        "point", &pcl::ImageSelections::point,
        "rectangle", &pcl::ImageSelections::rectangle,
        "clip_low", &pcl::ImageSelections::clipLow,
        "clip_high", &pcl::ImageSelections::clipHigh,
        "clipped_low", &pcl::ImageSelections::clippedLow,
        "clipped_high", &pcl::ImageSelections::clippedHigh
    );

    // Bind pcl::WCSKeywords class
    lua->new_usertype<pcl::WCSKeywords>("WCSKeywords",
        sol::constructors<pcl::WCSKeywords()>(),
        "read", &pcl::WCSKeywords::Read,
        "to_properties", &pcl::WCSKeywords::ToProperties,
        "extract_world_transformation", &pcl::WCSKeywords::ExtractWorldTransformation,
        "objctra", &pcl::WCSKeywords::objctra,
        "objctdec", &pcl::WCSKeywords::objctdec,
        "radesys", &pcl::WCSKeywords::radesys,
        "equinox", &pcl::WCSKeywords::equinox,
        "dateobs", &pcl::WCSKeywords::dateobs,
        "dateend", &pcl::WCSKeywords::dateend,
        "longobs", &pcl::WCSKeywords::longobs,
        "latobs", &pcl::WCSKeywords::latobs,
        "altobs", &pcl::WCSKeywords::altobs,
        "focallen", &pcl::WCSKeywords::focallen,
        "xpixsz", &pcl::WCSKeywords::xpixsz,
        "crval1", &pcl::WCSKeywords::crval1,
        "crval2", &pcl::WCSKeywords::crval2,
        "crpix1", &pcl::WCSKeywords::crpix1,
        "crpix2", &pcl::WCSKeywords::crpix2,
        "cd1_1", &pcl::WCSKeywords::cd1_1
    );

    // Bind pcl::ProjectionBase class
    lua->new_usertype<pcl::ProjectionBase>("ProjectionBase",
        "clone", &pcl::ProjectionBase::Clone,
        "proj_code", &pcl::ProjectionBase::ProjCode,
        "identifier", &pcl::ProjectionBase::Identifier,
        "name", &pcl::ProjectionBase::Name,
        "get_wcs", &pcl::ProjectionBase::GetWCS,
        "direct", &pcl::ProjectionBase::Direct,
        "inverse", &pcl::ProjectionBase::Inverse,
        "check_broken_line", &pcl::ProjectionBase::CheckBrokenLine,
        "projection_origin", &pcl::ProjectionBase::ProjectionOrigin,
        "distance", [](const pcl::DPoint & p1, const pcl::DPoint & p2) {
            auto result = pcl::ProjectionBase::Distance(p1, p2);
            return result;
        },
        "distance_fast", [](const pcl::DPoint & p1, const pcl::DPoint & p2) {
            auto result = pcl::ProjectionBase::DistanceFast(p1, p2);
            return result;
        },
        "reference_celestial_coordinates", &pcl::ProjectionBase::ReferenceCelestialCoordinates,
        "reference_native_coordinates", &pcl::ProjectionBase::ReferenceNativeCoordinates,
        "celestial_pole_native_coordinates", &pcl::ProjectionBase::CelestialPoleNativeCoordinates,
        "wcscode_to_projection_identifier", [](const pcl::IsoString & wcsCode) {
            auto result = pcl::ProjectionBase::WCSCodeToProjectionIdentifier(wcsCode);
            return result;
        },
        "projection_identifier_to_wcscode", [](const pcl::IsoString & identifier) {
            auto result = pcl::ProjectionBase::ProjectionIdentifierToWCSCode(identifier);
            return result;
        }
    );

    // Bind pcl::PropertyDescription structure
    lua->new_usertype<pcl::PropertyDescription>("PropertyDescription",
        "id", &pcl::PropertyDescription::id,
        "type", &pcl::PropertyDescription::type
    );

    // Bind ColorCalibrationResult structure
    lua->new_usertype<ColorCalibrationResult>("ColorCalibrationResult",
        "color_matrix", &ColorCalibrationResult::colorMatrix,
        "red_scale", &ColorCalibrationResult::redScale,
        "green_scale", &ColorCalibrationResult::greenScale,
        "blue_scale", &ColorCalibrationResult::blueScale,
        "red_offset", &ColorCalibrationResult::redOffset,
        "green_offset", &ColorCalibrationResult::greenOffset,
        "blue_offset", &ColorCalibrationResult::blueOffset,
        "rms_color_error", &ColorCalibrationResult::rmsColorError,
        "systematic_bverror", &ColorCalibrationResult::systematicBVError,
        "systematic_vrerror", &ColorCalibrationResult::systematicVRError,
        "stars_used", &ColorCalibrationResult::starsUsed,
        "calibration_quality", &ColorCalibrationResult::calibrationQuality,
        "recommendations", &ColorCalibrationResult::recommendations
    );

    // Bind pcl::LZ4HCCompression class
    lua->new_usertype<pcl::LZ4HCCompression>("LZ4HCCompression",
        "algorithm_name", &pcl::LZ4HCCompression::AlgorithmName,
        "max_compression_level", &pcl::LZ4HCCompression::MaxCompressionLevel,
        "default_compression_level", &pcl::LZ4HCCompression::DefaultCompressionLevel
    );

    // Bind pcl::String class
    lua->new_usertype<pcl::String>("String",
        sol::constructors<pcl::String()>(),
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign,
        "assign", &pcl::String::Assign
    );

    // Bind pcl::Compression::Performance structure
    lua->new_usertype<pcl::Compression::Performance>("Performance",
        "size_reduction", &pcl::Compression::Performance::sizeReduction,
        "throughput", &pcl::Compression::Performance::throughput,
        "number_of_threads", &pcl::Compression::Performance::numberOfThreads
    );

    // Bind pcl::StatusMonitor class
    lua->new_usertype<pcl::StatusMonitor>("StatusMonitor",
        sol::constructors<pcl::StatusMonitor()>(),
        "initialize", &pcl::StatusMonitor::Initialize,
        "disable_initialization", &pcl::StatusMonitor::DisableInitialization,
        "enable_initialization", &pcl::StatusMonitor::EnableInitialization,
        "is_initialization_enabled", &pcl::StatusMonitor::IsInitializationEnabled,
        "is_initialized", &pcl::StatusMonitor::IsInitialized,
        "is_completed", &pcl::StatusMonitor::IsCompleted,
        "is_aborted", &pcl::StatusMonitor::IsAborted,
        "set_info", &pcl::StatusMonitor::SetInfo,
        "complete", &pcl::StatusMonitor::Complete,
        "callback", &pcl::StatusMonitor::Callback,
        "callback", &pcl::StatusMonitor::Callback,
        "set_callback", &pcl::StatusMonitor::SetCallback,
        "return_value", &pcl::StatusMonitor::ReturnValue,
        "info", &pcl::StatusMonitor::Info,
        "count", &pcl::StatusMonitor::Count,
        "total", &pcl::StatusMonitor::Total,
        "clear", &pcl::StatusMonitor::Clear,
        "refresh_rate", []() {
            auto result = pcl::StatusMonitor::RefreshRate();
            return result;
        },
        "set_refresh_rate", [](unsigned int ms) {
            pcl::StatusMonitor::SetRefreshRate(ms);
        }
    );

    // Bind pcl::TimePoint class
    lua->new_usertype<pcl::TimePoint>("TimePoint",
        sol::constructors<pcl::TimePoint()>(),
        "is_valid", &pcl::TimePoint::IsValid,
        "get_calendar_time", &pcl::TimePoint::GetCalendarTime,
        "get_calendar_time", &pcl::TimePoint::GetCalendarTime,
        "get_calendar_time", &pcl::TimePoint::GetCalendarTime,
        "year", &pcl::TimePoint::Year,
        "month", &pcl::TimePoint::Month,
        "day", &pcl::TimePoint::Day,
        "day_fraction", &pcl::TimePoint::DayFraction,
        "to_string", &pcl::TimePoint::ToString,
        "to_string", &pcl::TimePoint::ToString,
        "to_iso_string", &pcl::TimePoint::ToIsoString,
        "to_iso_string", &pcl::TimePoint::ToIsoString,
        "to_string", &pcl::TimePoint::ToString,
        "to_iso_string", &pcl::TimePoint::ToIsoString,
        "to_iso_string", &pcl::TimePoint::ToIsoString,
        "jdi", &pcl::TimePoint::JDI,
        "jdf", &pcl::TimePoint::JDF,
        "jd", &pcl::TimePoint::JD,
        "mjd", &pcl::TimePoint::MJD
    );

    // Bind pcl::StatusCallback class
    lua->new_usertype<pcl::StatusCallback>("StatusCallback",
        sol::constructors<pcl::StatusCallback()>(),
        "initialized", &pcl::StatusCallback::Initialized,
        "updated", &pcl::StatusCallback::Updated,
        "completed", &pcl::StatusCallback::Completed,
        "info_updated", &pcl::StatusCallback::InfoUpdated
    );

    // Bind CatalogStar structure
    lua->new_usertype<CatalogStar>("CatalogStar",
        "id", &CatalogStar::id,
        "ra", &CatalogStar::ra,
        "dec", &CatalogStar::dec,
        "magnitude", &CatalogStar::magnitude,
        "spectral_type", &CatalogStar::spectralType,
        "pixel_pos", &CatalogStar::pixelPos,
        "is_valid", &CatalogStar::isValid,
        "mag_bp", &CatalogStar::magBP,
        "mag_rp", &CatalogStar::magRP,
        "color_bp_rp", &CatalogStar::colorBP_RP,
        "has_extended_data", &CatalogStar::hasExtendedData
    );

    // Bind pcl::UniqueFileChecks structure
    lua->new_usertype<pcl::UniqueFileChecks>("UniqueFileChecks",
        "exists", &pcl::UniqueFileChecks::exists,
        "overwrite", &pcl::UniqueFileChecks::overwrite
    );

    // Bind pcl::SplineWorldTransformation class
    lua->new_usertype<pcl::SplineWorldTransformation>("SplineWorldTransformation",
        "is_empty", &pcl::SplineWorldTransformation::IsEmpty,
        "clone", &pcl::SplineWorldTransformation::Clone,
        "direct", &pcl::SplineWorldTransformation::Direct,
        "inverse", &pcl::SplineWorldTransformation::Inverse,
        "approximate_linear_transform", &pcl::SplineWorldTransformation::ApproximateLinearTransform,
        "version", &pcl::SplineWorldTransformation::Version,
        "initialize_grid_interpolations", &pcl::SplineWorldTransformation::InitializeGridInterpolations,
        "has_grid_interpolations", &pcl::SplineWorldTransformation::HasGridInterpolations,
        "grid_interpolation_delta", &pcl::SplineWorldTransformation::GridInterpolationDelta,
        "property_prefix", []() {
            auto result = pcl::SplineWorldTransformation::PropertyPrefix();
            return result;
        },
        "to_properties", &pcl::SplineWorldTransformation::ToProperties,
        "serialize", &pcl::SplineWorldTransformation::Serialize,
        "number_of_control_points", &pcl::SplineWorldTransformation::NumberOfControlPoints,
        "native_control_points", &pcl::SplineWorldTransformation::NativeControlPoints,
        "image_control_points", &pcl::SplineWorldTransformation::ImageControlPoints,
        "rbftype", &pcl::SplineWorldTransformation::RBFType,
        "spline_order", &pcl::SplineWorldTransformation::SplineOrder,
        "rbftype_name", &pcl::SplineWorldTransformation::RBFTypeName,
        "max_spline_points", &pcl::SplineWorldTransformation::MaxSplinePoints,
        "get_spline_lengths", &pcl::SplineWorldTransformation::GetSplineLengths
    );

    // Bind pcl::ZLibCompression class
    lua->new_usertype<pcl::ZLibCompression>("ZLibCompression",
        "algorithm_name", &pcl::ZLibCompression::AlgorithmName,
        "max_compression_level", &pcl::ZLibCompression::MaxCompressionLevel,
        "default_compression_level", &pcl::ZLibCompression::DefaultCompressionLevel
    );

    // Bind pcl::RecursivePointSurfaceSpline class
    lua->new_usertype<pcl::RecursivePointSurfaceSpline>("RecursivePointSurfaceSpline",
        sol::constructors<pcl::RecursivePointSurfaceSpline()>(),
        "clear", &pcl::RecursivePointSurfaceSpline::Clear,
        "is_recursive", &pcl::RecursivePointSurfaceSpline::IsRecursive,
        "is_valid", &pcl::RecursivePointSurfaceSpline::IsValid,
        sol::meta_function::call, &pcl::RecursivePointSurfaceSpline::operator()
    );

    // Bind pcl::FindFileInfo structure
    lua->new_usertype<pcl::FindFileInfo>("FindFileInfo",
        "name", &pcl::FindFileInfo::name,
        "attributes", &pcl::FindFileInfo::attributes,
        "size", &pcl::FindFileInfo::size,
        "number_of_links", &pcl::FindFileInfo::numberOfLinks,
        "user_id", &pcl::FindFileInfo::userId,
        "group_id", &pcl::FindFileInfo::groupId,
        "created", &pcl::FindFileInfo::created,
        "last_accessed", &pcl::FindFileInfo::lastAccessed,
        "last_modified", &pcl::FindFileInfo::lastModified
    );

    // Bind StarStatisticsChartDialog class
    lua->new_usertype<StarStatisticsChartDialog>("StarStatisticsChartDialog",
        "tr", [](const char * s, const char * c, int n) {
            auto result = StarStatisticsChartDialog::tr(s, c, n);
            return result.toStdString();
        },
        "update_rgbbutton_state", &StarStatisticsChartDialog::updateRGBButtonState,
        "set_image_data", &StarStatisticsChartDialog::setImageData,
        "set_detected_stars", &StarStatisticsChartDialog::setDetectedStars,
        "set_catalog_stars", &StarStatisticsChartDialog::setCatalogStars
    );

    // Bind pcl::Bitmap class
    lua->new_usertype<pcl::Bitmap>("Bitmap",
        sol::constructors<pcl::Bitmap()>(),
        "pixel_format", &pcl::Bitmap::PixelFormat,
        "set_pixel_format", &pcl::Bitmap::SetPixelFormat,
        "render", [](const ImageVariant & image, int zoom, display_channel displayChannel, bool transparency, const ImageVariant * mask, mask_mode maskMode, bool maskInverted, const uint8 ** LUT, bool fastDownsample, bool (*)() callback) {
            auto result = pcl::Bitmap::Render(image, zoom, displayChannel, transparency, mask, maskMode, maskInverted, LUT, fastDownsample, callback);
            return result;
        },
        "get_dimensions", &pcl::Bitmap::GetDimensions,
        "width", &pcl::Bitmap::Width,
        "height", &pcl::Bitmap::Height,
        "bounds", &pcl::Bitmap::Bounds,
        "is_empty", &pcl::Bitmap::IsEmpty,
        "pixel", &pcl::Bitmap::Pixel,
        "pixel", &pcl::Bitmap::Pixel,
        "set_pixel", &pcl::Bitmap::SetPixel,
        "set_pixel", &pcl::Bitmap::SetPixel,
        "scan_line", &pcl::Bitmap::ScanLine,
        "scan_line", &pcl::Bitmap::ScanLine,
        "mirrored", &pcl::Bitmap::Mirrored,
        "mirrored_horizontally", &pcl::Bitmap::MirroredHorizontally,
        "mirrored_vertically", &pcl::Bitmap::MirroredVertically,
        "scaled", &pcl::Bitmap::Scaled,
        "scaled", &pcl::Bitmap::Scaled
    );

    // Bind LuaControlledWidget class
    lua->new_usertype<LuaControlledWidget>("LuaControlledWidget",
        "tr", [](const char * s, const char * c, int n) {
            auto result = LuaControlledWidget::tr(s, c, n);
            return result.toStdString();
        },
        "get_status_label", &LuaControlledWidget::getStatusLabel,
        "get_progress_bar", &LuaControlledWidget::getProgressBar,
        "get_slider", &LuaControlledWidget::getSlider,
        "set_status", [](LuaControlledWidget& obj, const std::string& text) {
            obj.setStatus(QString::fromStdString(text));
        },
        "set_progress", &LuaControlledWidget::setProgress,
        "start_animation", &LuaControlledWidget::startAnimation,
        "stop_animation", &LuaControlledWidget::stopAnimation,
        "get_slider_value", &LuaControlledWidget::getSliderValue,
        "animate_progress", &LuaControlledWidget::animateProgress,
        "get_status_text", &LuaControlledWidget::getStatusText,
        "set_status_string", &LuaControlledWidget::setStatusString,
        "status_changed", [](LuaControlledWidget& obj, const std::string& text) {
            obj.statusChanged(QString::fromStdString(text));
        },
        "progress_changed", &LuaControlledWidget::progressChanged,
        "animation_started", &LuaControlledWidget::animationStarted,
        "animation_stopped", &LuaControlledWidget::animationStopped
    );

    // Specialized binding for pcl::AstrometricMetadata
    lua->new_usertype<pcl::AstrometricMetadata>("AstrometricMetadata",
        sol::constructors<pcl::AstrometricMetadata()>(),
        "build", &pcl::AstrometricMetadata::Build,
        "is_valid", &pcl::AstrometricMetadata::IsValid,
        "width", &pcl::AstrometricMetadata::Width,
        "height", &pcl::AstrometricMetadata::Height,
        "bounds", &pcl::AstrometricMetadata::Bounds,
        "image_to_celestial", &pcl::AstrometricMetadata::ImageToCelestial,
        "celestial_to_image", &pcl::AstrometricMetadata::CelestialToImage,
        "resolution", &pcl::AstrometricMetadata::Resolution,
        "observation_start_time", [](pcl::AstrometricMetadata& obj) {
            auto result = obj.ObservationStartTime();
            if (result.IsDefined()) {
                return result();
            } else {
                return sol::nil;
            }
        },
        "observation_end_time", [](pcl::AstrometricMetadata& obj) {
            auto result = obj.ObservationEndTime();
            if (result.IsDefined()) {
                return result();
            } else {
                return sol::nil;
            }
        },
        "location_longitude", [](pcl::AstrometricMetadata& obj) {
            auto result = obj.LocationLongitude();
            if (result.IsDefined()) {
                return result();
            } else {
                return sol::nil;
            }
        },
        "location_latitude", [](pcl::AstrometricMetadata& obj) {
            auto result = obj.LocationLatitude();
            if (result.IsDefined()) {
                return result();
            } else {
                return sol::nil;
            }
        },
        "location_height", [](pcl::AstrometricMetadata& obj) {
            auto result = obj.LocationHeight();
            if (result.IsDefined()) {
                return result();
            } else {
                return sol::nil;
            }
        },
        "set_location_longitude", &pcl::AstrometricMetadata::SetLocationLongitude,
        "set_location_latitude", &pcl::AstrometricMetadata::SetLocationLatitude,
        "set_location_height", &pcl::AstrometricMetadata::SetLocationHeight,
        "pixel_size", [](pcl::AstrometricMetadata& obj) {
            auto result = obj.PixelSize();
            if (result.IsDefined()) {
                return result();
            } else {
                return sol::nil;
            }
        },
        "set_pixel_size", &pcl::AstrometricMetadata::SetPixelSize,
        "reference_system", &pcl::AstrometricMetadata::ReferenceSystem,
        "set_reference_system", &pcl::AstrometricMetadata::SetReferenceSystem,
        "validate", &pcl::AstrometricMetadata::Validate,
        "to_properties", &pcl::AstrometricMetadata::ToProperties,
        "summary", &pcl::AstrometricMetadata::Summary,
        sol::meta_function::less_than, &pcl::AstrometricMetadata::operator<
    );

    // Bind pcl::PointSurfaceSpline class
    lua->new_usertype<pcl::PointSurfaceSpline>("PointSurfaceSpline",
        sol::constructors<pcl::PointSurfaceSpline()>(),
        "initialize", &pcl::PointSurfaceSpline::Initialize,
        "clear", &pcl::PointSurfaceSpline::Clear,
        "is_valid", &pcl::PointSurfaceSpline::IsValid,
        "spline_x", &pcl::PointSurfaceSpline::SplineX,
        "spline_y", &pcl::PointSurfaceSpline::SplineY,
        "max_spline_points", &pcl::PointSurfaceSpline::MaxSplinePoints,
        "set_max_spline_points", &pcl::PointSurfaceSpline::SetMaxSplinePoints,
        "simplifiers_enabled", &pcl::PointSurfaceSpline::SimplifiersEnabled,
        "enable_simplifiers", &pcl::PointSurfaceSpline::EnableSimplifiers,
        "disable_simplifiers", &pcl::PointSurfaceSpline::DisableSimplifiers,
        "simplifier_reject_fraction", &pcl::PointSurfaceSpline::SimplifierRejectFraction,
        "set_simplifier_reject_fraction", &pcl::PointSurfaceSpline::SetSimplifierRejectFraction,
        "error_x", &pcl::PointSurfaceSpline::ErrorX,
        "error_y", &pcl::PointSurfaceSpline::ErrorY,
        "truncated_x", &pcl::PointSurfaceSpline::TruncatedX,
        "truncated_y", &pcl::PointSurfaceSpline::TruncatedY,
        "truncated", &pcl::PointSurfaceSpline::Truncated,
        "linear_function", &pcl::PointSurfaceSpline::LinearFunction,
        "set_linear_function", &pcl::PointSurfaceSpline::SetLinearFunction
    );

    // Bind pcl::ParallelProcess class
    lua->new_usertype<pcl::ParallelProcess>("ParallelProcess",
        sol::constructors<pcl::ParallelProcess()>(),
        "is_parallel_processing_enabled", &pcl::ParallelProcess::IsParallelProcessingEnabled,
        "enable_parallel_processing", &pcl::ParallelProcess::EnableParallelProcessing,
        "disable_parallel_processing", &pcl::ParallelProcess::DisableParallelProcessing,
        "max_processors", &pcl::ParallelProcess::MaxProcessors,
        "set_max_processors", &pcl::ParallelProcess::SetMaxProcessors,
        "swap", &pcl::ParallelProcess::Swap
    );

    // Bind pcl::File::Find class
    lua->new_usertype<pcl::File::Find>("Find",
        sol::constructors<pcl::File::Find()>(),
        "begin", &pcl::File::Find::Begin,
        "next_item", &pcl::File::Find::NextItem,
        "end", &pcl::File::Find::End,
        "search_path", &pcl::File::Find::SearchPath
    );

    // Bind pcl::Mutex class
    lua->new_usertype<pcl::Mutex>("Mutex",
        "lock", &pcl::Mutex::Lock,
        "unlock", &pcl::Mutex::Unlock,
        sol::meta_function::call, &pcl::Mutex::operator(),
        "try_lock", &pcl::Mutex::TryLock,
        "spin_count", &pcl::Mutex::SpinCount
    );

    // Bind pcl::RGBColorSystem class
    lua->new_usertype<pcl::RGBColorSystem>("RGBColorSystem",
        sol::constructors<pcl::RGBColorSystem()>(),
        "is_unique", &pcl::RGBColorSystem::IsUnique,
        "is_alias_of", &pcl::RGBColorSystem::IsAliasOf,
        "ensure_unique", &pcl::RGBColorSystem::EnsureUnique,
        "gamma", &pcl::RGBColorSystem::Gamma,
        "is_srgb", &pcl::RGBColorSystem::IsSRGB,
        "is_linear", &pcl::RGBColorSystem::IsLinear,
        "rgbto_xyzmatrix", &pcl::RGBColorSystem::RGBToXYZMatrix,
        "xyzto_rgbmatrix", &pcl::RGBColorSystem::XYZToRGBMatrix,
        "ciexnormalization_factor", &pcl::RGBColorSystem::CIEXNormalizationFactor,
        "cieznormalization_factor", &pcl::RGBColorSystem::CIEZNormalizationFactor,
        "ciea_normalization_offset", &pcl::RGBColorSystem::CIEaNormalizationOffset,
        "ciea_normalization_factor", &pcl::RGBColorSystem::CIEaNormalizationFactor,
        "cieb_normalization_offset", &pcl::RGBColorSystem::CIEbNormalizationOffset,
        "cieb_normalization_factor", &pcl::RGBColorSystem::CIEbNormalizationFactor,
        "ciec_normalization_factor", &pcl::RGBColorSystem::CIEcNormalizationFactor,
        "chromaticity_xcoordinates", &pcl::RGBColorSystem::ChromaticityXCoordinates,
        "chromaticity_ycoordinates", &pcl::RGBColorSystem::ChromaticityYCoordinates,
        "luminance_coefficients", &pcl::RGBColorSystem::LuminanceCoefficients,
        "assign", &pcl::RGBColorSystem::Assign
    );

    // Bind pcl::LinearWorldTransformation class
    lua->new_usertype<pcl::LinearWorldTransformation>("LinearWorldTransformation",
        "is_empty", &pcl::LinearWorldTransformation::IsEmpty,
        "clone", &pcl::LinearWorldTransformation::Clone,
        "direct", &pcl::LinearWorldTransformation::Direct,
        "inverse", &pcl::LinearWorldTransformation::Inverse,
        "approximate_linear_transform", &pcl::LinearWorldTransformation::ApproximateLinearTransform
    );

    // Bind pcl::PixelTraitsLUT structure
    lua->new_usertype<pcl::PixelTraitsLUT>("PixelTraitsLUT",
        "p_flut8", &pcl::PixelTraitsLUT::pFLUT8,
        "p_fluta", &pcl::PixelTraitsLUT::pFLUTA,
        "p1flut8", &pcl::PixelTraitsLUT::p1FLUT8,
        "p_flut16", &pcl::PixelTraitsLUT::pFLUT16,
        "p_flut20", &pcl::PixelTraitsLUT::pFLUT20,
        "p_dlut8", &pcl::PixelTraitsLUT::pDLUT8,
        "p_dluta", &pcl::PixelTraitsLUT::pDLUTA,
        "p1dlut8", &pcl::PixelTraitsLUT::p1DLUT8,
        "p_dlut16", &pcl::PixelTraitsLUT::pDLUT16,
        "p_dlut20", &pcl::PixelTraitsLUT::pDLUT20,
        "p8lut16", &pcl::PixelTraitsLUT::p8LUT16,
        "p8lut20", &pcl::PixelTraitsLUT::p8LUT20,
        "p16lut8", &pcl::PixelTraitsLUT::p16LUT8,
        "p16lut20", &pcl::PixelTraitsLUT::p16LUT20,
        "p20lut8", &pcl::PixelTraitsLUT::p20LUT8,
        "p20lut16", &pcl::PixelTraitsLUT::p20LUT16,
        "p24lut8", &pcl::PixelTraitsLUT::p24LUT8,
        "p24lut16", &pcl::PixelTraitsLUT::p24LUT16,
        "p32lut8", &pcl::PixelTraitsLUT::p32LUT8,
        "p32lut16", &pcl::PixelTraitsLUT::p32LUT16,
        "p32lut20", &pcl::PixelTraitsLUT::p32LUT20
    );

    // Bind pcl::LeafNode structure
    lua->new_usertype<pcl::LeafNode>("LeafNode",
        "points", &pcl::LeafNode::points,
        "data", &pcl::LeafNode::data
    );

    // Bind pcl::LZ4Compression class
    lua->new_usertype<pcl::LZ4Compression>("LZ4Compression",
        "algorithm_name", &pcl::LZ4Compression::AlgorithmName,
        "max_compression_level", &pcl::LZ4Compression::MaxCompressionLevel,
        "default_compression_level", &pcl::LZ4Compression::DefaultCompressionLevel
    );

    // Bind pcl::IsoString class
    lua->new_usertype<pcl::IsoString>("IsoString",
        sol::constructors<pcl::IsoString()>(),
        "set_to_length", &pcl::IsoString::SetToLength,
        "resized_to_null_terminated", &pcl::IsoString::ResizedToNullTerminated,
        "squeezed", &pcl::IsoString::Squeezed,
        "substring", &pcl::IsoString::Substring,
        "left", &pcl::IsoString::Left,
        "right", &pcl::IsoString::Right,
        "suffix", &pcl::IsoString::Suffix,
        "prefix", &pcl::IsoString::Prefix,
        "trimmed", &pcl::IsoString::Trimmed,
        "trimmed_left", &pcl::IsoString::TrimmedLeft,
        "trimmed_right", &pcl::IsoString::TrimmedRight,
        "left_justified", &pcl::IsoString::LeftJustified,
        "right_justified", &pcl::IsoString::RightJustified,
        "center_justified", &pcl::IsoString::CenterJustified,
        "enclosed", &pcl::IsoString::Enclosed,
        "single_quoted", &pcl::IsoString::SingleQuoted,
        "double_quoted", &pcl::IsoString::DoubleQuoted,
        "unquoted", &pcl::IsoString::Unquoted,
        "case_folded", &pcl::IsoString::CaseFolded
    );

    // Bind pcl::ImageVariant class
    lua->new_usertype<pcl::ImageVariant>("ImageVariant",
        sol::constructors<pcl::ImageVariant()>(),
        "image_ptr", &pcl::ImageVariant::ImagePtr,
        "image_ptr", &pcl::ImageVariant::ImagePtr,
        "is_same_image", &pcl::ImageVariant::IsSameImage,
        "is_float_sample", &pcl::ImageVariant::IsFloatSample,
        "is_complex_sample", &pcl::ImageVariant::IsComplexSample,
        "bits_per_sample", &pcl::ImageVariant::BitsPerSample,
        "bytes_per_sample", &pcl::ImageVariant::BytesPerSample,
        "width", &pcl::ImageVariant::Width,
        "height", &pcl::ImageVariant::Height,
        "number_of_pixels", &pcl::ImageVariant::NumberOfPixels,
        "number_of_channels", &pcl::ImageVariant::NumberOfChannels,
        "last_channel", &pcl::ImageVariant::LastChannel,
        "is_valid_channel_index", &pcl::ImageVariant::IsValidChannelIndex,
        "number_of_nominal_channels", &pcl::ImageVariant::NumberOfNominalChannels,
        "has_alpha_channels", &pcl::ImageVariant::HasAlphaChannels,
        "number_of_alpha_channels", &pcl::ImageVariant::NumberOfAlphaChannels,
        "number_of_samples", &pcl::ImageVariant::NumberOfSamples,
        "number_of_nominal_samples", &pcl::ImageVariant::NumberOfNominalSamples,
        "number_of_alpha_samples", &pcl::ImageVariant::NumberOfAlphaSamples
    );

    // Bind pcl::GridInterpolation class
    lua->new_usertype<pcl::GridInterpolation>("GridInterpolation",
        sol::constructors<pcl::GridInterpolation()>(),
        "is_valid", &pcl::GridInterpolation::IsValid,
        "clear", &pcl::GridInterpolation::Clear,
        "reference_rect", &pcl::GridInterpolation::ReferenceRect,
        "delta", &pcl::GridInterpolation::Delta,
        "interpolation_matrix", &pcl::GridInterpolation::InterpolationMatrix
    );

    // Bind pcl::ImageGeometry::Geometry structure
    lua->new_usertype<pcl::ImageGeometry::Geometry>("Geometry",
        "width", &pcl::ImageGeometry::Geometry::width,
        "height", &pcl::ImageGeometry::Geometry::height,
        "number_of_channels", &pcl::ImageGeometry::Geometry::numberOfChannels
    );

    // Bind pcl::ImageColor class
    lua->new_usertype<pcl::ImageColor>("ImageColor",
        "rgbworking_space", &pcl::ImageColor::RGBWorkingSpace,
        "set_rgbworking_space", &pcl::ImageColor::SetRGBWorkingSpace,
        "default_rgbworking_space", []() {
            auto result = pcl::ImageColor::DefaultRGBWorkingSpace();
            return result;
        },
        "set_default_rgbworking_space", [](const RGBColorSystem & RGBWS) {
            pcl::ImageColor::SetDefaultRGBWorkingSpace(RGBWS);
        },
        "reset_default_rgbworking_space", []() {
            pcl::ImageColor::ResetDefaultRGBWorkingSpace();
        },
        "is_color", &pcl::ImageColor::IsColor,
        "color_space", &pcl::ImageColor::ColorSpace,
        "channel_id", &pcl::ImageColor::ChannelId
    );

    // Bind EnhancedStarMatch structure
    lua->new_usertype<EnhancedStarMatch>("EnhancedStarMatch",
        "detected_index", &EnhancedStarMatch::detectedIndex,
        "catalog_index", &EnhancedStarMatch::catalogIndex,
        "pixel_distance", &EnhancedStarMatch::pixelDistance,
        "magnitude_difference", &EnhancedStarMatch::magnitudeDifference,
        "confidence", &EnhancedStarMatch::confidence,
        "triangle_error", &EnhancedStarMatch::triangleError,
        "supporting_matches", &EnhancedStarMatch::supportingMatches,
        "is_geometrically_valid", &EnhancedStarMatch::isGeometricallyValid,
        "is_photometrically_valid", &EnhancedStarMatch::isPhotometricallyValid,
        "snr", &EnhancedStarMatch::snr,
        "fwhm", &EnhancedStarMatch::fwhm,
        "ellipticity", &EnhancedStarMatch::ellipticity
    );

    // Bind LuaWindow class
    lua->new_usertype<LuaWindow>("LuaWindow",
        "tr", [](const char * s, const char * c, int n) {
            auto result = LuaWindow::tr(s, c, n);
            return result.toStdString();
        },
        "get_window_id", &LuaWindow::getWindowId,
        "get_window_title", &LuaWindow::getWindowTitle,
        "is_visible", &LuaWindow::isVisible,
        "add_label", &LuaWindow::addLabel,
        "add_button", &LuaWindow::addButton,
        "add_text_edit", &LuaWindow::addTextEdit,
        "add_slider", &LuaWindow::addSlider,
        "add_line_edit", &LuaWindow::addLineEdit,
        "set_on_button_click", &LuaWindow::setOnButtonClick,
        "set_on_slider_change", &LuaWindow::setOnSliderChange,
        "set_on_window_close", &LuaWindow::setOnWindowClose,
        "set_on_window_show", &LuaWindow::setOnWindowShow,
        "set_on_text_change", &LuaWindow::setOnTextChange,
        "get_widget_text", &LuaWindow::getWidgetText,
        "set_widget_text", &LuaWindow::setWidgetText,
        "get_slider_value", &LuaWindow::getSliderValue,
        "set_slider_value", &LuaWindow::setSliderValue,
        "show", &LuaWindow::show,
        "set_background_color", &LuaWindow::setBackgroundColor
    );

    // Bind pcl::Homography class
    lua->new_usertype<pcl::Homography>("Homography",
        sol::constructors<pcl::Homography()>(),
        sol::meta_function::call, &pcl::Homography::operator(),
        "inverse", &pcl::Homography::Inverse,
        "transformation_matrix", &pcl::Homography::TransformationMatrix,
        "is_valid", &pcl::Homography::IsValid,
        "is_affine", &pcl::Homography::IsAffine,
        "ensure_unique", &pcl::Homography::EnsureUnique
    );

    // Bind pcl::ImageColor::Color structure
    lua->new_usertype<pcl::ImageColor::Color>("Color",
        "color_space", &pcl::ImageColor::Color::colorSpace,
        "rgbws", &pcl::ImageColor::Color::RGBWS
    );

    // Bind pcl::Property class
    lua->new_usertype<pcl::Property>("Property",
        sol::constructors<pcl::Property()>(),
        "identifier", &pcl::Property::Identifier,
        "id", &pcl::Property::Id,
        "value", &pcl::Property::Value,
        "set_value", &pcl::Property::SetValue,
        "type", &pcl::Property::Type,
        "description", &pcl::Property::Description,
        "is_valid_identifier", [](const pcl::IsoString & id) {
            auto result = pcl::Property::IsValidIdentifier(id);
            return result;
        },
        "is_valid_identifier", [](const pcl::IsoString::ustring_base & id) {
            auto result = pcl::Property::IsValidIdentifier(id);
            return result;
        },
        "is_valid", &pcl::Property::IsValid,
        sol::meta_function::equal_to, &pcl::Property::operator==,
        sol::meta_function::less_than, &pcl::Property::operator<
    );

    // Bind pcl::FileInfo class
    lua->new_usertype<pcl::FileInfo>("FileInfo",
        sol::constructors<pcl::FileInfo()>(),
        sol::meta_function::less_than, &pcl::FileInfo::operator<,
        sol::meta_function::equal_to, &pcl::FileInfo::operator==,
        "path", &pcl::FileInfo::Path,
        "drive", &pcl::FileInfo::Drive,
        "directory", &pcl::FileInfo::Directory,
        "name", &pcl::FileInfo::Name,
        "extension", &pcl::FileInfo::Extension,
        "suffix", &pcl::FileInfo::Suffix,
        "complete_suffix", &pcl::FileInfo::CompleteSuffix,
        "name_and_extension", &pcl::FileInfo::NameAndExtension,
        "name_and_suffix", &pcl::FileInfo::NameAndSuffix,
        "exists", &pcl::FileInfo::Exists,
        "attributes", &pcl::FileInfo::Attributes,
        "is_directory", &pcl::FileInfo::IsDirectory,
        "is_file", &pcl::FileInfo::IsFile,
        "is_symbolic_link", &pcl::FileInfo::IsSymbolicLink,
        "symbolic_link_target", &pcl::FileInfo::SymbolicLinkTarget,
        "is_hidden", &pcl::FileInfo::IsHidden,
        "is_readable", &pcl::FileInfo::IsReadable
    );

    // Bind pcl::File::Progress class
    lua->new_usertype<pcl::File::Progress>("Progress",
        "total", &pcl::File::Progress::Total,
        "value", &pcl::File::Progress::Value,
        "text", &pcl::File::Progress::Text,
        "set_value", &pcl::File::Progress::SetValue,
        "add", &pcl::File::Progress::Add,
        "set_text", &pcl::File::Progress::SetText,
        "is_aborted", &pcl::File::Progress::IsAborted
    );

    // Bind StarMatch structure
    lua->new_usertype<StarMatch>("StarMatch",
        "detected_index", &StarMatch::detectedIndex,
        "catalog_index", &StarMatch::catalogIndex,
        "distance", &StarMatch::distance,
        "magnitude_diff", &StarMatch::magnitudeDiff,
        "is_good_match", &StarMatch::isGoodMatch
    );

    // Bind LuaWindowFactory class
    lua->new_usertype<LuaWindowFactory>("LuaWindowFactory",
        "tr", [](const char * s, const char * c, int n) {
            auto result = LuaWindowFactory::tr(s, c, n);
            return result.toStdString();
        },
        "setup_window_menu", &LuaWindowFactory::setupWindowMenu,
        "create_window", &LuaWindowFactory::createWindow,
        "register_widget", [](LuaWindowFactory& obj, QWidget * widget, const std::string& title) {
            obj.registerWidget(widget, QString::fromStdString(title));
        },
        "unregister_widget", &LuaWindowFactory::unregisterWidget,
        "update_widget_title", [](LuaWindowFactory& obj, QWidget * widget, const std::string& newTitle) {
            obj.updateWidgetTitle(widget, QString::fromStdString(newTitle));
        },
        "add_window_to_menu", &LuaWindowFactory::addWindowToMenu,
        "remove_window_from_menu", &LuaWindowFactory::removeWindowFromMenu,
        "update_keyboard_shortcuts", &LuaWindowFactory::updateKeyboardShortcuts,
        "update_window_action_state", &LuaWindowFactory::updateWindowActionState,
        "update_window_menu", &LuaWindowFactory::updateWindowMenu,
        "get_window_count", &LuaWindowFactory::getWindowCount,
        "close_all_windows", &LuaWindowFactory::closeAllWindows,
        "show_all_windows", &LuaWindowFactory::showAllWindows,
        "hide_all_windows", &LuaWindowFactory::hideAllWindows,
        "get_window_titles", &LuaWindowFactory::getWindowTitles,
        "find_window_by_title", &LuaWindowFactory::findWindowByTitle,
        "find_window_by_id", &LuaWindowFactory::findWindowById
    );

    // Bind pcl::FITSHeaderKeyword class
    lua->new_usertype<pcl::FITSHeaderKeyword>("FITSHeaderKeyword",
        sol::constructors<pcl::FITSHeaderKeyword()>(),
        "is_null", &pcl::FITSHeaderKeyword::IsNull,
        "is_string", &pcl::FITSHeaderKeyword::IsString,
        "is_boolean", &pcl::FITSHeaderKeyword::IsBoolean,
        "is_numeric", &pcl::FITSHeaderKeyword::IsNumeric,
        "get_numeric_value", &pcl::FITSHeaderKeyword::GetNumericValue,
        "strip_value_delimiters", &pcl::FITSHeaderKeyword::StripValueDelimiters,
        "fix_value_delimiters", &pcl::FITSHeaderKeyword::FixValueDelimiters,
        "trim", &pcl::FITSHeaderKeyword::Trim,
        "name", &pcl::FITSHeaderKeyword::name,
        "value", &pcl::FITSHeaderKeyword::value,
        "comment", &pcl::FITSHeaderKeyword::comment
    );

    // Bind pcl::Error class
    lua->new_usertype<pcl::Error>("Error",
        sol::constructors<pcl::Error()>(),
        "message", &pcl::Error::Message,
        "caption", &pcl::Error::Caption,
        "shown_unformatted", &pcl::Error::ShownUnformatted
    );

    // Bind ValidationResult structure
    lua->new_usertype<ValidationResult>("ValidationResult",
        "catalog_stars", &ValidationResult::catalogStars,
        "matches", &ValidationResult::matches,
        "unmatched_detected", &ValidationResult::unmatchedDetected,
        "unmatched_catalog", &ValidationResult::unmatchedCatalog,
        "total_detected", &ValidationResult::totalDetected,
        "total_catalog", &ValidationResult::totalCatalog,
        "total_matches", &ValidationResult::totalMatches,
        "match_percentage", &ValidationResult::matchPercentage,
        "average_position_error", &ValidationResult::averagePositionError,
        "rms_position_error", &ValidationResult::rmsPositionError,
        "summary", &ValidationResult::summary,
        "is_valid", &ValidationResult::isValid
    );

    // Bind pcl::File class
    lua->new_usertype<pcl::File>("File",
        sol::constructors<pcl::File()>(),
        "is_open", &pcl::File::IsOpen,
        "file_path", &pcl::File::FilePath,
        "file_name", &pcl::File::FileName,
        "mode", &pcl::File::Mode,
        "can_read", &pcl::File::CanRead,
        "can_write", &pcl::File::CanWrite,
        "position", &pcl::File::Position,
        "set_position", &pcl::File::SetPosition,
        "seek", &pcl::File::Seek,
        "seek_end", &pcl::File::SeekEnd,
        "rewind", &pcl::File::Rewind,
        "is_eof", &pcl::File::IsEOF,
        "size", &pcl::File::Size,
        "resize", &pcl::File::Resize,
        "read", &pcl::File::Read,
        "write", &pcl::File::Write,
        "read", &pcl::File::Read,
        "write", &pcl::File::Write,
        "read", &pcl::File::Read
    );

    // Bind pcl::File::Error class
    lua->new_usertype<pcl::File::Error>("Error",
        "exception_class", &pcl::File::Error::ExceptionClass,
        "file_path", &pcl::File::Error::FilePath,
        "error_message", &pcl::File::Error::ErrorMessage,
        "message", &pcl::File::Error::Message
    );

    // Bind pcl::Compression::Subblock structure
    lua->new_usertype<pcl::Compression::Subblock>("Subblock",
        "compressed_data", &pcl::Compression::Subblock::compressedData,
        "uncompressed_size", &pcl::Compression::Subblock::uncompressedSize,
        "checksum", &pcl::Compression::Subblock::checksum
    );

    // Bind StarMaskResult structure
    lua->new_usertype<StarMaskResult>("StarMaskResult",
        "star_centers", &StarMaskResult::starCenters,
        "star_radii", &StarMaskResult::starRadii,
        "star_fluxes", &StarMaskResult::starFluxes,
        "star_valid", &StarMaskResult::starValid,
        "mask_image", &StarMaskResult::maskImage
    );

    // Bind pcl::StandardStatus class
    lua->new_usertype<pcl::StandardStatus>("StandardStatus",
        sol::constructors<pcl::StandardStatus()>(),
        "initialized", &pcl::StandardStatus::Initialized,
        "updated", &pcl::StandardStatus::Updated,
        "completed", &pcl::StandardStatus::Completed,
        "info_updated", &pcl::StandardStatus::InfoUpdated
    );

    // Bind pcl::Compression class
    lua->new_usertype<pcl::Compression>("Compression",
        sol::constructors<pcl::Compression()>(),
        "algorithm_name", &pcl::Compression::AlgorithmName,
        "min_compression_level", &pcl::Compression::MinCompressionLevel,
        "max_compression_level", &pcl::Compression::MaxCompressionLevel,
        "default_compression_level", &pcl::Compression::DefaultCompressionLevel,
        "compression_level", &pcl::Compression::CompressionLevel,
        "set_compression_level", &pcl::Compression::SetCompressionLevel,
        "set_default_compression_level", &pcl::Compression::SetDefaultCompressionLevel,
        "byte_shuffling_enabled", &pcl::Compression::ByteShufflingEnabled,
        "enable_byte_shuffling", &pcl::Compression::EnableByteShuffling,
        "disable_byte_shuffling", &pcl::Compression::DisableByteShuffling,
        "item_size", &pcl::Compression::ItemSize,
        "set_item_size", &pcl::Compression::SetItemSize,
        "subblock_size", &pcl::Compression::SubblockSize,
        "set_subblock_size", &pcl::Compression::SetSubblockSize,
        "checksums_enabled", &pcl::Compression::ChecksumsEnabled,
        "enable_checksums", &pcl::Compression::EnableChecksums,
        "disable_checksums", &pcl::Compression::DisableChecksums,
        "compress", &pcl::Compression::Compress,
        "uncompress", &pcl::Compression::Uncompress
    );

    // Bind pcl::SourceCodeError class
    lua->new_usertype<pcl::SourceCodeError>("SourceCodeError",
        sol::constructors<pcl::SourceCodeError()>(),
        "line_number", &pcl::SourceCodeError::LineNumber,
        "column_number", &pcl::SourceCodeError::ColumnNumber,
        "message", &pcl::SourceCodeError::Message,
        "show", &pcl::SourceCodeError::Show
    );

    // Bind MenuIntegratedWidget class
    lua->new_usertype<MenuIntegratedWidget>("MenuIntegratedWidget",
        "tr", [](const char * s, const char * c, int n) {
            auto result = MenuIntegratedWidget::tr(s, c, n);
            return result.toStdString();
        },
        "register_with_menu", [](MenuIntegratedWidget& obj, const std::string& title) {
            obj.registerWithMenu(QString::fromStdString(title));
        },
        "unregister_from_menu", &MenuIntegratedWidget::unregisterFromMenu,
        "set_window_title", [](MenuIntegratedWidget& obj, const std::string& title) {
            obj.setWindowTitle(QString::fromStdString(title));
        },
        "set_custom_title", [](MenuIntegratedWidget& obj, const std::string& title) {
            obj.setCustomTitle(QString::fromStdString(title));
        },
        "reset_to_auto_title", &MenuIntegratedWidget::resetToAutoTitle,
        "show", &MenuIntegratedWidget::show,
        "hide", &MenuIntegratedWidget::hide,
        "set_visible", &MenuIntegratedWidget::setVisible,
        "is_registered", &MenuIntegratedWidget::isRegistered,
        "is_using_auto_generated_title", &MenuIntegratedWidget::isUsingAutoGeneratedTitle,
        "get_menu_title", [](MenuIntegratedWidget& obj) {
            auto result = obj.getMenuTitle();
            return result.toStdString();
        },
        "visibility_changed", &MenuIntegratedWidget::visibilityChanged,
        "title_changed", [](MenuIntegratedWidget& obj, const std::string& title) {
            obj.titleChanged(QString::fromStdString(title));
        }
    );

    // Bind pcl::ZstdCompression class
    lua->new_usertype<pcl::ZstdCompression>("ZstdCompression",
        "algorithm_name", &pcl::ZstdCompression::AlgorithmName,
        "min_compression_level", &pcl::ZstdCompression::MinCompressionLevel,
        "max_compression_level", &pcl::ZstdCompression::MaxCompressionLevel,
        "default_compression_level", &pcl::ZstdCompression::DefaultCompressionLevel
    );

    // Bind pcl::AstrometricMetadata::DescriptionItems structure
    lua->new_usertype<pcl::AstrometricMetadata::DescriptionItems>("DescriptionItems",
        "reference_matrix", &pcl::AstrometricMetadata::DescriptionItems::referenceMatrix,
        "wcs_transformation_type", &pcl::AstrometricMetadata::DescriptionItems::wcsTransformationType,
        "control_points", &pcl::AstrometricMetadata::DescriptionItems::controlPoints,
        "spline_lengths", &pcl::AstrometricMetadata::DescriptionItems::splineLengths,
        "spline_errors", &pcl::AstrometricMetadata::DescriptionItems::splineErrors,
        "projection_name", &pcl::AstrometricMetadata::DescriptionItems::projectionName,
        "projection_origin", &pcl::AstrometricMetadata::DescriptionItems::projectionOrigin,
        "resolution", &pcl::AstrometricMetadata::DescriptionItems::resolution,
        "rotation", &pcl::AstrometricMetadata::DescriptionItems::rotation,
        "reference_system", &pcl::AstrometricMetadata::DescriptionItems::referenceSystem,
        "observation_start_time", &pcl::AstrometricMetadata::DescriptionItems::observationStartTime,
        "observation_end_time", &pcl::AstrometricMetadata::DescriptionItems::observationEndTime,
        "observer_location", &pcl::AstrometricMetadata::DescriptionItems::observerLocation,
        "focal_distance", &pcl::AstrometricMetadata::DescriptionItems::focalDistance,
        "pixel_size", &pcl::AstrometricMetadata::DescriptionItems::pixelSize,
        "field_of_view", &pcl::AstrometricMetadata::DescriptionItems::fieldOfView,
        "center_coordinates", &pcl::AstrometricMetadata::DescriptionItems::centerCoordinates,
        "top_left_coordinates", &pcl::AstrometricMetadata::DescriptionItems::topLeftCoordinates,
        "top_right_coordinates", &pcl::AstrometricMetadata::DescriptionItems::topRightCoordinates,
        "bottom_left_coordinates", &pcl::AstrometricMetadata::DescriptionItems::bottomLeftCoordinates,
        "bottom_right_coordinates", &pcl::AstrometricMetadata::DescriptionItems::bottomRightCoordinates,
        "reference_catalog", &pcl::AstrometricMetadata::DescriptionItems::referenceCatalog,
        "creation_time", &pcl::AstrometricMetadata::DescriptionItems::creationTime,
        "creation_software", &pcl::AstrometricMetadata::DescriptionItems::creationSoftware
    );

    // Bind pcl::AtomicInt class
    lua->new_usertype<pcl::AtomicInt>("AtomicInt",
        sol::meta_function::equal_to, &pcl::AtomicInt::operator==,
        sol::meta_function::not_equal_to, &pcl::AtomicInt::operator!=,
        "load", &pcl::AtomicInt::Load,
        "store", &pcl::AtomicInt::Store,
        "increment", &pcl::AtomicInt::Increment,
        "decrement", &pcl::AtomicInt::Decrement,
        "reference", &pcl::AtomicInt::Reference,
        "dereference", &pcl::AtomicInt::Dereference,
        "test_and_set", &pcl::AtomicInt::TestAndSet,
        "fetch_and_store", &pcl::AtomicInt::FetchAndStore,
        "fetch_and_add", &pcl::AtomicInt::FetchAndAdd
    );

    // Bind pcl::AbstractImage class
    lua->new_usertype<pcl::AbstractImage>("AbstractImage",
        "number_of_nominal_channels", &pcl::AbstractImage::NumberOfNominalChannels,
        "number_of_nominal_samples", &pcl::AbstractImage::NumberOfNominalSamples,
        "has_alpha_channels", &pcl::AbstractImage::HasAlphaChannels,
        "number_of_alpha_channels", &pcl::AbstractImage::NumberOfAlphaChannels,
        "number_of_alpha_samples", &pcl::AbstractImage::NumberOfAlphaSamples,
        "select_channel", &pcl::AbstractImage::SelectChannel,
        "selected_channel", &pcl::AbstractImage::SelectedChannel,
        "select_channel_range", &pcl::AbstractImage::SelectChannelRange,
        "select_nominal_channels", &pcl::AbstractImage::SelectNominalChannels,
        "select_alpha_channels", &pcl::AbstractImage::SelectAlphaChannels,
        "reset_channel_range", &pcl::AbstractImage::ResetChannelRange,
        "number_of_selected_channels", &pcl::AbstractImage::NumberOfSelectedChannels,
        "first_selected_channel", &pcl::AbstractImage::FirstSelectedChannel,
        "last_selected_channel", &pcl::AbstractImage::LastSelectedChannel,
        "get_selected_channel_range", &pcl::AbstractImage::GetSelectedChannelRange,
        "select_point", &pcl::AbstractImage::SelectPoint,
        "select_point", &pcl::AbstractImage::SelectPoint,
        "reset_point", &pcl::AbstractImage::ResetPoint,
        "selected_point", &pcl::AbstractImage::SelectedPoint,
        "select_rectangle", &pcl::AbstractImage::SelectRectangle
    );

    // Bind pcl::ImageGeometry class
    lua->new_usertype<pcl::ImageGeometry>("ImageGeometry",
        "width", &pcl::ImageGeometry::Width,
        "height", &pcl::ImageGeometry::Height,
        "number_of_channels", &pcl::ImageGeometry::NumberOfChannels,
        "last_channel", &pcl::ImageGeometry::LastChannel,
        "is_valid_channel_index", &pcl::ImageGeometry::IsValidChannelIndex,
        "is_empty", &pcl::ImageGeometry::IsEmpty,
        "bounds", &pcl::ImageGeometry::Bounds,
        "number_of_pixels", &pcl::ImageGeometry::NumberOfPixels,
        "number_of_samples", &pcl::ImageGeometry::NumberOfSamples,
        "row_offset", &pcl::ImageGeometry::RowOffset,
        "pixel_offset", &pcl::ImageGeometry::PixelOffset
    );

    // Bind pcl::SurfaceSimplifier::point structure
    lua->new_usertype<pcl::SurfaceSimplifier::point>("point",
        "x", &pcl::SurfaceSimplifier::point::x,
        "y", &pcl::SurfaceSimplifier::point::y,
        "z", &pcl::SurfaceSimplifier::point::z
    );

    // Bind StarChartWidget class
    lua->new_usertype<StarChartWidget>("StarChartWidget",
        "tr", [](const char * s, const char * c, int n) {
            auto result = StarChartWidget::tr(s, c, n);
            return result.toStdString();
        },
        "draw_axis_ticks", &StarChartWidget::drawAxisTicks,
        "set_star_data", &StarChartWidget::setStarData,
        "set_plot_mode", &StarChartWidget::setPlotMode,
        "set_color_scheme", &StarChartWidget::setColorScheme,
        "set_show_legend", &StarChartWidget::setShowLegend,
        "set_show_grid", &StarChartWidget::setShowGrid,
        "reset_zoom", &StarChartWidget::resetZoom,
        "size_hint", &StarChartWidget::sizeHint,
        "minimum_size_hint", &StarChartWidget::minimumSizeHint,
        "star_clicked", &StarChartWidget::starClicked,
        "star_hovered", &StarChartWidget::starHovered
    );

    // Bind StarOverlay structure
    lua->new_usertype<StarOverlay>("StarOverlay",
        "center", &StarOverlay::center,
        "radius", &StarOverlay::radius,
        "flux", &StarOverlay::flux,
        "color", &StarOverlay::color,
        "visible", &StarOverlay::visible
    );

    // Bind StarMatchingParameters structure
    lua->new_usertype<StarMatchingParameters>("StarMatchingParameters",
        "max_pixel_distance", &StarMatchingParameters::maxPixelDistance,
        "search_radius", &StarMatchingParameters::searchRadius,
        "max_magnitude_difference", &StarMatchingParameters::maxMagnitudeDifference,
        "use_magnitude_weighting", &StarMatchingParameters::useMagnitudeWeighting,
        "use_triangle_matching", &StarMatchingParameters::useTriangleMatching,
        "min_triangle_stars", &StarMatchingParameters::minTriangleStars,
        "triangle_tolerance_percent", &StarMatchingParameters::triangleTolerancePercent,
        "use_distortion_model", &StarMatchingParameters::useDistortionModel,
        "max_distortion_pixels", &StarMatchingParameters::maxDistortionPixels,
        "min_match_confidence", &StarMatchingParameters::minMatchConfidence,
        "min_matches_for_validation", &StarMatchingParameters::minMatchesForValidation,
        "use_proper_motion_correction", &StarMatchingParameters::useProperMotionCorrection,
        "target_epoch", &StarMatchingParameters::targetEpoch,
        "use_bayesian_matching", &StarMatchingParameters::useBayesianMatching
    );

    // Bind pcl::Homography::NormalizedPoints structure
    lua->new_usertype<pcl::Homography::NormalizedPoints>("NormalizedPoints",
        "n", &pcl::Homography::NormalizedPoints::N,
        "t", &pcl::Homography::NormalizedPoints::T
    );

    // Bind pcl::Variant class
    lua->new_usertype<pcl::Variant>("Variant",
        sol::constructors<pcl::Variant()>(),
        "assign", &pcl::Variant::Assign,
        "transfer", &pcl::Variant::Transfer,
        "transfer", &pcl::Variant::Transfer,
        "is_valid", &pcl::Variant::IsValid,
        "type", &pcl::Variant::Type,
        "to_type", &pcl::Variant::ToType,
        "clear", &pcl::Variant::Clear,
        "compare", &pcl::Variant::Compare,
        sol::meta_function::equal_to, &pcl::Variant::operator==,
        sol::meta_function::less_than, &pcl::Variant::operator<,
        "to_bool", &pcl::Variant::ToBool,
        "can_convert_to_bool", &pcl::Variant::CanConvertToBool,
        "to_boolean", &pcl::Variant::ToBoolean,
        "can_convert_to_boolean", &pcl::Variant::CanConvertToBoolean,
        "to_int", &pcl::Variant::ToInt,
        "can_convert_to_int", &pcl::Variant::CanConvertToInt,
        "to_int64", &pcl::Variant::ToInt64,
        "can_convert_to_int64", &pcl::Variant::CanConvertToInt64,
        "to_uint", &pcl::Variant::ToUInt
    );

    // Bind LuaChartWidget class
    lua->new_usertype<LuaChartWidget>("LuaChartWidget",
        "tr", [](const char * s, const char * c, int n) {
            auto result = LuaChartWidget::tr(s, c, n);
            return result.toStdString();
        },
        "set_frequency", &LuaChartWidget::setFrequency,
        "set_amplitude", &LuaChartWidget::setAmplitude,
        "set_phase", &LuaChartWidget::setPhase,
        "get_frequency", &LuaChartWidget::getFrequency,
        "get_amplitude", &LuaChartWidget::getAmplitude,
        "get_phase", &LuaChartWidget::getPhase,
        "add_point", &LuaChartWidget::addPoint,
        "clear_series", &LuaChartWidget::clearSeries,
        "plot_function", &LuaChartWidget::plotFunction,
        "set_title", &LuaChartWidget::setTitle,
        "set_axis_labels", &LuaChartWidget::setAxisLabels
    );

    // Bind StarCatalogValidator class
    lua->new_usertype<StarCatalogValidator>("StarCatalogValidator",
        "tr", [](const char * s, const char * c, int n) {
            auto result = StarCatalogValidator::tr(s, c, n);
            return result.toStdString();
        },
        "validate_stars_advanced", &StarCatalogValidator::validateStarsAdvanced,
        "set_matching_parameters", &StarCatalogValidator::setMatchingParameters,
        "get_matching_parameters", &StarCatalogValidator::getMatchingParameters,
        "query_catalog_with_patterns", &StarCatalogValidator::queryCatalogWithPatterns,
        "calibrate_distortion_model", &StarCatalogValidator::calibrateDistortionModel,
        "apply_distortion_correction", &StarCatalogValidator::applyDistortionCorrection,
        "set_catalog_source", &StarCatalogValidator::setCatalogSource,
        "set_validation_mode", &StarCatalogValidator::setValidationMode,
        "set_matching_tolerance", &StarCatalogValidator::setMatchingTolerance,
        "set_wcsdata", &StarCatalogValidator::setWCSData,
        "set_wcsfrom_metadata", [](StarCatalogValidator& obj, const std::string& metadata) {
            obj.setWCSFromMetadata(QString::fromStdString(metadata));
        },
        "get_wcsdata", &StarCatalogValidator::getWCSData,
        "has_valid_wcs", &StarCatalogValidator::hasValidWCS,
        "set_wcsfrom_pclkeywords", &StarCatalogValidator::setWCSFromPCLKeywords,
        "set_wcsfrom_image_metadata", &StarCatalogValidator::setWCSFromImageMetadata,
        "set_wcsfrom_solver", &StarCatalogValidator::setWCSFromSolver,
        "sky_to_pixel", &StarCatalogValidator::skyToPixel,
        "pixel_to_sky", &StarCatalogValidator::pixelToSky,
        "query_catalog", &StarCatalogValidator::queryCatalog
    );

    // Bind GenericDataTableWidget class
    lua->new_usertype<GenericDataTableWidget>("GenericDataTableWidget",
        "tr", [](const char * s, const char * c, int n) {
            auto result = GenericDataTableWidget::tr(s, c, n);
            return result.toStdString();
        },
        "setup_ui", [](GenericDataTableWidget& obj, const std::string& title) {
            obj.setupUI(QString::fromStdString(title));
        },
        "set_lua_state", &GenericDataTableWidget::setLuaState,
        "display_data", [](GenericDataTableWidget& obj, const sol::object & data, const std::string& dataName) {
            obj.displayData(data, QString::fromStdString(dataName));
        },
        "set_custom_headers", [](GenericDataTableWidget& obj, const std::string& headers) {
            obj.setCustomHeaders(QString::fromStdString(headers));
        },
        "set_custom_row_labels", [](GenericDataTableWidget& obj, const std::string& rowLabels) {
            obj.setCustomRowLabels(QString::fromStdString(rowLabels));
        },
        "set_matrix_metadata", [](GenericDataTableWidget& obj, const std::string& title, const std::string& description) {
            obj.setMatrixMetadata(QString::fromStdString(title), QString::fromStdString(description));
        },
        "refresh_display", &GenericDataTableWidget::refreshDisplay,
        "apply_filter", &GenericDataTableWidget::applyFilter,
        "copy_to_clipboard", &GenericDataTableWidget::copyToClipboard,
        "save_as_csv", &GenericDataTableWidget::saveAsCSV,
        "clear_table", &GenericDataTableWidget::clearTable
    );

    // Bind pcl::Node structure
    lua->new_usertype<pcl::Node>("Node",
        "rect", &pcl::Node::rect,
        "nw", &pcl::Node::nw,
        "ne", &pcl::Node::ne,
        "sw", &pcl::Node::sw,
        "se", &pcl::Node::se
    );

    // Bind TrianglePattern structure
    lua->new_usertype<TrianglePattern>("TrianglePattern",
        "star_indices", &TrianglePattern::starIndices,
        "side1", &TrianglePattern::side1,
        "side2", &TrianglePattern::side2,
        "side3", &TrianglePattern::side3,
        "angle1", &TrianglePattern::angle1,
        "angle2", &TrianglePattern::angle2,
        "angle3", &TrianglePattern::angle3,
        "area", &TrianglePattern::area,
        "centroid", &TrianglePattern::centroid,
        "ratio12", &TrianglePattern::ratio12,
        "ratio13", &TrianglePattern::ratio13,
        "ratio23", &TrianglePattern::ratio23,
        "normalized_area", &TrianglePattern::normalizedArea
    );

    // Bind pcl::Exception class
    lua->new_usertype<pcl::Exception>("Exception",
        sol::constructors<pcl::Exception()>(),
        "exception_class", &pcl::Exception::ExceptionClass,
        "message", &pcl::Exception::Message,
        "format_info", &pcl::Exception::FormatInfo,
        "caption", &pcl::Exception::Caption,
        "show", &pcl::Exception::Show,
        "shown_unformatted", &pcl::Exception::ShownUnformatted,
        "show_on_console", &pcl::Exception::ShowOnConsole,
        "is_console_output_enabled", []() {
            auto result = pcl::Exception::IsConsoleOutputEnabled();
            return result;
        },
        "enable_console_output", [](bool ) {
            pcl::Exception::EnableConsoleOutput();
        },
        "disable_console_output", [](bool disable) {
            pcl::Exception::DisableConsoleOutput(disable);
        },
        "is_guioutput_enabled", []() {
            auto result = pcl::Exception::IsGUIOutputEnabled();
            return result;
        },
        "enable_guioutput", [](bool ) {
            pcl::Exception::EnableGUIOutput();
        },
        "disable_guioutput", [](bool disable) {
            pcl::Exception::DisableGUIOutput(disable);
        }
    );

    // Bind StarMaskGenerator class
    lua->new_usertype<StarMaskGenerator>("StarMaskGenerator",
        "detect_stars_old", [](const ImageData & imageData, float threshold) {
            auto result = StarMaskGenerator::detectStarsOld(imageData, threshold);
            return result;
        },
        "detect_stars_advanced", [](const ImageData & imageData, float sensitivity, int structureLayers, int noiseLayers, float peakResponse, float maxDistortion, bool enablePSFFitting) {
            auto result = StarMaskGenerator::detectStarsAdvanced(imageData, sensitivity, structureLayers, noiseLayers, peakResponse, maxDistortion, enablePSFFitting);
            return result;
        },
        "dumpcat", [](int & catalogStars) {
            StarMaskGenerator::dumpcat(catalogStars);
        },
        "validate_star_detection", []() {
            StarMaskGenerator::validateStarDetection();
        }
    );

    // Bind LuaMatrix class
    lua->new_usertype<LuaMatrix>("LuaMatrix",
        "get", &LuaMatrix::get,
        "set", &LuaMatrix::set,
        "get_rows", &LuaMatrix::getRows,
        "get_cols", &LuaMatrix::getCols,
        "multiply", &LuaMatrix::multiply,
        "add", &LuaMatrix::add,
        "subtract", &LuaMatrix::subtract,
        "transpose", &LuaMatrix::transpose,
        "scale", &LuaMatrix::scale,
        "determinant", &LuaMatrix::determinant,
        "fill_random", &LuaMatrix::fillRandom,
        "fill_identity", &LuaMatrix::fillIdentity,
        "to_string", &LuaMatrix::toString,
        "get_row", &LuaMatrix::getRow,
        "get_col", &LuaMatrix::getCol
    );

    // Bind pcl::TwoSidedEstimate structure
    lua->new_usertype<pcl::TwoSidedEstimate>("TwoSidedEstimate",
        "low", &pcl::TwoSidedEstimate::low,
        "high", &pcl::TwoSidedEstimate::high
    );

    // Bind pcl::LinearTransformation class
    lua->new_usertype<pcl::LinearTransformation>("LinearTransformation",
        sol::constructors<pcl::LinearTransformation()>(),
        sol::meta_function::less_than, &pcl::LinearTransformation::operator<,
        sol::meta_function::equal_to, &pcl::LinearTransformation::operator==,
        "determinant", &pcl::LinearTransformation::Determinant,
        "is_singular_matrix", &pcl::LinearTransformation::IsSingularMatrix,
        "transform", &pcl::LinearTransformation::Transform,
        sol::meta_function::call, &pcl::LinearTransformation::operator(),
        "transform_inverse", &pcl::LinearTransformation::TransformInverse,
        "multiply", &pcl::LinearTransformation::Multiply,
        "inverse", &pcl::LinearTransformation::Inverse,
        "a00", &pcl::LinearTransformation::A00,
        "a01", &pcl::LinearTransformation::A01,
        "a02", &pcl::LinearTransformation::A02,
        "a10", &pcl::LinearTransformation::A10,
        "a11", &pcl::LinearTransformation::A11,
        "a12", &pcl::LinearTransformation::A12,
        "to_matrix", &pcl::LinearTransformation::ToMatrix,
        "to2x3matrix", &pcl::LinearTransformation::To2x3Matrix,
        "to_vector", &pcl::LinearTransformation::ToVector,
        "to_string", &pcl::LinearTransformation::ToString
    );

    // Bind BrightStarDatabase::BrightStar structure
    lua->new_usertype<BrightStarDatabase::BrightStar>("BrightStar",
        "name", &BrightStarDatabase::BrightStar::name,
        "id", &BrightStarDatabase::BrightStar::id,
        "ra", &BrightStarDatabase::BrightStar::ra,
        "dec", &BrightStarDatabase::BrightStar::dec,
        "magnitude", &BrightStarDatabase::BrightStar::magnitude,
        "spectral_type", &BrightStarDatabase::BrightStar::spectralType,
        "constellation", &BrightStarDatabase::BrightStar::constellation
    );

    // Bind pcl::PointGridInterpolation class
    lua->new_usertype<pcl::PointGridInterpolation>("PointGridInterpolation",
        sol::constructors<pcl::PointGridInterpolation()>(),
        "is_valid", &pcl::PointGridInterpolation::IsValid,
        "clear", &pcl::PointGridInterpolation::Clear,
        "reference_rect", &pcl::PointGridInterpolation::ReferenceRect,
        "delta", &pcl::PointGridInterpolation::Delta,
        "xinterpolation_matrix", &pcl::PointGridInterpolation::XInterpolationMatrix,
        "yinterpolation_matrix", &pcl::PointGridInterpolation::YInterpolationMatrix
    );

    // Bind ImageDisplayWidget class
    lua->new_usertype<ImageDisplayWidget>("ImageDisplayWidget",
        "tr", [](const char * s, const char * c, int n) {
            auto result = ImageDisplayWidget::tr(s, c, n);
            return result.toStdString();
        },
        "set_image_data", &ImageDisplayWidget::setImageData,
        "clear_image", &ImageDisplayWidget::clearImage,
        "set_zoom_factor", &ImageDisplayWidget::setZoomFactor,
        "zoom_factor", &ImageDisplayWidget::zoomFactor,
        "set_auto_stretch", &ImageDisplayWidget::setAutoStretch,
        "auto_stretch", &ImageDisplayWidget::autoStretch,
        "set_stretch_limits", &ImageDisplayWidget::setStretchLimits,
        "get_stretch_limits", &ImageDisplayWidget::getStretchLimits,
        "set_star_overlay", &ImageDisplayWidget::setStarOverlay,
        "clear_star_overlay", &ImageDisplayWidget::clearStarOverlay,
        "set_star_overlay_visible", &ImageDisplayWidget::setStarOverlayVisible,
        "is_star_overlay_visible", &ImageDisplayWidget::isStarOverlayVisible,
        "set_validation_results", &ImageDisplayWidget::setValidationResults,
        "clear_validation_results", &ImageDisplayWidget::clearValidationResults,
        "set_catalog_overlay_visible", &ImageDisplayWidget::setCatalogOverlayVisible,
        "set_validation_overlay_visible", &ImageDisplayWidget::setValidationOverlayVisible,
        "is_catalog_overlay_visible", &ImageDisplayWidget::isCatalogOverlayVisible,
        "is_validation_overlay_visible", &ImageDisplayWidget::isValidationOverlayVisible,
        "set_wcsdata", &ImageDisplayWidget::setWCSData
    );

    // Bind ImageReader class
    lua->new_usertype<ImageReader>("ImageReader",
        sol::constructors<ImageReader()>(),
        "read_file", [](ImageReader& obj, const std::string& filePath) {
            auto result = obj.readFile(QString::fromStdString(filePath));
            return result;
        },
        "image_data", &ImageReader::imageData,
        "has_image", &ImageReader::hasImage,
        "clear", &ImageReader::clear,
        "set_image_data", &ImageReader::setImageData,
        "last_error", [](ImageReader& obj) {
            auto result = obj.lastError();
            return result.toStdString();
        },
        "supported_formats", []() {
            auto result = ImageReader::supportedFormats();
            return result.toStdString();
        },
        "format_filter", []() {
            auto result = ImageReader::formatFilter();
            return result.toStdString();
        },
        "detect_file_type", [](const std::string& filePath) {
            auto result = ImageReader::detectFileType(QString::fromStdString(filePath));
            return result.toStdString();
        }
    );

    // Bind pcl::RecursivePointSurfaceSpline::Node structure
    lua->new_usertype<pcl::RecursivePointSurfaceSpline::Node>("Node",
        "position", &pcl::RecursivePointSurfaceSpline::Node::position,
        "value", &pcl::RecursivePointSurfaceSpline::Node::value,
        "weight", &pcl::RecursivePointSurfaceSpline::Node::weight
    );

    // Bind EnhancedValidationResult structure
    lua->new_usertype<EnhancedValidationResult>("EnhancedValidationResult",
        "enhanced_matches", &EnhancedValidationResult::enhancedMatches,
        "detected_triangles", &EnhancedValidationResult::detectedTriangles,
        "catalog_triangles", &EnhancedValidationResult::catalogTriangles,
        "triangle_matches", &EnhancedValidationResult::triangleMatches,
        "geometric_rms", &EnhancedValidationResult::geometricRMS,
        "photometric_rms", &EnhancedValidationResult::photometricRMS,
        "astrometric_accuracy", &EnhancedValidationResult::astrometricAccuracy,
        "matching_confidence", &EnhancedValidationResult::matchingConfidence,
        "scale_error", &EnhancedValidationResult::scaleError,
        "rotation_error", &EnhancedValidationResult::rotationError,
        "residual_vectors", &EnhancedValidationResult::residualVectors,
        "radial_distortions", &EnhancedValidationResult::radialDistortions,
        "enhanced_summary", &EnhancedValidationResult::enhancedSummary
    );

    // Bind RGBPhotometryAnalyzer class
    lua->new_usertype<RGBPhotometryAnalyzer>("RGBPhotometryAnalyzer",
        "tr", [](const char * s, const char * c, int n) {
            auto result = RGBPhotometryAnalyzer::tr(s, c, n);
            return result.toStdString();
        },
        "analyze_star_colors", &RGBPhotometryAnalyzer::analyzeStarColors,
        "set_star_catalog_data", &RGBPhotometryAnalyzer::setStarCatalogData,
        "calculate_color_calibration", &RGBPhotometryAnalyzer::calculateColorCalibration,
        "set_aperture_radius", &RGBPhotometryAnalyzer::setApertureRadius,
        "set_background_annulus", &RGBPhotometryAnalyzer::setBackgroundAnnulus,
        "set_color_index_type", [](RGBPhotometryAnalyzer& obj, const std::string& type) {
            obj.setColorIndexType(QString::fromStdString(type));
        },
        "get_star_color_data", &RGBPhotometryAnalyzer::getStarColorData,
        "get_last_calibration", &RGBPhotometryAnalyzer::getLastCalibration,
        "spectral_type_to_color_index", [](const std::string& spectralType) {
            auto result = RGBPhotometryAnalyzer::spectralTypeToColorIndex(QString::fromStdString(spectralType));
            return result;
        },
        "color_index_to_rgb", [](double bv_index) {
            auto result = RGBPhotometryAnalyzer::colorIndexToRGB(bv_index);
            return result;
        },
        "color_analysis_completed", &RGBPhotometryAnalyzer::colorAnalysisCompleted,
        "calibration_completed", &RGBPhotometryAnalyzer::calibrationCompleted
    );

    // Bind EnhancedStarMatcher class
    lua->new_usertype<EnhancedStarMatcher>("EnhancedStarMatcher",
        "match_stars_advanced", &EnhancedStarMatcher::matchStarsAdvanced,
        "perform_initial_matching", &EnhancedStarMatcher::performInitialMatching,
        "perform_triangle_matching", &EnhancedStarMatcher::performTriangleMatching,
        "perform_geometric_validation", &EnhancedStarMatcher::performGeometricValidation,
        "generate_triangle_patterns", &EnhancedStarMatcher::generateTrianglePatterns,
        "generate_triangle_patterns", &EnhancedStarMatcher::generateTrianglePatterns,
        "match_triangle_patterns", &EnhancedStarMatcher::matchTrianglePatterns,
        "analyze_distortions", &EnhancedStarMatcher::analyzeDistortions,
        "calculate_match_confidence", &EnhancedStarMatcher::calculateMatchConfidence,
        "filter_low_quality_matches", &EnhancedStarMatcher::filterLowQualityMatches,
        "set_parameters", &EnhancedStarMatcher::setParameters,
        "get_parameters", &EnhancedStarMatcher::getParameters
    );

    // Bind AcceleratedMatrix class
    lua->new_usertype<AcceleratedMatrix>("AcceleratedMatrix",
        "get", &AcceleratedMatrix::get,
        "set", &AcceleratedMatrix::set,
        "get_rows", &AcceleratedMatrix::getRows,
        "get_cols", &AcceleratedMatrix::getCols,
        "get_data", &AcceleratedMatrix::getData,
        "get_data", &AcceleratedMatrix::getData,
        "multiply_accelerate", &AcceleratedMatrix::multiplyAccelerate,
        "multiply_vector", &AcceleratedMatrix::multiplyVector
    );

// Generated bindings for 86 classes
