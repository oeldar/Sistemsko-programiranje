- Koncept paralelizma u kodu smo do sada implementirali koristeci IO multipleksiranje koristenjem `select` sistemskog poziva. U sustini, mi smo samo simulirali paralelizam.
- Jedan od primjera gdje se zahtijeva paralelizam u kodu je server. Server treba da servisira vise klijenata istovremeno a ne da klijent ceka da na njega dodje red.
- Drugi primjer je na klijentskoj strani. Npr. aplikacija koja ima neko graficko okruzenje tj. GUI. Ona treba istovremeno da recimo motri na pomjeranje misa ali i da npr. ceka na neki paket sa mreze. Dvije stvari trebaju da se rade istovremeno.

---
- Na operativnim sistemima smo vidjeli da mozemo ostvariti paralelizam na nivou vise aplikacija. Tada smo napravili takvu infrastrukturu koja je omogucila da se na jednom procesoru istovremeno izvrsava vise aplikacija. Medjutim, to je bio paralelizam na nivou vise aplikacija a ne na nivou jedne aplikacije. Nas sadasnji cilj je ostvariti paralelizam unutar jedne aplikacije.
- Tada smo definisali proces kao apstrakciju okruzenja u kojem se izvrsavaju programi. Rekli smo da je proces okruzenje u kojem se izvrsava program. To je dakle apstrakcija koju kreira kernel. Putem te apstrakcije, aplikacija koja se izvrsava unutar nje, dobija virtuelni CPU i virtuelnu memoriju i ona smatra da samo za sebe ima cijeli procesor i cijelu memoriju. Takvi sistemi se zovu multitasking sistemi. Jedini nacin da jedna aplikacija radi vise stvari istovremeno jeste da se forka i da onda parent proces radi jedan dio posla a drugi dio posla da radi child proces. Mada, ovime nastaju dva razlicita procesa pa i ne mozemo govoriti o nekom paralelizmu na nivou aplikacije na ovaj nacin. Ovi procesi su separatni i ne dijele memoriju.
- Dakle, **multitasking** je mogucnost da se proizvoljan broj procesa izvrsava konkurentno.
- Napomenimo jos jednom da multitasking ne podrazumijeva da jedan proces radi vise stvari odjednom nego da se na jednom procesoru izvrsava vise razlicitih procesa. Oni se vremenski multipleksiraju na procesoru i rasporedjuju raspolozivim jezgrima procesora konceptom schedulinga.

---
- Sljedeci korak koji se desio u razvoju operativnih sistema jeste uvodjenje paralelizma na nivou samog procesa. Cilj je bio da omogucimo vise paralelnih sekvenci instrukcija u jednom procesu.
- Ta mogucnost se zove **multithreading**. Takav sistem unutar jednog procesa omogucava aplikaciji da od operativnog sistema zatrazi jos jedan virtuelni CPU.
- Dakle, ovako je do sada islo: Program se izvrsava u okruzenju koji se zove proces. Svaki proces dobije od kernela svoju `proc` strukturu u kojoj ima svoje page tabele, svoj virtuelni procesor itd. Dakle, proces za sebe dobije jedan cijeli virtuelni procesor sto mu omogucava da se izvrsava tako kao da ima cijeli fizicki procesor samo za sebe. Znamo da to proces samo misli jer pristup stvarnom, fizickom procesoru proces ima samo kada ga scheduler odabere i kaze mu da se nastavi izvrsavati.
- Konceptom multitredinga, proces zatrazi od kernela jos jedan virtuelni procesor koji onda sada moze izvrsavat neki drugi niz instrukcija paralelno sa ovim prvim virtuelnim kojeg je dobio. Naravno da ce biti moguce da se ove dvije razlicite sekvence instrukcija jednog procesa istovremeno paralelno izvrsavaju ako imamo dva fizicka jezgra.
- Jasno je da taj cpu kojeg dobijemo moramo zaposliti necim. On mora nesto izvrsavati. Nema smisla zatraziti jos jedan virtuelni cpu i onda mu nista ne dati da radi. Nas prvi cpu izvrsva kod od `main` funkcije jer je ona defaultni entry point. Drugom cpu-u kojeg zatrazimo od operativnog sistema moramo eksplicitno dati funkciju koju ce izvrsavati.
- Novi stack mora da dobije svoj stack i stack na kernel strani. Dakle, svaki novi cpu dobije svoj stack ali su svi ti stackovi u istom virtuelnom adresnom prostoru originalnog procesa.
- Dakle, sada za jedan proces imamo vise stackova.
- Znaci nit je u sustini novi virtuelni cpu za jedan proces. Svaka nit ima svoj stack unutar jednog originalnog virtuelnog adresnog prostora.
- Dakle recimo da imamo neki proces A. Njegova virtuelna memorija izgleda nesto poput:

```
----------
Stack
----------

----------
Heap
----------
Data
----------
Code
----------
```

- Sada kada imamo jos jednu nit u nasem procesu, ovaj virtuelni adresni prostor ce izgledat kao

```
----------
Stack1
----------

----------
Stack2
----------

----------
Heap
----------
Data
----------
Code
----------
```
- pri cemu je stack1 stack od maina a stack2 stack od funkcije koja se izvrsava na drugoj niti. Lokalne varijable maina i foo-a su lokalne za te fje. Medjutim, i main i foo imaju zajednicki heap, data i code sekcije.
- U foo koja se izvrsava na nekoj drugoj niti mozemo imati pointer na neku varijablu na heapu koju takodjer moze da vidi main iz prve niti.

---
- Svi unixi su danas osim multitasking i multitreding sistemi tj. imaju aplikacioni interfejs za kreiranje niti. Taj API se zove POSIX pthread.
- Od verzije C++11, standardna biblioteka ima svoj API koji omogucava rad sa nitima bez koristenja pthread biblioteke direktno.

---
- Dakle, niti dijele isti adresni prostor u okviru procesa u kojem su kreirane.
- Svaka nit unutar tog adresnog prostora ima funkciju koju izvrsava i svoj stek za lokalne varijable te funkcije.
- Inicijalno, program ima samo jednu nit koja izvrsava funkciju `main`. Ova nit je specijalna jer njenim terminiranjem se terminira cijeli proces.
- Heap i globalni objekti programa su vidljivi i mogu se manipulirati iz svake niti kreirane u programu.

---
- Hajde sad da vidimo konkretno kako u C++ kreiramo niti. Engleski termin za nit je thread.
- Rekli smo da C++ koristi API iz biblioteke `thread` koji je wrapper za sistemske pozive koji kreiraju i handlaju niti a koji su na unix sistemima prevedini u skladu sa pthread apijem.
- Potrebno je prvo ukljuciti zaglavlje `#include <thread>`

```c
#include <thread>
#include <iostream>

void foo() {
	std::cout << "Pozdrav od niti: " <<
		std::this_thread::get_id() << std::endl;
}

int main() {
	std::thread t1{foo}; // Kreiramo objekat tipa thread.
	t1.join();
	foo();
	return 0;
}
```

