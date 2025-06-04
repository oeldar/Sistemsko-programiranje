- Rekli smo da ako vise niti zeli pristupiti istom resursu samo radi citanja ili bilo kojeg konstantnog metoda, tada nema problema.
- Medjutim, ako bar jedna nit zeli vrsiti nekonstantan metod nad resursom kojem mogu pristupiti i druge niti, tada imamo stanje utrke.
- Rekli smo da stanje utrke rjesavamo upotrebom sinhronizacijskih primitiva.
- Sinhronizacijski primitiv koji smo koristili je `std::mutex`.
- Nacin na koji koristimo `mutex` je sljedeci:
- Prije kriticnog koda (tj. dijela koda u kojem pristupamo dijeljenom resursu sa nekonstantnim metodom) pozvat cemo

```cpp
// neka imamo globalnu varijablu std::mutex mtx;

mtx.lock();
// kriticna sekcija
mtx.unlock();

// drugi nacin je:
std::lock_guard<std::mutex> m{mtx};
// kriticna sekcija
```

- Na operativnim sistemima smo koristili spinlock. Medjutim ta metodologija nije bas efikasna jer ce nit koja ceka da se spinlock releasa konstantno biti u fazonu hej jesil gotov, hej jesil gotov troseci resurse procesora.
- Ako koristimo mutex, niti koje cekaju na resurs koji je zakljucan se prebacuju u waiting stanje i time ne trose resurse procesora.

---
- Dakle rekli smo da nam `std::mutex` obezbjedjuje ekskluzivno vlasnistvo nad resursom.
- Tri stvari koje OS mora obezbijediti da bi imao multitreding su: infrastrukturu za kreiranje niti, mutex i condition varijable.

---
- Recimo da imamo funkciju `foo` koja ce nad nekim objektom sa heapa pozivati samo konstantan metod. Npr. citat ce taj objekat.
- Ako vise threadova istovremeno izvrsava `foo`, tada nema stanja utrke jer svaki vrsi samo konstantne metode nad dijeljenim objektom sa heapa.
- Medjutim, pretpostavimo da imamo funkciju `bar` koja ce mijenjati taj isti objekat sa heapa. Ona, ako postuje thread safety, treba prije operacije mijenjanja tog objekta lockati mutex i nakon toga ga unlockati da ne bi doslo do potencijalnog stanja utrke kada bi vise niti izvrsavalo funkciju `bar`.
- Eh, problem koji mutex ima je taj sto ako je objekat zakljucan, i iz drugog threada koji izvrsava `foo` koja zeli samo da procita taj objekat, ona to nece moci uraditi jer mutex nema metodologiju pomocu koje moze zakljuciti da li neki metod zeli da cita zakljucani objekat ili da ga mijenja.
- U tom slucaju ce cak i thread na kojem se izvrsava `foo` koji ne zeli da mijenja objekat nego samo da ga cita ici u waiting iako ga treba samo procitati. To nije bas lijepo. Zato postoji *condition varijabla*.

---
- Al hajmo mi prije condition varijable da pogledamo sta su atomski tipovi.
- Atomski tip je wrapper za klasicni tip koji omogucava da se operacije nad objektom tog tipa izvrsavaju atomski. To znaci da ih nista ne moze prekinuti. Razmisljajmo kao da se takve operacije na nivou asemblera prevode u jednu instrukciju i samim tim ne postoji mogucnost da ona bude prekinuta.
- Npr. ako bismo pisali funkciju `foo`

```cpp
void foo(shared_ptr<int> p) {
	*p++;
}

int main(){
	shared_ptr<int> ptr = std::make_shared<int>(0);
	std::thread t1{foo, ptr};
	std::thread t2{foo, ptr};
	t1.join();
	t2.join();
	return 0;
}
```

- Ovdje ocigledno imamo problem jer vise niti pristupa istom resursu `p` na heapu. Imamo stanje utrke.

- Prvi nacin da ovo rijesimo je koristeci mutex:

```cpp
std::mutex mtx;

void foo(shared_ptr<int> p) {
	std::lock_guard<std::mutex> lg{mtx};
	*p++;
}

int main(){
	shared_ptr<int> ptr = std::make_shared<int>(0);
	std::thread t1{foo, ptr};
	std::thread t2{foo, ptr};
	t1.join();
	t2.join();
	return 0;
}
```

- Medjutim kada bismo sad imali jos jednu nit `t3` koja bi izvrsavala fju `bar` koja samo recimo couta `*p`, nit `t3` bi morala da ceka dok druge niti ne zavrse sa svojim izvrsavanjem iako ona samo hoce da cita. I hajde oke, jos uvijek ne rjesavamo taj problem ali eto samo da naglasim.

- Drugi nacin kako bismo ovo mogli uraditi je sljedeci:

```cpp
void foo(shared_ptr<atomic<int>> p) {
	*p++;
}

int main(){
	shared_ptr<std::atomic<int>> ptr = std::make_shared<std::atomic<int>>(0);
	std::thread t1{foo, ptr};
	std::thread t2{foo, ptr};
	t1.join();
	t2.join();
	return 0;
}
```

- Ako neko kaze da ima lockless data strukturu to znaci da je koristio atomske tipove za sinhronizaciju a ne mutex.
- Eh sad mozemo na uslovne varijable.

---
- Uslovne varijable omogucavaju neekskluzivni pristup nekoj kriticnoj sekciji koda.