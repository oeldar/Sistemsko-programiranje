- `ls` - izlista sadrzaj trenutnog direktorija. Skriveni folderi su nevidljivi.
- `ls -l` - izlista sadrzaj trenutno direktorija u formatu liste sa dodatnim informacijama koje su formata
	```sh
	-rw-rw-r--  1 eldar eldar 853937 Mar 16 23:25  Diplomski.pdf 
	```
	
	- **`-rw-rw-r--`** → Dozvole za fajl:
	    - **`-`** → Objekat je fajl (`d` bi značilo da je direktorijum).
	    - **`rw-`** → Vlasnik (`eldar`) ima **čitati (r)** i **pisati (w)** prava, ali ne i izvršiti (`-`).
	    - **`rw-`** → Grupa (`eldar`) ima ista prava.
	    - **`r--`** → Ostali korisnici mogu samo čitati fajl.
	- **`1`** → Broj hard linkova ka fajlu.
	- **`eldar`** → Vlasnik fajla.
	- **`eldar`** → Grupa kojoj fajl pripada.
	- **`853937`** → Veličina fajla u bajtovima (≈ 853 KB).
	- **`Mar 16 23:25`** → Datum i vreme poslednje modifikacije fajla.
	- **`Diplomski.pdf`** → Naziv fajla.

- `ls -lh` - isti ispis kao i gore samo koristi human readable format za velicinu fajla, pa ce nam umjesto `853937` pisati `834K`.
- `ls -lhi` - gornjim podacima dodaje i broj inodea asociranog sa tim fajlom.
- `ls -lhia` - dodatni switch `-a` omogucava da se prikazu i skriveni fajlovi i direktoriji. To su oni cije ime pocinje sa `.`
---
- `mkdir ime` - kreira direktorij naziva `ime` u direktoriju iz kojeg je pokrenuta komanda `mkdir`. 
- `touch ime` - kreira fajl naziva `ime` u direktoriju iz kojeg je pokrenuta komanda `touch`.

>[!info] Primjedba
> Pretpostavimo da se nalazimo u direktoriju `/home/eldar/Desktop/proba` koji je za pocetak prazan odnosno sadrzi samo `.` i `..` . Kreirajmo u tom direktoriju novi direktorij imena `subdir` komandom `mkdir subdir`. Kreirajmo i fajl `foo.txt` komandom `touch foo.txt`. Ako izlistamo sadrzaj direktorija komandom `ls -lhi` vidjet cemo
>```sh
>30034744 -rw-rw-r-- 1 eldar eldar    0 Mar 17 22:47 foo.txt
>30034742 drwxrwxr-x 2 eldar eldar 4.0K Mar 17 22:47 subdir
>```
>1. Primijetimo da je velicina fajla `foo.txt` 0 B a direktorija 4 KB iako je direktorij prazan. To je zato jer kad kreiramo direktorij, za njega se odmah odvoji jedan sektor diska. Na nasem sistemu je velicina sektora 4 KB. U taj sektor se onda smjesti niz `dirent` struktura iako one nisu inicijalizirane. Zbog toga je velicina praznog direktorija 4 KB.
>2. Primijetimo da je i broj linkova na `subdir` postavljen na 2. To je zbog toga jer u ovom trenutnom `/home/eldar/Desktop/proba` folderu imamo link na `subdir` a i u `subdir`-u imamo link `.` koji pokazuje na samoag sebe tj. `subdir`.

---
- `ln postojeci_fajl ime_linka` - kreira hard link na vec postojeci fajl. Hard link je u sustini samo drugo ime za vec postojeci fajl tj. broj inodea kreiranog hard linka je isto kao i broj inodea za fajl na kojeg je hard link kreiran.

