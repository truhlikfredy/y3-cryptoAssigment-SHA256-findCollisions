rm breakHash >/dev/null 2>&1
rm breakHashStatic >/dev/null 2>&1

gcc -std=c11 -O3 sha256.c breakHash.c -o breakHash
echo "Linked libraries for breakHash:"
ldd breakHash

gcc -std=c11 -O3 -static sha256.c breakHash.c -o breakHashStatic
echo "Linked libraries for breakHashStatic:"
ldd breakHashStatic

echo "Running the code:"
time ./breakHash
