- Dakle, rekli smo da nas program u sustini predstavlja skup stanja. Ta stanja su sacinjena od objekata koji si u memoriji.
- Pozivanjem metoda i funkcija, mi mijenjamo stanje programa. Usput mozemo proizvesti i neki popratni efekat.
- Ti metodi i funkcije u bilo kojem trenutku mogu baciti iznimku. Postavljamo pitanje: Kada se desi iznimka i dodjemo do `catch` bloka u kojem se ta iznimka treba da tretira, u kojem stanju je mog program u tom trenutku u odnosu na stanje u kojem je bio prije poziva metoda koji je generisao iznimku?
- U vezi sa tim smo definisali onaj *exception safety*.
- U opstem slucaju, mi hocemo da imamo jedno stanje programa, pozovemo metod i nakon sto se izvrsi metod treba da znamo u tacno kojem stanju je objekat kojeg je taj metod promijenio kao i u kojem stanju je cijeli program.
- Rekli smo da kod jake garancije znamo da ako se desi iznimka, stanje programa ce biti vraceno na stanje u kojem je taj program bio prije pozivanja metoda koji je generisao iznimku. Dakle, potpuno reverziramo akciju koji je metod izvrsio. Ovakvu garanciju je jako tesko napraviti. Jedan od rijetkih metoda koji ima jaku garanciju je `push_back()` nad vektorom.
- Ako je metod imao i neki popratni efekat poput slanja paketa na mrezu ili ispisa na ekran, tada je nemoguce reverzirati njegovu akciju.
- Zato imamo nesto sto se zove osnovna ganarcija. Metod koji ima osnovnu garanciju nam osigurava da ce eventualno proizvesti poznate popratne efekte (koje ne moze reverzirat), ali ce sve dinamicke resurse osloboditi i nece dovesti do resource leaka. To znaci da ce delete-at sve objekte sa heapa, closeati sve otvorene fajl deskriptore, unjoinati zapocete threadove ili unlockati zakljucane mutexe.

---

Posmatrajmo sljedeci kod:

```c
Foo* makeFoo(Log* l) {
	l->open();
	auto f = new Foo{};
	l->record(f);
	l->close();
	return f;
}
```

- Ovo je klasicni primjer koda kojeg cemo koristiti pri logiranju dogadjaja u nasoj aplikaciji. Ideja je da smo proslijedili neki log fajl `l` kojeg cemo otvoriti, u njega upisati stanje objekta `Foo f` i zatim kad to uradimo zatvoriti fajl `l`.
- Kakve garancije spram iznimku pruza funkcija `make_foo`?
- Pa u prvoj liniji nad objektom `l` se poziva `open()` koji ce otvoriti fajl deskriptor asociran sa fajlom `l`. Oke, dakle kernel nam je dao jedan resurs na koristenje a to je fajl deskriptor.
- Ako u sljedecoj liniji vec konstruktor `Foo{}` baci iznimku, nas kod nikad nece doci do linije `l->close()` i resurs koji nam je kernel dao nikad nece biti zatvoren. Kazemo da je doslo do *resource leak*-a tj. curenja resursa. Fajl deskriptor asociran sa `l` ostaje zauvijek otvoren. Ako se ovo desi par puta u nasoj aplikaciji, ona ce garant crashirati.
- U drugoj liniji na heapu kreiramo objekat tipa `Foo`. Ako se u metodu `record` koji je ispod toga desi iznimka, mi nikada necemo sa heapa ukloniti taj objekat. Dakle, jos jedan resource leak u nasem kodu.
- Dakle, ova funkcija nema nikakve garancije.
- Nas cilj je da to popravimo. Zelimo da ostvarimo bar osnovnu garanciju.
- Zbog ovoga uvodimo poseban tip ciji je jedini zadatak da nam obezbijedi osnovnu garanciju.
- Dakle, uvest cemo tipove ne zbog neke business logike nego zbog exception safety-a.
---
- Ideja je da napravimo neki wrapper za `Log` objekat.

```c
struct LogGuard {
  Log* l_;
  LogGuard(Log* l) : l_{l} { 
    l_->open(); 
  }
  ~LogGuard() { 
    l_->close(); 
  }
};
```

- Konstruktor poziva `open()`
- Deskruktor poziva `close()` kada objekat izadje iz opsega
- Ovo nam garantuje da ce `close()` biti uvijek pozvan ca i ako dodje do iznimke u funkciji.
- Primijetimo da je ovaj tip koristan samo u ovom slucaju za razliku od sljedeceg tipa kojeg bismo mogli koristiti i u drugim projektima.

```c
template<typename T>
struct ScopedPtr<T>{
	T* p_;

	ScopedPtr(T* p): p_{p} {};
	~ScopedPtr(){
		delete p_;
	}
}
```

