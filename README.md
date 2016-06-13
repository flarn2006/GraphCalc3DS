# Graphing Calculator for 3DS

I guess you could say this is the exact opposite of games that run on graphing calculators.

Requires [sf2dlib](https://github.com/xerpi/sf2dlib).

## Controls

* **Circle pad:** Pan view, move cursor
* **L / R:** Zoom
* **Up / Down:** Change active plot (for equation editing/trace cursor)
* **Left / Right:** Fine trace, change screen page
* **X:** Hold for free cursor
* **Y:** Hold to trace graph (hold **B** to snap to units)
* **Select:** Toggle alt-function mode (like the 2nd key)
* **Start:** Quit

## About variable sliders

Adjustable variable sliders are a new feature in version 1.3. They enable you to add values to graphs that you can easily adjust and experiment with, without the need to manually enter in different values (which can be annoying if they aren't at the end of the equation.) The sliders are on a new "page" on the screen; press Left or Right on the D-pad to switch pages. (This is the only other page as of 1.3, but there may be more in the future, and the code is already written to support more than two.)

You will then see four sliders, labeled 'a', 'b', 'c', and 'd'. The labels to the left are actually buttons; pressing them will insert the variable into the equation just like the 'x' button on the main page. To the right of each slider is a button labeled "0-1". This will reset the range of the slider to the default of, well, 0 to 1. You can extend the range of the sliders by holding down L or R while dragging past the left or right. You can shorten the range by setting the slider to the value you want to be the minimum or maximum, activating alt-mode (press Select) and pressing the button to the left (for minimum) or right (for maximum) of the slider.

## Important notes

* Equations must be entered in RPN (Reverse Polish Notation). This means, for example, rather than "sin(4 + x)", you would enter "4 x + sin".
* If you've entered a number and want to start entering a new number immediately after, press the decimal point key twice, or only once if there's already a decimal point.