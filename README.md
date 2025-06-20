# reward-wallet

# install cmake
⚙️ How to build using CMake in VSCode (Windows):
Install CMake & Compiler

Install CMake

Install MSVC via Visual Studio or install MinGW

Install extensions in VS Code

CMake Tools

C/C++ (by Microsoft)

Configure project
Open VS Code in your project folder and hit:

Ctrl + Shift + P → type CMake: Configure

Then Ctrl + Shift + P → CMake: Build

# make sure these configs below are included in setting.json
  "cmake.useCMakePresets": "always",
  "cmake.preferredGenerators": ["MinGW Makefiles"],
  "cmake.configureOnOpen": true,
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"

✅ 3. Building on Each Platform
▶️ On macOS
cmake --preset macos-clang
cmake --build --preset macos-clang
./build/macos-clang/output/Reward-Wallet
▶️ On Windows (with MinGW)
cmake --preset windows-mingw
cmake --build --preset windows-mingw
build\windows-mingw\output\Reward-Wallet.exe
✅ 4. Optional .gitignore
Ignore OS-specific and build outputs:

# macOS

.DS_Store
build/macos-clang/
src/main # (mac may create an executable named `main`)

# Windows

build/windows-mingw/

cmake --preset windows-mingw
cmake --build --preset windows-mingw


\*.exe
✅ 5. Bonus: Platform-specific code (if needed)
You can use platform checks:

#ifdef \_WIN32
std::cout << "Running on Windows" << std::endl;
#elif **APPLE**
std::cout << "Running on macOS" << std::endl;
#endif
✅ Summary
Task macOS Windows (MinGW)
Configure cmake --preset macos-clang cmake --preset windows-mingw
Build cmake --build --preset macos-clang cmake --build --preset windows-mingw
Run ./build/macos-clang/output/MyApp build/windows-mingw/output/MyApp.exe
