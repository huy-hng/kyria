add_subdirectory_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM display/)

# if(CONFIG_ZMK_RGB_UNDERGLOW)
	target_sources(app PRIVATE rgb/behavior_rgb_underglow.c)
	target_sources(app PRIVATE rgb/rgb_extra.c)
	target_sources(app PRIVATE rgb/control.c)
	target_sources(app PRIVATE rgb/effects.c)

	if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
		target_sources(app PRIVATE rgb/layer_color.c)
	endif()
# endif()
