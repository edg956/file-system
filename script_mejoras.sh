#!/bin/bash

echo "#######################################################################"
echo "$./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo ""
echo "#######################################################################"
echo "$./mi_mkdir disco 6 /dir1/"
./mi_mkdir disco 6 /dir1/
echo ""
echo "#######################################################################"
echo "$./mi_touch disco 6 /dir1/fic1"
./mi_touch disco 6 /dir1/fic1
echo ""
echo "$./mi_escribir disco /dir1/fic1 'Hey esto es una prueba' 0"
./mi_escribir disco /dir1/fic1 "hey esto es una prueba" 0
echo ""
echo "#######################################################################"
echo "$./mi_mkdir disco 6 /dir2/"
./mi_mkdir disco 6 /dir2/
echo ""
echo "#######################################################################"
echo "$./mi_cp"
./mi_cp
echo ""
echo "$./mi_cp disco /dir1/fic1 /dir2"
./mi_cp disco /dir1/fic1 /dir2
echo ""
echo "$./mi_cp disco /dir1/fic1 /dir2/"
./mi_cp disco /dir1/fic1 /dir2/
./mi_ls disco /dir2/
echo ""
echo "#######################################################################"
echo "$./mi_cat disco /dir1/fic1"
./mi_cat disco /dir1/fic1
echo ""
echo "$./mi_stat disco /dir1/fic1"
./mi_stat disco /dir1/fic1
echo ""
echo "#######################################################################"
echo "$./mi_ls disco /dir2/"
./mi_ls disco /dir2/
echo ""
echo "$./mi_cat disco /dir2/fic1"
./mi_cat disco /dir2/fic1
echo ""
echo "$./mi_stat disco /dir2/fic1"
./mi_stat disco /dir2/fic1

# PRUEBAS DE MI_CP
