```mermaid
sequenceDiagram
    participant Client
    participant WebServer
    participant ConfigParser
    participant NetworkManager
    participant HTTPHandler
    participant FileSystem

    Client->>WebServer: Send Request (HTTP)
    WebServer->>ConfigParser: Load Config (on startup)
    ConfigParser-->>WebServer: Return Config

    WebServer->>NetworkManager: Initialize Sockets
    NetworkManager-->>WebServer: Sockets Ready

    loop Accept Connections
        Client->>NetworkManager: Connect
        NetworkManager-->>WebServer: New Client Socket

        WebServer->>HTTPHandler: Parse HTTP Request
        HTTPHandler->>ConfigParser: Validate Config Rules
        ConfigParser-->>HTTPHandler: Return Validation Status
        HTTPHandler-->>WebServer: Return Parsed Request

        alt Valid Request
            WebServer->>FileSystem: Resolve URI
            FileSystem-->>WebServer: Resource (or Error)
            WebServer->>HTTPHandler: Build HTTP Response
            HTTPHandler-->>WebServer: Response Ready
            WebServer->>NetworkManager: Send Response
            NetworkManager-->>Client: Deliver Response
        else Invalid Request
            WebServer->>HTTPHandler: Build Error Response
            HTTPHandler-->>WebServer: Error Response Ready
            WebServer->>NetworkManager: Send Response
            NetworkManager-->>Client: Deliver Error Response
        end

        WebServer->>NetworkManager: Close Connection (if needed)
        
    end
