- **Unix sistem** je porodica operativnih sistema zasnovanih na originalnom **Unix** operativnom sistemu razvijenom 1969. godine u **Bell Labs-u** od strane **Kena Thompsona, Denisa Ričija** i drugih istraživača. Unix je postavio temelje modernim operativnim sistemima i danas ima brojne verzije i derivacije.
- Unix kernel stvara jedinstven interfejs i infrastrukturu za
	- proizvoljan broj korisnika
	- autorizaciju i sigurnost
	- paralelizam i koordinaciju (procesi i niti)
	- vezu sa vanjskim uredjajima (fajl sistem)

# Unix korisnici i grupe

- Unix sistem moze imati proizvoljan broj korisnika koji su organizirani u grupe.
- Razlog za postojanje vise korisnika i grupa jeste definisanje vlasnistva nad sistemskim resursima i kontrolisanje permisija kod pristupa ovim resursima.
- Svaki korisnik ima unikatno simbolicko ime (user name), ime grupe kojoj pripada (group name) i brojacani identifikator (uid - id korisnika, gid - id grupe kojoj taj korisnik pripada).
- Osnovne informacije o korisnicima su sadrzane u fajlu `/etc/passwd` a sifre korisnika u enkriptovanom obliku u `/etc/shadow` fajlu koji je readable samo za privilegovane korisnike.
- Svaki korisnik je clan minimalno jedne grupe koja se zove primarna grupa a moze bit clan i sekundarnih grupa. Informacije o postojecim grupama se nalaza u fajlu `/etc/group`.
- Sistem obavezno ima root korisnika i root grupu sa `uid = 0` i `gid = 0`.
- Jednog posebnog usera zovemo *superuser* i on ima posebne privilegije u sistemu. Njegov `uid` i `gid` su postavljeni na `0` i taj korisnik se zove **root user**. Root user na unix sistemima ima sve privilegije i moze pristupati bilo kojem fajlu u sistemu bez obzira na permisije na tom fajlu, salje signale bilo kojem procesu i brise i dodaje druge korisnike itd.

## `etc/passwd` fajl

- Ovaj fajl sadrzi po jednu liniju za svakog korisnika u sistemu.
- Svaka od linija je u formatu
```
username:password:UID:GID:comment:home_directory:shell
```
- `username` - korisnicko ime korisnika
- `password` - enkriptovana sifra. Obicno je postavljeno na `x` iz sigurnosnih razloga jer ovom fajlu moze pristupiti svaki korisnik. Ako je ovo polje prazno, tada za tog korisnika nije potrebno unijeti sifru prilikom prijave.
- `UID` - brojcani identifikator korisnika. Ako ima vrijednost `0`, tada je to root korisniki ima superuser privilegije. Obicno je samo jedan takav korisnik u sistemu i zove se `root`. 
- `GID` - brojcani identifikator grupe kojoj posmatrani korisnik pripada i to njegove prve, primarne grupe. Ostale grupe kojima on pripada se nalaze u fajlu `/etc/group`
- `comment` - opis korisnika (može sadržati puno ime, dodatne informacije, često nije obavezno)
- `home_directory` - putanja do home direktorija tog korisnika. U tom direktoriju se korisnik nalazi nakon prijavljivanja. Ovo polje je u sustini vrijednost `HOME` environment varijable.
- `shell` - podrazumijevani komandni interfejs. To je shell koji se otvara npr. `/bin/bash`.

>[!info] Kod mene
>Kad ja na svom racunaru kazem `cat /etc/passwd` dobijam veliki output ali mogu izmedju ostalog da vidim:
>```
>root:x:0:0:root:/root:/bin/bash
>eldar:x:1000:1000:Eldar:/home/eldar:/bin/bash
>```

## `/etc/group` fajl
- Sadrzi informacije o svim grupama i koji korisnici im pripadaju.
- Jedna linija u ovom fajlu odgovara jednoj grupi.
- Linije su u formatu
```
group_name:password:GID:user1,user2,user3
```
gdje 
- `group_name` predstavlja ime grupe
- `password` sadrzi `x` iz maloprije opisanih razloga
- `GID` jedinstveni brojcani identifikator grupe (0 za `root` grupu, 1000 za prvu korisnicki kreiranu grupu)
- `user1, user2, user3` - lista korisnika koji pripadaju toj grupi

>[!info] Komanda `groups`
>Pomocu komande
>```
>groups user_name
>```
>dobijamo informacije o tome kojih sve grupa je korisnik `user_name` clan. Ako ne proslijedimo argument `user_name`, tada dobijamo informacije o trenutnom useru.


# Unix autentifikacija

- Svaki proces se izvrsava u vlasnistvu nekog korisnika i grupe. Tj. neki korisnik sistema je morao pokrenuti taj proces a korisnik pripada nekoj grupi.
- Dakle, svaki proces sa sobom ima asocirane korisnicke identifikatore (UIDs) i grupne identifikatore (GIDs)
- Ponekad ove identifikatore nazivamo *kredencije procesa*.
- Postoje sljedece kredencije:
	- ruid i rgid (*real user ID i real group ID*)
	- euid i egid (*effective user ID i effective group ID*)
	- suid i sgid (*saved user ID i saved group ID*)

>[!info] Bitno
>Nakon procesa pokretanja sistema koji ide sljedecim nizom
>$$\texttt{bootloader}\to\texttt{kernel}\to\texttt{init}$$
>dalje `init` poziva `login` program. Korisnik se identificira sistemu kroz login program unosenjem username-a i sifre.
>Nakon uspjesnog logiranja, obicno se pokrece shell sa ruid, euid i suid postavljenim na id korisnika koji se autentificirao sistemu.
>
>Napomenimo jos jednom, da su ove kredencije vezane za proces. One postoje da bi se kontrolisalo kako trenutni proces kojeg je pokrenuo neki korisnik moze pristupati resursima tj. koje dozvole ima.

## ruid, rgid
- ovo je uid i gid korisnika koji je kreirao proces. Kad se proces kreira, on nasljedjuje ove identifikatore od svog roditelja.
## euid, egid
- ne kontam ovo
## suid, sgid
- ne kontam ni ovo

# Fajl sistem dozvole
- Svaki inode je u necijem vlasnistvu. Mora postojati korisnik i grupa kojem pripada odredjeni inode.
- 
