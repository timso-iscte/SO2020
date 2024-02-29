#!/bin/bash
if [ -z "$1" -a -z "$2" -a "$#" -ne 2 ];then
	echo " Argumentos ivalidos"
else
	echo "Número de pacientes em "$1": "$(grep -c "$1" pacientes.txt)
	i=0
	while read line; do
	j=$(echo $line | awk -F\; '{print $7}')
		if [ "$2" -le "$j" ];then
			((i++))
		fi
		done < medicos.txt
		echo "Número de médicos com saldo superior a "$2": "$i
fi



