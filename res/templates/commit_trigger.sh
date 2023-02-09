#!/bin/sh

# Author: Alan Tao, 23/02/06

# Creates a HTTPS request to the webhook whenever a commit is made,
# Changelist descriptions are directly relayed, meaning no special
# handlers or Pull Request markings and etc. within the script. It
# is P4DC that parses these commits and actually creates threads.

webhook="__webhook__"

function webhook() {
    curl \
        -i \
        -H "Accept: application/json" \
        -H "Content-Type: application/json" \
        -X POST \
        --data "{\"content\":\"${1}\",\"embeds\":[]}" \
        $webhook
}

changelist="$1"
workspace="$2"
user="$3"

msg=":satellite: New commit by \`$user\` titled \`$changelist\` (submitted from `$workspace`)\n\`\`\`"

p4 fstat -e "$changelist" //... | while read -r line
do
    name=$(echo "$line" | sed 's/[\. ]\+\([^ ]\+\) .\+/\1/')
    msg="$msg$name, "
done
msg="$msg\`\`\`"

webhook $msg

exit 0