- Unutar zaglavlja `thread` postoji klasa imena `thread` definisana u namespace `std`. Ta klasa ima konstruktor koji prima ime funkcije koja ce se izvrsavati na threadu kojeg kreiramo. Ovaj konstruktor u pozadini, ako ovaj kod kompajliramo na unix sistemu, poziva sistemske pozive iz pthread interfejsa i efektivno kaze *Kreiraj novu nit za ovaj proces i daj joj da izvrsava kod funkcije foo.*
- Ovime smo efektivno rekli, *daj mi jos jedan virtuelni CPU i neka on izvrsava kod od fje foo*.
- `main` naravno ima svoj thread. 
- Ako bi kojim slucajem funkcija `foo` uzimala parametre tj. imala potpis `foo(int, double, std::string)` tada bismo `t1` konstruisali npr. koristeci `std::thread t1{foo, 2, 3.2, "hej"};`
- Konstruktor za `thread` je varijadicni konstruktor sto znaci da uzima proizvoljan broj argumenata. Ako funkcija prima argumente i ako u `thread` ne proslijedimo argumente, dobit cemo gresku u kompajliranju. Najbolja varijanta je kada gresku dobijemo u kompajliranju tj. compile timeu a ne runtimeu.
- Vec prelaskom na sljedecu liniju imamo paralelizam u nasem kodu jer imamo dva jezgra. Jedno jezgro ce nastaviti izvrsavat kod fje `main` a drugo ce poceti izvrsavat kod fje `foo`. Svako od jezgri ima svoj stek i koristi ga pri izvrsenju koda ovih fja.
- Unutar fje `foo` pozivamo fju `get_id()` koja se nalazi u namespaceu `this_thread` a koji je u namespaceu `std` u biblioteci `thread`. Ova funkcija nam vrati id trenutne niti. Svaka nit ima jedinstven id. Ovaj id se nuzno ne mora mapirati na id niti na nivou operativnog sistema ali imamo garanciju i u C++ da je jedinstven broj. Uzgred recenu, na linuxu se ovo mapira sa id-jem niti kojeg koristi i operativni sistem.
- Kod kompajliranja ovakvog koda trebamo dodati jos i flag `-lpthread` koji ce sluziti linkeru. Linker nas program mora linkat sa onim dijelom biblioteke operativnog sistema u kojem se nalaze definicije POSIX pthread data i funkcija operativnog sistema. Ovaj swich znaci da linkamo sa bibliotekom imena `libpthread.so` ili `libpthread.a`. Ekstenzija `.so` znaci da je biblioteka dinamicki linkana. Ako kompajliramo kod bez ovog switcha, necemo dobiti gresku od kompajlera nego od linkera. Uzgred receno, da vidimo od kojih biblioteka zavisi nas ozvrsni fajl, to mozemo uraditi koristeci `ldd a.out`.
- Dakle, funkcija `foo` ispisuje id niti na kojoj se `foo` trenutno izvrsava.
- Mi nemamo garanciju od trenutka kada kreiramo novu nit kada ce ona iz runnable stanja preci u running stanje. To znaci da u gore navedenom programu, kad ga pokrenemo, ne znamo hoce li se prvo ispisati id niti na kojoj se izvrsava `main` ili id niti na kojoj se izvrsava `foo`.
- Napomenimo da se nit terminira u trenutku kada izadjemo iz funkcije koju ona izvrsava. Dakle, nasa nit `t1` ce nestati u trenutku kada zavrsimo zadnju liniju funkcije `foo` kojom smo je zaposlili. I tada se terminira samo nit `t1`. Jedan slucaj je poseban: Kada se izadje iz `main`-a tada se terminira glavna nit na kojoj se izvrsavao `main` ali i sve niti koje su napravljene u tom `main`-u i to bezuslovno.
- Sta radi `join()` vidjet cemo kasnije. Za sad cemo reci samo da je on garancija da ce se prvo izvrsiti `foo` na niti `t1` a zatim `foo` od `main`-a.

---
- Pogledajmo sljedeci primjer:

```c
void foo(int i) {
	while (i-- > 0) {
		std::cout << "Nit: " << std::this_thread::get_id()
			<< " i: " << i << std::endl;
	}
}

int main() {
	std::thread t1{foo, 5};
	foo(5);
	t1.join();
	return 0;
}
```

- U ovom primjeru ne pozovemo `join` nad niti koju smo prethodno kreirali odma. To za posljedicu ima da se i ta nit i nasa glavna nit `main`-a izvrsavaju bukvalno istovremeno, na dva razlicita jezgra. Sjetimo se da kada bismo nakon definisanja niti `t1` rekli `t1.join()` to bi znacilo da cemo sacekati da nit `t1` zavrsi sa izvrsavanjem svoje funkcije pa onda nastaviti sa izvrsavanjem niti u kojoj se izvrsava `main` kao u primjeru maloprije. Ovdje to nije slucaj. Imamo pravi paralelizam.
- Sta je sada ovdje problem? Pa rekli smo da svaka nit ima svoj stek tako da ovaj argument `i` nece predstavljat nikakav problem. Funkcija `foo` sa niti `t1` ne vidi `i` iz funkcije `foo` pozvane iz `main`-a tako da tu nema nikakvih problema. Dekrementiranje varijable `i` u ovim nitima ne utice na drugo `i`.
- Problem je sto u funkciji `foo` koristimo globalni objekat `std::cout` koji, zato jer je globalni, zivi u data sekciji naseg programa. Data sekciji imaju pristup sve niti. Globalni objekat je jedan i vidjljiv je sa svakog steka svih niti u nasem kodu. Ovo nas je dovelo do stanja utrke jer vise razlicitih jezgara zeli da pristupi istom resursu istovremeno. Zbog toga je ispis ovog programa nedefinisan i izgleda nesto kao:

```
Nit: Nit: 123 321 i: i: 44
Nit: 123 i: 3
Nit: 123 i: 2
Nit: 123 i: 1
Nit: 123 i: 0
Nit: 321 i: 3
Nit: 321 i: 2
Nit: 321 i: 1
Nit: 321 i: 0
```

