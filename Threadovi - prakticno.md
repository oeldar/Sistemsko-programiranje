- Rekli smo da nit kreiramo pomocu objekta `std::thread nit{foo, argumenti_za_foo}`
- Od tog trenutka imamo nit koja se paralelno izvrsava sa niti u kojom je ona kreirana.
- Nit na kernel strani moze biti u stanjima u kojima moze biti i proces: running, runnable, waiting itd.
- Na user strani tj. u programu kojeg pisemo, nit moze biti u
	- joinable stanju
	- unjoinable stanju
- Nit koja je tek kreirana je u joinable stanju i takva ostaje sve dok nad njom ne pozovemo `.join()` ili `.detach()`
- Ako napustimo opseg u kojem je kreirana nit i u kojem je ona ostala u joinable stanju, pozvat ce se `std::terminate` i program ce terminirati. To ne valja.
- Dakle, prije nego napustimo opseg u kojem je deklarisana nit, ta nit treba biti prebacena u unjoinable stanje metodom `join()` ili `detach()`

---
- Ako nad nekom niti dva puta pozovemo `.join()` ili `.detach()`, program ce terminirati. To ne valja. Zato je uvijek potrebno, prije nego pozovemo `join()` ili `detach()` provjeriti da li je nit u joinable stanju:

```cpp
std::thread t{foo};
if (t.joinable())
	t.join(); // ili t.detach();
```



