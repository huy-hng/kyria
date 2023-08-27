# target_include_directories(app PRIVATE include)

add_subdirectory_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM src/display/)
target_sources(app PRIVATE src/behaviors/behavior_display_navigation.c)

if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	target_sources(app PRIVATE src/utils.c)
endif()

if(CONFIG_RGB_BACKLIGHT)
	target_sources(app PRIVATE src/rgb/main.c)
	target_sources(app PRIVATE src/rgb/animation.c)
	target_sources(app PRIVATE src/rgb/change.c)
	target_sources(app PRIVATE src/rgb/effects.c)
	target_sources(app PRIVATE src/behaviors/behavior_rgb_backlight.c)

	if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
		target_sources(app PRIVATE src/rgb/layer_color.c)
	endif()
endif()
