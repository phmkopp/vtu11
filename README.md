~~~
         __        ____ ____
___  ___/  |_ __ _/_   /_   |
\  \/ /\   __\  |  \   ||   |
 \   /  |  | |  |  /   ||   |
  \_/   |__| |____/|___||___|

~~~

vtu11
Small lib to read/write vtu files

This is stupid

# Decoding base64 data with python


~~~py
import numpy
import base64

# Decode base64 encoded data back to raw binary
raw = base64.b64decode('YAAAAAAAAAA=')

# Interpret binary data as unsigned 32 bit integers
data = numpy.frombuffer(raw, dtype=numpy.uint64)

# Prints: [96]
print(data)
~~~
