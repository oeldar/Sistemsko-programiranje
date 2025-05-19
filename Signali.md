- Signal je poruka da se dogodio neki dogadjaj.
- Te poruke dobijaju procesi a one nose informaciju o odredjenom dogadjaju u sistemu.
- Razliciti dogadjaji u unix sistemima generisu razlicite signale.
- Izvor signala moze biti neki uredjaj ili neki drugi proces.
- Te signale procesu dostavlja kernel.
- Proces procesira njemu dostavljene signale.
- Svaki signal ima ime, broj i akciju.
- Ime i broj predstavljaju identifikator signala. To je fiksno u unix specifikaciji. Dakle, desi se neki dogadjaj A. On generise signal koji ima neko simbolicko ime i brojcani identifikator.
- Osim toga, signal ima i akciju.
- Akcija predstavlja sta proces treba da uradi kad mu bude dostavljen taj signal.
- Neke od mogucih akcija su:
	- terminiranje izvodjenja procesa
	- zaustavljanje izvodjenja procesa
	- nastavljanje izvodjenja procesa
	- ignorisanje signala
	- procesiranje signala putem handler funkcije - svaki proces moze imati posebnu funkciju koju ce izvrsiti ako mu se dostavi odredjeni signal. Ovu akciju mi custom pisemo. 

---

- Postoji mnogo mogucih signala na unix sistemu.
- Svaki od njih ima neku svoju defaultnu akciju koju ce izvrsit proces kojem se dostavi taj signal.
- Akcije `Term` i `Core` terminiraju proces. Razlika je sto akcija `Core` tj. core dumped jos generise i neke podatke koji su korisni za debagiranje. Ali obje akcije terminiraju proces.
- Mi ove defaultne akcije mozemo i promijeniti u odredjenom procesu tako sto napisemo custom handler za neki signal i onda ga *instaliramo* u proces.
- To je moguce uraditi za sve signale osim za signal `SIGKILL` ciji je broj `9` i cija je akcija ta da terminira proces kojem se dostavi ovaj signal.
- Vecina signala na unixu vrsi terminiranje.
- Npr. signal `SIGCHILD` ce biti poslan procesu parentu kad god neki njegov child proces buide terminiran ili zaustavljen.

---

## Generisanje signala

- Signale generisu razni izvori.
- To mogu biti uredjaji, drugi procesi ali i sam kernel.
- Uobicajeno neko generise signal, kernel dostavlja taj signal procesu a proces ga procesira.
- Npr. Ctrl+C generise `SIGINT` signal. Ovime terminiramo proces. Dakle, akcija koja se izvrsi u procesu kada on primi `SIGINT` signal je po defaultu `Term`.
- Ctrl+Z generise `SIGSTP` signal. To je signal kojim stopiramo proces koji je u foregroundu. Dakle, akcija koju izvrsi proces kada primi `SIGSTP` signal je `Stop`.
- Iz samog basha mozemo bilo kojem procesu poslati signal pomocu komande `kill`.
- Sintaksa je sljedeca: `kill -ImeSignala pid` gdje umjesto imena signala mozemo koristiti i njegov brojcani identifikator, npr. `kill -SIGINT 2834` ce procesu sa pidom 2834 poslati `SIGINT` signal. Ako taj proces nema neki custom handler za taj signal, onda ce on biti terminiran.
- Bash ovom komandom u pozadini poziva sistemski poziv `int kill(pid_t pid, int sig)`
- Proces moze sam sebi poslati signal koristeci sistemski poziv `int raise(int sig)`.
- Osim toga, postoji jos nacina da proces sam sebi generise signal:
	- ako se u procesu desi dijeljenje s nulom, on ce generisati i sam sebi poslati signal `SIGFPE`
	- ako programski brojac postavimo na dio memorije koji nije kod i fetchiramo tu *instrukciju* koja je u tom slucaju ilegalna (jer nije istrukcija), dobit cemo `SIGILL` signal
	- ako dereferenciramo dangling pointer ili ilegalno pristupimo memoriji na neki drugi nacin, generisat cemo signal `SIGSEGV`.
	- svi ovi signali po defaultu uzrokuju `Core` akciju tj. core dumped.

---

