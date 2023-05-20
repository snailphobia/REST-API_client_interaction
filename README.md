# Client HTTP - REST API

## Descriere

    Acest proiect reprezinta o aplicatie client HTTP care permite utilizatorului sa faca request-uri catre un server HTTP. Implementarea a fost realizata in C++, folosind o parte din codul sursa din laboratorul de HTTP pentru a facilita functiile de conexiune. Sunt implementate toate utilitatile cerute in enuntul temei.

## Detalii de implementare

        Am construit clase de tip Request si Response, peste care am construit metodele diferite in functie de cerinta fiecareia. De asemenea, am folosit si wrappere peste functiile din laborator pentru abstractizare mai buna. O implementare perfect completa si generica ar fi insemnat folosirea unui map 
    pentru headerele fiecarui request legat la un string reprezentand valorile in cauza, insa aici am avut nevoie doar de campurile Cookies si Authorization, pe care le-am parsat separat, pentru simplitate in scriere.

        Am folosit o implementare proprie a unui parser JSON (am dat tab cu nerusinare in fata copilotului), este practic un obiect ce poate fi atasat oricarui alt obiect si ofera functionalitatea de a transforma un string in vector de perechi si viceversa. Este nevoie de el doar in obiectele de tip POSTRequest si Response.

## Testare

~~Intentia a fost sa creez si Unit Tests, doar ca era putin mai complicat de simulat fluxul de date la tastatura in acel context. Am testat manual, cu ajutorul serverului pus la dispozitie, si am obtinut rezultatele dorite.~~

> AVEM UNIT TESTING!!!

        Deoarece checkerul pus la dispozitie in repo nu functioneaza de nicio culoare local (si probabil nici oriunde altundeva), am propus o solutie de testare a codului. Am folosit GoogleTest pentru a crea un flux similar cu cel care se cere in enuntul temei, precum si comenzi individuale.

> Usage: ./client [anything] : ruleaza testele automate; ./client (fara argumente) : ruleaza clientul "de mana"
---
> in modul de testare nu este afisat nimic, iar in modul de rulare manuala se afiseaza prompturi conform cerintei
---
> inputurile pentru testare sunt in folderul tests, pot fi modificate oricum, atat timp cat sunt in formatul cerut; altfel, cel mai probabil obiectul Request nu va fi creat, va fi returnat (void*)1 sau NULL.
---
> Dependentele pentru GTest: cmake, make, gcc, g++ (ultimele 2 versiuni, nu conteaza care e primul), in interiorul directorului depend_scripts se afla un script care instaleaza toate aceste dependente, daca nu sunt deja instalate. Scriptul a fost testat pe Ubuntu 20.04, dar ar trebui sa functioneze si pe alte distributii Linux.

    Spor la treaba!

---

~ Cezar Zlatea, 321CB
