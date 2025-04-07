- S bit je razlicito od sticky bit.
- Rekli smo da se u fajlu `/etc/passwd` nalaze svi useri naseg sistema tj. informacije o svakom useru. Po jedna linija za svakog usera formata:
```
username:password:UID:GID:comment:home_directory:shell
```
gdje sifra naravno enkriptovana a najcesce tu stoji x iz sigurnosnih razloga. Sifre se nalaze u `/etc/shaddow` fajlu i to enkriptovane.

- Postoji program koji sluzi za mijenjanje sifre i smjesten je u `/usr/bin/passwd`.  Pokrecemo ga tako sto kazemo samo `passwd` jer nam je `/usr/bin` vec na `$PATH` varijabli.
- Ako pogledamo njegove informacije sa `ll /usr/bin/passwd` vidjet cemo da su postavljene na
$$\texttt{-rwsr-xr-x}$$
- Oke, ocigledno se radi o fajlu zbog `-`. Njegov vlasnik je `root` ali zbog executable permisije za others mi ovaj fajl mozemo pokrenut.
- Primijetimo da nam je sto se tice permisija vlasnika `rws` gdje nam executable permisija stoji `s` umjesto `x`. 
- Ako mi sad u drugoj instanci basha kao eldar pokrenemo passwd i u prvoj instanci basha pogledamo koji se procesi trenutno izvrsavaju i sa kojim kredencijama koristeci komandu
```
ps -axo euid,suid,ruid,gid,egid,comm | grep passwd
```
dobit cemo output
```
 0     0  1000  1000  1000 passwd
```
- Vidimo da je efektivni id postavljen na 0 tj. na root privilegije. To je omogucio onaj `s` bit. 
- S bit sluzi sljedecem: Ako za neki fajl kao others imamo execute permisiju, i pokrenemo ga, inace bi efektivni id za taj pokrenuti proces bio postavljen na id korisnika koji ga je pokrenuo. Medjutim, s bit omogucava da se efektivni id postavi na 0 tj. ima root permisije.
- Dakle euid je 0 jer je zbog s bita receno hej pokreni ovaj program kao root, a ruid nek ostane na id onog koji je stvarno pokreno.
- `suid` sluzi da sacuva eh ovo mi nije jasno.