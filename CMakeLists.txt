cmake_minimum_required(VERSION 3.10)

project(BlitRenderer VERSION 0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(BlitRenderer
                src/Engine/Main.cpp

                src/Engine/Inputs/glfwInputs/glfwInputs.h
                src/Engine/Inputs/glfwInputs/glfwInputs.cpp

                src/Engine/GameObjects/mesh.h
                
                src/Rendering/Vulkan/Bootstrap/VkBootstrap.cpp
                
                src/Rendering/Vulkan/SDKobjects/VulkanSDKobjects.cpp
                src/Rendering/Vulkan/SDKobjects/VulkanSDKobjects.h
                
                src/Rendering/Vulkan/VulkanRenderer/VulkanPipeline.cpp
                src/Rendering/Vulkan/VulkanRenderer/VulkanPipeline.h
                src/Rendering/Vulkan/VulkanRenderer/VulkanRenderer.h
                src/Rendering/Vulkan/VulkanRenderer/VulkanRendererInterface.cpp
                src/Rendering/Vulkan/VulkanRenderer/VulkanRenderLoop.cpp
                src/Rendering/Vulkan/VulkanRenderer/VulkanSetup.cpp 
                src/Rendering/Vulkan/VulkanRenderer/VulkanShaderData.h)

target_include_directories(BlitRenderer PUBLIC 
                            "${PROJECT_SOURCE_DIR}/ExternalVendors/GLFW/include"
                            "${PROJECT_SOURCE_DIR}/src"
                            "${PROJECT_SOURCE_DIR}/ExternalVendors/VkBootstrap"
                            "${PROJECT_SOURCE_DIR}/ExternalVendors/VmaAllocator"
                            "${PROJECT_SOURCE_DIR}/ExternalVendors/Vulkan/Include")

target_link_directories(BlitRenderer PUBLIC 
                        "${PROJECT_SOURCE_DIR}/ExternalVendors/GLFW/lib-vc2022"
                        "${PROJECT_SOURCE_DIR}/ExternalVendors/Vulkan/Lib")

target_link_libraries(BlitRenderer PUBLIC 
                    glfw3.lib
                    vulkan-1.lib)

find_program(GLSL_VALIDATOR glslangValidator HINTS $"{PROJECT_SOURCE_DIR}/ExternalVendors/Vulkan/Bin")
set(GLSL_VALIDATOR "${PROJECT_SOURCE_DIR}/ExternalVendors/Vulkan/Bin/glslangValidator.exe")

file(GLOB_RECURSE GLSL_SOURCE_FILES
      "VulkanShaders/*.glsl"
      )
  
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/VulkanShaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/VulkanShaders/"
    COMMAND ${GLSL_VALIDATOR} -V --target-env vulkan1.3 ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)
  
add_custom_target(
    VulkanShaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(BlitRenderer VulkanShaders)