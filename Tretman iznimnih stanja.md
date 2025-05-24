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
- 