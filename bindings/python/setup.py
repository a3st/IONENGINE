import os
import subprocess
import json
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])


class CMakeBuildExt(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        self.build_temp = os.path.join(os.path.dirname(__file__), "build")
        os.makedirs(self.build_temp, exist_ok=True)

        project_root = os.path.join(os.path.dirname(__file__), "..", "..")

        config = "Debug" if self.debug else "Release"

        cmake_args = [
            "-DCMAKE_BUILD_TYPE=" + config,
            "-DCMAKE_TOOLCHAIN_FILE:STRING=C:/Users/dima7/.vcpkg/scripts/buildsystems/vcpkg.cmake",
            "-DVCPKG_TARGET_TRIPLET:STRING=x64-windows-static-md",
            "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY="
            + os.path.abspath(
                os.path.join(self.build_lib, self.distribution.packages[0])
            ),
            "-DCMAKE_SHARED_LIBRARY_SUFFIX_CXX="
            + self.get_ext_filename(ext.name).lstrip(ext.name),
            "-B" + os.path.abspath(self.build_temp),
            "-S" + os.path.abspath(project_root),
            "-G Ninja",
        ]

        build_args = [
            "--build",
            os.path.abspath(self.build_temp),
            "--config",
            config,
            "--target",
            ext.name,
        ]

        try:
            output = subprocess.check_output(
                [
                    os.path.join(
                        "C:/",
                        "Program Files (x86)",
                        "Microsoft Visual Studio",
                        "Installer",
                        "vswhere.exe",
                    ),
                    "-products",
                    "*",
                    "-format",
                    "json",
                    "-utf8",
                    "-prerelease",
                    "-requires",
                    "Microsoft.Component.MSBuild",
                ]
            )

            data = json.loads(output)
            cmd_path = os.path.join(
                data[0]["installationPath"], "VC", "Auxiliary", "Build", "vcvars64.bat"
            )

        except FileNotFoundError:
            raise RuntimeError("Error: vswhere.exe not found")

        subprocess.run([cmd_path, "&", "cmake"] + cmake_args, cwd=self.build_temp)
        subprocess.run([cmd_path, "&", "cmake"] + build_args, cwd=self.build_temp)


setup(
    name="ionengine",
    packages=["ionengine"],
    version="1.0.0",
    author="Dmitriy Lukovenko",
    author_email="mludima23@gmail.com",
    description="IONENGINE 3d graphics engine",
    ext_modules=[CMakeExtension("engine"), CMakeExtension("platform")],
    cmdclass={"build_ext": CMakeBuildExt},
    include_package_data=True,
)
