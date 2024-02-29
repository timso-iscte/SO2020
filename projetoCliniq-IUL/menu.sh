#!/bin/bash
while [ 1 ];do
echo "1. Cria pacientes"
echo "2. Cria médicos"
echo "3. Stats"
echo "4. Avalia médicos"
echo "0. Sair"

read opcao

	case $opcao in
	1) ./cria_pacientes.sh;;
	2) echo "Introduza Nome, Cedula, Especialidade, email"
		read -a dados
		./cria_medico.sh "${dados[0]} ${dados[1]}" "${dados[2]}" "${dados[3]}" "${dados[4]}";;
	3) echo "Introduza localidade e saldo"
		read -a dado
		./stats.sh "${dado[0]}" "${dado[1]}";;
	4) ./avalia_medicos.sh;;
	0) break
	esac
done

