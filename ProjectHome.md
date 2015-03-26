### What is it? ###
Face Finder is project made for graduation on AGH University of Science and Technology in Cracov, Poland. Its goal is to detect face on the image at recognize it according to given database.
Engine is written to make easy use of optimization libraries like BLAS or CUDA. It optionally use OpenCV libraries too.
User interface is made completely in Qt4.

### How to run it? ###
  1. Download all files from repository (Source->Checkout)
  1. Ensure you have Qt4 SDK installed on your computer
  1. Build project FaceFinder2.pro using Qt tools
  1. Run application you've just built

### How to use it? ###
Program needs database of faces that should be recognized. You must provided example pictures to allow program learning each face from database. To actually learn system new faces you must run learning process for neural network.

### Video guide ###
Quick screencast from work with application is available on Youtube: http://www.youtube.com/watch?v=sftewf5q-mQ