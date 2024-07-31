#!/bin/bash

# Function to validate version format (X.Y.Z)
function validate_version() {
  local re='^[0-9]+\.[0-9]+\.[0-9]+$'
  if [[ ! $1 =~ $re ]]; then
    echo "Error: Invalid version format. Please use X.Y.Z (e.g., 1.2.3)"
    exit 1
  fi
}

# Function to validate build number (positive integer)
function validate_build() {
  local re='^[0-9]+$'
  if [[ ! $1 =~ $re ]]; then
    echo "Error: Invalid build number. Please enter a positive integer."
    exit 1
  fi
}

# Get version and build number from arguments
while getopts "v:b:" opt; do
  case $opt in
    v) version=$OPTARG ;;
    b) build=$OPTARG ;;
    *) echo "Usage: $0 -v version -b build" && exit 1 ;;
  esac
done

# Validate arguments
validate_version $version
validate_build $build

# Set variables based on arguments
V_MAJOR=${version%%.*}              # Extract major version (everything before first .)
V_MINOR=${version#*.}               # Extract minor and patch versions (everything after first .)
V_PATCH=${V_MINOR#*.}               # Extract patch version (everything after second .)
V_MINOR=${V_MINOR%%.*}              # Extract minor version (remove patch part)
V_BUILD=$build                      # Use user-provided build number

# Get developer email from git config (assuming it's set)
DEVELOPER=$(git config --local user.email)
if [[ -z "$DEVELOPER" ]]; then
  echo "Warning: Could not get developer email from git config."
fi

# Get current date and time
CURRENT_TIME=$(date +%s)

# Get current branch name
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)

# Get latest commit hash
COMMIT_HASH=$(git rev-parse HEAD)

# Construct header file content
header_content="#ifndef __VERSION_H__
#define __VERSION_H__


#ifdef __cplusplus
extern \"C\" {
#endif


#define BUILD_DATE $CURRENT_TIME
#define MAJOR $V_MAJOR
#define MINOR $V_MINOR
#define BATCH $V_PATCH
#define BUILD $V_BUILD
#define VERSION_STR \"$version\"
#define DEVELOPER \"$DEVELOPER\"
#define COMPILE_TIME \"$(date)\"
#define CURRENT_BRANCH \"$CURRENT_BRANCH\"
#define COMMIT_HASH \"$COMMIT_HASH\"


#ifdef __cplusplus
}
#endif


#endif // __VERSION_H__"

# Constant file path
header_file="../ezlopi-util/ezlopi-util-version/ezlopi_util_version.h"

# Write header file content to the specified file
echo "$header_content" > "$header_file"

echo "Version information written to $header_file"

# Example: Further processing based on version components (optional)
if [[ $V_MAJOR -gt 3 ]]; then
  echo "Major version change detected. Update documentation!"
fi
