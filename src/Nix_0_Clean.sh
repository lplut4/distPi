#!/bin/bash

if [ -d "Dependencies" ]; then
    rm -rf "Dependencies"
fi
    
git clean -fxd
