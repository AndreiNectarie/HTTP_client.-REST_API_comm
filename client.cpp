#include <iostream>
#include <string>
#include "helpers.h"
#include "requests.h"
#include "buffer.h"


using namespace std;

void register_client(int sockfd, string username, string password) {
    char *message;
    char *response;
    nlohmann::json j;
    j["username"] = username;
    j["password"] = password;
    string data = j.dump();
    char** body_data = (char**)malloc(sizeof(char*));
    body_data[0] = (char*)malloc((data.length() + 1) * sizeof(char));
    strcpy(body_data[0], data.c_str()); 
    message = compute_post_request(HOST, REGISTER_API, JSON, body_data, data.length(), NULL, 0, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    int status_code;
    sscanf(response, "HTTP/1.1 %d", &status_code);
    if(status_code == 201){
        cout << "SUCCESS: Client registered." << endl;
    }
    else {
        cout << "ERROR: Username not available!" << endl;
    }
    free(body_data);
    free(response);
    free(message);
}

char* login_client(int sockfd, string username, string password) {
    char *message;
    char *response;
    nlohmann::json j;
    j["username"] = username;
    j["password"] = password;
    string data = j.dump();
    char** body_data = (char**)malloc(sizeof(char*));
    body_data[0] = (char*)malloc((data.length() + 1) * sizeof(char));
    strcpy(body_data[0], data.c_str()); 
    message = compute_post_request(HOST, LOGIN_API, JSON, body_data, data.length(), NULL, 0, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    int status_code;
    sscanf(response, "HTTP/1.1 %d", &status_code);
    if(status_code == 200){
        cout << "SUCCESS: Signed in." << endl;
    }
    else {
        cout << "ERROR: Username or password invalid." << endl;
        return "error";
    }
    char* set_cookie_header = strstr(response, "Set-Cookie: ");
    char* cookie = NULL;
    if (set_cookie_header) {
        set_cookie_header += strlen("Set-Cookie: "); // Skip past the "Set-Cookie: "
        char* cookie_end = strchr(set_cookie_header, ';');
        if (cookie_end) {
            // Copy the cookie into a new string
            int cookie_length = cookie_end - set_cookie_header;
            cookie = (char*)malloc((cookie_length + 1) * sizeof(char));
            strncpy(cookie, set_cookie_header, cookie_length);
            cookie[cookie_length] = '\0'; // Null-terminate the string
        }
    }
    free(body_data);
    free(response);
    free(message);
    return cookie;
}

string enter_library(int sockfd, char* cookie) {
    char *message;
    char *response;
    message = compute_get_request(HOST, LIBRARY_API, NULL, &cookie, 1, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    int status_code;
    sscanf(response, "HTTP/1.1 %d", &status_code);
    if(status_code == 200){
        cout << "SUCCESS: You now have permissions to read and write in library." << endl;
    }
    else {
        cout << "ERROR: Library access denied. (code = " << status_code << ")" << endl;
        return "error";
    }

    char* token_header = strstr(response, "token");
    char* token = NULL;
    if (token_header) {
        token_header += strlen("token\":\""); // Skip past the "token": "
        char* token_end = strchr(token_header, '"');
        if (token_end) {
            // Copy the token into a new string
            int token_length = token_end - token_header;
            token = (char*)malloc((token_length + 1) * sizeof(char));
            strncpy(token, token_header, token_length);
            token[token_length] = '\0'; // Null-terminate the string
        }
    }
    free(response);
    free(message);
    return token;
}

void get_books(int sockfd, char* lib_token, char* cookie) {
    char *message;
    char *response;
    message = compute_get_request(HOST, BOOKS_API, NULL, &cookie, 1, lib_token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    char *body = strstr(response, "\r\n\r\n");
    if (body) {
        body += 4; // Skip past the "\r\n\r\n"
    } else {
        body = response; // If no headers were found, the whole response is the body
    }

    // Copy the body into a new string to return
    char *body_copy = (char*)malloc(strlen(body) + 1);
    strcpy(body_copy, body);

    cout << "SUCCESS: " << body_copy << endl;
    free(response);
    free(body_copy);
    free(message);
}

void get_book(int sockfd, char* lib_token, char* login_cookie, int id) {
    char *message;
    char *response;
    char url[50] = "";
    strcat(url, "/api/v1/tema/library/books/");
    strcat(url, to_string(id).c_str());
    message = compute_get_request(HOST, url, NULL, NULL, 0, lib_token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    if (strncmp(response, "HTTP/1.1 200 OK", 15) != 0) {
        cout << "ERROR: Book with the id= " << id << " doesn't exist!" << endl;
        free(response);
        return;
    }
    char *body = strstr(response, "\r\n\r\n");
    if (body) {
        body += 4; // Skip past the "\r\n\r\n"
    } else {
        body = response; // If no headers were found, the whole response is the body
    }

    // Copy the body into a new string to return
    char *body_copy = (char*)malloc(strlen(body) + 1);
    strcpy(body_copy, body);
    cout << "SUCCESS: " << body_copy << endl;
    free(response);
    free(body_copy);
    free(message);
}

void add_book(int sockfd, char* lib_token, char* login_cookie, string title,
             string author, string genre, string publisher, int page_count) {
    char *message;
    char *response;
    nlohmann::json j;
    j["title"] = title;
    j["author"] = author;
    j["genre"] = genre;
    j["page_count"] = page_count;
    j["publisher"] = publisher;

    string data = j.dump();
    char** body_data = (char**)malloc(sizeof(char*));
    body_data[0] = (char*)malloc((data.length() + 1) * sizeof(char));
    strcpy(body_data[0], data.c_str());
    message = compute_post_request(HOST, BOOKS_API, JSON, body_data, data.length(), NULL, 0, lib_token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    if (strncmp(response, "HTTP/1.1 200 OK", 15) == 0) {
        cout << "SUCCESS: Book was added into the library." << endl;
    }
    else {
        char *body = strstr(response, "\r\n\r\n");
        if (body) {
            body += 4; // Skip past the "\r\n\r\n"
        } else {
            body = response; // If no headers were found, the whole response is the body
        }
        char *body_copy = (char*)malloc(strlen(body) + 1);
        strcpy(body_copy, body);
        cout << "ERROR: Book was not added into the library: " << body_copy << endl;
        free(body_copy);
    }
    free(response);
    free(message);
}

void delete_book(int sockfd, char* lib_token, char* login_cookie, int id) {
    char *message;
    char *response;
    char url[50] = "";
    strcat(url, "/api/v1/tema/library/books/");
    strcat(url, to_string(id).c_str());
    message = compute_delete_request(HOST, url, NULL, 0, lib_token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    if (strncmp(response, "HTTP/1.1 200 OK", 15) == 0) {
        cout << "SUCCESS: Book with the id= " << id << " has been removed!" << endl;
    }
    else {
        char *body = strstr(response, "\r\n\r\n");
        if (body) {
            body += 4; // Skip past the "\r\n\r\n"
        } else {
            body = response; // If no headers were found, the whole response is the body
        }
        char *body_copy = (char*)malloc(strlen(body) + 1);
        strcpy(body_copy, body);
        cout << "ERROR: Book with the id=" << id << " hasn't been removed!" << endl;
        free(body_copy);
    }
    free(response);
    free(message);
}

void logout(int sockfd, char* login_cookie) {
    char *message;
    char *response;
    message = compute_get_request(HOST, LOGOUT_API, NULL, &login_cookie, 1, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    if(strstr(response, "HTTP/1.1 200 OK") != NULL) {
        cout << "SUCCESS: You have logged out!" << endl;
    }
    else {
        char *body = strstr(response, "\r\n\r\n");
        if (body) {
            body += 4; // Skip past the "\r\n\r\n"
        } else {
            body = response; // If no headers were found, the whole response is the body
        }
        char *body_copy = (char*)malloc(strlen(body) + 1);
        strcpy(body_copy, body);
        cout << "ERROR: Logout error: " << body_copy << endl;
        free(body_copy);
    }
    free(response);
    free(message);
}

int main() {
    char *login;
    char *login_cookie = NULL;
    char *lib_token = "error";
    int sockfd;
    ssize_t n;
    string id;

    char command[50];
    while(1) {
        sockfd = start_connection();
        DIE(sockfd < 0, "ERROR: failed to connect to the server");
        string line;
        getline(cin, line);
        strncpy(command, line.c_str(), sizeof(command) - 1);
        command[sizeof(command) - 1] = '\0'; // ensure null termination
        if (strcmp(command, "register") == 0) {
            if(login_cookie != NULL) {
                cout << "ERROR: Can't register while already signed in. Logout first." << endl;
                continue;
            }
            string username, password;
            cout << "username=";
            getline(cin, username);
            cout << "password=";
            getline(cin, password);
            if(strchr(username.c_str(), ' ')) {
                cout << "ERROR: Username must not contain spaces." << endl;
                continue;
            }
            register_client(sockfd, username, password);
        }
        else if(strcmp(command, "login") == 0) {
            if(login_cookie != NULL) {
                cout << "ERROR: You are already signed in." << endl;
                continue;
            }
            string username, password;
            cout << "username=";
            getline(cin, username);
            cout << "password=";
            getline(cin, password);
            if(strchr(username.c_str(), ' ')) {
                cout << "ERROR: Username must not contain spaces." << endl;
                continue;
            }
            login_cookie = login_client(sockfd, username, password);
        }
        else if (strcmp(command, "enter_library") == 0) {
            if (login_cookie == NULL) {
                cout << "ERROR: You are not signed in." << endl;
                continue;
            }
            string token = enter_library(sockfd, login_cookie);
            lib_token = (char*)malloc((token.length() + 1) * sizeof(char));
            strcpy(lib_token, token.c_str());
        }
        else if (strcmp(command, "get_books") == 0) {
            if(lib_token == "error") {
                cout << "ERROR: Library can't be accesed without requesting permission first (enter_library)." << endl;
            }
            else {
                get_books(sockfd, lib_token, login_cookie);
            }
        }
        else if (strcmp(command, "get_book") == 0) {
            if(lib_token == "error") {
                cout << "ERROR: Library can't be accesed without requesting permission first (enter_library)." << endl;
            }
            else {
                cout << "id=";
                getline(cin, id);
                int id_int;
                try {
                    id_int = stoi(id);
                    if(id_int < 0) {
                        throw invalid_argument("Negative value");
                    }
                } catch (invalid_argument& e) {
                    cout << "ERROR: 'id' must be a positive integer." << endl;
                    continue;
                }
                get_book(sockfd, lib_token, login_cookie, id_int);
            }
        }
        else if (strcmp(command, "add_book") == 0) {
            if (lib_token == "error") {
                cout << "ERROR: Library can't be accesed without requesting permission first (enter_library)." << endl;
                continue;
            }
            string title, author, genre, publisher, page_count;
            cout << "title=";
            getline(cin, title);
            cout << "author=";
            getline(cin, author);
            cout << "genre=";
            getline(cin, genre);
            cout << "publisher=";
            getline(cin, publisher);
            cout << "page_count=";
            getline(cin, page_count);
            if(title == "" || author == "" || genre == "" || publisher == "" || page_count == ""){
                cout << "ERROR: You need to fill in all the fields in this order: title, author, genre, publisher, page_count!" << endl;
                continue;
            }
            int page_count_int;
            try {
                page_count_int = stoi(page_count);
                if(page_count_int < 0) {
                    throw invalid_argument("Negative value");
                }
            } catch (invalid_argument& e) {
                cout << "ERROR: Number of pages has to be a positive integer." << endl;
                continue;
            }
            add_book(sockfd, lib_token, login_cookie, title, author, genre, publisher, page_count_int);
        }
        else if (strcmp(command, "delete_book") == 0) {
            if (lib_token == "error") {
                cout << "ERROR: Library can't be accesed without requesting permission first (enter_library)." << endl;
                continue;
            }
            cout << "id=";
            getline(cin, id);
            int id_int;
            try {
                id_int = stoi(id);
                if(id_int < 0) {
                    throw invalid_argument("Negative value");
                }
            } catch (invalid_argument& e) {
                cout << "ERROR: 'id' must be a positive integer." << endl;
                continue;
            }
            delete_book(sockfd, lib_token, login_cookie, id_int);
        }
        else if(strcmp(command, "logout") == 0) {
            if (login_cookie != NULL) {
                logout(sockfd, login_cookie);
                login_cookie = NULL;
                lib_token = "error";
            }
            else {
                cout << "ERROR: You are not signed in." << endl;
            }
        }
        else if(strcmp(command, "exit") == 0){
            break;
        }
        else {
            cout << "ERROR: Invalid command. Make sure not to type spaces." << endl;
        }
        close_connection(sockfd);
    }
    
    return 0;
}
