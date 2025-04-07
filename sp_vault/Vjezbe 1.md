Neka imamo folder `sp` unutar kojeg se nalaze folderi `subdir` i fajlovi `test1.txt` i `test2.c`.
- `ln test2.c hl` - kreira link naziva `hl` koji ce pokazivat na isto kao i `test2.c`
	- imamo neki prostor u memoriji u kojem je `test2.c`. "*Kreirali*" smo link koji ce na to pokazivati. Kreirali smo novi inode `hl` koji pokazuje na istu lokaciju kao i `test2.c`. Efektivno, nismo ustvari kreirali novi inode. Po broju inodea za `hl` i `test2.c` vidimo da je to isti inode.
- `ln -s test1.txt sl` - kreirali smo soft link (`-s`). On kreira novi inode sto vidimo po broju inodea. Vidimo da nam taj `sl` ima velicinu 9 i vidimo na sta tacno referencira. Broj referenci na taj link a i na fajl na koji je `sl` link je 1.
- `ln sl hl` - kreiranje novog hard linka na vec postojeci link.
- `mv test2.c subdir/test3.c` - uzima ono sto das kao prvi argument `test2.c` i prebaci na putanju koju das kao drugi argument `subdir/test3.c`.
- 