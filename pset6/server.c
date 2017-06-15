//
// server.c
//
// Computer Science 50
// Problem Set 6
//

// feature test macro requirements
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED

// limits on an HTTP request's size, based on Apache's
// http://httpd.apache.org/docs/2.2/mod/core.html
#define LimitRequestFields 50
#define LimitRequestFieldSize 4094
#define LimitRequestLine 8190

// number of bytes for buffers
#define BYTES 512

// header files
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// types
typedef char BYTE;

// prototypes
bool connected(void);
void error(unsigned short code);
void freedir(struct dirent** namelist, int n);
void handler(int signal);
char* htmlspecialchars(const char* s);
char* indexes(const char* path);
void interpret(const char* path, const char* query);
void list(const char* path);
bool load(FILE* file, BYTE** content, size_t* length);
const char* lookup(const char* path);
bool parse(const char* line, char* path, char* query);
const char* reason(unsigned short code);
void redirect(const char* uri);
bool request(char** message, size_t* length);
void respond(int code, const char* headers, const char* body, size_t length);
void start(short port, const char* path);
void stop(void);
void transfer(const char* path, const char* type);
char* urldecode(const char* s);

// server's root
char* root = NULL;

// file descriptor for sockets
int cfd = -1, sfd = -1;

// flag indicating whether control-c has been heard
bool signaled = false;