- U novijoj C++ literaturi, klasicni pointer npr. `int*` zovemo *raw* pointer.
- Vidimo da i ovdje prilikom izlaska iz opsega imamo garanciju da ce se resurs sa heapa osloboditi.
- Problem sa raw pointerom je sljedeci. Zamislimo da imamo funkciju `foo(int* p)`. Ovaj raw pointer moze biti pointer na `p` koje je na steku ali moze biti i pointer na objekat na heapu. Ne postoji nacin da saznamo gdje je taj objekat.
- Za funkciju koja ce deleteat objekat asociran sa proslijedjenim raw pointerom kazemo da zeli da bude vlasnik tog objekta.
- Bilo bi lijepo kada bi svaka funkcija kada deleta nesto sa heapa to postavi na `nullptr` tako da onaj koji sljedeci koristi taj objekat moze provjeriti je li `nullptr` tj. da li ga je neko vec obrisao. Vecina programera nije tako disciplinovana. Dakle, raw pointeri su nesigurni.
- Ne postoji nikakav pametan nacin da znamo da li je resurs na heapu aktivan tj. da li ga je neko vec obrisao i da li ga jos uvijek mozemo koristiti.
- U gornjem primjeru, kazemo da je `ScopedPointer` postao vlasnik resursa na heapu.
- Da bi bilo moguce koristiti resurs na heapu i kada izadjemo iz scopea u kojem je definisan `ScopePointer`, definisemo operaciju `release`

```c
struct ScopePointer<T>{
...
	T* release(){
		T* temp = p_;
		p_ = nullptr;
		return temp;
	}
}
```

- Ovom operacijom otpustamo vlasnistvo nad objektom na heapu. Neposredno prije napustanja scopea znaci mozemo da predamo nekom vlasnistvo nad tim objektom na heapu ili da ga automatski dealociramo.
- Sada bi onaj nas kod od maloprije izgledao ovako:

```c
void make_foo(Log* l) {
	LockGuard lg{l};
	ScopedPtr<Foo> ptr{new Foo{}};
	l->record(f);
	return ptr.release();
}
```

- Ako bi se sad desila iznika u konstruktoru `Foo{}`, vise necemo imati onaj resource leak jer nam C++ runtime garantuje da ce pozvati destruktore nad svim lokalnim varijablama definisanim u scopeu u kojem se desila iznimka koje su definisane prije linije u kojoj se desila iznimka.
---

- U C++ su razvijeni novi tipovi pointera.
- Ti tipovi su `unique_ptr, shared_ptr, weak_ptr`.
- Ovi tipovi se nalaze u zaglavlju `memory`

---
- Recimo da imamo

```cpp
struct radnik {
	std::string ime;
	int godine;

	radnik(std::string i, int g)
	: ime{i}, godine{g} {};
	~radnik() {
		std::cout << "unisten" << std::endl;
	}
}
```

- Sada mozemo napraviti raw pointer na objekat tipa `radnik` sa `radnik* p = new radnik("foo", 25);`
- Od ovog raw pointera mi mozemo napravit `unique_pointer` na sljedeci nacin `std::unique_pointer<radnik> up{p}`
- Sad se mozemo ponasat ko da je `up` obicni pointer i radit s njega sve sto bismo radili i sa raw pointerom npr. `up->ime = "bar"` ce promijeniti ime.
- Ono sto mogu uraditi sa obicnim pointerima jeste da mozemo napraviti novi pointer na istog radnika. Recimo ako kazemo `auto np(p)`, imat cemo i varijablu `np` tipa `radnik*` koja pokazuje na istog radnika na kojeg pokazuje i varijabla `p`.
- Ako to pokusamo sa `up` tj. `auto nup(up)` dobit cemo error jer je `up` unikatni vlasnik resursa na heapu. Kada bismo mogli kopirati, ne bismo imali unikatnog vlasnika.
- Ideja je da koristenjem ovih tipova budem eksplicitan sta zelim sa tim tipovima.
- Ako koristim `unique_pointer` to znaci da ce samo jedan objekat biti vlasnik.
- Mozemo prenijeti vlasnistvo na drugi objekat koristenjem `std::move` tj. ako kazemo

```cpp
std::unique_ptr<radnik> up{p};
auto nup = std::move(up);
```

- Ovo prolazi. Od sada je `nup` vlasnik objekta na heapu na kojeg pokazuje raw pointer `p`. Objekat `up` je sada `nullptr`. Ako je neki `unique_ptr` jednak `nullptr`, to znaci da je `up` predao vlasnistvo nekom drugom objektu.

---
- Ako imamo funkciju ciji je potpis `void foo(std::unique_pointer<radnik> up)` i probam je pozvat sa `foo(up)` gdje je `up` tipa `std::unique_pointer<radnik>` dobit cemo error jer zelimo novu varijablu tipa `up` lokalnu za funkciju `foo` inicijalizirati sa `up` koji je `unique_pointer`. Jedini nacin da to uradimo jeste da fju pozovemo sa `foo(std::move(up))`.
- Pomocu `unique_pointer`-a mozemo iskazati zelju funkcije da zeli da preuzme vlasnistvo nad objektom.
- Moderni C++ programer zna da ako funkcija zeli samo da koristi objekte sa heapa, onda treba da prima objekat kao  raw pointer. Dakle, `void foo(radnik* p)`. To cemo pozvati sa `foo(up.get())` jer `.get()` nad `unique_pointer`-om  vraca raw pointer.
- 