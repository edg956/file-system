bold=$(tput bold)
normal=$(tput sgr0)

make

echo "${bold}./mi_rm disco /dir2/dir21/${normal}"
./mi_rm disco /dir2/dir21/ #o mi_rmdir

echo "${bold}./mi_rm disco /dir2/dir21/fic2${normal}"
./mi_rm disco /dir2/dir21/fic2

echo "${bold}./mi_stat disco /dir1/dir11/fic1${normal}"
./mi_stat disco /dir1/dir11/fic1 #Hemos borrado 1 enlace

echo "${bold}./mi_rm disco /dir2/dir21/fic2${normal}"
./mi_rm disco /dir2/dir21/fic2

echo "${bold}./mi_rm disco /dir2/dir21/${normal}"
./mi_rm disco /dir2/dir21/ #o mi_rmdir

echo "${bold}./mi_ls disco /dir2/dir21/${normal}"
./mi_ls disco /dir2/dir21/

echo "${bold}./mi_rm disco /dir2/dir21/fic4${normal}"
./mi_rm disco /dir2/dir21/fic4

echo "${bold}./mi_rm disco /dir2/dir21/fic5${normal}"
./mi_rm disco /dir2/dir21/fic5

echo "${bold}./mi_rm disco /dir2/dir21/${normal}"
./mi_rm disco /dir2/dir21/ #o mi_rmdir

echo "${bold}./mi_ls disco /dir2/${normal}"
./mi_ls disco /dir2/

echo "################################################################################"
echo "# Comprobamos que al crear 17 subdirectorios los bloques de datos del padre son 2 
# (en un bloque caben 16 entradas de directorio), 
# y que al eliminar un subdirectorio el directorio padre tiene 1 bloque de datos
################################################################################"
echo "${bold}./mi_mkdir disco 6 /d1/${normal}"
./mi_mkdir disco 6 /d1/

echo "# creamos 17 subdirectorios sd0, sd1..., sd16 en d1"
# creamos 17 subdirectorios sd0, sd1..., sd16 en d1
for i in {0..16}
do
echo "${bold}./mi_mkdir disco 6 /d1/sd$i/${normal}"
./mi_mkdir disco 6 /d1/sd$i/
done

echo "################################################################################"
echo "# Mostramos la metainformacion del directorio para ver que tiene 2 bloques de datos"
echo "${bold}./mi_stat disco /d1/${normal}"
./mi_stat disco /d1/

echo "################################################################################"
echo "# Listamos el directorio para ver sus subdirectorios"
echo "${bold}./mi_ls disco /d1/${normal}"
./mi_ls disco /d1/

echo "################################################################################"
echo "# Eliminamos el subdirectorio sd3 de d1"
echo "${bold}./mi_rm disco /d1/sd3/${normal}"
./mi_rm disco /d1/sd3/ #o mi_rmdir

echo "################################################################################"
echo "# Mostramos la metainformacion de d1 para ver que ahora tiene 1 bloque de datos"
echo "${bold}./mi_stat disco /d1/${normal}"
./mi_stat disco /d1/

echo "
################################################################################"
echo "# Volvemos a listar el directorio para ver que se ha eliminado un subdirectorio"
echo "${bold}./mi_ls disco /d1/${normal}"
./mi_ls disco /d1/

make clean
