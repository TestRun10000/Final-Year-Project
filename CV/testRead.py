import easyocr
import cv2 as cv
import numpy as np

# reader=easyocr.Reader(['en'])
im=cv.imread("hi.jpg")
ig=cv.cvtColor(im,cv.COLOR_RGB2GRAY)
kernel = np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]])
ig = cv.filter2D(ig, -1, kernel)
# ig=cv.blur(ig,(11,11))
ig=cv.adaptiveThreshold(ig,255,cv.ADAPTIVE_THRESH_GAUSSIAN_C,cv.THRESH_BINARY_INV,3,2)
cv.imwrite("a.jpg",ig)
# a=reader.readtext(ig)
print(a)