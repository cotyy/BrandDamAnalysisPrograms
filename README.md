# Brand Dam Analysis Programs

This repository contains a collection of C++ programs developed by Bruce Brand (HetroDyne, brucebrand.hetrodyne@msn.com) for the analysis and evaluation of dams and related structures. These tools were originally written for use in engineering applications involving arch dams, stability analysis, uplift pressure modeling, and other structural mechanics concepts.

The source code has been open-sourced and organized to support modern development workflows. Minimal modifications have been made initially to preserve the original logic and structure. The programs are built using Visual Studio 2022 or later.

This repository is maintained by Coty Young as a release of Bruce Brand’s original engineering software.

---

## 📁 Project Structure

- `common/` — Shared `.cpp` and `.h` files used by all tools
- `Docs/` — Documentation and legacy reference material (e.g., ARVAR manual)
- `old versions/` — Archived original, unmodified source code (e.g., `arvar5_old.cpp`)
- `tools/` — One folder per tool, each containing:
  - Source file(s), `.sln`, `.vcxproj`, and Visual Studio metadata
  - A `build/` subfolder with:
    - `x64/Debug/` — Final `.exe` and `.pdb`
    - `intermediate/x64/Debug/` — Compiler logs and object files
- `build.props` — Shared Visual Studio output configuration
- `.gitignore` — Excludes executables and build artifacts
- `README.md` — Project overview, build instructions, license

---

## 🛠️ Building the Tools

### Requirements
- Windows
- Visual Studio with the **Desktop development with C++** workload installed
- CMake is *not* required — the projects use native `.sln` and `.vcxproj` files

### Build Instructions
1. Clone the repository
2. Open the `.sln` file in any `tools/<ToolName>/` folder
3. Build using the `x64 | Debug` configuration (or as preferred)
4. The compiled `.exe` will be located in `build/<ToolName>/x64/Debug/`

> ✅ Each project uses shared source files from `common/`. Ensure the `common/` folder is referenced in your include paths (already set in project configs).

---

## 📜 License

This project is licensed under the [MIT License](LICENSE).

---

## 🙏 Acknowledgments

Original code authored by **Bruce Brand**  
Preserved and maintained with appreciation for its continued utility in dam safety.  
This repository supports ongoing efforts to modernize and sustain critical tools used in risk assessments and analysis.