>[!info] Primjer
>Preptostavimo da u folderu `proba` imamo fajl `foo.txt` sa brojem inodea 45. Ako kazemo `ln foo.txt hlink` tada smo kreirali novi hard link (razmisljaj: u niz `dirent` struktura asociran s trenutnim direktorijem, dodali smo novu `dirent` strukturu sa postavljenim poljima `inum` na 45, `name` na `'hlink'`). Dakle kada kazemo `ls -lhi` dobit cemo nesto poput:
>```sh
>45 -rw-rw-r-- 2 eldar eldar    0 Mar 17 22:47 foo.txt
>45 -rw-rw-r-- 2 eldar eldar    0 Mar 17 22:47 hlink
>```
>Primijetimo da je sada broj referenci povecan na 2 jer imamo dva hard linka koji pokazuju na isti fajl.
>Sustina je da je `hlink` samo drugi naziv za vec postojeci inode. I `hlink` i `foo.txt` svojim poljem `addrs` iz `dinode` strukture pokazuju na iste sektore data sekcije u kojoj je sadrzaj tog fajla. Kad bismo obrisali `foo.txt`, i dalje bismo mogli pristupit sadrzaju fajla preko hard linka `hlink`.
>U sustini, nije kreiran novi link nego je kreirano novo ime za vec postojeci inode preko nove `dirent` strukture. `foo.txt` i `hlink` su inodei koji pokazuju na isti fajl u fajl sistemu.

>[!info] Hard link na direktorij
>Ne mozemo praviti hard link na direktorij. Kad kreiramo hard link, novi fajl dobija isti inode kao originalni fajl. Ta dva fajla dijele isti zapis u data sekciji. Kad bismo mogli kreirat hard link na direktorij, narusili bismo hijerarhijsku strukturu fajl sistema. Kad bismo to mogli, kreirali bismo ciklicke strukture u fajl sistemu i onda bismo potencijalno mogli upasti u beskonacnu petlju.

- `rm fajl` - brise fajl tacnije brise link imena `fajl`

>[!info] Brisanje
>Recimo da u direktoriju `proba` imamo fajl `foo.txt` i imamo hard link `hlink` na ovaj fajl. Dakle, broj referenci u inode strukturi `nlink` je postavljen na 2 i to vidimo pomocu `ls`-a.
>Ako obrisemo npr. `foo.txt` sa `rm foo.txt`, tada ce broj linkova biti postavljen na 1. Istom fajlu jos uvijek mozemo pristupiti preko linka `hlink`. Kad bismo i njega izbrisali, polje `nlink` u tom inodeu bi bilo postavljeno na 0. To je indikacija OSu da je taj fajl izbrisan i vise mu nemamo pristup. Medjutim, sektor u kojem je bio sadrzaj fajla je nepromijenjen. Svi podaci tog fajla su i dalje tu. Jedino sta je promijenjeno jeste da je bit u bitmap sekciji asociran sa tim sektorom u data sekciji postavljen na 0 sto OSu daje informaciju da je taj sektor slobodan i OS ce ga u buducnosti moci koristiti da u njega upise neke druge podatke. Tek tada, kada prvobitni fajl bude prepisan, on je u potpunosti izgubljen.

---
- `ln -s file slink` - kreira simbolicki link za vec postojeci fajl `file`

>[!info] Simbolicki link
>Simbolicki link je poseban fajl koji sluzi kao referenca na drugi fajl ili direktorij. On je dakle, novi inode, ciji sadrzaj nije sadrzaj fajla na kojeg je link iz data sekcije nego referenca na fajl na kojeg je on link. Simbolicki link mozemo prepoznati tako sto umjesto `-` ili `d` kod informacija o fajlu ima `l`.

