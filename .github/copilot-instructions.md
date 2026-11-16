# Copilot Instructions For openclsolarsystem

## Project Scope

- Primary app: `src/OpenCLSolarSystem` (C++/OpenCL, wxWidgets, GLEW)
- Supporting toolchain: `src/OrbToSlf` (.NET 8 projects and tests)
- Build system: CMake with Ninja generators

## Development Expectations

- Keep changes minimal and focused on the requested task.
- Preserve existing public behavior unless the task explicitly requests a behavior change.
- Avoid broad refactors and formatting-only edits in unrelated files.
- Match existing style in the touched file (naming, spacing, brace style, includes order).

## C++ And OpenCL Guidelines

- Prefer small, localized edits in these files:
  - `application.*`, `frame.*`, `glcanvas.*`, `clmodel.*`, `kernels.*`, `global.*`
- If changing host-side kernel setup, ensure corresponding kernel signatures and argument ordering remain consistent with:
  - `src/OpenCLSolarSystem/adamsfma.cl`
- `src/OpenCLSolarSystem/adamsfma.cl` is the single source of the OpenCL kernels. CMake embeds it into a generated `kernels.cpp` in the build directory (from `kernels.cpp.in`); never add a hand-maintained copy of the kernel source.
- Keep OpenCL kernel changes portable to OpenCL 1.2 where possible.
- Avoid introducing heavy dependencies unless clearly required.

## Build And Validation

- Preferred VS Code tasks:
  - `CMake Debug`
  - `CMake Release`
- Equivalent command pattern:

```bash
cmake -G Ninja -B build -Wno-dev -S src/OpenCLSolarSystem -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug -j$(nproc)
```

- For `OrbToSlf` changes, validate with:

```bash
dotnet test src/OrbToSlf/OrbToSlf.Tests/OrbToSlf.Tests.csproj
dotnet test src/OrbToSlf/OrbToSlfConsole.Tests/OrbToSlfConsole.Tests.csproj
```

## Safety Checks Before Finishing

- Confirm modified targets compile.
- If kernels are modified, verify host-side launch/argument wiring still matches.
- Summarize what changed and list any validation that could not be run.

## Non-Goals

- Do not change licensing files unless explicitly requested.
- Do not rename major modules or move project structure unless explicitly requested.
