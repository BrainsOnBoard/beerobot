This is the program for the "bee robot", which allows a user to control the robot using an Xbox controller while viewing the output of the attached Kodak PixPro camera as it would appear through a bee's eyes. At present, both the robot and the camera must be connected to separately over wifi, so two wifi cards are needed.

You should be able to compile it by running `make` in the main folder.

### iniparser
The [iniparser library](https://github.com/ndevilla/iniparser) for handling .ini files is included as a git submodule, but you may have to do:

```
git submodule update --init
```

to download it before you run `make`.

### OpenCV issues
It seems like the current version of OpenCV that comes with Ubuntu won't work with the PixPro, because it gives a segfault if the stream returned is not 640x480 and the PixPro can only do 1024x1024. (A slightly later version of OpenCV v2.4 does seem to work though, so a fix might happen soon.) Version 3 of OpenCV works fine and you can get this from a PPA:

```sudo add-apt-repository ppa:timsc/opencv-3.2 && sudo apt update```

Then you just have to upgrade your opencv packages. It does mean upgrading all the OpenCV packages on your computer to v3 though, which might not be what you want. You should also be able to get it to work by building OpenCV v3 from source.

### Kodak PixPro
The wifi password for the PixPro is 12345678. Only one device can connect to it at once otherwise it breaks.

If you need to charge it while using it, you have to use a charging cable as if you plug in a regular data cable it will stop acting as a camera and start acting as USB mass storage.

### Xbox controller
The controller needs to be turned on before the program is launched.

The controls are:

* A: forwards
* B: backwards
* D-pad: for turning left and right on the spot

### Keyboard shortcuts
These are mostly for calibrating the unwrapping of the panoramic camera image. Default values should be ok though.

* Esc: quit program (you'll also have to press any button on the Xbox controller to quit the other thread)
* C: toggle display of calibration screen
* space bar: toggle whether moving 1 or 5 pixels for calibration
* left/right/up/down: move centre of unwrapping
* Q/A: make outer circle bigger/smaller
* W/S: make inner circle bigger/smaller
