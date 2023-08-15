if(CONFIG_ZMK_DISPLAY)
	# target_sources_ifdef(CONFIG_ZMK_RGB_UNDERGLOW app PRIVATE src/widgets/output_status.c)
	# target_sources_ifdef(CONFIG_ZMK_RGB_UNDERGLOW app PRIVATE src/widgets/layer_status.c)
	# target_sources_ifdef(CONFIG_CUSTOM_WIDGET_BATTERY_STATUS app PRIVATE src/widgets/battery_status.c)
	# target_sources_ifdef(CONFIG_CUSTOM_WIDGET_PERIPHERAL_STATUS app PRIVATE src/widgets/peripheral_status.c)

	target_sources_ifdef(CONFIG_WIDGET_BONGO_CAT app PRIVATE src/widgets/bongo_cat_images.c)
	target_sources_ifdef(CONFIG_WIDGET_BONGO_CAT app PRIVATE src/widgets/bongo_cat.c)

	# add_subdirectory_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM src/widgets/icons)
endif()

zephyr_library()


zephyr_library_include_directories(${ZEPHYR_LVGL_MODULE_DIR})
zephyr_library_include_directories(${ZEPHYR_BASE}/lib/gui/lvgl/)
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE src/custom_status_screen.c)
# zephyr_library_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM src/custom_status_screen.c)
zephyr_library_sources(${ZEPHYR_BASE}/misc/empty_file.c)
zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers)

target_sources_ifdef(CONFIG_ZMK_RGB_UNDERGLOW app PRIVATE src/behavior_rgb_underglow.c)
target_sources_ifdef(CONFIG_ZMK_RGB_UNDERGLOW app PRIVATE src/rgb_underglow.c)
target_sources_ifdef(CONFIG_ZMK_RGB_UNDERGLOW app PRIVATE src/layer_color.c)

# target_sources_ifdef(CONFIG_CUSTOM_WIDGET_BONGO_CAT app PRIVATE src/widgets/bongo_cat.c)
