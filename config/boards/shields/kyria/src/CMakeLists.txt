add_subdirectory_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM display/)

if(CONFIG_ZMK_RGB_UNDERGLOW)
	target_sources(app PRIVATE behavior_rgb_underglow.c)
	target_sources(app PRIVATE rgb_underglow.c)

	if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
		target_sources(app PRIVATE layer_color.c)
	endif()
endif()

