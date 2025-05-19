- Unix filozofija je "*Sve je fajl.*"
- Tako unix vidi i uredjaje, kao fajlove.
- Vec smo vidjeli da u fajl sistemu mozemo imati obicne fajlove, direktorije a sada cemo vidjeti da su i uredjaji predstavljeni kao fajlovi.
- Uredjaji su smjesteni u folderu `/dev`.
- Kad izlistamo sadrzaj nekog direktorija sa `ls -lhi`, fajlovi imaju oznaku `-`, direktoriji `d` a uredjaji imaju oznaku
	- `c` - karakter uredjaji: tastatura, terminal itd.
	- `b` - blok uredjaji: diskovi
- Razlika izmedju blok i karakter uredjaja je u tome kako se vrsi transfer podataka izmedju procesa koji cita i pise iz uredjaja. `c` znaci da se ide u stream formi tj. sekvenci karaktera a `b` znaci da citamo i pisemo u chunksima.
- Inodei asocirani sa uredjajima nemaju nikakav sadrzaj u data sekciji tj. ovi fajlovi su prazni.
- Dakle, ovi fajlovi su veliki 0 B, tj. u njima nema nista.

---

- Citanje i pisanje u ove uredjaje ima razlicite side efekte u zavisnosti od toga u koji uredjaj pisemo tj. citamo.
- Ako npr. pisemo u uredjaj koji je mrezna kartica, side efekat je da saljemo paket na mrezu. Ako pisemo u uredjaj koji je graficka kartica, side efekat je da ce se na ekranu nesto iscrtati.
- Ovi fajlovi koji predstavljaju uredjaje su u sustini veza sa dijelom kernela koji je drajver za taj uredjaj. To je skup funkcija u kernelu zaduzen za handlanje prekida tog uredjaja.
- Svaki fajl koji je asociran sa uredjajem ima dva broja:
	- **major broj**
	- **minor broj**
	- Razmisljamo ovako: Bilo kakva operacija nad uredjajem ce morati aktivirati dio kernela u kojem se nalaze funkcije koje handlaju tu operaciju nad datim uredjajem. Taj dio kernela zovemo drajver uredjaja. Postoji vise uredjaja i za svaki imamo drajver. Dakle, negdje u kernelu imamo vektor pointera koji pokazuju na lokaciju u memoriji na kojoj se nalazi drajver za taj uredjaj. **Major** broj nekog uredjaja je indeks u tom vektoru na kojem se nalazi pointer na drajver za taj uredjaj. Npr. ako imamo vise terminal uredjaja, svi oni ce imati isti major broj npr. 4. To znaci da su svi terminali uredjaji koji pripadaju istoj kategoriji. Minor broj je konkretna instanca uredjaja. Npr. svi terminal uredjaji ce imati isti major broj ali ce svaki imati jedinstven minor broj koji ga identificira.

# Terminal
- Unix je nastao 70ih godina kada nije bilo racunara koji lice na danasnje racunare.
- Originalni dizajn je bio takav da je procesor koji je mogao da izvrsava unix operativni sistem bio izuzetno skup i glomazan ali je imao odlicne performanse pa je vise korisnika moglo istovremeno da koristi racunar.
- Prema tome, bilo je potrebno razviti metodologiju kako da vise korisnika pristupi racunaru, salje mu komande i da dobije rezultat.
- Kljucni uredjaj koji je ovo omogucio je **terminal**.
- Terminal se sastoji od ekrana i tastature.
- Ekran moze da prikazuje karaktere.
- Tastaturom vrsimo korisnicki unos.
- Terminal je povezan sa racunarom serijskom vezom odnosno kablom koji se sastoji od dvije zice. Kroz jednu zicu iz terminala u racunar odlazi unos sa tastature a drugom zicom do ekrana dolazi karakter kojeg sljedeceg treba prikazati.
- Dakle, terminal je omogucio da vise korisnika bude spojeno na isti racunar i da istovremeno komunicira sa procesima na tim racunarima.
- Dakle, kad na tastaturi terminala pritisnemo neki taster, po odlaznoj zici putuje elektricni signal koji dolazi do procesora gdje se detektuje interrupt. Aktivira se dio kernela (drajver) terminala koji taj karakter sacuva negdje u memoriju i handla na nacin na koji je to definisano tim dijelom kernel koda odnosno drajverom terminala.
- Terminali kao fizicki uredjaji imaju fiksan broj linija i broj karaktera po liniji.

