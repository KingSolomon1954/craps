import os
from conan import ConanFile
# from conan.tools.cmake import cmake_layout
from conan.tools.cmake import CMake

class AppRecipe(ConanFile):
    name = "RoyalCraps"  # Library name published to Conan Center
    license = "MIT Software License"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def layout(self):
        self.folders.root = "../.."
        if self.settings.build_type == "Debug":
            btype = "debug"
        if self.settings.build_type == "Release":
            btype = "prod"
        self.folders.build = os.path.join("_build", btype)
        self.folders.generators = os.path.join("_build", btype, "generators")

    def requirements(self):
        self.requires("nlohmann_json/[~3.12.0]")
        self.requires("boost/1.84.0")
        self.requires("cxxopts/3.2.0")
        self.requires("rang/3.2")
        self.requires("yaml-cpp/[~0.8.0]")
        self.requires("doctest/[~2.4.11]")
        
    def configure(self):
        self.options["boost"].header_only = True  # Optional: if you're using header-only

    def build_requirements(self):
        pass

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
