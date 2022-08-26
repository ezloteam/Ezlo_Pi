#!/bin/bash

idf.py build 

retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Command failed"
fi