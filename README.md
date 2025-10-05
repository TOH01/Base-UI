# Base-UI

[![Build Status](https://github.com/TOH01/Base-UI/actions/workflows/ci.yml/badge.svg)](https://github.com/TOH01/Base-UI/actions/workflows/ci.yml)
[![Test Results](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/TOH01/Base-UI/main/badges/test-results.json)](https://github.com/TOH01/Base-UI/actions)
[![Latest Release](https://img.shields.io/github/v/release/TOH01/Base-UI?include_prereleases)](https://github.com/TOH01/Base-UI/releases)

## 📦 Downloads

Download the latest release from the [Releases page](https://github.com/TOH01/Base-UI/releases/latest):

- **Narcia Demo** - `narcia-release.zip`
- **Calendar Demo** - `calendar-release.zip`
- **All Demos** - `all-releases.zip`

## 🛠️ Building from Source

### Prerequisites

- MSYS2 with MinGW64
- GCC compiler
- Make

### Build Instructions

```bash
# Build Narcia demo
mingw32-make DEMO=narcia BUILD_TYPE=release

# Build Calendar demo
mingw32-make DEMO=calendar BUILD_TYPE=release

# Run tests
mingw32-make test
```