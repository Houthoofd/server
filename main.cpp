#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

const char* root_dir = "/var/www/html"; // Change this to the root directory of your web server

void serve_file(int client_socket, const string& filename, const string& content_type) {
    string full_path = root_dir + filename;
    cout << full_path << endl;
    ifstream file(full_path.c_str());

    if (!file.is_open()) {
        stringstream error_msg;
        error_msg << "Failed to open file: " << full_path << endl;
        write(client_socket, error_msg.str().c_str(), error_msg.str().size());
        return;
    }

    stringstream response_body;
    response_body << file.rdbuf();
    file.close();

    stringstream response_header;
    response_header << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: " << content_type << "\r\n"
                    << "Content-Length: " << response_body.str().size() << "\r\n"
                    << "Connection: close\r\n"
                    << "\r\n";

    write(client_socket, response_header.str().c_str(), response_header.str().size());
    write(client_socket, response_body.str().c_str(), response_body.str().size());
}

void serve_404(int client_socket) {
    stringstream response_body;
    response_body << "<html><head><title>404 Not Found</title></head>"
                  << "<body><h1>404 Not Found</h1></body></html>";

    stringstream response_header;
    response_header << "HTTP/1.1 404 Not Found\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << response_body.str().size() << "\r\n"
                    << "Connection: close\r\n"
                    << "\r\n";

    write(client_socket, response_header.str().c_str(), response_header.str().size());
    write(client_socket, response_body.str().c_str(), response_body.str().size());
}

void handle_client(int client_socket) {
    char buffer[4096];
    int bytes_read = read(client_socket, buffer, sizeof(buffer));

    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }

    string request(buffer, bytes_read);

    size_t start = request.find_first_of(' ');
    size_t end = request.find_first_of(' ', start+1);

    if (start == string::npos || end == string::npos) {
        serve_404(client_socket);
        close(client_socket);
        return;
    }

    string method = request.substr(0, start);
    string uri = request.substr(start+1, end-start-1);

    if (uri == "/") {
      uri = "/index.html";
    }

    size_t ext_start = uri.find_last_of('.');
    string ext = uri.substr(ext_start+1);
    cout << ext << endl;

    string content_type = "text/plain";

    if (ext == "html") {
        content_type = "text/html";
    } else if (ext == "css") {
        content_type = "text/css";
    } else if (ext == "js") {
        content_type = "application/javascript";
    }
    string full_path = root_dir + uri;
    cout << full_path << endl;
    ifstream file(full_path.c_str());
    if (!file.is_open()) {
      serve_404(client_socket);
      return;
    }
    stringstream response_body;
    response_body << file.rdbuf();
    file.close();
    stringstream response_header;
    response_header << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: " << content_type << "\r\n"
                    << "Content-Length: " << response_body.str().size() << "\r\n"
                    << "Connection: close\r\n"
                    << "\r\n";
    write(client_socket, response_header.str().c_str(), response_header.str().size());
    write(client_socket, response_body.str().c_str(), response_body.str().size());


    serve_file(client_socket, uri, content_type);
    close(client_socket);
}

int main(int argc, char** argv) {
    int server_socket = 0;
    int opt = 1;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        cerr << "Failed to create server socket: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        cerr << "Failed to set socket options: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        cerr << "Failed to bind server socket: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == -1) {
        cerr << "Failed to listen on server socket: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Server listening on port 8080..." << endl;

    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);

        if (client_socket == -1) {
            cerr << "Failed to accept client connection: " << strerror(errno) << endl;
            continue;
        }

        handle_client(client_socket);
    }

    return 0;
}