int main(int argc, char* argv[])
{
    // a global variable defined in errno.h that's "set by system 
    // calls and some library functions [to a nonzero value]
    // in the event of an error to indicate what went wrong"
    // 
    // errno, i.e., error number describes whatever error occurred using library functions and nonzero values
    errno = 0;

    // default to port 8080
    int port = 8080;

    // string describing server.c usage
    const char* usage = "Usage: server [-p port] /path/to/root";

    // parse command-line arguments
    int opt;
    
    // int getopt(int argc, char * const argv[], const char *optstring);
    // continues to return "option characters", i.e. command line flags like "-p" or "-h",
    // until there are no more flags, in which case getopt() returns -1
    
    // stores port number if specified with -p option
    while ((opt = getopt(argc, argv, "hp:")) != -1)
    {
        switch (opt)
        {
            // -h
            case 'h':
                printf("%s\n", usage);
                return 0;

            // -p port
            case 'p':
                port = atoi(optarg);
                break;
        }
    }

    
    // ensure port is a non-negative short and path to server's root is specified (usu public)
    if (port < 0 || port > SHRT_MAX || argv[optind] == NULL || strlen(argv[optind]) == 0)
    {
        // announce usage
        printf("%s\n", usage);

        // return 2 just like bash's builtins
        return 2;
    }

    // start server with port num. specified by port, folder specified by argv[optind]
    start(port, argv[optind]);

    // listen for SIGINT (aka control-c)
    // act is a struct for storing the handle continuously
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    
    // sigaction function call changes action taken in response to a specific signal
    // in this case, it sets server.c's response to control-c to be "act"
    // handle contains the signal checking function
    sigaction(SIGINT, &act, NULL);

    // a message and its length
    // initialize message pointer and length descriptor
    char* message = NULL;
    size_t length = 0;

    // path requested
    // initialize path pointer
    char* path = NULL;

    // so far, the relevant pointers are the message, the message length, and the path
    // we also setup the structs for stopping the server via control-c
    
    // accept connections one at a time
    // infinite loop.. continuously check for new clients and requests, as well as the stop signal
    while (true)
    {
        // free last file path, if any
        if (path != NULL)
        {
            free(path);
            path = NULL;
        }

        // free last message, if any
        if (message != NULL)
        {
            free(message);
            message = NULL;
        }
        // reset message descriptor (length)
        length = 0;

        // close last client's socket, if any
        // each client needs to setup a socket to send and receive requests/responses
        if (cfd != -1)
        {
            close(cfd);
            cfd = -1;
        }

        // check for control-c
        if (signaled)
        {
            stop();
        }

        // check whether client is connected, if so, check for requests
        if (connected())
        {
            // if connected, check for their requests
            if (request(&message, &length)) // store the whole request and it's length
            {
                // extract message's request-line
                // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
                
                // Request-line is formatted as such: Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
                // Method : GET/POST/etc
                // Request-URI : Requested resource
                // HTTP-Version : Self-explanatory (e.g. 1.1)
                // CRLF signals the end, it's like a new line
                const char* haystack = message;
                
                // strstr returns a pointer to location of \r\n in the haystack string, aka CRLF
                const char* needle = strstr(haystack, "\r\n");
                // If no matches are found, set errno and continue
                if (needle == NULL)
                {
                    error(500);
                    continue;
                }
                // line as in request-line
                char line[needle - haystack + 2 + 1]; // length of desired info is needle + \r\n + \0
                strncpy(line, haystack, needle - haystack + 2); // copy over desired info from haystack to line
                line[needle - haystack + 2] = '\0'; // set last character of line to null character

                // log request-line
                printf("%s", line);

                // parse request-line
                char abs_path[LimitRequestLine + 1];
                char query[LimitRequestLine + 1];
                if (parse(line, abs_path, query))
                {
                    // URL-decode absolute-path
                    char* p = urldecode(abs_path);
                    if (p == NULL)
                    {
                        error(500);
                        continue;
                    }

                    // resolve absolute-path to local path
                    path = malloc(strlen(root) + strlen(abs_path) + 1);
                    if (path == NULL)
                    {
                        error(500);
                        continue;
                    }
                    
                    // root is the root folder, we have to append the absolute path to it to specify the local path
                    strcpy(path, root);
                    strcat(path, p);
                    free(p);
                    
                    // ensure path exists and can be accessed
                    if (access(path, F_OK) == -1)
                    {
                        error(404);
                        continue;
                    }

                    // if path to directory
                    struct stat sb;
                    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
                    {
                        // redirect from absolute-path to absolute-path/
                        if (abs_path[strlen(abs_path) - 1] != '/')
                        {   
                            // Uniform Resource Identifier (uri) is a string of characters used to identify a resource
                            char uri[strlen(abs_path) + 1 + 1];
                            strcpy(uri, abs_path);
                            strcat(uri, "/");
                            redirect(uri);
                            continue;
                        }

                        // use path/index.php or path/index.html, if present, instead of directory's path
                        char* index = indexes(path);
                        if (index != NULL)
                        {
                            free(path);
                            path = index;
                        }

                        // list contents of directory
                        else
                        {
                            list(path);
                            continue;
                        }
                    }
                    
                    // if it's not a path to a directory....
                    // look up MIME type for file at path
                    const char* type = lookup(path);
                    if (type == NULL)
                    {
                        error(501);
                        continue;
                    }

                    // interpret PHP script at path
                    if (strcasecmp("text/x-php", type) == 0)
                    {
                        interpret(path, query);
                    }

                    // transfer file at path
                    else
                    {
                        transfer(path, type);
                    }
                    
                    // By this point, you've resolve the file-path of the requested resource
                    // and you've dealt with the requested file's file-type accordingly
                    // either interpreting it, or "transferring" it
                }
            }
        }
    }
}

/**
 * Checks (without blocking) whether a client has connected to server. 
 * Returns true iff so.
 */
bool connected(void)
{
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    socklen_t cli_len = sizeof(cli_addr);
    cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
    if (cfd == -1)
    {
        return false;
    }
    return true;
}

/**
 * Responds to client with specified status code.
 * Builds the message from template -> body -> full response
 */
void error(unsigned short code)
{
    // determine code's reason-phrase
    // depending on error code submitted, returns a different string
    const char* phrase = reason(code);
    if (phrase == NULL)
    {
        return;
    }
    
    // template for response page's content
    char* template = "<html><head><title>%i %s</title></head><body><h1>%i %s</h1></body></html>";

    // render template
    // array size of body should be strlen(template) as a base
    // -2 twice for the %i %s 's
    // log10(code)+1 returns num digits in the code
    // strlenphrase returns num chars in the error descriptor message
    // not sure why it's *2+1
    
    char body[(strlen(template) - 2 - ((int) log10(code) + 1) - 2 + strlen(phrase)) * 2 + 1];
    // stores the filled-in template in the body string
    int length = sprintf(body, template, code, phrase, code, phrase);
    if (length < 0)
    {
        body[0] = '\0';
        length = 0;
    }

    // respond with error
    char* headers = "Content-Type: text/html\r\n";
    respond(code, headers, body, length);
}

