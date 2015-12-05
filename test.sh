#!/bin/sh



./md5 -q -s "12345678901234567890123456789012345678901234567890123456789012345678901234567890" | awk '{ if( $1 ==  "57edf4a22be3c955ac49da2e2107b67a" ) print "Test successed" ;   else print "Test failed" }'

./md5 -q -s "0123456789abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" | awk '{ if( $1 ==  "da7cc83e972ad89dd7d4d6b9031a45da" ) print "Test successed" ;   else print "Test failed" }'