---

- Eh, danas vise terminali kao fizicki uredjaji ne postoje. Mozemo ih naci samo u muzejima.
- Medjuti, i danas je terminal najbitniji nacin na koji covjek komunicira sa racunarom.
- Na modernim unixima postoje simulacije terminala koji kao fizicki uredjaj vise ne postoji.
- Postoje dva razlicita simulatora terminala.
- Prvi simulator je direktno u kernelu.
- Ideja je da iskoristimo display racunara i njegovu tastaturu i da primamo interrupte od tastature koje handla drajver za tastaturu i salje terminal emulatoru koji onda dalje prica sa line disciplinom pa sa TTY drajverom. Terminal emulator onda salje VGA drajveru sta da isprinta na display. Medjutim, losa strana ovog simulatora jeste sto ova simulacija terminala za sebe uzima cijeli monitor i cijelu tastaturu. Otvaramo ga sa Ctrl+Alt+F1. Tada ce na cijelom ekranu biti prikazan samo terminal i tastaturu cemo moci koristiti samo za njega.
- Druga simulacija je kroz prozore koja omogucava da odredjena aplikacija u grafickom okruzenju glumi terminal. Npr. Kitty, Gnome terminal itd. U tom slucaju su to uredjaji koji se zovu `pty` ili `pts` nasuprot onih prvih simulacija koji su oznaceni sa `tty`.
- Simulaciju terminala jos nazivamo i **pseudoterminal**.
- Pseudoterminali mogu dinamicki mijenjati broj linija i broj karaktera u liniji (broj redova i kolona).

---
- Procesi u vecini slucajeva imaju fajl deskriptore 0, 1 i 2 asocirane sa fajlom koji predstavlja terminal.
- Ako hocemo da provjerimo da li je neki deskriptor asociran sa fajlom koji je uredjaj tipa terminal, to mozemo uraditi koristeci fju `isatty(fd)` gdje je `fd` npr. 0 odnosno standardni ulaz i dobit cemo `1` ako je fajl deskriptor `fd` asociran sa terminalom (koji je uredjaj, koji je fajl).
- Funckija `ttyname(fd)` vraca putanju do fajla asociranog sa fajl deskriptorom `fd` ako je terminal.
- Komanda `./a.out 0 < foo.txt` pokrece program `a.out` ali mu nulti fajl deskriptor tj. standardni ulaz mijenja na fajl `foo.txt` a ne na terminal.

---
- Emulirani terminal inicijalno radi u modu koji se zove *canonical*.
- U tom modu
	- svaki uneseni karakter se odma prikazuje na ekranu i to nam implementira ona line disciplina
	- dozvoljeno je editovanje teksta u trenutnoj liniji unosa terminala
	- tipka enter signalizira kraj editovanja linije i kompletan input buffer line discipline se salje procesu koji cita terminal
	- odredjene kombinaci xje tipki rezultiraju generisanjem signala
	- odredjeni karakteri kao sto su `\t` se interpretiraju drugacije
