#ifndef VERSION_H
#define VERSION_H


#define VERSION "3.0"

/*
    3.0 17/08/2013 Qt5 support. No FTP support anymore (no QFtp in Qt anymore :-< )
    2.3 22/09/2010 Use jpg instead of png, so it's compatible with my ovh ftp account ;-)
    2.2 06/06/2010 Fix bug in threshold init, reload every 15s, but now it's based on the clock
    2.1 02/19/2010 Multi threading
    2.0 03/17/2009 Add FTP, first linux + windows version
    1.9 02/09/2009 Debug, save threshold, adjust markers size to image resolution, Flip image (because of bug in OpenCV < 1.1)
    1.8 02/05/2009 add timestamp
    1.7 02/05/2009 fps limiter, keep aspect ratio
    1.6 02/04/2009 Color, markers, owl
    1.5 02/03/2009 Record images upon motion detection
    1.0 01/27/2009 initial version

    2.2 06/06/2010 : Correction bug dans l'init du seuil
    2.1 19/02/2010 : Utilisation de threads : meilleure réactivité de l'interface, optimisation multi processeur; debug ftp, debug i18n, projet QtCreator
    2.0 17/03/2009 : Ajout FTP, première version Linux + Windows
    1.9 09/02/2009 : Debug, sauvegarde seuil, ajustement taille des marqueurs à la résolution de l'image, renversement de l'image (à cause bug OpenCV < 1.1)
    1.8 05/02/2009 : Ajout horodatage
    1.7 05/02/2009 : Limiteur du nombre d'images/s, Aspect ratio
    1.6 04/02/2009 : choix couleur, choix marqueur et HIBOU !
    1.5 03/02/2009 : enregistrement lors de mouvement, traduction fr
    1.0 27/01/2009 : Version initiale

Compatible avec OpenCV : 0.9.7, 1.0.0, 1.1.0, 2.0.0

J'utilise les versions logicielles suivantes pour le développement :
    Ubuntu 10.04
    QT     4.6.2
    OpenCV 2.0.0
    gcc    4.4


*/



#endif // VERSION_H
