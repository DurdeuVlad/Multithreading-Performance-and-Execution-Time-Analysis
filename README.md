


Analiza Performanței Multithreading și a Timpului de Execuție

Student: Durdeu Vlad Ioan

Structura Sistemelor de Calcul

Universitatea Tehnică din Cluj-Napoca

20 Octombrie 2024









Contents
Analiza Performanței Multithreading și a Timpului de Execuție	1
Contents	2
1. Introducere	3
1.1. Context	3
1.2. Obiective	3
1. Cercetare Bibliografică	4
2.1 Concepte de Multithreading	4
2.2 Măsurarea Timpului de Execuție în Medii Multithreading	5
2.3 Factori care Influențează Performanța	5
3. Analiză	6
3.1 Arhitectura și Structura aplicației	6
3.2 Analiza sincronizării și gestionării resurselor	6
4. Design	6
4.1 Diagrama de use case	7
4.2 Structura modulară și interfața cu utilizatorul	9
4.3 Implementarea algoritmilor și optimizări	9
4.4 Măsurarea și înregistrarea performanței	10
4.5 Procesarea rezultatelor	10


































Introducere

1.1. Context 
În domeniul informaticii moderne, optimizarea performanței este o preocupare esențială, în special atunci când se lucrează cu aplicații care necesită resurse intensive. Una dintre cele mai eficiente metode de îmbunătățire a performanței este utilizarea multithreading-ului, o tehnică de programare în care mai multe fire de execuție rulează simultan, folosind mai bine resursele procesorului și reducând timpul de execuție. 
Cu toate acestea, gestionarea și analiza timpului de execuție al programelor multithreading prezintă provocări specifice. Acest proiect explorează aceste dinamici prin măsurarea și analizarea timpului de execuție global și pe fir, având ca scop furnizarea de perspective asupra modului în care numărul de fire de execuție influențează performanța. Prin această analiză, urmărim să identificăm configurațiile optime pentru utilizarea firelor de execuție și eficiența timpului de execuție.
1.2. Obiective 
Obiectivul principal al acestui proiect este de a proiecta, implementa și analiza un program multithreading care măsoară și compară timpii de execuție globali și pe fir de execuție. Pentru a asigura performanța optimă, software-ul va fi dezvoltat folosind limbajul de programare C, cu unele instrucțiuni în limbaj de asamblare incluse pentru a reduce ineficiențele de timp asociate frecvent cu limbajele de nivel înalt.
Proiectul va realiza următoarele obiective: Dezvoltarea unui program multithreading care permite utilizatorilor să interacționeze prin comenzi din linia de comandă. Programul va oferi măsurători de timp de execuție și va permite configurarea dinamică a numărului de fire de execuție.
Implementarea algoritmilor cheie atât în C, cât și în asamblare, pentru a măsura timpul de execuție al diferitelor operații. Algoritmii testați includ: Algoritmi de sortare
Bubble Sort
Quick Sort
Merge Sort
Insertion Sort
Selection Sort
Heap Sort
Operații pe matrice
Înmulțirea matricei
Adunarea matricei
Transpunerea matricei
Algoritmul Strassen pentru înmulțirea matricei
Algoritmi de căutare:
Căutare liniară
Căutare binară
Furnizarea comenzilor utilizatorului pentru a interacționa cu programul, inclusiv:
run <algoritm> <fire_start> <fire_end> <dimensiune_date_start> <dimensiune_date_end>: Execută algoritmul specificat cu un anumit număr de fire (2^fire) și dimensiune a datelor  de inceput si final (se va lua 2^dimensiune_date_start si 2^dimensiune_date_end).
test <algoritm> <fire_start> <fire_end>: Execută algoritmul specificat cu un anumit număr de fire  (2^fire) și dimensiuni variate ale datelor.
analyze <algoritm>: Realizează analiza performanței pe un algoritm selectat cu diferite numere de fire și dimensiuni variate ale datelor.
verbose <true/false>: Activează/dezactivează modul detaliat.
help: Afișează comenzile disponibile și instrucțiunile de utilizare.

