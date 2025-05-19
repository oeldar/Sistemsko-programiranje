- Korisna stvar u programiranju je kompozicija.
- To podrazumijeva sljedece:
	- Ideja je da imam vise razlicitih koji su namijenjeni da rade samo jednu stvar.
	- Zelim da ostvarim komunikaciju izmedju tih programcica tj. da ih **komponujem** kako bih rijesio neki komplikovaniji problem.
	- Dakle, potrebna nam je metodologija kako ostvariti kompoziciju na unixu tj. kako ostvariti komunikaciju izmedju procesa na unix sistemima.
- Svaki proces ima svoj adresni prostor i izvrsava se nezavisno od drugog procesa.
- Kljucni problem koji trebamo da rijesimo jeste da ostvarimo komunikaciju dva procesa koji imaju separatne adresne prostore.
- Postoje dvije filozofije u pristupu tom problemu:
	- Prva ideja je da se dijelovi dva (ili vise) adresna prostora mapiraju u isti komad fizickog adresnog prostora. Dakle, dva procesa manipulisu istim fizickim okvirom. Ovo potencijalno moze stvoriti veliki problem a to je race condition koje rjesavamo koristenjem sinhronizacijskih primitiva npr. mutex. Ukratko, ova filozofija kaze sljedece: dva procesa imaju zajednicki resurs kojeg dijele i preko kojeg komuniciraju. To dovodi do race conditiona.
	- Druga filozofija je ona koju cemo mi prihvatiti. Komunikacija se ostvaruje razmijenom poruka izmedju vise adresnih prostora. Ova metodologija ima losije performanse ali smanjuje broj potencijalnih bugova jer eliminise problem race conditiona. Dakle, umjesto da procesi dijele nesto, oni komuniciraju tako sto razmjenjuju poruke.
- Dakle, mi cemo koristiti ovu drugu filozofiju a to je ostvarivanje komunikacije izmedju procesa transportom poruka izmedju procesa.

---

- Unix kernel koristi razlicite primitive kako bi ostvario ovakvu komunikaciju. Neki od njih su
	- `pipe`, `fifo` - primitiv koji omogucava razmijenu poruka izmedju dva procesa na istoj masini
	- `socket` - primitiv koji omogucava razmijenu poruka izmedju dva procesa koji nisu na istoj masini
	- `signal` - ono o cemu smo vec pricali
- Mi cemo se fokusirati na `pipe` i `fifo`.

---

# Pipe

- Pipe (cijev) omogucava polu dupleks razmjenu podataka izmedju procesa u obliku streama bajta. To znaci da jedan proces moze samo da salje informacije a drugi proces samo da prima informacije. Druga strana nema mogucnost da prvoj strani salje neke podatke. Komunikacija je uvijek jednostrana.
- Pipe se kreira koristenjem sistemskog poziva `int pipe(int fd[2])`.
- Ideja je da se u kernelu kreira cijev.
- Proces iz kojeg smo kreirali pipe, u svojoj tabeli fajl deskriptora ce rezervisati prva dva slobodna fajl deskriptora:
	- jedan fajl deskriptor ce biti konektovan na stranu cijevi u koju mozemo pisati
	- drugi ce biti konektovan na drugu stranu cijevi iz koje mozemo da citamo

>[!abstract] `pipe()`
>Posmatrajmo neki proces. U njemu imamo fajl deskriptore `0, 1` i `2` koji su konektovani na kontrolni terminal.
>- Kreirajmo niz `int fds[2]` od dva `int`-a.
>- Ako pozovemo `pipe(fds)`, kreirat ce se pipe a u niz `fds` ce biti prva dva slobodna file deskriptora tj. `fds = [3, 4]`.
>- Dakle, `fd[0]` ce biti fajl deskriptor asociran sa read krajem cijevi a `fd[1]` ce biti konektovan sa write krajem cijevi.
>- Primijetimo da jos uvijek u prici imamo samo jedan proces. Nemamo nikakvu komunikaciju izmedju dva procesa i ovo je u ovom trenutku potpuno beskorisno.

- Da bismo mogli pricati sa drugim procesom preko pipea, proces koji je kreirao pipe mora da se forka. Tada cemo moci uspostaviti komunikaciju tog procesa sa njegovim childom. Dakle, pomocu pipea mogu pricati samo procesi koji su u istoj naslijednoj hijerarhiji. Drugim rijecima, ako imamo dva procesa A i B pri cemu niti je A parent od B, niti je B parent od A, niti imaju zajednickog parenta, preko pipea ovi procesi ne mogu komunicirati.
- Primijetimo da je ovo moguce zbog cinjenice da se forkom fajl deskriptori parenta kopiraju u child.

