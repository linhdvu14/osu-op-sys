# ----------------------------------------------------------------------------------------
# mypython.py
# Author: Linh Vu
# Course: CS340-400 (Operating Systems), Summer 2016
# Programming Assignment 5 (Python Exploration)		Due: 7/25/2016
# Description: Creates 3 files containing 10 random lowercase characters each. Prints to
#            screen file contents, then two random integers in the range [1 .. 42] and 
#            their product.
# ----------------------------------------------------------------------------------------

from random import choice
from random import randint
from string import ascii_lowercase

# Generate the 3 files
print "Generating 3 files, each containing 10 random lowercase characters...."
for i in range(3):
    out_file = open("file" + str(i+1) + ".txt", "w")
    result = ''.join(choice(ascii_lowercase) for i in range(10))
    print "File", i+1, ":", result
    out_file.write(result)
    out_file.write("\n")
    out_file.close()
 
print "-----------------"    
    
# Generate the 2 integers
print "Generating 2 random integers in the range [1 .. 42]...."
num1 = randint(1, 42)
num2 = randint(1, 42)
prod = num1*num2
print "First integer:", num1
print "Second integer:", num2
print "Product:", prod