- Za svaki od signala, kernel ima pointer na funkciju koja ce se pozvati kad odredjeni proces primi taj signal. To je ona defaultna akcija u sustini.
- Defaultnu akciju mozemo promijeniti osim za `SIGKILL` i `SIGSTOP` pisanjem custom handlera.
- Recimo da smo napisali funkciju koja se zove `handler` i zelimo da se ona pozove ako proces primi signal `sginum`, to cemo omoguciti pozivanjem sistemskog poziva `signal(signum, handler)`. Funckija `handler` mora vracati `void` a primati `int`
- Ako nakon ovog hocemo nazad da vratimo da nam akcija bude default akcija tada cemo na mjesto handler fje da stavimo makro `SIG_DFL`, a ako hocem oda se signal ignorise onda cemo postaviti makro `SIG_IGN`. 

```c
#include <stdio.h>
#include <signal.h>

void mojHandler(int sig) {
	printf("Procesiram signal.");
}

int main() {
	signal(SIGSTP, SIG_IGN);
	signal(SIGINT, mojHandler);
	while(1)
		;
	return 0;
}
```

---

- Dakle, signale dobijaju procesi koji su u foregroundu a ne dobijaju ih oni procesi iz backgrounda.
- Kada korisnik u terminalu uradi Ctrl+Z, svim procesima u foregroundu se salje signal `SIGTSTP` cija je default akcija zaustavljanje izvrsenja procesa.
- Jedan od signala koji postoji je `SIGALRM` koji se salje kada u programu pozovemo fju `alarm(n)` koja generise taj signal nakon `n` sekundi.

---

- Rekli smo da procesi iz backgrounda mogu slobodno pisati u terminal ali da ne mogu citat iz njega. Ako pokusaju citat iz terminala, bit ce stopirani jer ce im se poslati signal `SIGTTIN` cija je default akcija `Stop`. 
- Postoji i signal `SIGTTOU` koji terminal salje ako proces iz pozadine pokusa ispisat nesto na kontrolni terminal. Medjutim, nas terminal je podesen da defautlno ne salje ovaj signal. Mi to ponasanje mozemo promijeniti rekonfigurisanjem terminala npr. koristeci `stty` utility. Osim toga, mozemo i overrideat handler za ove signale u procesu ako zelimo da se on drugacije ponasa.
- Ako kazemo `stty tostop` ti si reko terminalu da procesi iz backgrounda ne smiju ni pisat u terminal sto bi inace smjeli tj. terminal ce im slat `SIGTTOU`.
- Uzgred receno, da vidis konfiguraciju svog terminala mozes koristit `stty -a`. U otuputu ces imat listu flagova sa ili bez `-` ispred naziva flaga. Ako je `-` ispred naziva flaga, to znaci da u tom terminalu ta postavka nije aktivirana.
- Komanda `stty sane` resetuje postavke terminala na deafultne.
- Signal `SIGHUP` postoji iz vremena kada je terminal bio fizicki uredjaj. On se salje kada se terminal iskljucio sa racunara. Danas, kada terminal vise ne postoji kao fizicki uredjaj, ovaj signal se salje kada se ugasi aplikacija koja predstavlja emulator terminala npr. xterm, ghosty ili gnome-terminal.
- Kada zatvorimo emulator terminala, proces koji je bio taj emulator salje signal `SIGHUP` bashu koji je leader te sesije. Defaultna akcija za `SIGHUP` je term, medjutim bash je overrideao tu akciju svojim custom handlorom na nacin da svim procesima u svojoj sesiji posalje jedan `SIGHUP` signal koji ga onda tumace kao `Term`.
- Ako hocemo da pokrenemo proces i da ga otkacimo od trenutne sesije tako da ne prima ovaj signal, to mozemo uraditi tako sto kazemo `nohup <program>` gdje umjesto `<program> &` stavljamo ime programa kojeg hocemo da pokrenemo i da se ne terminira kada se zatvori terminal emultor. Standardni izlaz ovog procesa onda nece biti terminal nego fajl koji se zove `nohup.out`. To i ima smisla, jer kada izgubimo terminal, ovaj proces treba da nastavi sa svojim izvrsavanjem a na sta ce pisati ako nesto treba ako je izgubio terminal, pa zato mu standardni izlaz postane navedeni fajl.

---

