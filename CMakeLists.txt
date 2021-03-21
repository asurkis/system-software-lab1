cmake_minimum_required(VERSION 3.19)
project(Lab1)
include_directories(.)
add_executable(lab1
    main.c
    argparse.c argparse.h
    devicelist.c devicelist.h
    ui.c ui.h
    xfs.c xfs.h
    xfs_types.c xfs_types.h)
