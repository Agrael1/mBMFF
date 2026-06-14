from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout

class MBMFFConan(ConanFile):
    name = "mbmff"
    version = "1.0.0"
    license = "MIT"
    author = "Agrael1"
    url = "https://github.com/Agrael1/mBMFF"
    description = "A header-only C++ library for Base Media File Format."
    topics = ("bmff", "iso-bmff", "media", "header-only")
    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "include/*", "tools/*"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["MBMFF_BUILD_TEST"] = False
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build(target="amalgamate")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

    def package_id(self):
        self.info.clear()