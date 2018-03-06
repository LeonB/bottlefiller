#!/bin/bash

function getlines()
{
    echo "{"
    for file in *.log; do
        echo "\"$file\": ["
        cat $file \
            | grep -a "{" \
            | grep -a "}" \
        | tr '\n' ','
        echo "],"
    done
    echo "},"
}

lines=$(getlines)
# echo $lines; exit

# format json
echo $lines \
    | sed "s/N: //g" \
    | sed 's/ trappist,/ "trappist",/g' \
    | sed 's/: F/: "F"/g' \
    | sed 's/: T/: "T"/g' \
    | sed 's/,\],/\],/g' \
    | sed 's/\], }/]}/g' \
    | sed 's/,$//g' \
    | jq "$(cat stats.jq)"