Cercetare Bibliografică
2.1 Concepte de Multithreading
Multithreading este o tehnică puternică utilizată în programare pentru a permite ca mai multe părți ale unui program să fie executate simultan, ducând la o mai bună utilizare a CPU-ului și o performanță mai rapidă. Fiecare fir de execuție este o unitate ușoară, independentă de execuție în cadrul unui proces, care împărtășește același spațiu de memorie cu alte fire. Aceasta îl face mai eficient decât procesele, care operează în spații de memorie separate. Conform literaturii, firele sunt cele mai mici unități de execuție din cadrul unui proces, în timp ce procesele sunt mai consumatoare de resurse ale sistemului [1]. Una dintre diferențele de bază în multithreading este între concurență și paralelism. Concurența înseamnă că mai multe sarcini progresează simultan, dar nu neapărat în același timp. Paralelismul, pe de altă parte, se referă la execuția simultană propriu-zisă, posibilă pe procesoare multi-core [2]. 
Totuși, gestionarea mai multor fire presupune costuri de comutare de context, care implică salvarea și restaurarea stării firelor atunci când se trece de la un fir la altul. Acest lucru poate crea o suprasarcină care scade performanța [2]. Un alt concept important este cel al pool-urilor de fire, care sunt utilizate pentru a minimiza suprasarcina asociată cu crearea și distrugerea firelor. Pool-urile de fire permit reutilizarea firelor, îmbunătățind performanța prin reducerea timpului petrecut pentru configurarea firelor noi pentru sarcini [1].

2.2 Măsurarea Timpului de Execuție în Medii Multithreading
Măsurarea timpului de execuție în programele multithreading este esențială pentru evaluarea eficienței lor. Una dintre metodele cele mai comune este utilizarea timpului de ceas, care măsoară timpul efectiv care a trecut de la începutul până la finalul execuției unui program. Această metodă ia în considerare tot timpul necesar unui sistem pentru a finaliza o sarcină, inclusiv timpul de așteptare pentru resurse [3]. 
O altă măsură importantă este timpul de CPU, care se concentrează pe cantitatea de timp pe care CPU-ul o petrece executând activ programul. Acesta este diferit de timpul de ceas, deoarece exclude perioadele idle când CPU-ul nu lucrează la sarcina respectivă. În mediile multithreading, este, de asemenea, esențial să se urmărească performanța firelor individuale. Acest lucru se poate realiza folosind funcții de timp specifice firelor, cum ar fi clock_gettime() în C, pentru a măsura timpul petrecut de fiecare fir într-o sarcină [3].
2.3 Factori care Influențează Performanța
Mai mulți factori pot influența performanța programelor multithreading. Balansarea sarcinii este unul dintre aspectele critice. Dacă anumitor fire li se atribuie prea multă muncă, în timp ce altele rămân sub utilizate, eficiența generală a sistemului va scădea [2]. Asigurarea unei distribuții echitabile a sarcinilor între fire poate face o diferență semnificativă în performanță. 
De asemenea, securitatea firelor joacă un rol esențial în menținerea performanței. Când mai multe fire accesează resurse partajate, sunt necesare mecanisme de sincronizare, cum ar fi mutex-urile sau semafoarele, pentru a preveni probleme precum condițiile de cursă sau blocajele. 
Totuși, sincronizarea necorespunzătoare poate crea blocaje de performanță [2]. În final, un alt factor care poate degrada performanța este suprasarcina comutării de context. Pe măsură ce sunt introduse mai multe fire, sistemul trebuie să comute între ele, ceea ce adaugă latență. În sisteme cu un număr mare de fire, această suprasarcină poate încetini execuția programului [3].
3. Analiză
Obiectivul principal al analizei este identificarea modului în care diferiți algoritmi reacționează la creșterea numărului de fire de execuție și impactul acestei variabile asupra timpului total de execuție și utilizării resurselor procesorului. Folosind aceste date, se va putea măsura performanța multicore a procesorului dat.
3.1 Arhitectura și Structura aplicației
Aplicația este structurată într-o manieră modulară, pentru a facilita extinderea și întreținerea codului. Aceasta utilizează biblioteca thread, care permite crearea și gestionarea eficientă a firelor de execuție. Aplicația dispune de o interfață de linie de comandă prin care utilizatorul poate selecta algoritmul dorit, poate specifica numărul de fire și dimensiunea datelor.
Arhitectura include module dedicate pentru fiecare tip de algoritm: sortare, operații pe matrice și căutare. Fiecare modul gestionează atât algoritmul specific, cât și masurarea timpului pentru fiecare fir. Pentru a evita conflictele între fire, acestea sunt izolate iar fiecare lucrează doar cu o parte mica din datele de intrare sau cu o copie a datelor de intrare folosind principiul imutabilitatii datelor.
3.2 Analiza sincronizării și gestionării resurselor
Pentru a asigura o distribuție echilibrată a sarcinilor între fire, aplicația implementează tehnici de împărțire a datelor, cum ar fi divizarea vectorului sau a matricei în segmente egale, fiecare alocat unui fir specific. Totuși, această abordare nu funcționează la fel de eficient pentru toate tipurile de algoritmi, cum se întâmplă în cazul algoritmilor de căutare, unde balansarea sarcinii nu este mereu uniformă.
Pentru operațiile care utilizează resurse partajate, aplicația folosește mutex-uri pentru a preveni accesul simultan la resursele comune, minimizând astfel riscurile de inconsistență a datelor. De asemenea, se măsoară impactul comutării de context, iar creșterea numărului de fire induce o suprasarcină de comutare care poate afecta performanța.



