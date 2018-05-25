# Installationsanleitung zur Verwendung des GazeTrackers in Visual Studio 2015

Der GazeTracker wurde in Visual Studio 2015 und Windows 10 64 Bit entwickelt und getestet. Es wird daher empfohlen, diese Versionen zu verwenden.
In GazeTracker Analytics wird außerdem ein Video im XVID Codec erstellt, dessen Installation am Computer durchaus eine Vorraussetzung der problemlosen Anwendung ist.

**Installationsvorbereitungen - Downloads:**
- OpenCV für Windows - Version 3.1 (http://opencv.org/downloads.html)
- CMake 3.5.1 (https://cmake.org/download/)
- Python 3.5 im Paket "Anaconda für Windows"(https://www.continuum.io/downloads) 
- Qt 5.6.0 für Windows 64 Bit und Visual Studio 2015 (https://www.qt.io/download-open-source/#section-2)
- Qt5Package - Visual Studio Extension (zu finden in Visual Studio 2015 unter "Extras/Extensions und Updates..")

**Installationsanleitung:**
1.	OpenCV 3.1 in ein beliebiges Verzeichnis extrahieren - zB: „C:\OpenCV“. Darin befinden sich die Ordner „sources“ und „build“. In diesem Verzeichnis einen neuen Ordner „mybuild“ o.ä. erstellen, der später für den eigenen Build verwendet wird.
2.	Qt 5.6.0 mit dem Installer installieren. Hierbei einfach die Standard Einstellungen der Installation verwenden. (Module, welche mit deprecated markiert sind, können natürlich weggelassen werden)
3.	Umgebungsvariable „QTDIR“ für Qt erstellen, welche auf den Ordner „msvc2015_64“ im Installationsverzeichnis von Qt zeigt - zB: „C:\Qt\Qt5.6.0\5.6\msvc2015_64“.
4.	Umgebungsvariable „%QTDIR%\bin“ in die PATH Systemvariable hinzufügen.
5.	CMake mit Standard Einstellungen installieren und zur PATH Systemvariable hinzufügen lassen.
6.	PC neu starten damit die Variablen aktiv werden.
7.	CMake starten.
8.	In „Where is the source code:“ das „source“ Verzeichnis von OpenCV setzen - zB: „C:/OpenCV /sources“.
9.	In „Where to build the binaries:“ das „mybuild“ Verzeichnis von OpenCV setzen - zB: „C:/OpenCV /mybuild“.
10.	Auf „Configure“ klicken. Hier erscheint ein Dialog, darin wählt man „Visual Studio 14 2015 Win64“ aus, damit OpenCV mit 64 Bit erstellt wird. Außerdem wählt man hier „Use default native compilers“ aus und bestätigt den Dialog.
11.	Es erscheint nun eine Liste mit Einstellungsoptionen, wobei die meisten oder alle rot hinterlegt sind. Hier werden die Optionen „BUILD_DOCS“, „BUILD_opencv_python2“, „BUILD_opencv_python3“ und „WITH_VTK“ abgewählt. Um nun die Unterstützung für Qt im OpenCV Build zu haben, muss „WITH_QT“ angewählt werden.  
12.	Nun muss einige Male auf „Configure“ gedrückt werden (ca. 3x), bis alle rot hinterlegten Einträge verschwunden sind. Hier kann es sein, dass die PATH Variable zu Qt nicht gefunden wird, was dann als rot hinterlegter Eintrag erscheint. Diese muss wie in Schritt 3 und 4 beschrieben, hinzugefügt werden.
13.	Sobald alles weiß hinterlegt ist, kann auf „Generate“ gedrückt werden, danach wird der Build erstellt.
14.	CMake kann nun beendet werden.
15.	Nun wechselt man in das Verzeichnis des Builds (C:\OpenCV\mybuild) und öffnet „OpenCV.sln“. Hier sollte der Modus auf Debug und x64 eingestellt sein, da vorhin für 64 Bit ein Build erstellt wurde. 
16.	Darin das Projekt „ALL_BUILD“ suchen, mit der rechte Maustaste darauf klicken und Erstellen auswählen. (Damit werden die *.lib und *.dll Daten für das Debugging erstellt) Hierbei sollten bei dem Erstellen keine Fehler auftreten. Danach wird der Modus auf Release geändert und für den Release Modus das Projekt „ALL_BUILD“ nochmals erstellt. Hierbei sollten wieder keine Fehler auftreten.
17.	Hat alles geklappt, klickt man mit der rechten Maustaste auf „INSTALL“ und Erstellen. Dies ist unter „CMakeTargets“ zu finden. Der Schritt wird wie vorhin für Debug und Release extra ausgeführt. Mithilfe diesen Schrittes, werden alle *.lib und *.dll Daten in einen eigenen Ordner verschoben, welcher unter „..\mybuild\install\x64\vc14\..“ zu finden ist.
18.	Den Ordner „install“ kopiert man nun in das OpenCV Verzeichnis (C:\OpenCV). 
19.	Um OpenCV einheitlich verwenden zu können wird eine Umgebungsvariable „OPENCV_DIR“ angelegt, welche auf den Ordner „install\x64\vc14“ aus dem OpenCV Verzeichnis zeigt.
20.	Des Weiteren muss in der PATH Systemvariable ein Eintrag mit „%OPENCV_DIR%\bin“ ergänzt werden.
21.	PC neu starten damit die Variablen aktiv werden.
22.	OPTIONAL: In Visual Studio 2015 muss eventuell noch die Qt Version mit dem zugehörigen Pfad eingetragen werden, damit die Qt Funktionen verfügbar sind. Dazu öffnet man in Visual Studio 2015 den Menu Punkt „QT5/QT Options“. Falls hier schon die aktuellste Version eingetragen ist, muss man nichts mehr machen. Ansonsten klickt man auf „Add“ und schreibt den Versionsnamen und fügt den Pfad für den „mcsv2015_64“ Ordner ein - z.B: "C:\Qt\Qt5.6.0\5.6\msvc2015_64".


**_Nach dieser Installationsanleitung sollte sich das Projekt in Visual Studio 2015 ausführen lassen! Sollte dies nicht der Fall sein, sollte die Anleitung wiederholt werden und auf die genauen Versionen und Anweisungen geachtet werden._**