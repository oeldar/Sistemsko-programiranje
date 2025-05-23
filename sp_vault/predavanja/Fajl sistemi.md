
- Bitan dio racunara jesu uredjaji koji trajno pohranjuju podatke. Potreban nam je uredjaj koji ce i kad nema napajanja, cuvat nase podatke u racunaru. Takvi uredjaji se zovu **diskovi**.
- Kada kazemo memorija tada mislimo na RAM, trenutnu memoriju koja ne cuva podatke kada se izgubi napajanje ali koja je znacajno brza od diskova. Diskovi opet sa druge strane imaju mnogo veci kapacitet od RAMa ali je glavna mana to sto su mnogo spori u citanju i pisanju podataka.

- Operativni sistemi u posebnoj komponenti koja se zove **fajl sistem** implementiraju sve funkcije za transfer podataka izmedju memorije i diskova. Dakle, fajl sistem je poseban dio operativnog sistema koji ima zadatak da:
	- organizira i pohranjuju podatke
	- omoguce dijeljenje podataka izmedju aplikacija i korisnika
	- cuvaju podatke nakon gasenja sistema.

- Da bi fajl sistem mogao da obavlja te zadatke, moramo se dogovoriti da podaci na disku budu spremljeni u tacno odredjenom formatu. Razliciti fajl sistemi na razlicite nacine organiziraju podatke na disku. Mi cemo analizirati XV6 fajl sistem.

>[!info] Mediji za trajnu pohranu podataka
>- Spram tehnologije kojom su implementirani, danas postoje razliciti tipovi diskova npr. had diskovi (koriste magnetizam), SSD (koriste elektricitet), flash diskovi itd.
>- Zajednicko za sve ove diskove, bez obzira na tehnologiju koja se koristi, jeste da citaju i pisu podatke u blokovima bita. Diskovi su podijeljeni na niz blokova bita koje zovemo **sektori**. Standardna velicina sektora je 512 B. Danas su velicine tih sektora i 4 kB.
>- Sektori se adresiraju od nulte adrese.
>- Razliciti diskovi imaju razlicite brzine pristupa i transfera podataka iz pojedinacnih sektora ali je sustina da je i najbrzi disk nekoliko magnituda sporiji od RAMa.
>>[!abstract] Sustina
>>Sustina je da ovi diskovi podrzavaju dvije kljucne operacije:
>>- procitaj $k$-ti sektor - u tom slucaju se transfer vrsi sa diska u RAM
>>- snimi $k$-ti sektor - u tom slucaju se transfer vrsi iz RAMa na disk.

- Eh sad, podaci snimljeni na disk nisu nam od neke koristi ako nemamo metodologiju kako da ih pronadjemo i kako da ih onda i procitamo. Zbog tog, fajl sistemi odredjeni dio diska (neke njegove sektore) koriste za snimanje informacija kojim se opisuje organizacija sadrzaja snimljenog na disku. Isto tako, moramo dio gdje cemo cuvati informaciju koji sektor je zauzet a koji nije.

--- 

- Osnovna jedinica za pohranu podataka u sistemu je **fajl**.
- Radi lakseg pristupa za korisnike, fajlove organiziramo u **direktorije** koji mogu da sadrze fajlove i druge direktorije. Primijetimo da ovako formiramo strukturu stabla.
- Fajlovima i direktorijima dajemo imena koja krajnji korisnik koristi da bi pristupio podacima pohranjenim u tim fajlovima. To se ostvaruje kroz niz sistemskih poziva.
- Dakle, informacije proizvedene unutar nekog procesa cuvamo u logickim jedinicama koje zovemo fajlovi. Bitno je da fajlovi za krajnjeg korisnika apstrahuju komunikaciju sa diskovima. Krajnjeg korisnika ne zanima kako pristupiti nekom fajlu na disku. Osim toga, fajlovi su organizirani i upravljani od strane OSa.
- OS vidi fajl kao nestruktuiranu kontinualnu sekvencu bajta koju on moze mijenjati, prosirivati, brisati i trajno pohraniti na disku.
- OS naravno mora voditi racuna u kojim sektorima se nalaze podaci koji pripadaju odredjenom fajlu i o slobodnim sektorima na disku.

---

- xv6 oraganizira disk (fajl sistem) na 6 sekcija koje zovemo blokovi:
![[Pasted image 20250316154038.png]]

