#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 1 ]; then
  echo "Usage: $0 <PluginName>"
  echo "Example: $0 SimpleReverb"
  exit 1
fi

NEW_NAME="$1"

# Validate: must start with a letter, only alphanumeric
if ! [[ "$NEW_NAME" =~ ^[A-Za-z][A-Za-z0-9]*$ ]]; then
  echo "Error: name must be alphanumeric and start with a letter (e.g. SimpleReverb)"
  exit 1
fi

# Derive spaced version:  SimpleReverb → Simple Reverb
NEW_NAME_SPACED=$(echo "$NEW_NAME" | sed 's/\([A-Z]\)/ \1/g' | sed 's/^ //')

# Derive kebab-case:      SimpleReverb → simple-reverb
NEW_NAME_KEBAB=$(echo "$NEW_NAME" | sed 's/\([A-Z]\)/-\1/g' | sed 's/^-//' | tr '[:upper:]' '[:lower:]')

FILES=(
  "CMakeLists.txt"
  "Source/PluginProcessor.h"
  "Source/PluginProcessor.cpp"
  "Source/PluginEditor.h"
  "Source/PluginEditor.cpp"
  "ui/package.json"
  "ui/index.html"
  "ui/src/App.tsx"
)

for file in "${FILES[@]}"; do
  sed -i '' \
    -e "s/MyPlugin/$NEW_NAME/g" \
    -e "s/My Plugin/$NEW_NAME_SPACED/g" \
    -e "s/my-plugin-ui/${NEW_NAME_KEBAB}-ui/g" \
    "$file"
done

echo "Done: MyPlugin → $NEW_NAME"
echo "      My Plugin → $NEW_NAME_SPACED"
echo "      my-plugin-ui → ${NEW_NAME_KEBAB}-ui"
