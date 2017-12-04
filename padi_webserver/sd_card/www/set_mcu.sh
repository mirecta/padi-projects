#!/bin/bash

cd js/
gzip -f -k *.js
cd ../

cd css/
gzip -f -k *.css
gzip -f -k *.css.map
cd ../

gzip -f -k *.html
