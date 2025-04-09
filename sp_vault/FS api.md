>[!abstract] Napomene
>`O_SYNC` flag - setuje se na `STDOUT_FILENO` al ne znam zasto. 
>- Vjezbe 36. minuta, `dup` mi nije jasno

# `open()` 

- zivi u `unistd.h`
- Sluzi za otvaranje fajl deskriptora.
- `open(const char* path, int flags, ...)` - vraca int tj. broj kreiranog fajl deskriptora i to onaj najnizi slobodni.
- Prima putanju koja moze biti apsolutna ili relativna i flagove koji mogu biti npr:
	- `O_CREAT` - ako ne postoji fajl sa tom putanjom, onda ga kreira
	- `O_EXCL` - definisano samo ako se koristi sa `O_CREAT`. U tom slucaju, (`O_CREAT | O_EXCL`) osigurava da ce se fajl napraviti ako ne postoji ali da ako postoji i ukljucena su oba ova flaga, onda se desava error sa errno = `EEXIST` indicirajuci da fajl vec postoji pa sistemski poziv `open` vraca `-1`.
	- `O_RDONLY` - otvara fajl samo za citanje
	- `O_WRONLY` - otvara fajl samo za pisanje
	- `O_RDWR` - i jedno i drugo
	- `O_TRUNC` - ako je fajl otvoren za pisanje, ako ima neku od `O_WRONLY` ili `O_RDWR` flagova, onda mu ovaj flag postavlja velicinu na 0 B, tj. brise sav njegov sadrzaj pa se pisanje vrsi od pocetka.
	- `O_APPEND`

# `fcntl()`
- zivi u `fcntl.h`
- omogucava nam da citamo flagove postavljene nad nekim fajl deskriptorom
- `int fcntl(int fd, int op, <arg>)`
- `<arg>` je opcion
- Prima fajl deskriptor i operaciju koja moze biti
	- `F_GETFD` - vraca flagove postavljene nad file deskriptorom `fd`. Ignorise `<arg>`
	- `F_SETFD` - postavlja flagove fajl deskriptora `fd` na `<arg>`
	- `F_DUPDF` - pravi novi fajl deskriptor od postojeceg proslijedjenog koji je veci od `<arg>` i vraca ga

>[!example] `fcntl()`
>Linija 
>```c
>flags = fcntl(STDOUT_FILENO, F_GETFL, 0);
>```
>znaci
>- `STDOUT_FILENO`: fajl deskriptor standardnog izlaza (najčešće 1),
>- `F_GETFL`: tražimo trenutne flagove tog fajl deskriptora,
>- `0`: dodatni argument (nije potreban za `F_GETFL` ali se prosleđuje jer `fcntl` ima promenljiv broj argumenata u zavisnosti od komande).
>  
>Ova linija vraća **flagove koji su trenutno postavljeni** za fajl deskriptor `STDOUT_FILENO`. Ti flagovi mogu uključivati:
>- `O_APPEND`: zapisivanje ide na kraj fajla,
>- `O_NONBLOCK`: fajl se koristi u neblokirajućem modu,
>- `O_RDWR`, `O_WRONLY`, itd.
>  
>  ```c
>int flags = fcntl(STDOUT_FILENO, F_GETFL, 0);
fcntl(STDOUT_FILENO, F_SETFL, flags | O_NONBLOCK);
>  ```
>  Ovo:
>  1. Čita trenutne flagove,
>  2. Doda `O_NONBLOCK`,
>  3. s
>  4. tavi nove flagove nazad.


# `lseek()`

- Pomijera offset na bufferu
- Prima broj file deskriptora, offset i flag