#!/bin/env bash

ShowError() {
  code="$1"
  msg="${*:2}"
  printf "\033[1;31m[ ERROR ]:\033[91m $msg\033[0m\n"
  exit "$code"
}

ShowSuccess() {
    msg="${*}"
    echo -e "\033[1;32m[ SUCCESS ]:\033[92m $msg\033[0m"
}

ShowWarn() {
    msg="${*}"
    echo -e "\033[1;33m[ WARNING ]:\033[93m $msg\033[0m"
}

ShowInfo() {
    msg="${*}"
    echo -e "\033[1;34m[ INFO ]:\033[94m $msg\033[0m"
}