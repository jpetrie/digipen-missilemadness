cmake_minimum_required(VERSION 3.19)
project("MissileMadness")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER CMake)

set(GameTarget "MissileMadness")
set(DataTarget "MissileMadness.Data")

set(CodeFiles
  Code/debugging.cpp
  Code/debugging.h
  Code/error_handling.cpp
  Code/error_handling.h
  Code/handler_network.cpp
  Code/handler_network.h
  Code/handler_redraw.cpp
  Code/handler_redraw.h
  Code/handler_ui.cpp
  Code/handler_ui.h
  Code/mod_ai.cpp
  Code/mod_ai.h
  Code/mod_display.cpp
  Code/mod_display.h
  Code/mod_econ.cpp
  Code/mod_econ.h
  Code/mod_game.cpp
  Code/mod_game.h
  Code/mod_items.cpp
  Code/mod_items.h
  Code/mod_missiles.cpp
  Code/mod_missiles.h
  Code/mod_network.cpp
  Code/mod_network.h
  Code/mod_sound.cpp
  Code/mod_sound.h
  Code/mod_tank.cpp
  Code/mod_tank.h
  Code/mod_terrain.cpp
  Code/mod_terrain.h
  Code/mod_ui.cpp
  Code/mod_ui.h
  Code/mod_weapons.cpp
  Code/mod_weapons.h
  Code/mod_weapon_attributes.cpp
  Code/mod_weapon_attributes.h
  Code/tank.cpp
  Code/tank.h
  Code/tank.rc
  Code/utilities.cpp
  Code/utilities.h
)

set(DataFiles
  Data/Art/missilemadness.bmp
  Data/config/bgm.cfg
  Data/config/defaults.cfg
  Data/config/metaserver.cfg
  Data/items/armor_plating.txt
  Data/items/ninja_dash.txt
  Data/items/shield_lvl1.txt
  Data/items/shield_lvl2.txt
  Data/items/shield_lvl2.txt
  Data/weapons/WeaponFile.txt
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CodeFiles} ${DataFiles})

add_custom_target(${DataTarget} ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Data $<TARGET_FILE_DIR:${GameTarget}>/Data
)

add_executable(${GameTarget} WIN32 ${CodeFiles} ${DataFiles})
add_dependencies(${GameTarget} ${DataTarget})
set_target_properties(${GameTarget} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${GameTarget}>")
target_link_libraries(${GameTarget} winmm wsock32)
