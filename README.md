Birthday attack
---------------

There are supplied binaries, but only run them if you trust strangers too much. The static version should be runnable on many distributions because all the dependencies are linked with it.

To compile **compileThenRun.sh**  script can be run.

More comments about low-level details are in **breakHash.c**. 

To edit some parameters you need to edit the source code and recompile it first. Specification was to find 40bit collision, but because that was too easy for this implementation so I increased difficulty to 48 and 56bits.

The 56 bit run requires 3GB of ram, I wouldn't not recommend go further than that, but the technical limit is 64bit because I modified the sha256.c and made it a bit faster by only handling last 64bit of the hash.

48-bit collision run
--------------------

On 6y old i7 laptop (i7-2760QM) it finishes in 33 seconds(performance of single thread is more important than thread count because this is a single threaded application).
On 9y old dual core laptop (Core 2 Duo processor P8600) it finished in 48 seconds as shown below:

```
Trying to find collision in the last 48 bits (6 bytes).
1/4 Allocating 312,500 kB of ram for 32,000,000 entries !
2/4 Generating hashes (dot every 100,000 hashes)
................................................................................................................................................................................................................................................................................................................................
3/4 Sorting hashes
4/4 Searching for collision

===========
Collission found for ascii inputs "9676361" and "27224393"
============
bf3250d27a9e94db50dfe65d27169adadd48b65c2fbc3f439b99e4189aa2f894
09bdeaf7bf98d426c682e4c7761b67672a45806d6e6eb771e063e4189aa2f894
=============
To verify type:
echo -n "9676361" | sha256sum
echo -n "27224393" | sha256sum
=========
Exiting... have a nice day

real    0m47.753s
user    0m47.360s
sys     0m0.336s
```


Example of 56bit run
--------------------

On 6y old i7 laptop (i7-2760QM) it finishes in 4.5 minutes (performance of single thread is more important than thread count).
On 9y old dual core laptop (Core 2 Duo processor P8600) it finished under 7 minutes as shown below:

```
Trying to find collision in the last 56 bits (7 bytes).
1/4 Allocating 2,792,968 kB of ram for 260,000,000 entries !
===========
Collission found for ascii inputs "39251103" and "102233453"
============
b3a9332ce34badde4bf972d4416eb08ea5374683cbee41a76034a292b4a3a201
bee8d25dbef885af69092c26c3af42959961f1ac9e557037a534a292b4a3a201
=============
To verify type:
echo -n "39251103" | sha256sum
echo -n "102233453" | sha256sum
=========
Exiting... have a nice day

real    6m11.370s
user    6m9.578s
sys     0m1.551s
```