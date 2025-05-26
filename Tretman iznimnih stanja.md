- Mi kada pisemo kod, u sustini ostvarujemo komunikaciju izmedju razlicitih funkcija. Npr. cijeli kod nam je u mainu, main ce pozvati neku drugu fju koja nesto treba da odradi. Ona ce mozda pozvati neku drugu funkciju itd. Sustinski, cijeli nas program se svodi na pozivanje funkcija.
- Funkcija koja poziva neku drugu funkciju ocekuje da ta pozvana fja nesto odradi i vrati rezultat. Usput, funkcije pored sto mogu da vrate neki rezultat, one mogu da imaju i neki **popratni efekat** tj. da usput nesto urade:
	- posalje paket na mrezu
	- ispise nesto na ekran
	- upravlja nekim uredjajem itd.
	- u sustini, da promijeni stanje svijeta oko sebe.
- Ako funkcija da output za svaki moguci input, tada takvu funkciju zovemo **potpuna funkcija**.
- Funkcija koja za odredjeni input ne moze da da validan output se zove nepotpuna funkcija.
- Idealno bi bilo da svaka funkcija bude potpuna, medjutim to nije slucaj.
- Stanje u kojem funkcija ne moze da vrati rezultat za dati ulaz se zove abnormalna situacija.
- U POSIX domenu, metodologija kojom se ovo handla jesu error kodovi. Ideja je da ako se desi abnormalna situacija u funkciji, takva funkcija ce vratiti odredjenu vrijednost koja ce calleru dati do znanja da je doslo do abnormalne situacije i da izvrsavanje te funkcije smatra ne uspjesnim.
- Npr. ako funkcija `tcsetattr(ttyfd, &origTermios)` vrati `-1`, tada znamo da postavljanje postavki terminala asociranog sa fajl deskriptorom `ttyfd` na postavke zapisane u strukturi `origTermios` nije bilo uspjesno.
- Ovo je u C-u jedini nacin da detektujemo abnormalnu situaciju.
- Program da bi ispravno radio, on mora da radi za 100 % slucajeva, tako da treba da pokrijemo sve moguce situacije. Kazemo da treba da *tretiramo iznimna stanja*.
- Dakle, u C-u su error kodovi jedini nacin detekcije abnormalnih stanja. Uzgred receno, ovakav pristup je moguc u svakom programskom jeziku.
- C++ posjeduje i drugi mehanizam koji se zove iznimke (*exceptions*).

---
- Iznimke nisu nesto sto je inherentno asebmleru tj. ne postoji instrukcija u koju ce se iznimka prevesti. Zbog toga u C-u koji je najbizi asembleru i nemamo koncept iznimki.
- Medjutim, u C++ jeziku postoje iznimke koje su dio C++-a kao jezika. Iznimku u C++ tumaci C++ runtime kojeg mozemo shvatiti kao program koji izvrsava nas program.
- C ne ovisi ni od cega osim od sistemskih poziva sistema na kojem se izvrsava C kod dok C++ zavisi od standardne C biblioteke i samim tim od vec spomenutih sistemskih poziva ali i od runtime u kojem je implementirana infrastruktura za iznimke.

---
- Iznimke kreiraju alternativni kod programa u slucaju da se desi abnormalno stanje.
- U C++ da bismo generisali iznimku koristimo sljedecu sintaksu: `throw obj;` gdje je `throw` kljucna rijec a `obj` je objekat bilo kojeg tipa koji ima copy konstrukciju. Objekat kojeg bacamo moze biti i rvalue i lvalue.
- Objekat `obj` moze biti bilo kojeg tipa (za razliku od npr. Jave gdje `obj` mora imati tacno definisanu naslijednu hijerarhiju tj. mora naslijediti od objekta tipa `Exception`).
- Tip onog sto bacamo, treba da onome koji tretira iznimno stanje sugerise na to sta je iznimno stanje. Dakle, necemo bacat neke random objekte.