>[!abstract] Dakle,
>Pretpostavimo da imamo proces A.
>- U procesu A imamo fajl deskriptore `0, 1` i `2` konektovane na kontrolni terminal.
>- Neka je u A pozvan sistemski poziv `pipe(fds)`.
>- Sada A ima jos dva fajl deskriptora `fds[0] == 3` i `fds[1] == 4` pri cemu je trojka konektovana na read stranu pipea kreiranog u kernelu a cetvorka na write stranu tog pipea.
>- Ako se sada proces A forka i nastane njegov child proces B, proces B ce naslijediti sve od procesa A pa tako i fajl deskriptor tabelu.
>- Tj. proces B ce imati iste fajl deskriptore `0, 1, 2, 3` i `4` konektovane na iste entitete kao i za proces A. Ono sto je nama znacajno jeste da i A i B sada imaju fajl deskriptore povezane sa istim pipeom u kernelu.


---

- Kada napravimo cijev u kernelu, ona ima ogranicenu velicinu. Broj bajta koji stane u pipe je `PIPE_BUF`.
- Ako proces A pise u pipe, on puni taj pipe. Ako upise `PIPE_BUF` bajta u njega, cijev postane puna. Ako onda taj proces pokusa upisati u napunjenu cijev, proces A ce biti blokiran.
- Pipe iz kernela nestaje tek kada svi fajl deskriptori asocirani sa tim pipeom budu zatvoreni koristenjem `close` sistemskog poziva. Ako smo otvorili pipe u procesu A, forkali se, pipe nece biti dealociran samo ako zatvorimo fajl deskriptore asocirane s tim pipeom u procesu A nego to moramo odraditi i u procesu koji je nastao forkanjem od A.
- Ako nemamo niti jedan fajl deskriptor asociran sa read stranom pipea a pokusamo upisati u pipe, tada ce biti generisan signal `SIGPIPE` a `write` ce vratiti `-1` i postaviti `errno` na `EPIPE`. Dakle, imam dva nacina da ja kao pisac znam da nema druge strane na pipeu koja cita: ako mi `write` vrati `-1` i postavi `errno` na `EPIPE`; ili ako se generise signal `SIGPIPE`. Akcija za signa `SIGPIPE` je terminiranje, dakle proces koji je pokusao da pise u pipe u kojem nema druge strane ce biti terminiran.
- Citanje iz prazne cijevi blokira. Dakle, ako neki proces uradi `read` sa `fds[0]` a pipe je prazan, proces ce blokirat sve dok u cijevi ne bude bar jedan bajt. Ako niko nema konektovan na write strani pipea, ovaj drugi ce biti blokiran zauvijek.

---

- Dakle, potrebna nam je i metodologija kako da onaj koji pise u pipe zna da nema nekog ko ce citat; i kako da onaj koji cita iz pipe zna da nema nekog ko ce pisati u njega.


---

- Podsjetimo se najprije sta radi sistemski poziv `dup2(oldfd, newfd)`.
- Najkrace receno nakon izvrsavanja ove linije, na koji god fajl da je pokazivo `newfd`, od sad ce pokazivat na ono na sta pokazuje `oldfd`.
```c
int fd1 = open("fajl1.txt", ...);
int fd2 = open("fajl2.txt", ...);

// fd1 = 3 -> fajl1.txt
// fd2 = 4 -> fajl2.txt

dup2(fd1, fd2);
// fd1 = 3 -> fajl1.txt
// fd2 = 4 -> fajl1.txt
```

- Ovo mozemo da koristimo ako hocemo da recimo standardni izlaz preusmjerimo u neki fajl tj. kad god proces nesto hoce da isprinta na kontrolni terminal, to nece biti na terminalu nego u fajlu

```c
int fd = open("fajl.txt", ...);
dup2(fd, STDOUT_FILENO);
printf("Hej kako si?\n");

// "Hej kako si?\n" se nece prikazat u terminalu nego ce biti upisano u fajl "fajl.txt"
```

- Primijetimo da mi sa `dup2(fd1, fd2)` nismo promijenili vrijednosti intova `fd1` i `fd2`. Ako je `fd2` bio deskriptor sa brojem `4`, on ce i dalje biti `4`, samo ce fajl na koji on pokazuje biti promijenjen i to na onaj fajl na koji pokazuje `fd1`.

**Pogledaj na slajdu kako se sad pomocu `pipe` primitiva pravi filtriranje u bashu tj. `|`**

---

# FIFO
- Vidjeli smo da bismo ostvarili komunikaciju izmedju procesa, mozemo koristiti pipe.
- Medjutim, problem je u tome sto ti procesi koji onda na taj nacin mogu da komuniciraju, moraju biti u istoj naslijednoj hijerarhiji tj. ili moraju biti siblings ili jedan mora drugom biti parent. To je zbog cinjenice da forkanjem nekog procesa njegov child proces naslijedjuje njegovu `proc` strukturu a time i fajl deskriptor tabelu parenta.
- Jos jedna mana je ta sto je pipo komunikacija jednosmjerna.