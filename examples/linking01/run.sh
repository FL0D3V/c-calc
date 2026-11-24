#!/bin/bash

input="testFile.eval"
linker="testLinkerFile.link"

./bin/ccalc input -f $input -l $linker
