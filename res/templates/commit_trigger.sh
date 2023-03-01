#!/bin/bash

# Author: Alan Tao, 23/02/16
# Creates a HTTPS request to the webhook whenever a commit is made,
# Changelist descriptions are directly relayed, meaning no special
# handlers or Pull Request markings and etc. within the script. It
# is P4DC that parses these commits and actually creates threads.

webhook_call() {
    curl "__webhook__" \
        -H "Accept: application/json" -H "Content-Type: application/json" -X POST -d "$(echo "$1")"
}

changelist="$1"
workspace="$2"
user="$3"

P4CMD=/usr/bin/p4
P4DCTICKET=/opt/perforce/p4dc_ticket
LOGIN="(p4 login -a -p \"<username>\" | tee /opt/perforce/p4dc_ticket) && sed -i -E '/^Enter/d' /opt/perforce/p4dc_ticket && chmod +644 /opt/perforce/p4dc_ticket"

[[ -f $P4DCTICKET ]] || { echo "No P4DC ticket set in ${P4DCTICKET}. Please run \` $LOGIN \` on the server with the appropriate username and password." >&2; exit 1; }
msg=":satellite:"
msg+="$( \
    $P4CMD -P "$(cat "$P4DCTICKET")" describe -a -s -m 80 "$changelist" 2>&1 | \
    sed -E \
        -e 's/^\.\.\./> .../g' \
        -e 's/^\t/> /g' \
        -e '/^Change/s/$/**/' \
        -e '/^Change/s/^/**/' \
        -e 's/\t/  /g' \
        -e 's/"/\\"/g' \
        -e 's/\$//g' | \
    sed -E ':a;N;$!ba;s/\r{0,1}\n/\\n/g' \
    )"
json="{\"username\":\"__webhook_user__\",\"content\":\"$msg\"}"
webhook_call "$json"
exit 0
