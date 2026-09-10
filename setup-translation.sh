#!/bin/bash
# Script to download and build bergamot-translator for Elements Extract
# This script should be run once to set up the translation engine

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BIN_DIR="${SCRIPT_DIR}/bin"
MODELS_DIR="${SCRIPT_DIR}/models"

echo "=== Setting up bergamot-translator ==="

# Create directories
mkdir -p "${BIN_DIR}"
mkdir -p "${MODELS_DIR}"

# Check if bergamot is already installed
if command -v bergamot &> /dev/null; then
    echo "bergamot already found in PATH"
    bergamot --version 2>/dev/null || true
    exit 0
fi

# Build bergamot-translator from source
echo "Building bergamot-translator..."
cd /tmp

# Clone bergamot-translator
if [ ! -d "bergamot-translator" ]; then
    git clone --depth 1 https://github.com/browsermt/bergamot-translator.git
fi

cd bergamot-translator
mkdir -p build && cd build

# Build with cmake
cmake .. -DCMAKE_BUILD_TYPE=Release -DCOMPILE_WASM=off
make -j$(nproc)

# Copy binary
cp app/bergamot "${BIN_DIR}/"
echo "bergamot-translator built successfully!"
echo "Binary installed to: ${BIN_DIR}/bergamot"

# Download models
echo ""
echo "=== Downloading translation models ==="

# Model base URL
MODEL_BASE="https://data.statmt.org/bergamot/models"

# Languages to download
declare -A LANG_PAIRS=(
    ["en"]="es fr pt"
    ["es"]="en"
    ["fr"]="en"
    ["pt"]="en"
)

for SRC in "${!LANG_PAIRS[@]}"; do
    for TRG in ${LANG_PAIRS[$SRC]}; do
        MODEL_NAME="${SRC}${TRG}"
        MODEL_DIR="${MODELS_DIR}/${SRC}-${TRG}-tiny"
        
        if [ -d "${MODEL_DIR}" ]; then
            echo "Model ${SRC}-${TRG} already exists, skipping..."
            continue
        fi
        
        echo "Downloading model: ${SRC} → ${TRG}..."
        mkdir -p "${MODEL_DIR}"
        
        # Download and extract model
        cd "${MODEL_DIR}"
        wget -q "${MODEL_BASE}/${MODEL_NAME}/${MODEL_NAME}.student.tiny11.tar.gz" -O model.tar.gz
        tar -xzf model.tar.gz --strip-components=1
        rm model.tar.gz
        
        echo "  Model ${SRC}-${TRG} installed to ${MODEL_DIR}"
    done
done

echo ""
echo "=== Setup complete! ==="
echo "Binary: ${BIN_DIR}/bergamot"
echo "Models: ${MODELS_DIR}/"
echo ""
echo "You can now use Elements Extract with translation support."
