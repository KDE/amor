# AMOR
<p align="center">
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/billy.png" alt="billy" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/blob.png" alt="blob" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/bonhomme.png" alt="bonhomme" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/bsd.png" alt="bsd" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/eyes.png" alt="eyes" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/ghost.png" alt="ghost" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/neko-kuro.png" alt="neko-kuro" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/neko.png" alt="neko" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/tao.png" alt="tao" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/tux.png" alt="tux" />
<img src="https://raw.githubusercontent.com/KDE/amor/master/data/pics/preview/worm.png" alt="worm" />
</p>

## dependencies

if you do not have KDE env installed, you will need these dev dependencies installed before trying to compile the source code.

### ubuntu

```sh
sudo apt-get install cmake extra-cmake-modules qtbase5-dev libqt5x11extras5-dev libkf5doctools5 libkf5dbusaddons-dev libkf5coreaddons-dev libkf5i18n-dev libkf5config-dev libkf5windowsystem-dev libkf5xmlgui-dev
```

## installation

```sh
mkdir build && cd build
cmake ..
make
sudo make install
amor
```