>[!info] Primjer
>Recimo da u direktoriju `/home/eldar/proba` imamo fajl `foo.txt`. Naredbom `ls -s foo.txt slink` cemo kreirati novi fajl imena `slink` koji predstavlja soft link na fajl `foo.txt`.  Ako bismo sada rekli `ls -lhi` dobili bismo output
>```sh
>30034744 -rw-rw-r-- 1 eldar eldar    4 Mar 18 21:51 foo.txt
30034745 lrwxrwxrwx 1 eldar eldar    7 Mar 18 21:51 slink -> foo.txt
>```
>Primijetimo da `slink` ima drugi broj inodea i da je tipa `l` dok je `foo.txt` tipa `-` tj. fajl je. Primijetimo da se i broj referenci na fajl `foo.txt` nije povecao kreiranjem soft linka na njega.
>Sustina je da je sadrzaj fajla `foo.txt` neki niz znakova pohranjen u nekom sektoru data sekcije a gdje, govori polje `addrs` u `dinode` strukturi asociranoj sa fajlom `foo.txt`. 
>Sadrzaj fajla `slink` nije sadrzaj fajla `foo.txt`.  Eh sta je sadrzaj fajla `slink` tj. na koje sektore pokazuje polje `addrs` od inodea asociranog sa fajlom `slink`?
>
>Pa sadrzaj fajla `slink` je **putanja** do fajla kojeg targetira taj `slink`. Zasto onda kada kazemo `cat slink` dobijamo ispis onoga sto pise u fajlu `foo.txt` a ne putanju `/home/eldar/proba/foo.txt`? Pa to je zato sto ako programu `cat` proslijedimo simbolicki link, on ce ispisati sadrzaj fajla kojeg taj link targetira. Ako me zanima na sta `slink` targetira, mogu to vidjet pomocu `ls -lhi` ili mogu pozvati `readlink slink` koji ce mi za output dati ime fajla na kojeg `slink` pokazuje.
>
>Kada bismo izbrisali fajl `foo.txt` sa `rm foo.txt` npr., nas fajl `slink` bi i dalje postojao. Njegov sadrzaj je `"foo.txt"` a taj fajl nam vise ne postoji. Tada soft link postane nevalidan. Ako bismo rekli `cat slink`, program `cat` bi procitao sadrzaj fajla `slink` a to je `"foo.txt"` i pokusao da `cat`-a taj fajl, medjutim on ne postoji pa dobijamo `cat: slink: No such file or directory` sto i ima smisla.

# Permisije

>[!info] Permisije
>Kada kazemo `ls -lhi` dobijemo output poput:
>```sh
>30034744 -rw-rw-r-- 1 eldar eldar    2 Mar 20 13:08 foo.txt
30034742 drwxrwxr-x 2 eldar eldar 4.0K Mar 17 22:47 subdir
>```
>Sta nam znaci ovo `-rw-rw-r--` i `drwxrwxr-x`?
>- Prvi karakter se odnosi na tip fajla:
>	- `-` - obicni fajl
>	- `d` - direktorij
>	- `l` - simbolicki link
>	- `p` - pipe
>	- `f` - fifo
>	- `c` - karakter uredjaj
>	- `b` -blok uredjaj
>	- `s` - socket
>- Slijedi niz od 9 karaktera formata `rwx rwx rwx`. To su permisije.
>	- Prvi skup permisija se odnosi na vlasnika fajla. Tako ako je npr taj dio `rwx` vlasnik fajla onda ima permisiju da cita, pise i da izvrsava fajl. Ako se na mjestu bilo kojeg slova nalazi `-`, onda se ta permisija nema. Tako npr. `r-x` znaci da korisnik ima permisiju da cita i da izvrsava fajl ali nema permisiju da pise u njega.
>	- Drugi skup permisija se odnosi na grupu koju pripada vlasnik fajla. To znaci da svi useri koji su u istoj grupi kao i vlasnik faja imaju te permisije.
>	- Treci skup permisija se odnosi na sve ostale usere.
>>[!abstract] Konkretno
>>U nasem gornjem primjeru imali smo `-rw-rw-r--` sto prvo znaci da se radi o obicnom fajlu (`-`), zatim imamo `rw-`, `rw-` i `r--` sto znaci da vlasnik (`eldar`) ima pravo da cita i pise u fajl ali nema dozvolu da ga izvrsava. Isto tako i svi clanovi grupe `eldar` kojoj user `eldar` pripada. Svi ostali useri (oni koji ne pripadaju grupi `eldar` i nisu `eldar`), imaju dozvolu da samo citaju fajl.

