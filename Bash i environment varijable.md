- U bashu mozemo kreirati proizvoljan broj varijabli sljedecom sintaksom
```sh
ime_var=vrijednost
```
npr.
```
FOO=bar
```
- Od ovog trenutka u instanci basha u kojoj smo kreirali ovu bash varijablu, postoji varijabla imena `FOO` vrijednosti `bar`.
- Njoj mozemo u svakom trenutku pristupiti preko `$FOO` radi citanja ili pisanja. 
- Bash varijablu ispisujemo sa `echo $FOO`
- Medjutim, ovo je bash varijabla a nije environment varijabla.
- To znaci da o ovoj varijabli brine sam bash proces dok o enironment varijablama brine kernel.
- Ako bismo forkali trenutnu instancu basha i echoali `FOO` dobili bismo prazan ispis jer u toj novoj instanci basha nema ove bash varijable.
- Bash varijablu mozemo unaprijediti u environment varijablu i tada ce ona biti vidljiva u svim procesima koji nastanu forkanjem basha.
- To radimo tako sto kazemo `export FOO`
- Od tog trenutka, bash varijabla `FOO` nije samo bash varijabla nego i environment varijabla. 
- Environment varijable se cuvaju u `proc` strukturi procesa u vidu mape koja cuva parove kljuc tipa string u kljuc tipa string.
- Sada je u  mapi environemnt varijabli `proc` strukture trenutnog basha dodan par `(FOO, bar)` a rekli smo da ako napravimo novu instancu procesa forkanjem, ta instanca nasljedjuje sva polja iz `proc` strukture svog roditelja pa tako i onu mapu u kojoj su environment varijable.

- Ako odmah hocemo da kreiramo enviroment varijablu putem basha to mozemo jednom linijom
```
export BAR=tar
```
- gdje se, jasno, prvo kreirala bash varijabla `BAR` pa se onda upgradeala u environment varijablu.
- 