- Rekli smo da kernel dostavlja signal procesu, a proces ga procesira koristeci odgovarajuci handler. Sad cemo pogledat kako to detaljnije funkcionise.
- Pod dispozicijom signala podrazumijevamo da kernel za svaki signal odnosno njegov broj ima pointer na funkciju koja handla taj signal kad se nekom procesu on dostavi. Dakle, to je neka tabela pointera na funkcije.
- Osim toga, kernel za svaki proces biljezi i bit maske. Postoji pending i blocked baska.
- Dakle, u `proc` strukturi imamo maske `pending` i `blocked`.
- Kernel dostavlja odredjeni signal nekom procesu, tako sto u njegovoj `proc` strukturi promijeni odgovarajuci `pending` bit spram id-a signala kojeg dostavlja. Sa stanovista kernela, time je signal dostavljen procesu. Signal jos uvijek nije procesiran.
- Logicno pitanje koje mozemo postaviti: Sta ako imam neki proces koji spava i dostave mu se dva signala istog tipa? Posto imamo samo jedan `pending` bit po signalu u `proc` strukturi, bit ce procesiran samo jedan dostavljeni signal.
- Ako je u blocked bit maski odredjeni bit setovan na 1, to znaci da je signal asociran sa tim bitom blokiran. Mi time ne zabranjujemo dostavu signala, samo ga blokiramo.
- Sistemskim pozivom `fork` child nasljedjuje dispoziciju od roditelja kao i listu blokiranih signala ali ne i pending status. To je zato jer su signali generisani i namijenjeni su procesu spram njegovog pida. Sjetimo se da je pid childa razlicit od pida parenta pa ako je nesto namijenjeno parentu, bilo bi pogresno da to procesira njegov child. 
- Sistemskim pozivom `exec` resetuje se dispozicija i lista blokiranih signala.

---

- Da bi proces procesirao signal, on mora biti u running stanju sto je logicno jer mora izvrsiti odgovarajuci handler za taj signal.
- Pretpostavimo da se izvrsava proces A. Neki proces B procesu A posalje signal. Umjedjuvremenu, scheduler je proces A prebacio u `RUNNABLE` stanje i taj proces ceka da ga scheduler ponovo odabere i pocene izvrsavat tj da ga prebaci u `RUNNING` stanje.
- Kada se desi da scheduler odabere proces A za nastavak izvrsavanja, do sad smo ucili da ce se proces A nastavit izvrsavat od instrukcije gdje je poslednji put stao. 
- Medjutim, prije nego se to desi, kernel odradi sljedecu analizu:
	- Pogleda bit masku `pending` tog procesa. Ta bit maska je u sustini vektor bita koji ima jedinice na onim mjestima koji odgovaraju signalima koji je dostavljen tom procesu. Npr recimo da su procesu A dostavljen signal ciji su brojcani identifikatori 2 i 5. Vektor `pending` tog procesa ce imati `1` na 2. i 5. indeksu  a na svim ostalim mjestima nule.
	- Pogleda `blocked` masku tog procesa. To je u susitini vektor bita koji ima jedinice na onim mjestima koji odgovaraju signalima koji su blokirani za taj proces.
	- Kernel zatim proracuna vektor bita `vek` na nacin `vek = pending & ~blocked`.
	- Primijetimo da ce vektor `vek` imati jedinice na onim mjestima koji odgovaraju primljenim signalima koji ujedno nisu blokirani.
	- Ako vektor `vek` ima sve nule, to znaci da taj proces nije primio niti jedan signal ili da je primio neki signal ali je on blokiran.
- Nakon sto kernel proracuna `vek` i svi biti u njemu imaju vrijedjnost `0`, proces nastavlja da se izvrsava gdje je ranije bio prekinut.
- Ako u vektoru `vek` postoje biti koji su `1`, kernel bira najnizi koji je `1` i forsira proces da proces pocne izvrsavati registrovanu akciju za taj signal. Neka je to $k$-ti indeks. Tada se $k$-ti pending bit postavlja na 0 a $k$-ti blocked bit na 1. Kada se okonca akcija, $k$-ti blocked bit se vraca na 0. Dakle, dok traje izvrsavanje handlera, blokiramo signale istog tipa.

---

## Blokiranje signala

- Postoji nacin da rucno blokiramo signale u nekom procesu.
- Dakle, onom `blocked` maskom pored kernela, moze manipulisati i korisnik putem posebnog API-a.
- Za tu namjenu koristimo posebnu data strukturu `sigset_t`. U toj data strukturi cuvamo informaciju koje signale zelimo da blokiramo. To je u sustini kontejner bita. Redni broj bita je brojcani identifikator signala kojeg hocemo da blokiramo, kao i u `blocked` maski.
- Terminologija je da ako je signal unutar seta, tada nas proces blokira taj signal.

>[!abstract] API za manipulaciju signalima
>Neka smo definisali instancu ove data strukture sa `sigset_t sset`.
>- Ako hocemo da odblokiramo sve signale, to radimo pozivanjem fje `sigemptyset(&sset)`
>- Ako hocemo da blokiramo sve signale, to radimo pozivajuci fju `sigfillset(&sset)`
>- Ako hocemo da dodamo signal `signo` u set to radimo sa `sigaddset(&sset, signo)`
>- Ako hocemo da izbrisemo signal `signo` iz seta to radimo sa `sigdelset(&sset, signo)`
>- Ako nas zanima da li je signal `signo` unutar seta to radimo sa `sigismember(&sset, signo)`

