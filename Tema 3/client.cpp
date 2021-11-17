#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "helpers.h"
#include "nlohmann/json.hpp"
#include "requests.h"

using nlohmann::json;
using namespace std;

string find_message(string str, int mode, int pos1, int pos2, char *s1, char *s2) {
    unsigned first;
    unsigned last;
    string messageStr;
    switch (mode) {
        case 0:
            first = str.find(s1);
            last = str.find_last_of(s2);
            messageStr = str.substr(first + pos1, last - first - pos2);
            break;
        case 1:
            first = str.find(s1);
            last = str.find(s2);
            messageStr = str.substr(first + pos1, last - first - pos2);
    }

    return messageStr;
}

int main(int argc, char *argv[]) {
    char *host = (char *)"34.118.48.238";
    string response;
    int sockfd;

    string cookie;
    string jwt;

    char *ip_hostname = inet_ntoa(*((struct in_addr *)gethostbyname(host)->h_addr_list[0]));
    while (1) {
        string instruction;
        getline(cin, instruction);

        if (instruction == "register") {
            if (!cookie.empty()) {
                cout << "Please logout first" << endl;
                continue;
            }

            cout << "Enter Username: ";
            string username;
            cin >> username;

            cout << "Enter Password: ";
            string password;
            cin >> password;

            // este creat JSON-ul pentru register
            json register_json{
                {"username", username},
                {"password", password},
            };

            response = server_request(&sockfd, ip_hostname, register_json, (char *)"/api/v1/tema/auth/register", cookie, "", 0);

            // Primesc succes la crearea contului
            if (response.find("201 Created") != string::npos) {
                cout << "Account has been created" << endl;
            } else {
                cout << find_message(response, 0, 10, 9, (char *)R"({"error":")", (char *)"!") << endl;
            }
        } else if (instruction == "login") {
            // login
            if (!cookie.empty()) {
                cout << "Please logout first" << endl;
                continue;
            }
            cout << "Enter username: ";
            string username;
            getline(cin, username);

            cout << "Enter password: ";
            string password;
            getline(cin, password);

            json login_json{
                {"username", username},
                {"password", password},
            };

            response = server_request(&sockfd, ip_hostname, login_json, (char *)"/api/v1/tema/auth/login", cookie, "", 0);

            // verific daca am primit cookie
            // si daca nu sunt deja logat
            if (response.find("Cookie") != string::npos && cookie.empty()) {
                cout << "Logged in as " << username << endl;
                cookie = find_message(response, 1, 0, 0, (char *)R"(connect.)", (char *)"; Path");
            } else if (cookie.empty()) {
                cout << find_message(response, 0, 10, 9, (char *)R"({"error":")", (char *)"!") << endl;
            }
        } else if (instruction == "enter_library") {
            //primesc raspunsul de la server folosind functia creata
            response = server_request(&sockfd, ip_hostname, NULL, (char *)"/api/v1/tema/library/access", cookie, jwt, 1);

            // verific daca am primit o eroare sau nu
            if (response.find("error") != string::npos) {
                // daca exista erori, le afisez aici
                cout << find_message(response, 0, 10, 9, (char *) R"({"error":")", (char *) "!") << endl;
            } else if (response.find("token") != string::npos) {
                // Salvez JWT-ul primit si afisez un mesaj corespunzator
                jwt = "Authorization: Bearer " + find_message(response, 0, 10, 11, (char *)R"({"token":")", (char *)R"("}")");
                cout << "You now have acces!" << endl;
            }
        } else if (instruction == "get_books" && !cookie.empty() && !jwt.empty()) {
            response = server_request(&sockfd, ip_hostname, NULL, (char *)"/api/v1/tema/library/books", cookie, jwt, 1);
            //verific daca am primit eroare, si o afisez
            if (response.find("error") != string::npos) {
                cout << find_message(response, 0, 10, 9, (char *)R"({"error":")", (char *)"!") << endl;
            } else if (response.find("id") != string::npos) {
                // salvez raspunsul primit in variabila BOOKS
                // dupa care prelucrez ulterior
                string books = find_message(response, 0, 0, -1, (char *)R"([{"id":)", (char *)R"("]")");

                // parsez cartile din raspuns
                json book_arr = json::parse(books);

                for (json &i : book_arr) {
                    //Afisez toate cartile cu informatii mai putine
                    cout << "ID: " << i["id"] << "," << endl;
                    cout << "Title: " << i["title"] << endl
                         << endl;
                }
            } else {
                cout << "No books" << endl;
            }
        } else if (instruction == "get_book" && !cookie.empty() && !jwt.empty()) {
            // nu exista cookie sau jwt
            // deci trebuie sa ma loghez si autorizez mai intai
            cout << "Book ID: " << endl;

            string bookId;
            cin >> bookId;

            int number = get_bookid(bookId);
            if (number == -1) {
                cout << "Please enter a valid number" << endl;
                continue;
            }

            char book_address[50];
            sprintf(book_address, "/api/v1/tema/library/books/%d", number);

            response = server_request(&sockfd, ip_hostname, NULL, book_address, cookie, jwt, 1);

            // verific daca am primit eroare de la server
            if (response.find("error") != string::npos) {
                cout << find_message(response, 0, 10, 9, (char *)R"({"error":")", (char *)"!") << endl;
            } else if (response.find(R"([{"title":)") != string::npos) {
                string book = find_message(response, 0, 0, -1, (char *)R"([{"title":)", (char *)R"("]")");

                // parsez json-ul primit
                json book_arr = json::parse(book);

                //Afisez cartea si toate informatiile
                cout << "ID: " << number << ", " << endl;
                cout << "Title: " << book_arr[0]["title"] << "," << endl;
                cout << "Author: " << book_arr[0]["author"] << "," << endl;
                cout << "Publisher: " << book_arr[0]["publisher"] << "," << endl;
                cout << "Genre: " << book_arr[0]["genre"] << "," << endl;
                cout << "Pages: " << book_arr[0]["page_count"] << endl;
            }
        } else if (instruction == "delete_book" && !cookie.empty() && !jwt.empty()) {
            // zona pentru stergerea unei carti
            string bookId;
            cout << "Enter book ID " << endl;

            cin >> bookId;
            int number = get_bookid(bookId);

            if (number == -1) {
                cout << "Please enter a valid number" << endl;
                continue;
            }
            char book_address[50];
            sprintf(book_address, "/api/v1/tema/library/books/%d", number);

            response = server_request(&sockfd, ip_hostname, "", book_address, cookie, jwt, 2);
            // verific daca am primit eroare, si afisez mesajul corespunzator.
            if (response.find("error") != string::npos) {
                cout << find_message(response, 0, 10, 9, (char *)R"({"error":")", (char *)"!") << endl;
            } else {
                cout << "Book succesfully deleted!" << endl;
            }
        } else if (instruction == "add_book" && !cookie.empty() && !jwt.empty()) {
            string title;
            cout << "Enter title: ";
            getline(cin, title);

            string author;
            cout << "Enter author: ";
            getline(cin, author);

            string genre;
            cout << "Enter genre: ";
            getline(cin, genre);

            string publisher;
            cout << "Enter publisher: ";
            getline(cin, publisher);

            string page_count;
            cout << "Enter page_count: ";
            getline(cin, page_count);

            // adaug informatiile in json
            json newBook{
                {"title", title},
                {"author", author},
                {"genre", genre},
                {"publisher", publisher},
                {"page_count", page_count},
            };

            response = server_request(&sockfd, ip_hostname, newBook, (char *)"/api/v1/tema/library/books", cookie, jwt, 3);
            // Verific erorile de la server
            if (response.find("error") != string::npos) {
                cout << find_message(response, 0, 10, 9, (char *)R"({"error":")", (char *)"!") << endl;
                // Daca primesc too many requests
            } else if (response.find("Too many requests") != string::npos) {
                cout << "Too many requests!" << endl;
            } else {
                cout << "Book added!" << endl;
            }
        } else if ((instruction == "add_book" || instruction == "get_books" || instruction == "get_book" || instruction == "delete_book")
                && (jwt.empty() || cookie.empty())) {
            cout << "Authorization missing!" << endl;
            continue;
        } else if (instruction == "logout") {
            response = server_request(&sockfd, ip_hostname, NULL, (char *)"/api/v1/tema/auth/logout", cookie, jwt, 1);
            // Verific daca exista erori
            // altfel sterg token-urile
            if (response.find("error") == string::npos) {
                // sterg jwt si cookie
                jwt.clear();
                cookie.clear();
                cout << "You are no longer logged in." << endl;
            } else {
                // afisez mesaj de eroare
                cout << "Login first" << endl;
            }
        } else if (instruction == "exit") {
            cout << "Client is closing" << endl;
            break;
        }
    }
    close_connection(sockfd);
    return 0;
}