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
>Simbolicki link je poseban fajl koji je asociran sa novim inodeom. Taj inode samo pokazuje na ovaj prvobitni inode. Isto kao da smo napravili precicu do originalnog fajla. 


