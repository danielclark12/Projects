Daniel Clark-Orive clar2161

hash(T item) is implemented by modding the length of the sum of the ascii characters in the string.

hash2(T item) is implemented by using open adressing so the size of the array needs to be at minimum
the number of items in the text file. If the number of items being added is larger than the size of the array
then there will be an infinite while loop that occurrs.

My add looks like add(T index, int hashToUse)
hashToUse is a parameter I put in so that I could specify which hash function to use. 1 is for hash and 2 is for hash2.

