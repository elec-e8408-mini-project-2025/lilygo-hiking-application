#!/bin/bash

rsync -Pa -e "ssh -i test/keys/deploy-key -p 49199" ../lilygo-hiking-application admin@jaajo.finag.fi:/home/admin/Documents/

ssh -i test/keys/deploy-key -p 49199 admin@jaajo.finag.fi "cd /home/admin/Documents/lilygo-hiking-application && ./install.sh"

ssh -i test/keys/deploy-key -p 49199 admin@jaajo.finag.fi "picocom -b 115200 /dev/ttyUSB0"