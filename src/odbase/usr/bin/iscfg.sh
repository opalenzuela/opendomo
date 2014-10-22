#!/bin/sh
#desc:Verifies that the given path is a configuration file

# En OpenDomo usamos muchos archivos de configuración  que emplean la sintaxis
# de Bash, de modo que pueden ser incluídos directamente en lugar de necesitar
# parsearlos cada vez (a golpe de grep y cut).
# No obstante,  aunque el método es muy cómodo tanto para la lectura como para
# la escritura de tales archivos, entraña un importante problema de seguridad,
# ya que tales archivos pueden contener comandos  que serán ejecutados con los
# permisos del usuario que lea la configuración.
# Este script verificará  que los archivos solamente contienen asignaciones de
# variables, inócuas para el sistema.

FILTER='
/^$/d                                # Elimina líneas vacías
/^#/d                                # Elimina líneas comentario
/^[A-Z0-9_]\+[\t ]*=[\t ]*.\+$/{        # Seleccionamos las posibles asignaciones
   /`/p                                 # Alertamos si existe acento grave
   /\\/p                                # Alertamos si existe barra invertida
   /;/p                                 # Alertamos si existe separador de línea
   d                                    # Aceptamos el resto
}
'


if test -f "$1"; then
	if test -r "$1"; then
		C=`sed -e "$FILTER" $1 | wc -l` 2>/dev/null
		#sed -e "$FILTER" $1 > salida.txt      # Sólo depuración para ver lo detectado
		# Si queda algo, puede ser peligroso!
		if test $C -gt 0; then
			echo "#ERR Suspicious strings in configuration"
			exit 3
		fi
	else
		echo "#ERR Missing privileges"
		exit 2
	fi
else
	echo "#ERR File not found"
	exit 1
fi
exit 0