/**
 * Frees memory allocated by scandir.
 * if the dirent struct pointer passed in is not null, each element is freed, then the struct itself is freed
 */
void freedir(struct dirent** namelist, int n)
{
    if (namelist != NULL)
    {
        for (int i = 0; i < n; i++)
        {
            free(namelist[i]);
        }
        free(namelist);
    }
}
 
/**
 * Handles signals
 * Checks the signal integer passed in, modifies the global "signaled" bool accordingly
 */
void handler(int signal)
{
    // control-c
    if (signal == SIGINT)
    {
        signaled = true;
    }
}

/**
 * Escapes string for HTML. Returns dynamically allocated memory for escaped
 * string that must be deallocated by caller.
 * 
 * "Escaping" a character simply means printing it in a safe manner
 * This is needed for odd characters such as ">" which might break the program
 */
char* htmlspecialchars(const char* s)
{
    // ensure s is not NULL
    if (s == NULL)
    {   
        return NULL;
    }

    // allocate enough space for an *unescaped copy* of s
    char* t = malloc(strlen(s) + 1);
    if (t == NULL)
    {
        return NULL;
    }
    // t is initialized as null
    t[0] = '\0';

    // iterate over characters in s, escaping as needed
    for (int i = 0, old = strlen(s), new = old; i < old; i++)
    {
        // escape &
        if (s[i] == '&')
        {
            // escape the character
            const char* entity = "&amp;";
            // increment size of t due to extra chars in escaping
            new += strlen(entity);
            // dynamically allocate memory for t
            t = realloc(t, new);
            // check if memory was allocated
            if (t == NULL)
            {
                return NULL;
            }
            // append the escaped character to t
            strcat(t, entity);
        }
        
        // repeat same process as above
        // escape "
        else if (s[i] == '"')
        {
            const char* entity = "&quot;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // escape '
        else if (s[i] == '\'')
        {
            const char* entity = "&#039;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // escape <
        else if (s[i] == '<')
        {
            const char* entity = "&lt;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // escape >
        else if (s[i] == '>')
        {
            const char* entity = "&gt;";
            new += strlen(entity);
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, entity);
        }

        // don't escape
        else
        {
            strncat(t, s + i, 1);
        }
    }

    // escaped string! If escaping process is unclear, see ampersand example    
    return t;
}

/**
 * Checks, in order, whether index.php or index.html exists inside of path.
 * Returns path to first match if so, else NULL.
 */
char* indexes(const char* path)
{
    char* end_of_path = NULL;
    end_of_path = strrchr(path,'/');
    char* php_path = malloc(end_of_path - path + 2 + 9);
    char* html_path = malloc(end_of_path - path + 2 + 10);
    
    strcpy(php_path, path);
    end_of_path = strrchr(php_path,'/');
    strcpy(end_of_path + 1, "index.php");
    
    if(!access(php_path, F_OK))
        return php_path;
        
    strcpy(html_path, path);
    end_of_path = strrchr(html_path,'/');
    strcpy(end_of_path + 1, "index.html");
    
    if(!access(html_path, F_OK))
        return html_path;
        
    return NULL;
}

/**
 * Interprets PHP file at path using query string.
 * Opens a pipe to the php interpreting, loads the interpreted content, responds with
 * dynamically generated HTML page
 */
void interpret(const char* path, const char* query)
{
    // ensure path to the php file is readable
    if (access(path, R_OK) == -1)
    {
        error(403);
        return;
    }

    // open pipe to PHP interpreter
    char* format = "QUERY_STRING=\"%s\" REDIRECT_STATUS=200 SCRIPT_FILENAME=\"%s\" php-cgi";
    char command[strlen(format) + (strlen(path) - 2) + (strlen(query) - 2) + 1];
    if (sprintf(command, format, query, path) < 0)
    {
        error(500);
        return;
    }
    // popen() opens a pipe to php-cgi (the php interpreter), which gives us the file pointer
    FILE* file = popen(command, "r");
    if (file == NULL)
    {
        error(500);
        return;
    }

    // load interpreter's content into memory (&content, &length)
    char* content;
    size_t length;
    if (load(file, &content, &length) == false)
    {
        error(500);
        return;
    }

    // close pipe
    pclose(file);

    // subtract php-cgi's headers from content's length to get body's length
    char* haystack = content;
    
    //\r\n is linereturn and newline, i.e. "CRLF"
    // we look for the end of the header, which is the end of the first line
    // signaled by two CRLF's
    char* needle = strstr(haystack, "\r\n\r\n");
    if (needle == NULL)
    {
        free(content);
        error(500);
        return;
    }

    // extract headers from content
    char headers[needle + 2 - haystack + 1];
    strncpy(headers, content, needle + 2 - haystack);
    headers[needle + 2 - haystack] = '\0';
    
    // respond with interpreter's content
    respond(200, headers, needle + 4, length - (needle - haystack + 4));

    // free interpreter's content that was previously allocated
    free(content);
}

/**
 * Responds to client with directory listing of path.
 */
void list(const char* path)
{
    // ensure path is readable and executable
    if (access(path, R_OK | X_OK) == -1)
    {
        error(403);
        return;
    }
    
    // open directory
    DIR* dir = opendir(path);
    if (dir == NULL)
    {
        return;
    }

    // buffer for list items
    // allocate one byte of memory and initialize it as null
    char* list = malloc(1);
    list[0] = '\0';

    // iterate over directory entries
    struct dirent** namelist = NULL;
    // find num of items in the specified directory
    int n = scandir(path, &namelist, NULL, alphasort);
    // iterate over each item
    for (int i = 0; i < n; i++)
    {
        // omit . from list
        if (strcmp(namelist[i]->d_name, ".") == 0)
        {
            continue;
        }

        // escape entry's name using previos htmlspecialchars fcn
        char* name = htmlspecialchars(namelist[i]->d_name);
        // if name couldn't be escaped..
        if (name == NULL)
        {
            free(list);
            freedir(namelist, n);
            error(500);
            return;
        }

        // append list item to buffer
        char* template = "<li><a href=\"%s\">%s</a></li>";
        // dynamically adjust memory size of list string
        list = realloc(list, strlen(list) + strlen(template) - 2 + strlen(name) - 2 + strlen(name) + 1);
        // check if memory was allocated
        if (list == NULL)
        {
            free(name);
            freedir(namelist, n);
            error(500);
            return;
        }
        // check if the fully concatenated string was formed without errors
        if (sprintf(list + strlen(list), template, name, name) < 0)
        {
            free(name);
            freedir(namelist, n);
            free(list);
            error(500);
            return;
        }

        // free escaped name
        free(name);
    }

    // free memory allocated by scandir
    freedir(namelist, n);

    // prepare response
    const char* relative = path + strlen(root);
    char* template = "<html><head><title>%s</title></head><body><h1>%s</h1><ul>%s</ul></body></html>";
    char body[strlen(template) - 2 + strlen(relative) - 2 + strlen(relative) - 2 + strlen(list) + 1];
    int length = sprintf(body, template, relative, relative, list);
    if (length < 0)
    {
        free(list);
        closedir(dir);
        error(500);
        return;
    }

    // free buffer
    free(list);

    // close directory
    closedir(dir);

    // respond with list
    char* headers = "Content-Type: text/html\r\n";
    respond(200, headers, body, length);
}

/**
 * Loads a file into memory dynamically allocated on heap.
 * Stores address thereof in *content and length thereof in *length.
 */
bool load(FILE* file, BYTE** content, size_t* length)
{
    *length = 0;
    *content = NULL;
    BYTE curr_byte;
    BYTE* buffer;
    
    while(fread(&curr_byte, 1, 1, file) != 0){
           
        if(*length == 0)
            buffer = malloc(sizeof(BYTE));
        else
            buffer = realloc(buffer, *length + 1);
            
        memcpy(buffer + *length, &curr_byte, 1);
    
        (*length)++;
    }
    
    buffer = realloc(buffer,*length + 1);
    *(buffer + *length) = '\0';
    
    *content = buffer;
    
    
    return true;
}

/**
 * Returns MIME type for supported extensions, else NULL.
 * Useful functions include strcasecmp, strcpy, and/or strrchr
 */
const char* lookup(const char* path)
{
    char* start;
    start = NULL;
    
    start = strrchr(path,'.');
    
    // if start points to the '.' char, start-path, by way of pointer arithmetic
    // is the number of chars between start and path + start itself.
    char filetype[strlen(path) - (start-path + 1) + 1];
    strcpy(filetype,start+1);
    
    if(!strcasecmp(filetype,"css"))
        return "text/css";
        
    else if(!strcasecmp(filetype,"html"))
        return "text/html";
    
    else if(!strcasecmp(filetype,"gif"))
        return "image/gif";
        
    else if(!strcasecmp(filetype,"ico"))
        return "image/x-icon";
    
    else if(!strcasecmp(filetype,"jpg"))
        return "image/jpeg";
    
    else if(!strcasecmp(filetype,"js"))
        return "text/javascript";
        
    else if(!strcasecmp(filetype,"php"))
        return "text/x-php";
    
    else if(!strcasecmp(filetype,"pnt"))
        return "image/png";
        
    else
        return NULL;
}

/**
 * Parses a request-line, storing its absolute-path at abs_path 
 * and its query string at query, both of which are assumed
 * to be at least of length LimitRequestLine + 1.
 */
bool parse(const char* line, char* abs_path, char* query)
{
    char* first_SP = strchr(line,' ');
    char* last_SP = strrchr(line,' ');
    char* err_ptr = NULL;
    char method[4];
    
    // check for GET and store method
    if((strstr(line, "GET") == line) && ((strstr(line, "GET") + 3) == first_SP)){
        
        strncpy(method, line, 3);
        method[3] = '\0';
        printf("%s\n", method);
        
        //db1 = method;
        
        err_ptr = strchr(method, ' ');
        if(err_ptr != NULL){
            error(405);
            return false;
        }
    }
    else{
        error(405);
        return false;
    }
    
    // check for presence of " and / then store request-target
    char req_target[last_SP - first_SP];
    
    if(*(first_SP + 1) == '/'){
        strncpy(req_target, first_SP + 1, (last_SP - first_SP));
        req_target[last_SP - first_SP - 1] = '\0';
        printf("%s\n", req_target);
        printf("length of req_target is %lu\n", strlen(req_target));
        
            if(strchr(req_target, '"') != NULL){
                error(400);
                return false;
            }
            
            if(strchr(req_target,' ') != NULL){
                error(400);
                return false;
            }
    }
    else{
        error(501);
        return false;
    }
    
    // store HTTP-version
    char* HTTP_ver_start = strstr(last_SP, "HTTP/1.1");
    char* CR = strchr(last_SP, '\r');
    
    if(HTTP_ver_start == (last_SP + 1)){
        char HTTP_ver[CR - HTTP_ver_start + 1];
        strncpy(HTTP_ver, HTTP_ver_start, (CR - HTTP_ver_start));
        HTTP_ver[CR - HTTP_ver_start + 1] = '\0';
        printf("%s\n", HTTP_ver);
        
        if(strchr(HTTP_ver,' ') != NULL){
            error(505);
            return false;
        }
    }
    else{
        error(505);
        return false;
    }
    
    // separate request-target into absolute path and query
    char* q_mark = strchr(req_target,'?');
    
    if(q_mark == NULL){
        // store abs_path
        strncpy(abs_path, req_target, (last_SP - req_target));
        strcpy(abs_path + (last_SP - req_target), "\0");
        printf("%s\n", abs_path);
        
        *query = '\0';
    }
    else{
        memcpy(abs_path, req_target, (q_mark - req_target)*sizeof(char));
        strcpy(abs_path + (q_mark - req_target), "\0");
        printf("%s\n", abs_path);
        
        memcpy(query, q_mark + 1, (CR - q_mark)*sizeof(char));
        strcpy(query + (CR - q_mark), "\0");
    }
        
    
    return true;
}

/**
 * Returns status code's reason phrase.
 *
 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6
 * https://tools.ietf.org/html/rfc2324
 */
const char* reason(unsigned short code)
{
    switch (code)
    {
        case 200: return "OK";
        case 301: return "Moved Permanently";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 414: return "Request-URI Too Long";
        case 418: return "I'm a teapot";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 505: return "HTTP Version Not Supported";
        default: return NULL;
    }
}

/**
 * Redirects client to uri, given name of uriwha
 */
void redirect(const char* uri)
{
    char* template = "Location: %s\r\n";
    char headers[strlen(template) - 2 + strlen(uri) + 1];
    if (sprintf(headers, template, uri) < 0)
    {
        error(500);
        return;
    }
    respond(301, headers, NULL, 0);
}

/**
 * Reads (without blocking) an HTTP request's headers into memory dynamically allocated on heap.
 * Stores address thereof in *message and length thereof in *length.
 * 
 * The end of the request's headers is signaled by CRLF CRLF
 */
bool request(char** message, size_t* length)
{
    // ensure socket is open
    if (cfd == -1)
    {
        return false;
    }

    // initialize message and its length
    *message = NULL;
    *length = 0;

    // read message 
    while (*length < LimitRequestLine + LimitRequestFields * LimitRequestFieldSize + 4)
    {
        // read request from socket
        BYTE buffer[BYTES]; //buffer is initialized as an array that holds bytes with 512 elements
        ssize_t bytes = read(cfd, buffer, BYTES); // read() reads into from a file descriptor in this case
                                                  // in this case, it reads the file descriptor from the socket, stores 512 elements
                                                  // in the buffer
        if (bytes < 0)
        {
            if (*message != NULL)
            {
                free(*message);
                *message = NULL;
            }
            *length = 0;
            break;
        }

        // reallocate *message to new address of greater size
        *message = realloc(*message, *length + bytes + 1);
        if (*message == NULL)
        {
            *length = 0;
            break;
        }
        // append contents of *message
        memcpy(*message + *length, buffer, bytes);
        // increment length based on num bytes read (512)
        *length += bytes;

        // null-terminate message thus far
        *(*message + *length) = '\0';

        // search for CRLF CRLF in the block that was just read
        int offset = (*length - bytes < 3) ? *length - bytes : 3;
        char* haystack = *message + *length - bytes - offset;
        char* needle = strstr(haystack, "\r\n\r\n");
        // if CRLF CRLF was found..
        if (needle != NULL)
        {
            // trim to one CRLF and null-terminate
            *length = needle - *message + 2;
            *message = realloc(*message, *length + 1);
            if (*message == NULL)
            {
                break;
            }
            *(*message + *length) = '\0';

            // ensure request-line is no longer than LimitRequestLine
            haystack = *message;
            needle = strstr(haystack, "\r\n");
            if (needle == NULL || (needle - haystack + 2) > LimitRequestLine)
            {
                break;
            }

            // count fields in message
            int fields = 0;
            haystack = needle + 2;
            while (*haystack != '\0')
            {
                // look for CRLF
                needle = strstr(haystack, "\r\n");
                if (needle == NULL)
                {
                    break;
                }

                // ensure field is no longer than LimitRequestFieldSize
                if (needle - haystack + 2 > LimitRequestFieldSize)
                {
                    break;
                }

                // look beyond CRLF
                haystack = needle + 2;
            }

            // if we didn't get to end of message, we must have erred
            if (*haystack != '\0')
            {
                break;
            }

            // ensure message has no more than LimitRequestFields
            if (fields > LimitRequestFields)
            {
                break;
            }

            // valid
            return true;
        }
    }

    // invalid
    if (*message != NULL)
    {
        free(*message);
    }
    *message = NULL;
    *length = 0;
    return false;
}

/**
 * Responds to a client with status code, headers, and body of specified length.
 * 
 * Example output:
 * HTTP/1.1 200 OK
    X-Powered-By: PHP/5.5.9-1ubuntu4.12
    Content-type: text/html
    
    <!DOCTYPE html>
    
    <html>
        <head>
            <title>hello</title>
        </head>
        <body>
                        hello, Alice
                </body>
    </html>
 */
void respond(int code, const char* headers, const char* body, size_t length)
{
    // determine Status-Line's phrase
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1
    const char* phrase = reason(code);
    if (phrase == NULL)
    {
        return;
    }

    // respond with Status-Line
    if (dprintf(cfd, "HTTP/1.1 %i %s\r\n", code, phrase) < 0)
    {
        return;
    }

    // respond with headers
    if (dprintf(cfd, "%s", headers) < 0)
    {
        return;
    }

    // respond with CRLF
    if (dprintf(cfd, "\r\n") < 0)
    {
        return;
    }

    // respond with body
    if (write(cfd, body, length) == -1)
    {
        return;
    }

    // log response line
    if (code == 200)
    {
        // green
        printf("\033[32m");
    }
    else
    {
        // red
        printf("\033[33m");
    }
    printf("HTTP/1.1 %i %s", code, phrase);
    printf("\033[39m\n");
}

/**
 * Starts server on specified port rooted at path.
 */
void start(short port, const char* path)
{
    // path to server's root
    root = realpath(path, NULL);
    if (root == NULL)
    {
        stop();
    }

    // ensure root is executable
    if (access(root, X_OK) == -1)
    {
        stop();
    }

    // announce root
    printf("\033[33m");
    printf("Using %s for server's root", root);
    printf("\033[39m\n");

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        stop();
    }

    // allow reuse of address (to avoid "Address already in use")
    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // assign name to socket
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("\033[33m");
        printf("Port %i already in use", port);
        printf("\033[39m\n");
        stop();
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)
    {
        stop();
    }

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)
    {
        stop();
    }
    printf("\033[33m");
    printf("Listening on port %i", ntohs(addr.sin_port));
    printf("\033[39m\n");
}

