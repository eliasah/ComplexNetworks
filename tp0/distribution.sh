#!/bin/sh

#  tp0.sh
#  
##  Created by Elias Abou Haydar on 15/01/14.
#

if [ $# -ne 1 ]         # si le nombre de paramètres n'est pas 2
then                    # affiche l'usage
    echo -n "nombres d'arguments pas suffisant\n Usage : \t distribution [input file].data"
    exit 1
fi

echo "Nombre de liens \t: \t"
cat $1 | wc -l
echo "Nombre de noeuds \t: \t"
cat $1 | tr " " "\n" | sort -nu | wc -l
echo "Conversion des données...\n"
cat $1 | tr " " "\n" | sort -n | uniq -c | awk '{print $2 " " $1;}' > result.data
echo "Géneration des données de distribution...\n"
cat result.data | cut -d " " -f2 | sort -n | uniq -c | awk '{print $2 " " $1;}' > distribution.data
echo "Géneration de la courbe de distribution de degrés en échelle doublement logarithmique\n"
gnuplot -e "set logscale xy; set terminal pdf; set output 'distribution.pdf'; plot [][1:40000] './distribution.data'"

# cleaning :
#rm result.data
#rm distribution.data
rm -f *~

exit 0
