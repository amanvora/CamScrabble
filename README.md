# Project Progress #


**Week2**

### Feb 23rd - Taruna ###
* Tried various methods for corner detection:
     * Method 1: Converted the image to grayscale, performed adaptive thresholding and then tried extracting rows/columns having highest intensity. This method didn't work out as after binarizing the image, all blocks converted to white hence lost the grid information.
     * Method 2: Tried various color filtering algorithms by converting to other color spaces (HSV, YUV), but nothing seemed to help much.

### Feb 23rd - Aman ###
* Used internet based images to extract letter regions from the frame difference.
* Frames are converted from RGB => Grayscale.
* Used MSE on the difference to calculate content changes. (Drawback: Fails for even slight misalignment in the frames).
* Performed two pass segmentation to get labels of the images.
* Extracted region with maximum label. (Both the steps above can be redundant if neural nets are robust for noise.)

### Feb 25th - Terna ###
* Created README

### Feb 25th - Taruna ###
* Continued my work on corner detection
     * Method 3: This is similar to method 1, but in this case I did not binarize the image, instead tried to find the high intensity regions(white lines) in the scrabble board. This method fails if image is even slightly deformed (tilted, bulged etc.)
     * Method 4: Detected the eight blocks (4 at corners and 4 halfway to corners) using color based filtering method. Binarized the image to convert region of interest as white region. Next I performed morphological processing to remove isolated white pixels.

### Feb 26th - Terna ###
* Added several prototype code files - neural net ocr + board + matrix

