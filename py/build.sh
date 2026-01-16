#!/bin/bash

# Set up virtual environment
VENV_DIR="../.venv"

# Install dependencies if not already installed
echo "Checking dependencies..."
if [ ! -d "$VENV_DIR" ]; then
    echo "Creating virtual environment..."
    python -m venv "$VENV_DIR"
fi

echo "Installing/updating packages..."
"$VENV_DIR/bin/pip" install -q notion-client pyinstaller || exit 1

# Configuration
PYINSTALLER="$VENV_DIR/bin/pyinstaller"
PYINSTALLER_OPTS="--onefile --console"
OUTPUT_DIR="dist"

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --strip)
            PYINSTALLER_OPTS="$PYINSTALLER_OPTS --strip"
            shift
            ;;
        --help)
            echo "Usage: $0 [--strip] [--help]"
            echo "  --strip    Build stripped binaries (smaller size)"
            echo "  --help     Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Build each executable with manual pyinstaller commands
echo "Building executables..."

echo "Building get_snap..."
$PYINSTALLER $PYINSTALLER_OPTS --name=get_snap get_snap.py

echo "Building set_snap..."
$PYINSTALLER $PYINSTALLER_OPTS --name=set_snap set_snap.py

echo "Building mkdir..."
$PYINSTALLER $PYINSTALLER_OPTS --name=mkdir mkdir.py

echo "Building rmdir..."
$PYINSTALLER $PYINSTALLER_OPTS --name=rmdir rmdir.py

echo "Building modify_file..."
$PYINSTALLER $PYINSTALLER_OPTS --name=modify_file modify_file.py

echo "Building touch..."
$PYINSTALLER $PYINSTALLER_OPTS --name=touch touch.py

echo "Building rm..."
$PYINSTALLER $PYINSTALLER_OPTS --name=rm rm.py

echo "Build complete! Executables are in $OUTPUT_DIR/"
