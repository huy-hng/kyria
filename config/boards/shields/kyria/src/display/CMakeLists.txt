target_sources(app PRIVATE widgets/debug_output.c)
if (CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	target_sources_ifdef(CONFIG_WIDGET_BATTERY_STATUS app PRIVATE widgets/battery_status.c)
	target_sources_ifdef(CONFIG_WIDGET_BONGO_CAT app PRIVATE widgets/bongo_cat.c)

	target_sources_ifdef(CONFIG_WIDGET_LAYER_STATUS app PRIVATE widgets/layer_status.c)
	target_sources_ifdef(CONFIG_WIDGET_OUTPUT_STATUS app PRIVATE widgets/output_status.c)

	# target_sources(app PRIVATE status_screen_central.c)
	target_sources(app PRIVATE status_screen_test.c)
else()
	target_sources_ifdef(CONFIG_WIDGET_PERIPHERAL_STATUS app PRIVATE widgets/peripheral_status.c)
	target_sources(app PRIVATE status_screen_peripheral.c)
endif()

add_subdirectory(icons)
