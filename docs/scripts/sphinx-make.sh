#!/bin/bash

if [ "$1" == "venv" ]
then
    python3.6 -m pip3.7 install --user virtualenv
    python3.6 -m virtualenv $VENV_DIR
    source $VENV_DIR/bin/activate;
    pip3.7 install -r $DOCS_DIR/etc/requirements.txt
else
    source $VENV_DIR/bin/activate;
    VERSION=`source $WS_ROOT/src/scripts/version`
    TM=`TZ=GMT date`
    sed -ie "s/**VPP Version:\*\* .*/**VPP Version:** $VERSION/" $DOCS_DIR/about.rst
    sed -ie "s/**Built on:\*\* .*/**Built on:** $TM/" $DOCS_DIR/about.rst
    rm $DOCS_DIR/about.rste
    make -C $DOCS_DIR $1
fi

deactivate
