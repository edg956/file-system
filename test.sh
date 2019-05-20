echo "***Script para comprobar enlaces y borrado de ficheros y directorios***"
echo
rm disco
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "######################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/"
./mi_mkdir disco 6 /dir1/
echo "$ ./mi_mkdir disco 6 /dir1/dir11/"
./mi_mkdir disco 6 /dir1/dir11/
echo "$ ./mi_mkdir disco 6 /dir1/dir12/"
./mi_mkdir disco 6 /dir1/dir12/
echo "$ ./mi_touch disco 6 /dir1/dir11/fic1"
./mi_touch disco 6 /dir1/dir11/fic1
echo "$ ./mi_mkdir disco 6 /dir2/"
./mi_mkdir disco 6 /dir2/
echo "$ ./mi_mkdir disco 6 /dir2/dir21/"
./mi_mkdir disco 6 /dir2/dir21/
echo "$ ./mi_escribir disco /dir1/dir11/fic1 "hoy puede ser un gran dia" 0"
./mi_escribir disco /dir1/dir11/fic1 "hoy puede ser un gran dia" 0
echo "$ ./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2"
echo "El contenido de /dir2/dir21/fic2 ha de pasar a ser el mismo de "
echo "/dir1/dir11/fic1 y tiene que haberse incrementado el nº de enlaces"
./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2
echo "$ ./mi_mkdir disco 6 /dir3/"
./mi_mkdir disco 6 /dir3/
echo "$ ./mi_link disco /dir1/dir11/fic1 /dir3/fic4"
echo "El contenido de /dir3/fic4 ha de pasar a ser el mismo de /dir1/dir11/fic1"
echo "y tiene que haberse incrementado el nº de enlaces del inodo común"
./mi_link disco /dir1/dir11/fic1 /dir3/fic4
echo "$ ./mi_touch disco 6 /dir3/fic3"
./mi_touch disco 6 /dir3/fic3
echo "$ ./mi_escribir disco /dir3/fic3 "!A por ello!" 5120"
./mi_escribir disco /dir3/fic3 "!A por ello!" 5120