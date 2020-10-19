# webserv
A webserv written in C++

## RFC Documentation
- [RFC0](https://tools.ietf.org/html/rfc7230) -> Message syntax and routing
- [RFC1](https://tools.ietf.org/html/rfc7231) -> Semantics and Content
- [RFC2](https://tools.ietf.org/html/rfc7232) -> Conditional request
- [RFC3](https://tools.ietf.org/html/rfc7233) -> Range requests
- [RFC4](https://tools.ietf.org/html/rfc7234) -> Caching
- [RFC5](https://tools.ietf.org/html/rfc7235) -> Authentication

## CGI
Common Gateway Interface (CGI) is an interface specification for web servers to execute programs like console applications 
(also called command-line interface programs) running on a server that generates web pages dynamically. 
Such programs are known as CGI scripts or simply as CGIs.

To launch a cgi we need some env variables: [(Documentation)](http://www.cgi101.com/book/index.html)

|        ENV        | Description                                                                                   |
|:------------------|-----------------------------------------------------------------------------------------------|
| AUTH_TYPE         | Identification type, if applicable                                                            |
| CONTENT_LENGTH    | similarly, size of input data (decimal, in octets) if provided via HTTP header                |
| CONTENT_TYPE      | Internet media type of input data if PUT or POST method are used, as provided via HTTP header |
| GATEWAY_INTERFACE | CGI/version                                                                                   |
| PATH_INFO         | path suffix, if appended to URL after program name and a slash                                |
| PATH_TRANSLATED   | corresponding full path as supposed by server, if PATH_INFO is present                        |
| QUERY_STRING      | The query string                                                                              |
| REMOTE_ADDR       | The IP address of the visitor                                                                 |
| REMOTE_IDENT      | see ident, only if server performed such lookup.                                              |
| REMOTE_USER       | The visitor's username (for .htaccess-protected pages) (user/password)                        |
| REQUEST_METHOD    | GET or POST                                                                                   |
| REQUEST_URI       | The interpreted pathname of the requested document or CGI (relative to the document root)     |
| SCRIPT_NAME       | The interpreted pathname of the current CGI (relative to the document root)                   |
| SERVER_NAME       | Your server's fully qualified domain name (e.g. www.cgi101.com)                               |
| SERVER_PORT       | The port number your server is listening on                                                   |
| SERVER_PROTOCOL   | Nom et version du protocole utilisé pour envoyer la requête au script CGI                     |
| SERVER_SOFTWARE   | The server software you're using (e.g. Apache 1.3)                                            |

Les headers de la request sont aussi a mettre en variables environement.

Une fois tout ca set, il faut utiliser fork avec execve et lancer le cgi qui est relatif a l'extension du fichier indique
dans la requete. 
Par exemple, test.php doit lancer php-cgi. Note : c'est le user qui défine quelle cgi est appliqué sur quels extensions/ensemble de fichier grâce au systeme de location déjà implémenté.
Example of how a cgi could be written in C : http://jkorpela.fi/forms/cgic.html

La [doc](http://nginx.org/en/docs/http/request_processing.html) qui explique comment Nginx proces une request avec et sans CGI.

## SELECT
[GOOD VIDEO](https://www.youtube.com/watch?v=Y6pFtgRdUts)