>[!abstract] Bitno:
>- Kada se baci iznimka, dakle odma nakon linije `throw obj;`, prestaje se izvrsavat trenutni opseg programa tj opseg `{ ... throw obj; ...}`. Dakle, sav kod od linije `throw obj;` pa do sljedeceg `}` se preskace. 
>- Nad svim lokalnim varijablama definisanim u tom bloku u kojem je `throw obj;` a koje su definisane prije `throw obj;` se poziva destruktor. Tu destrukciju nam **garantuje!** C++ runtime. Redoslijed poziva destruktora je obrnut od redoslijeda konstrukcija varijabli u tom bloku.
>- Kada se izvrsi zadnji destruktor, izlazi se iz trenutnog opsega i prelazi u opseg u kojem se nalazi opseg u kojem je iznimka bacena. U tom opsegu u kojem je pozvana funkcija koja je bacila iznimku mozemo takodjer imat neke lokalne varijable. I nad njima se takodjer poziva destruktor. U tom opsegu se takodjer preskace komad koda od mjesta fje koja je bacila iznimku i tako nastavljamo tu proceduru. Ovaj proces se zove **stack unwiding**. Preskacemo iz stack framea jedne fje u stack frame druge fje ne pomocu returna nego pomocu iznimki.
>- Ova procedura se nastavlja sve dok se u nekom od pozivajucih opsega eksplicitno ne tretira iznimka koristenjem `catch` sintakse. 
>- Ako se ovo nastavi sve do `main`-a i ni u `main`-u ne postoji `catch` blok koji tretira iznimku tipa koji je bacen, onda nas program prakticno ne tretira iznimku. Runtime u tom slucaju poziva funkciju `std::terminate` iz standardne biblioteke koja terminira proces i ispisati zbog koje iznimke je proces terminiran.

 ---
- Bacena iznimka se moze hvatati po vrijednosti ili referenci.
- Ako ne hvatamo primitivni tip, preporucuje s e hvatanje po konstantnoj referenci zbog cinjenici da iznimka obicno nosi informacije o necemu sto se dogodilo i nema potrebe da mi sad modificiramo taj objekat jer iz njega samo citamo informacije da se nesto desilo.
- Sintaksom `catch(const tip& ime) { blok;}` hvatamo iznimku tipa `tip`. To znaci da ako je negdje receno `throw obj;` i `obj` je tipa `tip`, ovaj `catch` blok ce tretirati bacenu iznimku.
- Sintaksom `catch(...)` hvatamo iznimku bilo kojeg tipa. U bloku takvog catcha mozemo tretirati sve iznimke koje se mogu desiti u nasem kodu. Ove tri tacke se zovu **elipsa**.
- Ako se prilikom tretiranja iznimke, dakle u bloku catch-a, kaze samo `throw;` bez argumenata, tada se s tog mjesta baca iznimka istog tipa kao i ona koja je dovela do tog catch bloka.

---
- Dio koda u kojem ocekujemo da ce se desiti iznimka stavljamo unutar `try { }` bloka. Nakon tog bloka stavljamo `catch(){ }` blok u kojem tretiramo eventualno bacenu iznimku u `try` bloku. Ako nemamo `try` a desi se iznimka u kodu, tada iznimka nije tretirana i vec smo rekli da ce program biti terminiran.

---
- Svaka funkcija, metod ukljucujuci i konstruktore, moze baciti iznimku osim jedne stvari a to je destruktor.
- Destruktor ne smije imati mogucnost da baci iznimku jer onda ne bismo imali onu garanciju u vezi stack unwindeing-a o kojoj smo pricali.
- Zanimljivo je da cak i konstruktor moze baciti iznimku. To se moze desiti npr. u sljedecoj situaciji. Recimo da konstruktor prima neki argument koji mu je potreban da konstruise objekat. Ako je proslijedjeni argument u stanju u kojem bi konstruktor proizveo nevalidan objekat, tada se recimo moze baciti iznimka.
- Znamo da je objekat u C++ upotpunosti konstruisan kad se obave tri stvari:
	1. Kad se alocira memorija za taj objekat
	2. Kad se inicijaliziraju clanovi u tom objektu
	3. Kad se izvrsi cijeli kod konstruktor metoda bez da se baci iznimka

