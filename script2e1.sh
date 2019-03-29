#leer y escribir texto de más de 1 bloque en diferentes offsets 
#(contenido en el fichero externo texto2.txt)

echo "################################################################################"
echo "$ rm disco"
rm disco
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "################################################################################"
echo "$ ./escribir disco "$(cat texto2.txt)" 1"
echo "#escribimos el texto contenido en text2.txt en los offsets  0, 5120, 256000, "
echo "#30720000 y 71680000, de inodos diferentes"
./escribir disco "$(cat texto2.txt)" 1
echo "################################################################################"
echo "$ ./leer disco 3 > ext4.txt"
echo "#leemos el contenido del inodo 3 (escrito en el offset 256000) y lo direccionamos"
echo "#al fichero externo ext4.txt"
./leer disco 3 > ext4.txt
echo "################################################################################"
echo "$ ls -l ext4.txt"
echo "#comprobamos cuánto ocupa el fichero externo ext4.txt"
echo "#(ha de coincidir con el tamaño en bytes lógico del inodo 3 y con total_leidos)"
ls -l ext4.txt
echo "################################################################################"
echo "$ cat ext4.txt"
echo "#usamos el cat del sistema para leer el contenido de nuestro fichero direccionado"
echo "#No hay que mostrar basura"
cat ext4.txt