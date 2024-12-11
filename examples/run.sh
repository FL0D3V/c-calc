#!/bin/bash

input="-f testFile.eval"
linker="-l testLinkerFile.link"

../bin/ccalc $input $linker
