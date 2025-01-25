#!/bin/bash

# Configure in MCUXpresso in the workspace a build variable for the .hex output directory
# 'Window->Preferences->C/C++->Build->Build Variables->Add...'
# Variable name: hexDir
# Type:          String
# Value:         absolute path to .hex output directory

# Configure in MCUXpresso for ALL build configurations:

# 'Project->Properties->C/C++ Build->Build Variables->Add...'
# Variable name: sw_version
# Type:          String
# Value:         x.xx

# 'Project->Properties->C/C++ Build->Settings->Build Artifact->Artifact name:'
# ${ProjName}_${ConfigName}_v${sw_version}

# 'Project->Properties->C/C++ Build->Settings->Build steps->Post-build steps->Edit...'
# "${ProjDirPath}/../../../script/build-script/post-build-steps.sh" "${CWD}" "${TargetChip}" "${BuildArtifactFileName}" "${BuildArtifactFileBaseName}" "${ConfigName}" "${sw_version}" "${hexDir}"

# fail on error
set -e

# error handler
exit_on_error() {
    echo "Last command failed. Exiting..."
    exit 1
}

# 1. parameter directory to search the sblib version
# return the sblib version with a dot
get_sbLibVersionWithDot() {
  local sbLibVersionHeaderDir="$1"
  local sbLibVersion

  if [ -d "${sbLibVersionHeaderDir}" ]; then
    sbLibVersion="$(grep -rwh "${sbLibVersionHeaderDir}" -e "#define SBLIB_VERSION")"
    sbLibVersion=$(echo "$sbLibVersion" | awk '{print $NF}' | sed 's/^0x//')
    sbLibVersionWithoutLeadingZeros=$(echo "$sbLibVersion" | sed 's/^0*//')
    if [[ ${#sbLibVersionWithoutLeadingZeros} -gt 2 ]]; then
      result="${sbLibVersionWithoutLeadingZeros:0:-2}.${sbLibVersionWithoutLeadingZeros: -2}"
    else
      result=""
    fi
  else
    result=""
  fi
  echo "$result"
}

# enable error handling
trap exit_on_error ERR

# Selfbus library version prefix
sbLibprefix="libv"

# App version prefix
appVersionPrefix="v"

# search strings and subfolders for .hex output directory
debug="debug"
release="release"
flashstart="flashstart"

currentWorkingDirectory="${1}"
#targetChip="${2}"
buildArtifactFileName="${3}"
buildArtifactFileBaseName="${4}"
appConfigName="${5}"
appVersion="${6}"
hexDir="${7}"

echo ""
echo "Selfbus post build steps:"

# find sbLib version in repository
scriptDir=$(dirname "$0")
sbLibSubModuleRepoFolder="software-arm-lib"
scriptPathDepth="../.."
sbLibVersionSubDir="sblib/inc/sblib/"

# check if we are in the software-arm-incubation repo and search there
sbLibVersionHeaderDir="${scriptDir}/${scriptPathDepth}/${sbLibSubModuleRepoFolder}/${sbLibVersionSubDir}"
sbLibVersionWithDot=$(get_sbLibVersionWithDot "$sbLibVersionHeaderDir")

if [ -z "${sbLibVersionWithDot}" ]; then
  # check if we are in the sblib repo and search there
  sbLibVersionHeaderDir="${scriptDir}/${scriptPathDepth}/${sbLibVersionSubDir}"
  sbLibVersionWithDot=$(get_sbLibVersionWithDot "$sbLibVersionHeaderDir")

  # check if sbLibVersionWithDot was not found
  if [ -z "${sbLibVersionWithDot}" ]; then
    sbLibVersionWithDot="x.x"
    echo "Selfbus library version was not found in the repository, set to \"${sbLibVersionWithDot}\""
  fi
fi

echo "Creating .hex/.bin files and adding sblib version \"${sbLibVersionWithDot}\" to filename"

outputFileName="${buildArtifactFileBaseName}"
# append build config name and app version
outputFileName="${outputFileName}_${appConfigName}_${appVersionPrefix}${appVersion}"
# replace spaces with underscores
outputFileName="${outputFileName// /_}"
# replace missing options (--)
outputFileName="${outputFileName//--/_}"
# fix trailing missing option
outputFileName="${outputFileName//-_/_}"
# fix leading missing option
outputFileName="${outputFileName//_-/_}"
# replace duplicate underscores
outputFileName="${outputFileName//__/_}"
# convert to lowercase
outputFileName=$(echo "${outputFileName}" | tr '[:upper:]' '[:lower:]')
# add lib version
outputFileName="${outputFileName}_${sbLibprefix}${sbLibVersionWithDot}"
echo "${outputFileName}"

# post-build-step from eclipse
arm-none-eabi-size "${buildArtifactFileName}"
# create .bin file
arm-none-eabi-objcopy -v -O binary "${buildArtifactFileName}" "${outputFileName}.bin"
# create .hex file
arm-none-eabi-objcopy -v -O ihex "${buildArtifactFileName}" "${outputFileName}.hex"

# check if hexDir is defined
if [ -z "${hexDir}" ]; then
    echo "Workspace build variable hexDir not defined or empty, exiting post-build-script."
    exit 1
fi

# add release/debug to hex output directory
case ${outputFileName} in
  *"${debug}"*)
    hexDir="${hexDir}"/"${debug}"
    ;;

  *"${release}"*)
    hexDir="${hexDir}"/"${release}"
    ;;
esac

# check adding flashstart to hex output directory
if [[ "${outputFileName}" =~ .*"${flashstart}".* ]]; then
  hexDir="${hexDir}"/"${flashstart}"
fi

# check for .hex output directory
if [ ! -d "${hexDir}" ]; then
  echo "Creating \"${hexDir}\""
  mkdir -p "${hexDir}"
fi

cp --verbose "${currentWorkingDirectory}/${outputFileName}.hex" "${hexDir}"/

# Do not activate checksum, not sure why, but at least .hex files get corrupted
# see also https://community.nxp.com/t5/Blogs/Hex-file-settings-in-MCUxpresso/bc-p/1131124/highlight/true#M53
# add checksums
#checksum -p ${targetChip} -d "${outputFileName}.bin"
#checksum -p ${targetChip} -d "${outputFileName}.hex"
