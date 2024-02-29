#!/bin/bash
if [ -f /etc/passwd ]; then
	cat /etc/passwd | grep "^a" | grep -E '[0-9]{5}' | head -10 | awk -F [:,] '{print $1";"$5";;;"$1"@iscte-iul.pt;100"}' | sed 's/^a*//g' > pacientes.txt

else
	echo "não existe"
fi
