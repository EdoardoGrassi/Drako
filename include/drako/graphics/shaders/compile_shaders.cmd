REM Compiles all shaders in current folder
FOR %f in (*.vert) DO C:/VulkanSDK/1.1.130.0/Bin/glslangValidator.exe -V %f
FOR %f in (*.frag) DO C:/VulkanSDK/1.1.130.0/Bin/glslangValidator.exe -V %f
pause