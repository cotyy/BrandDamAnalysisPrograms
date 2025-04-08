# Brand Dam Analysis Programs

This repository contains a collection of C++ programs developed by Bruce Brand (HetroDyne, brucebrand.hetrodyne@msn.com) for the analysis and evaluation of dams and related structures. These tools were originally written for use in engineering applications involving arch dams, stability analysis, uplift pressure modeling, and other structural mechanics concepts.

The source code has been open-sourced and organized to support modern development workflows. Minimal modifications have been made initially to preserve the original logic and structure. The programs are built using Visual Studio 2022 or later.

---

## 📁 Project Structure

.
├── common/              # Shared headers and source files used across all tools
│   ├── brandgraph19.cpp/.h
│   ├── bstring19.cpp/.h
│   ├── float2string.cpp/.h
│   └── brandheadr.h

├── docs/                # Documentation and supporting materials
│   └── ARVAR/           # Original ARVAR user manual

├── tools/               # One subfolder per tool/program
│   ├── archmkr5/
│   ├── arvar5/
│   ├── 2dstab/
│   ├── damgen/
│   ├── ldframe/
│   ├── tgrib/
│   ├── vf2d/
│   ├── wedge/
│   ├── wedgload/
│   └── westergaard/

│       Each contains:
│       ├── .sln/.vcxproj/.cpp        # Visual Studio project + source
│       └── build/                    # Build outputs (exe, pdb, obj, logs)
│           ├── x64/Debug/            # Final executable and debug symbols
│           └── intermediate/x64/...  # Compiler intermediate files

├── build.props          # Shared build configuration for all tools
├── .gitignore           # Ignores build artifacts, executables, temp files
└── README.md            # Project overview, build instructions, and license

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