- Postoji i *raw* mod.
- U njemu je sve ovo pogaseno.
- Aplikacija koju mozemo koristiti da rekonfigurisemo neke od ovih osobina je stty.
- Mozemo reci npr. `stty -echo`. Ovime ugasimo echo mod. Sta god da unesemo sa tastature to se nece prikazivati u liniji. Tek kad udarimo enter vidjet cemo sta smo unijeli. Zamisli, sve radi kao inace, samo ne vidis sta kucas. Ovaj trik koristi onaj passwd utility. Ako hocemo da ga vratimo nazad, kazemo `stty echo.

# API za konfiguraciju terminala

- Za promjenu moda operacija terminala mozemo koristiti api koji nam unix obezbjedjuje za tu namjenu. Za to je potrebno ukljuciti `termios.h` biblioteku.
- Utility `stty` koristi upravo ovaj api za promjenu moda operacija terminala.
- Koristi se struktura `struct termios` i dvije funkcije
	- `int tcgetattr(int fd, struct termios* tty)` - popuni strukturu `tty` sa atributima terminala asociranog sa fajl deskriptorom `fd`. Ovim pozivom dobijamo trenutne postavke terminala. Ako hocemo da ih promijenimo, koristimo
	- `int tcsetattr(int fd, int flag, struct termios* tty)` - prima fajl deskriptor asociran sa terminalom kojem mijenjam postavke, postavke koje sam podesio kroz strukturu `tty` i flagove kako cu aplicirati te postavke (da li odmah ili sacekati IO operaciju na uredjaju ali to i nije toliko bitno). Sustina je da ove dvije funkcije citaju i pisu postavke koristeci strukturu `termios`. 
- Struktura `termios` ima sljedecu definiciju

```c
struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
}
```

- Da ovo demonstriramo, pogledat cemo primjer. 

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

struct termios origTermios;
int ttyfd = STDIN_FILENO;
```

U sturkturu `origTermios` cemo snimiti originalne, inicijalne postavke terminala i to cemo cuvati kao globalnu varijablu. Globalnu varijablu `ttyfd` postavljamo na nulu i to je standardni ulaz tj. fajl deskriptor asociran sa terminalom.

```c
int main() {
	if (!isatty(ttyfd))
		fatal("Not a terminal");
	if (tcgetattr(ttyfd, &origTermios) < 0)
		fatal("Can't get termianl settings");
	if (atexit(tty_atexit) != 0)
		fatal("atexit: can't register tty reset");
	set_tty();
	process_term();
	return 0;
}
```

Funkcija `isatty(ttyfd)` ce nam vratiti `1` tj. true ako deskriptor `ttyfg` asociran sa terminalom. Ako nije, onda nema smisla da ista podesavamo jer se ocigledno ne radi o terminalu pa pozivamo funkciju `fatal` koja uzima poruku, ispisuje je i terminira program:

```c
void fatal(const char* message) {
	perror(message);
	exit(1);
}
```

Zatim smo pozvali `tcgetattr(ttyfg, &origTermios)` kako bismo dobili originalne, defaultne atribute terminala i to smo snimili u `origTermios`. Ako se i tu desi bilo koji problem tj. vrati nam se negativna vrijednost, terminirat cemo program.

Zatim pozivamo fju `atexit` koja prima pointer na drugu funkciju a mi prosljedjujemo `tty_atexit`. Funkcija `atexit` se poziva neposredno prije izlaska iz procesa (dakle kad pozovemo `return` ili `exit`) i ocekuje da joj proslijedimo neku nasu napisanu funkciju koju ce onda ona pozvati. Mi smo napisali `tty_atexit`:

```c
void tty_atexit() {
	tty_reset();
}
```

Vidimo da ona poziva funkciju `tty_reset()` koju smo mi napisali da radi tako da vrati postavke terminala na one originalne koje smo mi u toku izvrsavanja ovog programa promijenili:

```c
int tty_reset() {
	if (tcsetattr(ttyfd, TCSAFLUSH, &origTermios) < 0)
		return -1;
	return 0;
}
```

Ako sve ove ifove prodjemo, dolazimo do funkcije `set_tty()`.

```c
void set_tty() {
	struct termios raw;
	raw = origTermios;
	raw.c_lflags &= ~(ECHO | ICANON | ISIG);
	
	
}
```

Kreirali smo novu `termios` strukturu i nazvali je `raw` jer terminal hocemo da prebacimo iz kanonicnog u raw mode. Sada cemo ovoj strukturi mijenjat polja tako da ih postavimo na zeljena. Prvo smo kopirali originalne postavke u `raw` a zatim izvrnemo flagove:
	`raw.c_lflags &= ~(ECHO | ICANON | ISIG);` - ukidamo eho, kanonicni mod i procesiranje signala.