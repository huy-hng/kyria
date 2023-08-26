# target_include_directories(app PRIVATE include)

add_subdirectory_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM src/display/)
target_sources(app PRIVATE src/behaviors/behavior_display_navigation.c)

if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	target_sources(app PRIVATE src/utils.c)
endif()

# if(CONFIG_ZMK_RGB_UNDERGLOW)
	target_sources(app PRIVATE src/behaviors/behavior_rgb_underglow.c)
	target_sources(app PRIVATE src/rgb/rgb_extra.c)

	if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
		target_sources(app PRIVATE src/rgb/layer_color.c)
	endif()
# endif()
