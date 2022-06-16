#!/bin/bash

docker kill `docker ps | egrep -o "[0-9a-z]{12}"`