### Feb 26th - Taruna ###
* Continued my work on Method 4- Found the four corner coordinates corresponding to white blocks (which are matrix of 1s).
* Performed perspective transform on resulting image to fix the square sized 1200 by 1200 image.![grayscrabble.jpg](https://bitbucket.org/repo/pGnMGz/images/1773801207-grayscrabble.jpg)

### Feb 27th - Terna ###
* Added opencv code for possible fix to camera barrel distortion

### Feb 27th- Taruna ###
* Checked in python code -version 1 for detecting the grid coordinates.
    * Prints location of corners
    

### Feb 28th - Terna & Aman ###
* Thoroughly investigated operation of the camera and the video template code.
* Succesfully collected and exported image frames from CCS.
* Created C++ program to read and export collected frames: see readdata.cpp.


### Feb 29th - Terna ###
* Created improved version of readdata.cpp (see readdata.py). The new version has the following features:
     * Specify command line arguments.
     * Displays the image.
     * Save image to file by specifying output file name on command line.
* Converted previously single-layer neural net to a two layer network: 728-input layer, 32-neuron hidden layer with relu activation, 26-neuron output layer (no softmax).
* The change was made because I tested the previous network on a few instances of frames captured by the DSP board and it did not perform well. The added hidden layer significantly improved accuracy.
* The adaptive thresholding was modified such that the letters are now white and the background is black.


### Feb 29th - Terna ###
* Created adaptive thresholding function.
* Tested the algorithm on the DSP board and the performance is too slow (> 10 seconds).
* Optimized adaptive thresholding by using an integral images. Thus the algorithm uses two passes and improved performance by more than 17 times with window size of 15-by-15. The new algorithm performance is unaffected by window size unlike the first algorithm. See http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.420.7883&rep=rep1&type=pdf.

### 2nd March -Taruna ###
* Written C code to convert RGB to HSV and perform color filtering. The code does not give desired result when I ran it on the image frames generated by board. This is because the the Ycrcb to RGB conversion is not correct. Our next task is to get the correct RGB image from the Ycrcb frames and then do color filtering on the same.

### 2nd March -Aman ###
* Used mat.h for to store matrices and operate on them.
* Use of opencv only for imread and imshow.

### 3rd March - Terna ###
* Added a function Mat_maxResponse to locate local maxima in a grayscale image. Specify the search area and the window size for the response.
* Corrected several bugs in the function. See Mat.h and readdata.cpp for example use.

### 7th March -Taruna ###
* Added function Mat_mapping in mat.h file. this function maps coordinates between ideal and camera image.
* Added perspective.py to generate ideal scrabble board image.

### 7th March - Aman ###
* Changed mean square error to Integral image to find letter location.

### 8th March - Terna ###
* Changed network to 64-neuron hidden layer.
* Retrained network with 4096 samples per letter including combinations of gaussian noise, rotation, skewing and translation.
* Obtained 99% accuracy.

### 11th March - Taruna, Terna, Aman ###
* Debugging of erroneous data in memory issue.
* Integrated and verified corner detection code on DSP board.
* Integrated Mapping code (ideal image-->camera image) and verified the results.

### 15th March - Taruna, Terna, Aman ###
* Made a build on CCS to detect 28*28 blocks based on Mat_mapping function.
* Debugged Mat_mapping function.
* Performed live experiment to do adaptive thresholding on each block(28*28).

### 16th March- Taruna ###
* Checked in code : Fixed the errors in Mat_mapping function.


### 16th March - Terna ###
* Updated the code to match the main program on the DSP board.
* Retrained the network 98% accuracy (floating point).
* Generated fixed point parameters. Although the accuracy is good, we need better training data as the current data (which noise is added to) is already 'too clean'. 

### 18th March - Aman ###
* Added letter location to DMP's existing main.
* Added overWriteSubMatr in Mat to display a submatrix in display.
* Changed letter location detection method to thresholding of percentage change in sub squares.

### 20th March - Terna and Aman ###
* Tested for new letter locations across various regions of the scrabble board.


### 23th March - Terna ###
* Added a function to localize letters by examining the variance in the image. In theory, the letter will show more variance that the surroundings (shadows too). However, this needs to be tested for letters like I, L & J.
* Discovered that it is better to crop a letter then perform adaptive thresholding rather than the reverse process. It helps suppress the shadows at the edges. (obvious, now that I think about it) 

### 28th March - Aman ###
* Used Center of Mass using variance filter. Calculated a response to be thresholded against a constant.
* Used this thresholding method along with previous threshold of percentage change as a double check.
* Result: Increased the robsutness of the letter location algorithm, shadows are ignored and letter tiles with reasonable rotation are still detected.


### 28th March - Terna ###
* Retrained the network several times.
* Collected real world data and used it to retrain the network.

### 30th March- Taruna###
* Added file dictionary.cpp which contains implementation to add a word to trie as well as search any word from dictionary and display the total score of that word.

### 7th April - Aman and Terna ###
* Devised a method to detect primary word.
* Added function to extract origin, length and direction along which primary word occurs
* Need to test on board

### 8th April - Aman ###
* Primary word location detected very accurately. Length, origin of word and direction are set correctly.
* Use these 3 parameters for word score calculation.

### 8th April-Taruna ###
* Created hash table since trie structure didn't work properly on board due to dynamic memory allocation issues.
* Created Hash table maps 9998 out of 9999 words uniquely. "RIDGE" fails to map due to collision.
* hashtable.h = contains hashtable generated through code.
* dictionaryhash.cpp - C/C++ code for hash table implementation.
* dictionarytrie.cpp - Trie implementation for dictionary.
* words.h - 9999 words - Most frequent
* file2Array - python script to convert words stored in text file to array format(uppercase, sorted).

### 11th April - Aman ###
* Secondary words function : To detect branched words
* Need to check its working on board

### 20th April - Taruna , Terna ###
* Integrated code for card(red/green) detection.
* Worked on interface.
* Reference for dictionary used :
https://github.com/first20hours/google-10000-english

Link for presentation:
https://www.overleaf.com/5020387fsmmdf

Link for report:
https://www.overleaf.com/5021106jrdfdn

Deliverables:
1) Final Report
2) Scrabble rules - get print out
3) Poster print out
4) Video
5) Checkin final code , organize folder etc in bitbucket