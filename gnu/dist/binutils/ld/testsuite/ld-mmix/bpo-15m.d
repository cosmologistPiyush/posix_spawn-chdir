#source: start.s
#source: bpo-7.s
#as: -linker-allocated-gregs
#ld: -m mmo
#error: ^[^c][^h][^i][^l][^d].* undefined reference to `areg'$

# A BPO against an undefined symbol.
