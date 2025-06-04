## `std::unique_ptr`

- Rekli smo da kod raw pointera moramo voditi racuna o tome da resurs koji smo alocirali na heapu pomocu raw pointera obavezno moramo deleteati inace cemo imati resource leak.
- Da mi to ne bismo radili, uvodimo pojam pametnog pointera koji je wrapper za raw pointer.
- Jedan pametni pointer je `std::unique_ptr`. U nastavku vidimo kako mozemo kreirati `unique_ptr`
![[Pasted image 20250604194605.png]]
- Vidimo neke nacine na koje mozemo kreirati `unique_ptr`. Primijetimo da ga ne mozemo kreirati copy konstrukcijom.
- Ovaj pametni pointer je namijenjen za unikatno vlasnistvo nad nekim resursom. Tako ako imamo dva pametna pointera `p1` i `p2` i pokusamo ih konstruisati koristeci isti raw pointer, dobit cemo compile time error.
- Medjutim, dozvoljeno je `std::move`-ati `unique_ptr`. Tada taj kojeg smo moveali postaje `nullptr`.
- Kada imamo objekat `p` tipa `std::unique_ptr`, tada nad tim objektom mozemo raditi sve sto bismo radili nad raw pointerom npr. `*p.a`, `p->foo()` itd. 

>[!abstract] Najbitnije operacije nad objektom tipa `std::nullptr`
>- **Kreiranje:** `auto ptr = std::make_unique<Foo>(4);`
>- **Pristup objektu:** `ptr->metod();` ili `(*ptr).metod();`
>- **Provjera validnosti:** `if (ptr) {}` ili `if (ptr != nullptr) {}`
>- **Premjestanje vlasnistva:** - `std::unique_ptr<Foo> ptr1= std::make_unique<Foo>(4);` i nakon toga `std::unique_ptr<Foo> ptr2 = std::move(ptr1)l`. Od sada je `ptr1 == nullptr` a `ptr2` je vlasnik.
>- **Oslobadjanje vlasnistva** - `Foo* raw_ptr = ptr.release();`. Sada `ptr` postaje `nullptr` ali smo odgovorni da izvrsimo `delete raw_ptr`
>- **Resetovanje:** `ptr.reset(new Foo(20))` - brise stari objekat i postavlja novi. `ptr.reset()` - brise stari objekat a `ptr` postavlja na `nullptr`
>
>- **Pristup raw pointeru:** `Foo* raw = ptr.get()` - daje ti `Foo*` ali ti ne prenosi vlasnistvo. Ti ne smijes reci `delete`.


Kao vjezba rezonuj o sljedecem kodu:

```cpp
#include <iostream>
#include <memory>
#include <string>
using namespace std;
struct radnik {
    string ime;
    int godine;
    radnik(string i, int g) : ime{i}, godine{g} {
        cout << "kreiran: " << ime << endl;
    }
    ~radnik() { cout << "unisten: " << ime << endl; }
};
void foo(radnik* ptr) { std::cout << ptr->godine << std::endl; }
int main(void) {
    auto u_ptr = make_unique<radnik>("abc", 15);
    foo(u_ptr.get());
    u_ptr.reset(new radnik{"anon", 20});
    foo(u_ptr.get());
    return 0;
}
```

```cpp
#include <iostream>
#include <memory>
#include <string>
using namespace std;
struct radnik {
    string ime;
    int godine;
    radnik(string i, int g) : ime{i}, godine{g} {}
    ~radnik() { cout << "unisten" << endl; }
};
void foo(radnik* ptr) { std::cout << ptr->godine << std::endl; }
int main(void) {
    auto u_ptr = make_unique<radnik>("abc", 15);
    {
        auto ptr = move(u_ptr);
        std::cout << ptr->ime << std::endl;
        foo(ptr.get());
    }
    std::cout << (u_ptr == nullptr) << std::endl;
    return 0;
}
```

---
## `std::shared_ptr`

- Kod `unique_ptr`-a smo rekli da samo jedan objekat tog tipa moze biti vlasnik nekog resursa na heapu. Dakle, situacija poput

```cpp
Foo* foo = new Foo(3);
std::unique_ptr<Foo> p1(foo);
std::unique_ptr<Foo> p2(p1);
```

nije moguca jer zelimo da i `p1` i `p2` budu vlasnici istog objekta na heapu. Moguce je imati raw pointer i `unique_ptr` na isti objekat na heapu ali nije moguce imati dva `unique_ptr`-a. Dobit cemo gresku u kompajliranju jer je copy konstruktor izbrisan za ovaj tip.

- Tu nam u igru dolazi `std::shared_ptr`.
- Za ovo nam vazi isto sve kao i za `unique_ptr` u smislu da mozemo isto koristiti operator `*` i `->` kao da je raw pointer u pitanju.
- Evo primjera kako bismo ga mogli koristiti:
![[Pasted image 20250604201007.png]]
- Primijetimo da se destruktor pozove samo jednom i to kada nestane i zadnji objekat koji je pokazivao na objekat koji se destruisao.
- I ovdje je bolje da koristimo ovaj nacin konstrukcije `std::shared_ptr<Foo> p = std::make_shared<Foo>(2);`
![[Pasted image 20250604201812.png]]
- `.reset()` nad objektom tipa `shared_ptr` ce smanjiti broj referenci nad objektom u cijem je vlasnistvu taj bio. Ako je to bio poslednji, onda ce i deleteati taj objekat.

---

## `std::weak_ptr`

- Rekli smo da objekat tipa `std::unique_ptr` ima unikatno vlasnistvo nad nekim objektom na heapu i niko vise to ne moze imati.
- Objekte tipa `std::shared_ptr` mozemo kopirati i imati proizvoljan broj objekata tog tipa koji gledaju na isti objekat na heapu.
- Objekat tipa `std::weak_ptr` sluzi da referencira na objekat tipa `std::shared_ptr`. Znaci njega cemo praviti nesto kao

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(2);
std::weak_ptr<int> wp(sp);
```

- Dakle nakon ovoga ce `sp.use_count()` biti `1` a isto tako ce i `wp.use_count()` biti `1`.
- Dakle, `std::weak_ptr` ne povecava broj referenci. Kazemo da `std::weak_ptr` nema vlasnistvo nad objektom za razliku od `std::shared_ptr` i `std::unique_ptr`.
- Ako zelimo da provjerimo da li objekat postoji, koristimo `wp.expired()` koji vraca `true` ako objekat postoji i `false` ako ne postoji.
![[Pasted image 20250604203307.png]]
- Kad kazemo postoji, to znaci da provjeri da li je `nullptr`.
- `wp.lock()` vraca novi `shared_ptr` koji ce biti u vlasnistvu onog na sta `wp` pokazuje.
![[Pasted image 20250604203646.png]]