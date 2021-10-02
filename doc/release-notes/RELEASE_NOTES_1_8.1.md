# Nyancoin Core 1.8.1

Nyancoin Core 1.8.1 is primarily a bugfix release, bringing Nyancoin Core in
line with Bitcoin 0.9.3. Nyancoin Core 1.8.1 also adds in support for printing
paper wallets, and disables connections to pre-1.8 clients. This is a RECOMMENDED
upgrade due to the large number of bug fixes, but is not a required one.

Paper wallet support has been developed by AndyMeows (IRC username), and 
can be accessed via the "File" menu. It's intended for two purposes; to generate
small value paper wallets to hand out, and more permanent deposit paper wallets
for keeping balances offline.

IMPORTANT: If you are producing offline paper wallets, you should do so on a
computer that's disconnected from the Internet. While the wallet generator
does not save the keys it generates, this ensures there is no risk of a virus
or similar capturing the key from memory or interfering with its generation.

##  Release Notes


## Bitcoin Core Release Notes

Full release notes for Bitcoin Core 0.9.3 are available from
https://bitcoin.org/en/release/v0.9.3 , but highlights are included
below:


* Better orphan transaction handling
* Add -maxorphantx=<n> and -maxorphanblocks=<n> options for control over the maximum orphan transactions and blocks
* key.cpp: fail with a friendlier message on missing ssl EC support
* Upgrade OpenSSL to 1.0.1i (see https://www.openssl.org/news/secadv_20140806.txt - just to be sure, no critical issues for Bitcoin Core)

## Known Issues

There is a known issue with quantities in JSON used by the RPC interface (https://github.com/nyancoin/nyancoin/issues/134).
This stems from the use of a floating point precision number to hold fixed precision data.
Currently looking at solutions, which primarily either involve JSON which is not compliant
to the specification, or making the RPC layer slightly incompatible with the conventional
Bitcoin API.

This only affects values significantly below 1 Doge, however merchants may wish to round to
the nearest Doge on all transactions for now.

## Credits

Listed in strictly alphabetical order, using name listed in Github. This
includes those whose contributions to Bitcoin Core have been merged
into Nyancoin Core:

* Abderraouf Adjal
* Andymeows
* Daniel Kraft
* Gavin Andresen
* langerhans
* michilumin
* nameEO
* Patrick Lodder
* Ross Nicoll
* Wladimir J. van der Laan
* Whit J
