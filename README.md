# SOProyecto – File‑Processing Workflow (UCAB Operating Systems Project)

![Build status](https://img.shields.io/badge/GitHub%20Actions-Windows%20build-green)

> **Goal:** copy an input file *N* times, encrypt each copy with a custom Caesar‑plus‑numeric‑symmetry cipher, stream‑hash every ciphertext with SHA‑256, decrypt & byte‑verify against the original, and print precise timing metrics.  The project is graded on **total execution time (TT)** before and after your optimisations.

---

## 1. Project structure

```text
SOProyecto/
├── src/                 # C++98 source code  (flujo)
│   ├── main.cpp         # entry point
│   ├── cifrado.*        # cipher / decipher helpers
│   ├── hash.*           # streaming SHA‑256 helpers
│   └── verificar.*      # binary file‑compare
├── original.txt         # test file provided by the professor
├── CMakeLists.txt       # portable build script (MSVC, MinGW, Clang, GCC)
├── docs/Informe.md      # report & metrics (to be exported to .doc/.pdf)
└── .github/workflows/ci.yml  # Windows CI – builds & uploads flujo.exe
```

> **Note:** build artefacts (`build/`, `copias/`, `cifrados/`, `sha/`, `tmp.txt`) are git‑ignored.

---

## 2. Prerequisites

| Platform                          | Packages / tools                                                        |
| --------------------------------- | ----------------------------------------------------------------------- |
| **Ubuntu / Codespaces**           | `build-essential cmake ninja-build libssl-dev`                          |
| **Windows (MSVC)**                | Visual Studio 2022 (Build Tools incl. CMake & Ninja) + OpenSSL 3 64‑bit |
| **Windows (Dev‑C++ / MinGW‑w64)** | Dev‑C++ 6.3+ (C++98 mode is enough; no external libs required) |

---

## 3. Build & run

### 3.1 Linux / macOS / Codespace

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/flujo original.txt 10                # modo secuencial
./build/flujo opt original.txt 10            # modo optimizado
```

### 3.2 Windows – Visual Studio 2022

```ps1
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
build\Release\flujo.exe original.txt 10
build\Release\flujo.exe opt original.txt 10
```

### 3.3 Windows – Dev‑C++ quick guide

1. File ▸ New ▸ **Console Project (C++)**  → place inside repo root.
2. Add every `.cpp` & `.hpp` from `src/` to the project.
3. **Project Options ▸ Parameters ▸ C++ Compiler**  → keep the default (C++98).
4. No additional linker parameters needed.
5. Press **F11** (Compile) & run:

```ps1
flujo.exe original.txt 10
flujo.exe opt original.txt 10
```

---

## 4. Output & metrics

Running with `N = 10` prints:

```
Copiado   :   5 ms
Cif+Hash  : 265 ms
Verif     : 266 ms
TPPA      : 53.7 ms
TT        : 537 ms
✅ Verificación OK
```

Parallel run (`flujo opt`, N = 10):
```
TPPA      : 1.9 ms
TT        : 19 ms
✅ Verificación OK
```

Version tags:

* `v1-base-537ms` – initial baseline (unoptimised).
* Future tag `v2-opt‑<TT>` will contain multithread / mmap enhancements.

---

## 5. Continuous Integration

GitHub Actions workflow (`ci.yml`):

* Builds on **windows‑latest** using MSVC ▶ `flujo.exe`.
* Executes `flujo.exe original.txt 10` so logs always include fresh metrics.
* Uploads executable as a **workflow artifact** for offline testing.

---

## 6. Optimisation roadmap

* [ ] Buffer‑based streaming (64 KiB) – lower memory & hashing time.
* [ ] `std::thread` parallel loop – one copy per core.
* [ ] (optional) `mmap` on Linux / `MapViewOfFile` on Windows.
* [ ] Measure **DF** (TT\_base – TT\_opt) and **PM %** improvement.

---

## 7. Deliverables checklist

1. **ZIP**: source, CMakeLists.txt or `.dev`, `original.txt`, docs (no build artefacts).
2. **Executable**: `flujo.exe` (use `opt` argument for optimised mode).
3. **Informe.doc/.pdf**: methodology, tables, DF / PM, screenshots.
4. **README.md** (this file).

---

## 8. License

Public domain for academic purposes ‑ free to reuse in UCAB coursework.
