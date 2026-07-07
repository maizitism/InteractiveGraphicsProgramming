add_executable(Assignment02 main.cpp)
target_link_libraries(Assignment02 PRIVATE OpenGL::GL GLUT::GLUT GLEW::GLEW cy_headers)

# Runtime args/working dir (model as argv[1], cwd = this folder where teapot.obj
# lives) are configured in .vscode/launch.json -> "Debug Assignment02 (teapot)".