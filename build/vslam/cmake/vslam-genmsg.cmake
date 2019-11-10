# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "vslam: 1 messages, 0 services")

set(MSG_I_FLAGS "-Ivslam:/home/lq/Documents/visual-odometry/src/vslam/msg;-Istd_msgs:/opt/ros/kinetic/share/std_msgs/cmake/../msg;-Igeometry_msgs:/opt/ros/kinetic/share/geometry_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(gennodejs REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(vslam_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg" NAME_WE)
add_custom_target(_vslam_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "vslam" "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg" "geometry_msgs/Point"
)

#
#  langs = gencpp;geneus;genlisp;gennodejs;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(vslam
  "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/geometry_msgs/cmake/../msg/Point.msg"
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/vslam
)

### Generating Services

### Generating Module File
_generate_module_cpp(vslam
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/vslam
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(vslam_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(vslam_generate_messages vslam_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg" NAME_WE)
add_dependencies(vslam_generate_messages_cpp _vslam_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(vslam_gencpp)
add_dependencies(vslam_gencpp vslam_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS vslam_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(vslam
  "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/geometry_msgs/cmake/../msg/Point.msg"
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/vslam
)

### Generating Services

### Generating Module File
_generate_module_eus(vslam
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/vslam
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(vslam_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(vslam_generate_messages vslam_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg" NAME_WE)
add_dependencies(vslam_generate_messages_eus _vslam_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(vslam_geneus)
add_dependencies(vslam_geneus vslam_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS vslam_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(vslam
  "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/geometry_msgs/cmake/../msg/Point.msg"
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/vslam
)

### Generating Services

### Generating Module File
_generate_module_lisp(vslam
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/vslam
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(vslam_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(vslam_generate_messages vslam_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg" NAME_WE)
add_dependencies(vslam_generate_messages_lisp _vslam_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(vslam_genlisp)
add_dependencies(vslam_genlisp vslam_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS vslam_generate_messages_lisp)

### Section generating for lang: gennodejs
### Generating Messages
_generate_msg_nodejs(vslam
  "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/geometry_msgs/cmake/../msg/Point.msg"
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/vslam
)

### Generating Services

### Generating Module File
_generate_module_nodejs(vslam
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/vslam
  "${ALL_GEN_OUTPUT_FILES_nodejs}"
)

add_custom_target(vslam_generate_messages_nodejs
  DEPENDS ${ALL_GEN_OUTPUT_FILES_nodejs}
)
add_dependencies(vslam_generate_messages vslam_generate_messages_nodejs)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg" NAME_WE)
add_dependencies(vslam_generate_messages_nodejs _vslam_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(vslam_gennodejs)
add_dependencies(vslam_gennodejs vslam_generate_messages_nodejs)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS vslam_generate_messages_nodejs)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(vslam
  "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/geometry_msgs/cmake/../msg/Point.msg"
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/vslam
)

### Generating Services

### Generating Module File
_generate_module_py(vslam
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/vslam
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(vslam_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(vslam_generate_messages vslam_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/lq/Documents/visual-odometry/src/vslam/msg/position.msg" NAME_WE)
add_dependencies(vslam_generate_messages_py _vslam_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(vslam_genpy)
add_dependencies(vslam_genpy vslam_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS vslam_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/vslam)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/vslam
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_cpp)
  add_dependencies(vslam_generate_messages_cpp std_msgs_generate_messages_cpp)
endif()
if(TARGET geometry_msgs_generate_messages_cpp)
  add_dependencies(vslam_generate_messages_cpp geometry_msgs_generate_messages_cpp)
endif()

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/vslam)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/vslam
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_eus)
  add_dependencies(vslam_generate_messages_eus std_msgs_generate_messages_eus)
endif()
if(TARGET geometry_msgs_generate_messages_eus)
  add_dependencies(vslam_generate_messages_eus geometry_msgs_generate_messages_eus)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/vslam)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/vslam
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_lisp)
  add_dependencies(vslam_generate_messages_lisp std_msgs_generate_messages_lisp)
endif()
if(TARGET geometry_msgs_generate_messages_lisp)
  add_dependencies(vslam_generate_messages_lisp geometry_msgs_generate_messages_lisp)
endif()

if(gennodejs_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/vslam)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/vslam
    DESTINATION ${gennodejs_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_nodejs)
  add_dependencies(vslam_generate_messages_nodejs std_msgs_generate_messages_nodejs)
endif()
if(TARGET geometry_msgs_generate_messages_nodejs)
  add_dependencies(vslam_generate_messages_nodejs geometry_msgs_generate_messages_nodejs)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/vslam)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/vslam\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/vslam
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_py)
  add_dependencies(vslam_generate_messages_py std_msgs_generate_messages_py)
endif()
if(TARGET geometry_msgs_generate_messages_py)
  add_dependencies(vslam_generate_messages_py geometry_msgs_generate_messages_py)
endif()