---
- Stanje svakog programa je odredjeno stanjem objekata na heapu, stacku, stanjem globalnih objekata kao i stanjem registara.
- Kada se desi iznimka u kodu, program se nasao u abnormalnom stanju.
- Cilj tretiranja iznimke je da program ponovo dovede u normalno, jasno definisano stanje ako je to moguce ili ako nije, da ga terminira. A to ce najbolje uraditi bude li se tacno znalo kakva je iznimka bacena. To je stanje u kojem dodjemo kada dodjemo do `catch` bloka. 
- A da bismo mi znali rijesiti to abnormalno stanje, mi moramo jasno da znamo u kojem stanju smo program dobili kad smo dosli do `catch`-a. Dakle, kod koji prethodi iznimci, tj. kod koji se nalazi prije bacanja iznimke i kroz koji ce se proci tokom stack unwideinga, mora biti takav da nakon njegovog izvrsavanja jasno znamo u kojem stanju je nas program kad se dodje do tretiranja bacene iznimke u `catch` bloku.

---
# Exception safety

>[!example] Primjer
>Recimo da imamo `std::vector<int> vec = {1, 3, 2};`. Recimo da kazemo `try { vec.push_back(3)} catch(...) {};`. Metod `push_back` treba da alocira prostor za 4 elementa na heapu, prekopira `1, 3, 2` u taj novi prostor i da doda broj `3`. Zatim treba da prekine vezu sa starim nizom na heapu i da uspostavi vezu sa novim. Ovaj metod ce u pozadini vjerovatno pozvat niz drugih metoda da to uradi. Neki od tih metoda koji on pozove moze baciti iznimku. Mi ne znamo u kojem trenutku tj. pri kojoj operaciji se iznimka moze desiti. Kada se nama vrati objekat `vec` unutar `catch`-a, mi nemamo nikakvu garanciju kojem je stanju objekat `vec`, tj. da li se iznimka desila pri kopiranju elemenata, pri rezervaciji novog prostora na heapu ili pak negdje drugo. A cilj nam je da program dovedemo u neko jasno definisano stanje. Ovakvo stanje je sve samo ne definisano.

