Nume
Serie


    Pentru implementarea temei folosesc nlohmann pentru parsarea informatiilor.
Caut IP-ul serverului folosind functia gethostbyname. Dupa ce am gasit
IP-ul corect, citesc input-ul de la tastatura folosind getline.

    Exit: doar afisez mesajul corespunzator si inchid conexiunea cu serverul si 
programul.

    Register: verific daca am deja cookies adaugate si daca nu exista atunci pot inregistra utilizatorul
curent. Informatiile salvate in JSON sunt trimise apoi la server. Dupa ce deschid conexiunea, astept un raspuns,
iar in functie de raspunsul primit, afisez un mesaj corespunzator.

    Login: verific din nou daca exista cookies si daca nu exista urmez acelasi flow ca la register,
salvez informatiile in JSON, compun mesajul, il trimit, si astept un raspuns. Salvez cookie-ul primit
in string-ul "authroization".

    Logout: daca sunt logat, sterg cookie-ul si jwt-ul pentru librarie.

    Enter_library: trimit mesajul catre server, cu tot cu cookie-ul salvat la login, si daca primesc JWT,
atunci afisez un mesaj de succes, altfel un mesaj de eroare.

    Get_books: Trimit request-ul catre server, cu tot cu jwt-ul pentru autorizare, si parsez folosind
nlohmann pentru a le afisa mai usor.

    Get_book (doar una): verific daca numarul oferit de utilizator este un numar valid, dupa care trimit
request catre server si afisez cartea ceruta (in caz de eroare, afisez eroarea). 

    Add_book: verific sa am acces la biblioteca si sa fiu logat, dupa care trimit mesajul cu informatiile 
cartii catre server. In functie de raspuns, afisez un mesaj diferit.

    Delete_book: verific ca id-ul sa fie valid (sa fie numar valid), si daca am accesul necesar (cookie + jwt) atunci
sterg cartea respectiva. Daca primesc eroare, afisez eroarea.

    Pentru a ma asigura de o conexiune activa tot timpul, deschid o noua conexiune cu serverul la fiecare
comanda.