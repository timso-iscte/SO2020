#!/bin/bash
if [ -f medicos.txt ];then
	touch lista_negra_medicos.txt
	while read line;do
		nmr=$(echo $line | awk -F\; '{print $6}')
		rtng=$(echo $line | awk -F\; '{print $5}')
		cedula=$(echo $line | awk -F\; '{print $2}')
			if [ ! "$(grep "$cedula" lista_negra_medicos.txt)" ];then 
				if [ "$nmr" -gt 6 -a "$rtng" -lt 5 ];then
					echo $line >> lista_negra_medicos.txt 
				fi
			fi
	done < medicos.txt
	cat lista_negra_medicos.txt
else
	echo "Não existem médicos registados"
fi
