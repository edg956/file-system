bold=$(tput bold)
normal=$(tput sgr0)

make

echo "################################################################################"
echo "${bold}./mi_mkfs disco 100000${normal}"
./mi_mkfs disco 100000
echo "################################################################################"
echo "${bold}./mi_mkdir disco 6 /dir1/${normal}"
./mi_mkdir disco 6 /dir1/
echo "${bold}./mi_mkdir disco 6 /dir1/dir11/${normal}"
./mi_mkdir disco 6 /dir1/dir11/
echo "${bold}./mi_touch disco 6 /dir1/dir11/fic1${normal}"
./mi_touch disco 6 /dir1/dir11/fic1 #o mi_mkdir
echo "${bold}./mi_escribir disco /dir1/dir11/fic1 hellooooooo 0${normal}"
./mi_escribir disco /dir1/dir11/fic1 hellooooooo 0

echo "################################################################################"
echo "${bold}./mi_mkdir disco 6 /dir2/${normal}"
./mi_mkdir disco 6 /dir2/
echo "${bold}./mi_mkdir disco 6 /dir2/dir21/${normal}"
./mi_mkdir disco 6 /dir2/dir21/

echo "################################################################################"
echo "${bold}./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2${normal}"
./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2

echo "################################################################################"
echo "${bold}./mi_cat disco /dir2/dir21/fic2${normal}"
./mi_cat disco /dir2/dir21/fic2 #ha de mostrar mismo contenido que /dir1/dir11/fic1

echo "${bold}./mi_stat disco /dir1/dir11/fic1${normal}"
./mi_stat disco /dir1/dir11/fic1

echo "${bold}./mi_stat disco /dir2/dir21/fic2${normal}"
./mi_stat disco /dir2/dir21/fic2

echo "${bold}./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4${normal}"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4

echo "${bold}./mi_touch disco 6 /dir1/dir11/fic3${normal}" 
./mi_touch disco 6 /dir1/dir11/fic3 #o mi_mkdir

echo "${bold}./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4${normal}"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4

echo "${bold}./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic5${normal}"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic5

echo "${bold}./mi_stat disco /dir1/dir11/fic3${normal}"
./mi_stat disco /dir1/dir11/fic3
echo "################################################################################"

echo "${bold}./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic2${normal}"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic2 #camino2 NO ha de existir
make clean

