target_sources_ifdef(CONFIG_WIDGET_BATTERY_STATUS app PRIVATE widgets/battery_status.c)
target_sources_ifdef(CONFIG_WIDGET_OUTPUT_STATUS app PRIVATE widgets/output_status.c)
target_sources_ifdef(CONFIG_WIDGET_LAYER_STATUS app PRIVATE widgets/layer_status.c)
target_sources_ifdef(CONFIG_WIDGET_BONGO_CAT app PRIVATE widgets/bongo_cat.c)
# target_sources_ifdef(CONFIG_WIDGET_WPM_STATUS app PRIVATE widget/wpm_status.c)

if(CONFIG_ZMK_SPLIT AND (NOT CONFIG_ZMK_SPLIT_ROLE_CENTRAL))
	target_sources_ifdef(CONFIG_WIDGET_PERIPHERAL_STATUS app PRIVATE widgets/peripheral_status.c)
endif()

add_subdirectory_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM icons)

# TODO: separate split halves in here, use two different status_screens
target_sources(app PRIVATE widgets/menu.c)
target_sources(app PRIVATE custom_status_screen.c)