/**
 * Stop server, deallocating any resources.
 */
void stop(void)
{
    // preserve errno across this function's library calls
    int errsv = errno;

    // announce stop
    printf("\033[33m");
    printf("Stopping server\n");
    printf("\033[39m");

    // free root, which was allocated by realpath
    if (root != NULL)
    {
        free(root);
    }

    // close server socket
    if (sfd != -1)
    {
        close(sfd);
    }

    // stop server
    exit(errsv);
}

/**
 * Transfers file at path with specified type to client.
 */
void transfer(const char* path, const char* type)
{
    // ensure path is readable
    if (access(path, R_OK) == -1)
    {
        error(403);
        return;
    }

    // open file
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        error(500);
        return;
    }

    // load file's content
    BYTE* content;
    size_t length;
    if (load(file, &content, &length) == false)
    {
        error(500);
        return;
    }

    // close file
    fclose(file);

    // prepare response
    char* template = "Content-Type: %s\r\n";
    char headers[strlen(template) - 2 + strlen(type) + 1];
    if (sprintf(headers, template, type) < 0)
    {
        error(500);
        return;
    }

    // respond with file's content
    respond(200, headers, content, length);

    // free file's content
    free(content);
}

/**
 * URL-decodes string, returning dynamically allocated memory for decoded string
 * that must be deallocated by caller.
 */
char* urldecode(const char* s)
{
    // check whether s is NULL
    if (s == NULL)
    {
        return NULL;
    }

    // allocate enough (zeroed) memory for an undecoded copy of s
    char* t = calloc(strlen(s) + 1, 1);
    if (t == NULL)
    {
        return NULL;
    }
    
    // iterate over characters in s, decoding percent-encoded octets, per
    // https://www.ietf.org/rfc/rfc3986.txt
    for (int i = 0, j = 0, n = strlen(s); i < n; i++, j++)
    {
        if (s[i] == '%' && i < n - 2)
        {
            char octet[3];
            octet[0] = s[i + 1];
            octet[1] = s[i + 2];
            octet[2] = '\0';
            t[j] = (char) strtol(octet, NULL, 16);
            i += 2;
        }
        else if (s[i] == '+')
        {
            t[j] = ' ';
        }
        else
        {
            t[j] = s[i];
        }
    }

    // escaped string
    return t;
}
