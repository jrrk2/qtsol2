-- Query catalog and get stars
validator = StarCatalogValidator.new(nil)
validator:queryCatalog(300.0804, 22.748, 1)
catalog_stars = validator:getCatalogStars()

print("Found " .. #catalog_stars .. " catalog stars")

-- Create a StarChartWidget for plotting
chart = create_star_chart("Star Chart - Catalog Query Results")

-- Set the star data
chart:setStarData(catalog_stars)

-- Configure the chart appearance
-- Plot modes: 0=MagnitudeVsSpectralType, 1=RAvsDecPosition, 2=MagnitudeDistribution, 3=SpectralTypeDistribution
chart:setPlotMode(0)  -- Start with Magnitude vs Spectral Type

-- Color schemes: 0=SpectralTypeColors, 1=MagnitudeColors, 2=MonochromeColors
chart:setColorScheme(0)  -- Use spectral type colors

-- Enable legend and grid
chart:setShowLegend(true)
chart:setShowGrid(true)

-- Set window title and show the chart
chart:show()

-- Optional: Connect callback for star clicks
chart:connectStarClicked(function(star)
    print("Clicked star: " .. star:toString())
end)

-- Optional: Connect callback for star hovers
chart:connectStarHovered(function(star)
    print("Hovered star: " .. star.id .. " (Mag: " .. star.magnitude .. ")")
end)

print("Star chart created and displayed!")
print("You can:")
print("- Left click on stars for information")
print("- Mouse wheel to zoom in/out") 
print("- Right click to reset zoom")
print("- Drag to pan the view")

-- Optional: Create additional charts with different plot modes
print("\nCreating additional chart views...")

-- RA vs Dec position chart
position_chart = StarChartWidget.new(nil)
position_chart:setStarData(catalog_stars)
position_chart:setPlotMode(1)  -- RA vs Dec position
position_chart:setColorScheme(0)  -- Spectral type colors
--position_chart:setColorScheme(1)  -- Magnitude colors
position_chart:setShowLegend(true)
position_chart:setShowGrid(true)
position_chart:setWindowTitle("Star Positions (RA vs Dec)")
position_chart:show()

-- Magnitude distribution histogram
mag_dist_chart = StarChartWidget.new(nil)
mag_dist_chart:setStarData(catalog_stars)
mag_dist_chart:setPlotMode(2)  -- Magnitude distribution
mag_dist_chart:setColorScheme(0)  -- Spectral type colors
mag_dist_chart:setShowLegend(true)
mag_dist_chart:setShowGrid(true)
mag_dist_chart:setWindowTitle("Magnitude Distribution")
mag_dist_chart:show()

-- Spectral type distribution
spectral_dist_chart = StarChartWidget.new(nil)
spectral_dist_chart:setStarData(catalog_stars)
spectral_dist_chart:setPlotMode(3)  -- Spectral type distribution
spectral_dist_chart:setColorScheme(0)  -- Spectral type colors
spectral_dist_chart:setShowLegend(true)
spectral_dist_chart:setShowGrid(true)
spectral_dist_chart:setWindowTitle("Spectral Type Distribution")
spectral_dist_chart:show()

print("All chart views created!")
print("\nChart controls:")
print("- setPlotMode(0-3): Change plot type")
print("- setColorScheme(0-2): Change color scheme") 
print("- setShowLegend(true/false): Toggle legend")
print("- setShowGrid(true/false): Toggle grid")
print("- resetZoom(): Reset view to default")

-- Example of changing chart properties dynamically
print("\nYou can also modify charts after creation:")
print("Example: chart:setPlotMode(1)  -- Switch to RA/Dec view")
print("Example: chart:setColorScheme(2)  -- Switch to monochrome")