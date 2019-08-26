#!/bin/bash

proc-help(){
  cat << EOF
  proc help: show this message
  proc getxroot [run] [event]: get xroot link for an event
EOF
}

proc-getxroot(){
  local run="${1:-5152}"; shift
  local evt="${1:-89}"; shift
  samweb_path=$(getfile.py $run $evt) 
  # echo $samweb_path

  # split the string to get --nskip
  IFS=' ' # space is set as delimiter
  read -ra ADDR <<< "$samweb_path" # str is read into an array as tokens separated by IFS
  # for i in "${ADDR[@]}"; do # access each element of array
  #     echo "$i"
  # done
  samweb_path=${ADDR[0]}
  nskip=${ADDR[2]}

  setup_fnal_security > /dev/null&
  xroot_path=$(samweb2xrootd $samweb_path)  
  echo $xroot_path --nskip $nskip
}

proc-getjob(){
  local run="${1:-5152}"; shift
  local evt="${1:-89}"; shift
  xroot=$(proc-getxroot $run $evt)
  echo "lar -n1 -c pgrapher/experiment/pdsp/wcls-raw-to-sig.fcl $xroot"
  echo
  echo "mv protodune-data-check.root magnify-$run-$evt.root"
  echo
}

cmd="${1:-help}"; shift
proc-$cmd $@
