# WellIndexCalculator
A small library for well index calculation for FieldOpt.
The library now only consists of a list of functions that can be called.

The following libraries are needed:


- RPolyPlusPlus(https://github.com/sweeneychris/RpolyPlusPlus):

Important note. Later versions of RPoly makes WellIndexCalculator unstable. 
Clone the RPoly repo and then checkout an older commit with the following command:

git checkout c79e0ea87922b5d7624e76f9792f5fd915837c19


- Eigen3 library (http://eigen.tuxfamily.org/index.php?title=Main_Page)

RPolyPlusPlus depends on the eigen library. However RPolyPlusPlus links
to a different directory than the standard path for the Eigen library.
A workaround to make this link work is to simply copy the Eigen library
into THIS(write path here) path.

- Armadillo library (http://arma.sourceforge.net/)

Library was used in the beginning. Will be phased out shortly.

- FieldOpt::Model files need to be present.




