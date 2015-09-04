LIBBAALUE
=========

A collection of basic functions needed for baalue(d) (see also https://tjohann.wordpress.com/)


What is libbaalue?
-------------------
The functions within this library are the fundament of our Bananapi CAN Cluster. They provide easy access to network, jailhouse or other components of the system. Baalue (the QT based userinterface to the cluster) and baalued (the service daemon on a baalue-node) will link against this lib.

We provide a dynamic lib and also a static lib with cmake config(s). For an easy use include libbaalue.h and take a look at the functions in the header, or read the manpage

   man 3 libbaalue


