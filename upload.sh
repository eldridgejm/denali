#!/bin/bash

SERVER="eldridge@stdlinux.cse.ohio-state.edu"
DIRECTORY="WWW/denali"

rsync -a index.html ${SERVER}:${DIRECTORY}
rsync -a resources ${SERVER}:${DIRECTORY}

ssh $SERVER "cd $DIRECTORY && chmod -R 755 *"
