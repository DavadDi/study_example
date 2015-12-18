#bin/bash

url="http://10.1.11.34:8500/v1/"

for var in $@
do
    url="$url$var"
done

echo $url;

./http_get $url