>[!info] Znacenje permisija za direktorij
>Ako se radi o direktoriju `d`, tada permisije imaju sljedece znacenje
>- `r` - omogucava korisniku da izlista sadrzaj direktorija sa `ls` npr.
>- `w` - omogucava dodavanje i brisanje fajlova i direktorija unutar tog posmatranog direktorija
>- `x` - omogucava ulazak u direktorij sa npr. `cd`. Primijetimo da ako imamo permisiju da udjemo u direktorij, to ne znaci da mozemo i da izlistamo njegov sadrzaj.

>[!info] Permisije kao biti
>Posmatrajmo neki niz permisija `rwxrwxrwx`. Ovo je u racunarima predstavljeno kao trocifren broj (4+2+1)(4+2+1)(4+2+1) gdje broj 4 predstavlja `r`, broj 2 `w` a broj `1` x permisiju pa bi tako vazile ekvivalencije
>- `400` <=> `r-- --- ---`
>- `777` <=> `rwx rwx rwx`
>- `132` <=> `--x -wx -w-`
>
>Kad ja npr. kreiram fajl meni su default permisije `rw-rw-r--` sto znaci da su permisije postavljene na `664`. Za novokreirani direktorij je postavljeno na `775`.
>Kazemo da je default permisija za fajl `666` a za direktorij `777`. Pa kako je onda kod nas `664` i `775`? Pa to je zato jer postoji nesto sto se zove `umask`. To je maska koja se oduzima od default permisija. Kad u terminalu kazemo `umask` dobit cemo cemu je jednaka ta maska. Kod nas ce biti npr. `002` sto znaci da od default permisije za fajl `666` oduzimamo `002` i dobijamo `664`. Analogno i za direktorije.

- `umask` - ova komanda vraca koja je trenutna maska. 
- `umask nova_maska` - ova komanda postavlja trenutnu masku na `nova_maska`

>[!info] Promjena maske
>Rekli smo da su default permisije za fajlove `666`. Ako isprintamo koja nam je trenutna maska sa `umask` komandom, dobit cemo `002`. To znaci da kad kreiramo novi fajl, on ce imati permisije `666 - 002 = 664` <=> `rw-rw-r--`.
>Medjutim, mi ovu masku mozemo promijeniti. Ako komandom `umask 0222` promijenimo masku, sada ce nam novokreirani fajl imati permisije `666 - 222 = 444` <=> `r--r--r--`.
>>[!abstract] Napomena
>>Napomenimo da komandom `umask nova_maska` mijenjamo masku samo privremeno u trenutnoj sesiji shella. Ako to zelimo da uradimo trajno onda koristimo komandu `echo "umask nova_maska" > ~/.bashrc`

Primijetimo da `umask` utice samo na permisije za fajlove koji ce tek biti kreirani. Ako imamo neki fajl sa vec postavljenim permisijama, onda njegove permisije ne mozemo mijenjati pomocu `umask`. Tu u igru dolazi `chmod` komanda.

- `chmod trocifreni_broj ime_fajla` - postavlja fajlu `ime_fajla` permisije na `trocifreni_broj`. Npr. `chmod 227 foo.txt` bi fajlu `foo.txt` permisije postavilo na `277` odnosno `--xrwxrwx`.
- `chmod (u/g/o/a)(+/-)(r/w/x) ime_fajla`:
	- `chmod u+r foo.txt` - vlasniku dodaj `r` permisiju za fajl `foo.txt`
	- `chmod g+w bar.txt` - grupi kojoj pripada vlasnik dodaj `w` permisiju za fajl `bar.txt`
	- `chmod o-x a` - svim korisnicima koji nisu vlasnici ili ne pripadaju grupi kojoj pripada vlasnik, oduzmi `x` permisiju za fajl `a`
	- `chmod a+rw foo` - svima dodaj `r` i `w` permisiju
	- `chmod u+x,g-x,o+rwx foo`