- Potrebno je samo jos jednu stvar da uradimo a to je da kernelu kazemo da ovaj set koji smo napravili koristi kao `blocked` bit masku za nas proces. U tu svrhu koristimo funkciju `sigprocmask(how, &sset, &oldset)`.
- `how` moze biti
	- `SIG_BLOCK` - u tom slucaju blokiramo one signale koji se nalaze u `sset`
	- `SIG_UNBLOCK` - u tom slucaju odblokiramo one signale koji se nalaze u `sset`

>[!abstract] Zasto nam ovo treba?
>Pa pretpostavimo da u mainu koristimo neku globalnu varijablu `var`. Neka nas proces moze primati signal npr. `SIGINT` za kojeg smo postavili nas custom handler `handler`. Neka funkcija `handler` takodjer pristupa globalnoj varijabli `var`. Ovo potencijalno uzrokuje race condition. Ideja je sljedeca:
>- Blokirajmo signal `SIGINT` u mainu sve dok ne zavrsimo sa pristupom globalnoj varijabli `var` u mainu. Kad zavrsimo, odblokirat cemo signal `SIGINT`.
>```c
>int main() {
>	 signal(SIGINT, handler);
>	 // ...
>	 sigset_t sset;
>	 sigemptyset(&sset);
>	 sigaddset(&sset, SIGINT);
>	 sigprocmask(SIG_BLOCK, &sset, NULL);
>	 var += 100;
>	 sigprocmask(SIG_UNBLOCK, &sset, NULL);
>	 // ...
>}

---

- U novijim verzijama, postoji drugi nacin mijenjanja dispozicije i bit maski koristenjem drugih sistemskih poziva i drugih data struktura.
- Instaliranje handler funkcije pozivom funkcije `signal` ima problem na odredjenim sistemima jer
	- Recimo da smo sa `signal(SIGINT, mojHandler)` promijenili akciju za signal `SIGINT` na `mojHandler`. Kada se prvi put procesu dostavi i proces pocne procesirat signal `SIGINT`, izvrsit ce se `mojHandler` kao akcija za taj signal. Medjutim, nakon tog prvog izvrsavanja, handler za signal `SIGINT` ce se postaviti na default akciju.
	- Neki unix sistemi su imali sljedeci problem: Recimo da proces procesira signal A. Taj proces bi trebao da blokira signale istog tipa tj. signale A sve dok ne zavrsi sa procesiranjem tog prvobitnog signala A. Odredjeni unix sistemi to nisu mogli.
- Ove probleme rjesava funkcija `sigaction`. 
- Definisemo strukturu `struct sigaction sa;`
- Varijabla `sa` nam sluzi da u nju ubacimo informacije o dispoziciji signala kao i odgovarajuce bit maske. Sjetimo se jos jednom da pod dispozicijom podrazumijevamo tabelu pointera na funkcije koje se pozivaju kao handleri za signale a pod maskama podrazumijevamo `pending` i `blocked` bit maske. Bit maska `pending` ima jedinicu na onom mjestu koje odgovara brojacnom identifikatoru pristiglog signala a maska `blocked` jedinicu na onom mjestu koje odgovara brojcanom identifikatoru signala koji je blokiran.
- Ranije smo koristili funkciju `signal(signo, handler)` da mijenjamo dispoziciju signala. Ovdje koristimo ovu data strukturu `sa`.
- Metodologija je da polje `sa_handler` u ovoj strukturi inicijaliziramo handlerom kojeg smo napisali: `sa.sa_handler = foo;` gdje je `void foo(int sig)`.
- Koristeci ovaj api mozemo dodavati i odredjene flagove npr. `sa.sa_flags = SA_RESTART | SA_NOCLDSTOP`.
	- Recimo da imamo proces A koji se forkira i dobije child proces B. Kada B bude stopiran ili terminiran, on ce svom parentu tj. A-u poslati signal `SIGCHLD`. Default akcija za ovaj signal je ignorisanje. Dakle, parent ce ignorisati ovaj signal kad ga dobije od svog childa. Sa flagom `SA_NOCLDSTOP` u procesu A omogucavamo da ako se A forkira, njegov child nece slati signal `SIGCHLD` procesu A ako bude stopiran sto bi inace radio ali ce ga slati kad se terminira.
	- `SA_RESTART` - ne znam

