#!/bin/bash
# Instala Vocodex Pro sin tocar el Voxora antiguo.
set -e
cd "$(dirname "$0")"

VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"
mkdir -p "$VST3_DIR" "$AU_DIR"

# Nunca elimina Voxora.vst3 / Voxora.component.
rm -rf "$VST3_DIR/VocodexPro.vst3" "$AU_DIR/VocodexPro.component"
cp -R VocodexPro.vst3 "$VST3_DIR/"
cp -R VocodexPro.component "$AU_DIR/"

xattr -cr "$VST3_DIR/VocodexPro.vst3" "$AU_DIR/VocodexPro.component" 2>/dev/null || true
codesign --force --deep -s - "$VST3_DIR/VocodexPro.vst3" "$AU_DIR/VocodexPro.component" 2>/dev/null || true

echo ""
echo "Vocodex Pro instalado."
echo "El Voxora antiguo no ha sido modificado."
echo "Abre FL Studio > Options > Manage plugins > Find installed plugins."
echo ""
