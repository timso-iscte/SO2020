#!/bin/bash
if [ $# -ne 4 ];then
	echo "Os argumentos não podem ser null"
else
	if [ ! -f medicos.txt ];then 
		echo $1";"$2";"$3";"$4";0;0;0" >> medicos.txt
	else
		if [ ! "$(grep "$2" medicos.txt)" ];then
			echo $1";"$2";"$3";"$4";0;0;0" >> medicos.txt
		else
			echo $2
			echo "O médico já existe"
		fi
	fi
	cat medicos.txt
fi
