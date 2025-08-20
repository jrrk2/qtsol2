-- After implementing Qt Charts integration
chart = create_chart_window("Sine Wave Plotter")
chart:setFrequency(2.5)
chart:setAmplitude(1.5)
chart:plotFunction("sine", -10, 10, 1000)
chart:setTitle("Professional Sine Wave")
