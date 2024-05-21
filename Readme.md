# Program Flow

Programul intră într-un ciclu infinită, așteptând comenzi de la utilizator, cu conditia de iesire fiind comanda exit. În fiecare iterație a buclei, socket-ul este deschis și închis pentru a menține conexiunea activă cu serverul. Comanda utilizatorului este citită și, în funcție de comanda introdusă, se execută diferite acțiuni:

1. **register**: Utilizatorului i se cere să introducă un nume de utilizator și o parolă, apoi se înregistrează la server folosind aceste detalii. Daca username-ul este deja folosit, clientul este atentionat.

2. **login**: Clientului i se cere să introducă un nume de utilizator și o parolă, pentru a se autentifica la server folosind aceste detalii. Dacă autentificarea este reușită, cookie-ul de sesiune returnat de server este salvat.

3. **enter_library**: Dacă utilizatorul este autentificat (adică, login_cookie nu este NULL), tokenul returnat de server este salvat pentru viitoare actiuni asupra bibliotecii.

4. **get_books**: Dacă utilizatorul are acces la bibliotecă (adică, lib_token nu este "error"), toate cărțile din bibliotecă sunt afisate intr-un format JSON.

5. **get_book**: Dacă utilizatorul are acces la bibliotecă, se vor afisa toate detaliile legate de cartea cu id-ul introdus de client prin STDIN.

6. **add_book**: Dacă utilizatorul are acces la bibliotecă, acestuia i se cere să introducă detalii despre o carte, apoi cartea este adăugată în bibliotecă. Acele detalii legate de carte sunt trasmise prin JSON.

7. **delete_book**: Dacă utilizatorul are acces la bibliotecă, o carte specifică din bibliotecă este ștearsă, în funcție de ID-ul introdus de utilizator la tastatura.

8. **logout**: Dacă utilizatorul este autentificat, acesta este deconectat de la server.

9. **exit**: Programul se inchide fara niciun mesaj de confirmare.

Daca comanda nu este recunoscuta, clientul primeste notificare legat de asta.
------------------------------------------------------------------------------------------------------------------


# Requests to server

1) GET:
- Functia compute_get_request este responsabila pentru creearea GET request-urilor.

Aceasta este funcția primită la laborator, la care eu am adăugat și tokenul de autorizare Authorization Bearer pentru accesul la bibliotecă. 
După ce adaugă într-un buffer toate detaliile necesare request-ului, aceasta returnează buffer-ul.

2) POST
- Functia compute_post_request creeaza POST request-urile.

Aceasta este funcția primită la laborator, la care eu am adăugat și tokenul de autorizare Authorization Bearer pentru accesul la bibliotecă. 
După ce adaugă într-un buffer toate detaliile necesare request-ului, aceasta returnează buffer-ul.

3) DELETE
- Funcția compute_delete_request creează cereri DELETE. Este scrisă după modelul celei responsabile pentru POST, primită de la laborator.

Toate aceste cereri sunt trimise la server prin funcția **send_to_server** din laborator, iar răspunsurile de la server sunt primite prin funcția **receive_from_server**, tot din laborator.
------------------------------------------------------------------------------------------------------------------


# Folosirea bibliotecii nlohmann::json

Pt lucrul cu JSON am folosit biblioteca nlohmann astfel: 

1) Creez un obiect JSON gol;
2) Adaug perechile key-value în obiectul JSON.
3) Apoi convertesc obiectul JSON într-un string folosind metoda dump(), iar apoi acel string in transform intr-un sir de caractere pentru al trimite serverului.
------------------------------------------------------------------------------------------------------------------


# Explicarea algoritmului.

Fiecare comandă de la client este realizată prin funcțiile din fișierul client.cpp. Acestea urmează un tipar simplu: primesc parametrii necesari request-ului, verifica daca clientul are drepturile necesare (cookie, token), pregătesc restul de date necesare acestuia (obiecte JSON), creează request-ul necesar, îl trimit la server, așteaptă răspunsul și apoi îl interpretează, afișând la STDOUT statusul comenzii și, dacă este necesar, și răspunsul serverului, fără antet (de exemplu, get_books returnează lista de JSON de la server).