- Eh, da bi prelazak u novo stanje bio siguran (a u to stanje prelazimo u catch bloku nakon sto nam se desila iznimka), nas kod mora da ima neke garancije. Ovakvu sigurnost zovemo *exception safety*.
- Postoji nesto sto se zove jaka garancija ili *strong garanty*. Ovo je nesto slicno transakciji u bazama podataka gdje imamo sljedecu situaciju. Recimo da nesto treba da uradimo sa nekih podataka i da se ta akcija sastoji iz vise operacija. Taj niz operacija grupisemo u jednu transakciju. Ako se u bilo kojoj od operacija desi nesto nedefinisano, tada kazemo da je cijela transakcija nevalidna i podatke vracamo u stanje u kojem su bili prije izvrsavanja transakcije.
- Jaka garancija u kontekstu exception safety-a nam obezbjedjuje da je operacija koja eventualno proizvede iznimku reverzibilna bez ikakvih popratnih efekata. Ako pozovemo neku funkciju koja moze da baci iznimku, ako ne dodje do iznimke, ta funkcija ce validno uraditi ono sto i treba da uradi bez ikakvih problema. Medjutim, ako dodje do iznimke i kod je napisan tako da garantuje da ce u catch bloku u kojem se tretira iznimka, stanje programa biti jednako stanju u kojem je program bio prije izvrsavanja koda u kojem se desila iznimka, tada kazemo da imamo jaku garanciju.
- Dakle, ako nam neko kaze da je napisao metod `f1()` i da ima jaku garanciju spram exception safety-a to znaci sljedece: Ako se metod `f1` izvrsi kako treba, on ce uraditi ono sto i treba da radi i sve je super. Ako metod `f1` baci iznimku, tada imamo garanciju da ce program u `catch` bloku u kojem se tretira ta iznimka biti u onom stanju u kojem je bio prije nego je metod `f1` pozvan. Taj metod nam garantuje da ce sve promjene stanja programa reverzirat i vratiti ga u stanje u kojem je bio ako je doslo do iznimke.
- Mali broj operacija iz standardne biblioteke ima jaku garanciju. Jedan metod koji ima jaku garanciju je `push_back` tako da onaj primjer od maloprije sa `push_back`-om je bio samo ilustrativan a ne realan. Mi imamo garanciju da ako pozovemo `push_back` nad nekim vektorom i unutar njega se iz bilo kojeg razloga baci iznimka, ako je tretiramo, stanje vektora nad kojem smo pozvali ovaj `push_back` ce biti jednako stanju u kojem je taj vektor bio prije pozivanja tog `push_back`-a.
- Najpozeljniji i najbolji kod je onaj koji daje strong garanty.
- Ovo je jako tesko postici. Npr. recimo da smo napisali neki metod koji posalje paket na mrezu pa onda u narednom dijelu koda generise iznimku. On ne moze ici i ganjati paket na internetu da stanje vrati u ono u koje je bilo. To je nemoguce.
- Drugi nivo garancije je osnovna garancija tj. *basic garanty*. To je kada operacija koja eventualno baci iznimku, prouzrokuje poznate ireverzibilne popratne efekte ali obavezno oslobodi sve dinamicki zauzete resurse. Vecina metoda standardne biblioteke ima ovaj basic nivo garancije. To je minimum kojem trebamo teziti kada pisemo kod.
- Najbolja garancija je no-throw garancija i to je garancija da nas kod nece baciti iznimku. Funkcije i metode koji imaju no-throw garanciju potpisujemo kao `void foo() noexcept;`
- Ovu garanciju imaju svi destruktori.
- Ako metod oznacen kao `noexcept` ipak pozove `throw`, runtime ce pozvati `std::terminate` i bezuslovno terminirati program.
- Najgora garancija je nikakva garancija tj. *no garanty*. To se desava kod operacija koje u slucaju proizvedene iznimke ne pruzaju nikakvu garanciju o stanju programa tj. program je u nedefinisanom stanju. U `catch`-u, kada tretiramo takvu iznimku, jedino sta mozemo jeste da terminiramo program. Nema sanse da ga ponovo dovedemo u definisano stanje sto je i cilj catcha.

>[!abstract] Exception safety
>Sa stanovista exception safety-a, nas kod moze imati sljedece nivoe garancije:
>1. **Jaka garancija** - program se nalazi u jasno definisanom stanju A. Eventualno se desi iznimka. U catch bloku u kojem se tretira bacena iznimka garantovano nam je da ce stanje programa biti u stanju A tj. u stanju u kojem je bilo prije pozivanja metododa koji je bacio tu iznimku.
>2. **Osnovna garancija** - Ako neki metod ima osnovnu garanciju, i on pri svom izvrsavanju zauzme neki dinamicki resurs, i zatim se desi iznimka, mi tada imamo garanciju da ce svi dinamicki resursi biti oslobodjeni.
>3. **No-throw garancija** - metod koji sigurno nece baciti iznimku `void foo() noexcept;`. Ovakvi su svi destruktori.
>4. **Nema garancije** - Ako se desi iznimka, nemamo nikakvu garanciju u kojem stanju ce program biti i.e. program ce biti u nedefinisanom stanju - ne znamo sta je na heapu, stacku itd.

