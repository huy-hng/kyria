# target_include_directories(app PRIVATE include)

add_subdirectory_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM src/display/)
target_sources(app PRIVATE src/behaviors/behavior_display_navigation.c)
target_sources(app PRIVATE src/utils.c)

target_sources_ifdef(CONFIG_RGB_BACKLIGHT app PRIVATE src/behaviors/behavior_rgb_backlight.c)
add_subdirectory_ifdef(CONFIG_RGB_BACKLIGHT src/rgb/)
