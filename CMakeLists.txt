cmake_minimum_required(VERSION 2.8)

project(Assignment2)

add_executable(RayTracer.out src/RayTracer.cpp src/Ray.cpp src/SceneObject.cpp src/Sphere.cpp src/Plane.cpp src/TextureBMP.cpp src/Cylinder.cpp src/Cone.cpp src/Light.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} )

target_link_libraries( RayTracer.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLM_LIBRARY} )