- **Boot blok** - to je prvi sektor (prvih 512 B na disku). Da bi se disk mogao koristiti za pokretanje operativnog sistema, u prvom sektoru mora imati poseban program koji ima mogucnost da u RAM ucita kernel koji je negdje na tom disku. Prisjetimo se da je BIOS sa tog diska na neku lokaciju u RAM ucitao sadrzaj prvog sektora i postavio vrijednost programskog brojaca na tu lokaciju kako bi se to pocelo izvrsavat. Taj program je bootloader.
- **Super blok** - sadrzi samo jedan sektor koji dolazi odmah nakon nultog sektora u kojem je boot blok. U ovom bloku se nalaze informacije o ostalim sekcijama. Te informacije su pohranjene u strukturi `struct superblock`.
- **Inode blokovi** - Niz struktura `struct dinode` . Jedan inode predstavlja jedan fajl ili direktorij u fajl sistemu. U toj strukturi cuvamo informacije o inodeima. Izmedju ostalog, biljezimo velicinu fajla i sektore u kojima su pohranjeni podaci tog fajla.
- **Bitmap blokovi** - svaki bit u ovoj sekciji odgovara jednom sektoru na disku i govori da li je taj sektor slobodan ili nije.
- **Data blokovi** - u ovim sektorima su podaci koji pripadaju razlicitim fajlovima.
- **Log blokovi** - zanemarimo za sada.

## Super blok
- Rekli smo da je u sekciji super blok samo jedna struktura cija je definicija
```c
struct superblock{
	uint size;
	uint nblocks;
	uint ninodes;
	uint nlog;
}
```
- `size` predstavlja ukupan broj sektora na disku (ako npr. imamo 3 sektora po 512 B, onda imamo ukupno 3 * 512 B = 1536 B diska)
- `nblocks` predstavlja koliko sektora ima u data sekciji tj. koliko sektora na disku smo ostavili za podatke same po sebi.
- `ninodes` predstavlja koliko elemenata tipa `dinode` imamo u nizu `dinode`-a a taj niz je u sustini sekcija oznacena kao inode blokovi.
	- Primijetimo da jedan inode zauzima `sizeof(struct dinode)` bajta. Dakle, ako ih imamo `ninodes`, tada tih `ninodes` inodea zauzima `ninodes * sizeof(struct dinode)` bajta. Ako nam je jedan sektor 512 B, onda nam u inode blokovi sekciji ima `ninodes * sizeof(struct dinode) / 512` sektora.

# Inode blokovi
- Dakle, razmisljajmo za sad da je svaki fajl i svaki direktorij asociran sa jednim inodeom.
- Sadrzaj fajla je pohranjen u onoj data sekciji, u nekim sektorima te sekcije.
- Inode blok sekcija se sastoji od niza inodea.
- Svaki inode je modelovan stukturom
```c
struct dinode {
	short type;
	short major;
	short minor;
	short nlink;
	uint size;
	uint addrs[NDIRECT + 1];
};
#define NDIRECT 12
```

- `type` - tip inodea. Moze biti inicijaliziran sa
	- `T_DIR -> 1` - inode pokazuje na direktorij
	- `T_FILE -> 2` - inode pokazuje na fajl
	- `T_DEV -> 3` - inode pokazuje na uredjaj
	- `0` - element je slobodan
- `major`, `minor` - samo ako je element uredjaj
- `nlink` - broj referenci na dati element
- `size` - kolicina sadrzaja asocirana s tim inodeom u B
- `addrs` - niz u kojem se nalaze adrese sektora na disku u kojima je sadrzaj elementa

# Direktoriji
- Direktoriji formiraju infrastrukturu za imenovanje fajlova.
- Oni asociraju korisnicki definisana imena fajlova sa inodeima u fajl sistemu.
- Direktorij je i sam inode tipa `T_DIR` ciji je sadrzaj sekvenca parova koje zovemo hard linkovi `(korisnicko ime, broj inode-a)`.
- Dakle, svaki `dinode` element tipa `T_DIR` predstavlja jedan direktorij. Sadrzaj tog direktorija je u onoj data sekciji snimljen kao niz elemenata tipa `dirent`:
```c
struct dirent {
	ushort inum;
	char name[DIRSIZ];
}
#define DIRSIZ 14
```
- Dakle ova struktura je ono sto u sustini mi zovemo hard link.
- `inum` je broj inodea
- `name` je ime odabranog inodea tj. inodea rednog broja `inum`.

>[!abstract] Dakle
>- Svaki fajl ima sa sobom asociran inode.
>- Inode je modeliran strukturom `dinode` u kojoj izmedju ostalog ima i polje `addrs` koje je niz u kojem se nalaze adrese sektora u kojima je sadrzaj fajla asociranog sa posmatranim inodeom.
>- Ako je polje `type` u inodeu postavljeno na `T_DIR`, tada onaj `addrs` sadrzi adresu sektora u data sekciji unutar koje je niz struktura `dirent`
>- Taj `dirent` predstavlja jedan hard link `(ime, broj inodea)` 
>>[!info] Gdje je u strukturi `dinode` ime fajla s kojim je taj inode asociran?
>>Pa nema ga. Svaki fajl mora biti unutar nekog direktorija. A ako je neki element direktorij, tada `inode` asociran s tim direktorijem u svom polju `addrs` cuva adrese sektora u kojima je niz hard linkova modeliranih strukturom `dirent`. E u toj strukturi `dirent` imamo polja `inum` i `name`. 
>>- Recimo da imamo neku sliku na disku koja je asocirana sa 23. inodeom u nizu inodea iz inode sekcije. Na koji nacin joj mozemo dat ime `foo.png`? Pa ona za pocetak mora biti u nekom direktoriju. Dakle, postoji inode tipa `T_DIR`. Njegovo polje `addrs` sadrzi adrese sektora u kojima je sadrzaj asociran sa tim inodeom u data sekciji. Taj sadrzaj nije nista drugo nego niz hard linkova `(ime, broj_inodea)` modeliran preko strukture `dirent`. Dakle, negdje u tom nizu cemo imati objekat tipa `dirent` sadrzaja `('foo.png', 23)`.

