#!/bin/sh

./encrypt tmp $1
./decrypt tmp tmp2
#diff $1 tmp2

