- Shell je program koji sluzi da se pomocu njega pokrecu drugi procesi.
- To je dakle, program koji prima, tumaci i izvrsava komande.
- Postoje razni shellovi: bash, sh, zsh itd.
- Dakle, bash je jedna od konkretnih implementacija shella.
- Terminal je uredjaj koji je u sustini interfejs izmedju korisnika i racunara. On sam po sebi ne razumije komande koje unosimo. Mozemo ga shvatiti samo kao posrednika izmedju korisnika i basha.
- Primjeri terminala su xterm, gnome-terminal, kitty, ghosty, tty1 itd.

---
- Dakle, shell je aplikacija napravljena da se pomocu nje mogu pokretat drugi procesi.
- Moderniji shellovi idu korak dalje pa imaju svoje neke mini programske jezike koji nam omogucavaju da direktno u shellu pisemo neke programcice.
- Ali sustina je u tome da shell minimalno treba da omoguci da mi iz njega pokrenemo neki proces navodeci ime tog programa.

---
- Skup procesa zovemo grupa.
- Skup grupa procesa zovemo sesija.
- Dakle, sve procese pokrecemo iz shella sa kojim komuniciramo kroz terminal.
- Shell je onaj proces sa kojim pricamo na slave strani a terminal emulator je na master strani.
- Dakle, kada pokrenemo racunar, kroz terminal pricamo sa login procesom. Logiramo se, dobijemo kredencije usera u kojeg smo se logirali i onda se login forka i pokrece bash.
- Ono sto ukucamo u terminal se dalje proslijedjuje bash procesu koji to zna da tumaci.
- Ovaj jedan terminal ima svoju jednu sesiju.
- Svaka sesija ima svoj vodeci proces i to je u nasem slucaju bash.
- Svi procesi ce sa sobom imati asocijaciju kojoj sesiji pripadaju `sid` i kojoj grupi pripadaju `pgid` a to su id od leadera.

---

- Sesija ima jednu grupu procesa u prvom planu (foregorund) i proizvoljan broj grupa procesa u pozadini (background).
- Ona grupa procesa koja je u foregroundu ima slobodan pristup terminalu. Ti procesi u bilo kojem trenutku mogu upisati nesto u terminal ili citati nesto iz terminala.
- Ako neki proces iz backgrounda mogu pisati u terminal u bilo kojem trenutku ali ako pokusaju citat iz terminala onda ce se cijeloj toj grupi poslati signal koji pauzira procese tj. preci ce u waiting stanje sve dok ih neko ne odblokira.

>[!abstract] Dakle,
> - Kad otvorimo terminal, pokrenemo bash proces.
> - Kreira se sesija i grupa kojoj ce pripadat bash proces.
> - Taj bash proces je vodja sesije i on interpretira teksutalne komande koje upisujemo u terminal.
> - Grupa kojoj pripada bash je u foregroundu sto znaci da bash moze u bilo kojem trenutku pisati i citati u terminal bez smetnje.
> - Ako pokrenemo drugi proces sa `./a.out`, kreira se nova grupa (bash za svaki novi proces kreira novu grupu) koja pripada istoj pocetnoj sesiji i ta grupa se postavlja u prvi plan. To znaci da je sad grupa kojoj pripada bash u backgroundu i mi ne mozemo reci npr `ls`.
> - Ali ako pokrenemo drugi proces sa `./a.out &`, kreira se nova grupa koja pripada istoj pocetnoj sesiji i ta grupa se postavlja u background. Primijetimo da ako proces `a.out` ispisuje nesto na standardni izlaz, on ce to moci uraditi bez problema ali nece moci citati sa terminala. Mi sada mozemo reci npr. `ls` i izvrsit ce se `ls` u bashu. Primijetimo da ce se i napraviti treca grupa za `ls`.
> - Ako je proces sad u backgroundu, on ne moze konzumirati nikakav input od tastautre.

---

- Bash grupe procesa zove jobs.
- Sa komandom `jobs` mozemo vidjeti koje grupe imamo trenutno pri cemu se ne ispisuje grupa u kojoj je session leader. Dobit cemo neki output kao sto je npr. `[1]+ Running ./a.out &`.
- Ako hocemo da ovaj proces postavimo u foreground, to mozemo uraditi tako sto kazemo `fg %1`.
- Sa signalom Ctrl+Z mozemo foreground grupi procesa reci da se privremeno stopira, ne da se terminira nego samo da se stopira. Stanje takvog procesa je onda Stopped. 
- Ako hocemo da ga nastavimo onda mozemo koristiti `bg` ako hocemo da ga nastavimo u backgroundu, ili sa `fg` ako hocemo u foregroundu. 

# Pipeanje

- Pretpostavimo da imamo dva programa:
```cpp
// prvi.cpp
#include <iostream>
int main (int argc, char *argv[]) {
  std::cout << "hej";
  return 0;
}

// drugi.cpp
#include <iostream>
int main (int argc, char *argv[]) {
  std::string foo;
  std::cin >> foo;
  std::cout << "iz drugog " << foo;
  return 0;
}
```

- Rekli smo da kada pokrenemo neki program sa `./a.out`, tada se standardni ulaz, izlaz i error konektuju na terminal.
- Medjutim, ako izvrsimo pipeanje `./prvi | ./drugi`, tada standardni izlaz prvog vise nije terminal nego postaje standardni ulaz drugog. Prema tome, ispis komande `./prvi | ./drugi` bi bio `"iz drugog hej"`.

- U slucaju pipeanja, recimo da pokrenemo procese `A | B | C | D`, ppid za sva 4 procesa ce biti pid basha. Nece se praviti posebna grupa za svaki od ovih procesa kao sto bi se to desilo kad bismo ih pojedinacno pokretali nego ce svi pripadati grupi ciji je leader proces A tj. imat ce pgrp od A. Grupa koja ce bit u foregroundu tj tpgid ce biti postavljen na pid od A tj. grupa kojoj pripadaju A, B, C i D je foreground grupa.
- stdout(A) = stdin(B), stdout(B) = stdin(C), stdout(C) = stdin(D).