4. Design
Designul aplicației este gândit pentru a asigura utilizatorului o interfață simplă, cu puține comenzi însă explicate detaliat, dar și modularitatea necesară pentru o eventuală extindere prin adăugarea unei interfețe grafice sau a mai multor algoritmi.
4.1 Diagrama de use case 


	Diagrama de use case detaliază o bucla de execuție a programului pentru toate inputurile posibile de la user.
4.2 Structura aplicației și interfața de utilizator


Aplicația este organizată modular, fiecare algoritm fiind separat într-un modul propriu care gestionează logica de calcul. Măsurarea timpului se face într-un mod generalizat într-un singur loc, aplicandu-se o strategie de Divide et Impera bazat pe dacă algoritmul are la baza date plasate într-un tablou sau matrice  
Interfața de linie de comandă simplifică inițierea execuției unui algoritm specific, configurând numărul de fire și dimensiunea datelor. Dacă una sau ambele date nu sunt specificate, atunci algoritmul va face un test folosind date generale generate universal.
4.3 Implementarea algoritmilor și optimizări
	Fiecare algoritm este împărțit în trei categorii: de sortare, de căutare și de operațiuni pe matrice. Folosind principiul mostenirii, se va crea o clasa Algorithm care va fi extinsă de alte trei clase abstracte care reprezinta cele trei categorii mentionate mai sus. Algoritmii declarați la secțiunea 1.2 vor extinde una dintre cele trei clase abstracte și vor implementa doar operațiunea. Algorithm va avea funcțiile generice de măsurat timpul de execuție, împărțirea pe thread-uri a execuției codului dar și semnăturile funcțiilor virtuale care vor fi definite în clasele copil și care răspund de generarea datelor și impartirea responsabilitatilor între thread-uri.
În cazul algoritmilor de sortare, aplicația implementează metode clasice, precum Bubble Sort și Quick Sort, folosind divizarea vectorului inițial între fire, astfel încât fiecare fir să sorteze o parte din date. După finalizarea sortării, vectorul va fi interclasat iar rezultatul returnat.
 Pentru algoritmii de căutare, aplicația experimentează cu divizarea datelor în blocuri și distribuirea acestora între fire. Totuși, pentru algoritmii de căutare, pentru a asigura balansarea eficientă a datelor, atunci cand unul dintre fire a găsit numărul căutat, va opri execuția a firelor rămase și se vor returna rezultatele culese.
Pentru algoritmi de calcul a matricelor, împărțirea se va implementa în submatrici iar fiecare fir de lucru va primi responsabilitatea construirii acelei submatrici. În vederea evitării problemelor de sincronizare, fiecare fir de execuție va primii și o copie a datelor originale.
La final, se va construii rezultatul din rezultatele parțiale oferite de fiecare fir de lucru. și se verifică corectitudinea răspunsului dat.
4.4 Măsurarea și înregistrarea performanței
Pentru evaluarea performanței, aplicația măsoară timpul de execuție global, precum și timpul CPU pentru fiecare fir. Funcția clock_gettime() este utilizată pentru a obține date precise despre execuția fiecărui fir. Aceste măsurători permit analiza și verificarea modului de împărțire a taskului dat pentru a ne asigura de corectitudinea modului de lucru.
Se va masura si migrarea thread-urilor pentru fiecare bucla executata de fiecare thread.
4.5 Procesarea rezultatelor
	După executarea algoritmului, toate datele returnate de firele de lucru sunt centralizate și prelucrate pentru afișare. Datele furnizate de algoritm sunt de asemenea verificate pentru a asigura corectitudinea lor. În cazul în care rezultatul final nu este corect se va returna o eroare.
Bibliografie:
Multithreading and concurrency fundamentals - Cameron Wilson
Concurrency and Performance - James William Steven Parker
Context Switching Overhead - Satyadeep Ashwathnarayana
Hardware Influence on Multithreading - Mario Nemirovsky Dr., Professor 


