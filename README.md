Static File Server

The server has an HTTP Parser which is implemented in HTTPRequestParser.cc. The
parser relies on token states and reads the input character by character and
stores each complete string token as a string in an ArrayList. The first three
entries in the arraylist will be the method, URL, and HTTP version (in that
order). Afterwards, the arraylist format such that (arr[i], arr[i+1]) represent
a (key, value) pair. When generating the response, I retrieve the URL value and
copy the associated file into a buffer. I generate a header including the size
of the file as content-length, then finally copy the file buffer into the final
response buffer.

Sources Cited:
https://www.geeksforgeeks.org/regex_replace-in-cpp-replace-the-match-of-a-string-using-regex_replace/
Discussion slides 
