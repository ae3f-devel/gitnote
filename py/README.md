# Notion Python Utilities

## Overview
Python executables for syncing git repositories to Notion pages.

## Build
```bash
./build.sh
```

Build with stripped binaries (smaller size):
```bash
./build.sh --strip
```

The build script automatically creates and uses a virtual environment at `../.venv/`.

## Manual PyInstaller Commands

Standard build:
```bash
pyinstaller --onefile --console --name=get_snap get_snap.py
pyinstaller --onefile --console --name=set_snap set_snap.py
pyinstaller --onefile --console --name=mkdir mkdir.py
pyinstaller --onefile --console --name=rmdir rmdir.py
pyinstaller --onefile --console --name=modify_file modify_file.py
pyinstaller --onefile --console --name=touch touch.py
pyinstaller --onefile --console --name=rm rm.py
```

Stripped build:
```bash
pyinstaller --onefile --console --strip --name=get_snap get_snap.py
pyinstaller --onefile --console --strip --name=set_snap set_snap.py
pyinstaller --onefile --console --strip --name=mkdir mkdir.py
pyinstaller --onefile --console --strip --name=rmdir rmdir.py
pyinstaller --onefile --console --strip --name=modify_file modify_file.py
pyinstaller --onefile --console --strip --name=touch touch.py
pyinstaller --onefile --console --strip --name=rm rm.py
```

## File Structure
- [utils.py](./utils.py)
- [get_snap.py](./get_snap.py)
- [set_snap.py](./set_snap.py)
- [mkdir.py](./mkdir.py)
- [rmdir.py](./rmdir.py)
- [modify_file.py](./modify_file.py)
- [touch.py](./touch.py)
- [rm.py](./rm.py)
- [build.sh](./build.sh)
- [requirements.txt](./requirements.txt)

## Usage Examples

### Fetch Remote Commit Hash
```bash
dist/get_snap secret_xxx 1234567890abcdef
```

### Set Remote Commit Hash
```bash
dist/set_snap secret_xxx 1234567890abcdef abc123def456
```

### Create Directory
```bash
dist/mkdir secret_xxx 1234567890abcdef src/app
```

### Remove Directory
```bash
dist/rmdir secret_xxx 1234567890abcdef src/app
```

### Modify File
```bash
dist/modify_file secret_xxx 1234567890abcdef src/main.c
```

### Create New File
```bash
dist/touch secret_xxx 1234567890abcdef README.md
```

### Remove File
```bash
dist/rm secret_xxx 1234567890abcdef README.md
```

## Verbose Mode
Add `-v` or `--verbose` flag for detailed logging:
```bash
dist/get_snap secret_xxx 1234567890abcdef -v
```
