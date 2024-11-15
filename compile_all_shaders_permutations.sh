cd `dirname $0`
rm -rf compiled_shaders
mkdir -p compiled_shaders
find sdk/src/backends/vk/shaders/fsr3upscaler -type f -exec ./compile_shader_permutations.sh '{}' \;