- Napomenimo da ispis nece uvijek ovako izgledati i nema nacina da unaprijed predvidimo kakav ce ispis biti. Ovo je posljedica stanja utrke koji se desio. Program je nedeterministican.
- Ovo moze dovesti i do crashiranja programa ali ovdje nije dovelo jer `cout` garantuje da nece crashirat ali ispis nece biti deterministican.
- S tim u vezi imamo pojam *thread safety*. Kazemo da je metod ili funkcija thread safe ako pozivanje te funkcije ili metoda iz vise niti nece izazvat nedeterministicno ponasanje. Nijedan metod iz standardne biblioteke nije thread safe.

---
- Kada kreiramo nit pomocu `std::thread nit{foo}` mi dobijamo nit na kernel strani. Asocirani smo sa novom niti na kernel strani. Prvi argument ovom konstruktoru je funkcija koja ce se izvrsavat na novoj niti.
- Objekat tipa `thread` u sebi ima stanje. Nit na kernel strani ima stanja: running, runnable, waiting itd.
- Nit na user strani ima dva stanja: **joinable** ili **unjoinable**.
- Ako je objekat u joinable stanju to znaci da sa sobom ima asociranu validnu nit na kernel strani.
- Ako je objekat u unjoinable stanju, to znaci da sa sobom nema asociranu validnu nit na kernel strani.
- Razlozi zbog kojih objekat tipa `thread` moze ne imati sa sobom asociranu validnu nit su sljedeci:
	- objekat kreiran pomocu deafult konstruktora npr. `std::thread t1{}`
	- Na objektu obavljena `std::move` operacija npr. `std::move(t1)`. Ne smije kreirati drugi objekat tipa `thread` kopi konstrukcijom ali se smije moveat u drugi objekat tipa `thread` s tim da taj iz kojeg smo moveali postaje unjoinable. Kopi konstruktor i operator `=` su izbrisani za objekte tipa `std::thread`. Samo jedna nit moze biti asocirana sa jednim objektom tipa `thread`.
	- Na objektu obavljena `join` operacija
	- Na objektu obavljena `detach` operacija

---
- Dakle rekli smo da objekat tipa `thread` moze biti u joinable i u unjoinable stanju. U joinable stanju sa sobom ima asociranu validnu nit na kernel strani a u unjoinable nema.
- Ako imamo neki opseg unutar kojeg smo kreirali `thread` i zaposlili ga izvrsavanjem funkcije `foo` npr.

```c
{
	std::thread t{foo};
}
```

- Znamo da ce kada budemo izasli iz ovog opsega da se okine destruktor nad objektom `t` tipa `thread`.
- Ako je u trenutku kad se izvrsava destruktor objekta `t`, taj objekat u joinable stanju, kompletan program ce se terminirati odmah.
- Ovo je jakon bitan detalj kojeg moramo uzeti u obzir kada pisemo program u kojem cemo koristiti niti. Prije izlaska iz opsega u kojem je definisan objekat tipa `thread`, taj objekat mora biti prebacen u joinable stanje pozivom metoda `join` ili `detach` ili ga moveat u neki drugi objekat tipa `thread` koji ce nadziviti ovaj scope.
- Metod `join` pozvan nad objektom tipa `std::thread` pauzira onu nit koja je kreirala taj objekat sve dok nit nad kojom je pozvan `join` ne okonca izvrsvanje.
- Npr.

```c
//main
{
	std::thread t1{foo}; // od ovog trenutka nit t1 izvrsava foo
	... // a u istom tom trenutku nit na kojoj je main izvrsva ovaj kod
	t1.join(); // ovdje kazemo niti na kojoj je main: hej sacekaj, nemoj vise nista radit dok nit t1 ne zavrsi sa izvrsavanjem funkcije foo.
}
```

- Dakle, koristeci `join` omogucavamo da jedna nit saceka da druga nit zavrsi sa svojim poslom.
- Ako pozovemo `detach` umjesto `join` na objektu tipa `std::thread`, taj objekat ce takodjer biti u unjoinable stanju sto znaci da sigurno mozemo napustiti opseg u kojem je objekat tipa `std::thread` bez da se program terminira. Nit ce se dati na upravljanje C++ runtimeu. Dakle, nit koju smo detachali ce se nastaviti izvrsavati samo ce objekat asociran s tom niti biti u unjoinable stanju i mi toj niti necemo moci pristupiti. Mi nikada u nasem kodu vise necemo onda moci znaci da li je ta nit zavrsila s radom ili nije ali slobodno mozemo napustiti trenutni opseg.
- 