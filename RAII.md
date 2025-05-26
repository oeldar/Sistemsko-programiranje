- Posmatrajmo sljedeci kod:

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
- Dakle, ova funkcija nema nikakve garancije.
- Nas cilj je da to popravimo.

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