- Za direktorij kazemo da sadrzi sve elemente fajl sistema koji su u njegovom nizu `dirent` struktura. Svi elementi sadrzani unutar istog direktorija moraju biti imenovani razlicito.
- Kad god se u nekom direktoriji referencira neki inode, xv6 inkrementira polje `nlink` u `dinode` strukturi tog elementa.
- Kad se desi da je polje `nlink` nekog inodea dostiglo vrijednost `0`, xv6 uklanja sav sadrzaj na disku asociran s datim elementom i onda se taj element u bitmap sekciji oznacava kao slobodan. Dakle, da bi se element nalazio u fajl sistemu, on mora biti referenciran bar u jednom direktoriju.

- Primijetimo da direktoriji organiziraju cijeli fajl sistem u strukturu stabla.
- Na vrhu tog stabla je root inode imena `/` i rednog broja `1`.
- Kada pretrazujemo neki fajl, stablo se u pretrazi moze referencirati apsolutnom putanjom spram root inodea npr. `/home/eldar/foo.png`
- Ili relativnom putanjom spram inodea asociranim s poljem `cwd` strukture `proc` trenutnog procesa. Npr. `eldar/foo.png`. Pretpostavka je da je `cwd` polje trenutnog procesa `/home`

- Svaki direktorij sadrzi minimalno dva para:
	- `.` - strukturu `dirent` cije polje `name` ima vrijednost `'.'` putem koje dati direktorij ima referencu na svoj `dinode` element u fajl sistemu
	- `..` - strukturu `dirent` cije polje `name` ima vrijednost `'..'` putem koje dati direktorij ima referencu na `dinode` element od direktorija u kojem se trenutno nalazi.

>[!abstract] Root direktorij
>- Rekli smo da je u inode bloku niz `dinode` struktura koje modeliraju jedan inode.
>- Nulti element (inode) u tom nizu ima posebne osobine:
>	- uvijek je prisutan u fajl sistemu
>	- uvijek je tipa `T_DIR` i mora da sadrzi elemente `.` i `..` koji referenciraju `dinode` element pod opet rednim brojem `1`. Malo pojasnjenje. Kad kazem sadrzi elemente tada podrazumijevam sljedece. Prvi inode u nizu inodea je poseban. On u svom clanu `addrs` sadrzi adresu sektora u data sekciji na kojem se nalazi niz `dirent` struktura. U tom nizu se obavezno uvijek nalaze minimalno dva elementa tipa `dirent` i to oni sa `uname = '.'` i `uname = '..'`. Za ovaj prvi inode, `inum` je postavljeno na `1` dakle gledaju na sam inode koji je referencirao ovaj poseban direktorij.
>	- dakle, ovo je jedini direktorij koji nije sadrzan u nekom drugom direktoriju.
>	- zovemo ga `root` jer predstavlja vrh stabla fajl sistema. To stablo je formirano zahvaljujuci do sad opisanoj organizaciji sa inodeima i direntima.

>[!info] Koliki fajlovi na xv6 mogu biti?
>- Rekli smo da informacije o fajlu cuvamo u `dinode` strukturi koja modelira inode. Jedno od polja u `dinode` strukturi je `uint addrs[NDIRECT + 1]`. 
>- To je niz od 13 `uint`-ova jer je `#define NDIRECT 12`. 
>- Dakle, svaki inode sadrzi 13 pointera.
>- Prvih dvanaest adresa iz niza `addrs` pokazuju na lokacije sektora u kojima je pohranjeno prvih 512 B fajla. 
>- Kada bismo samo ovo imali na raspolaganju, fajl bi mogao biti velik maksimalno `7 * 512 B`.
>- Zadnji element u nizu `addrs` cuva adresu sektora u data sekciji u kojoj se nalazi niz pointera na naredne data blokove datog fajla.
>- Ako nam je sektor velik 512 B a svaki `uint` zauzima 4 B, znaci da u jednom sektoru mozemo imati 512/4 = 128 pointera.
>- Tih 128 pointera dalje pokazuje na nekih 128 sektora po 512 B pa dolazimo do maksimalne velicine fajla od `12*512 + 128*512 = 71680 B = 70 kB`.

