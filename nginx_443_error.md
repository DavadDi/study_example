# Nginx多主机共享443端口的安全证书问题

## 1. 事情的起源

同一台机器作为Jira/Gitlab的入口，采用Nginx作为统一代理入口，服务端口为443，配置上采用 server_name 进行区分。

	Jira的域名为：  jira.xxxx.com
	Gitlab域名为：  gitlab.xxxx.com

证书为正规机构颁发的统配证书  *.xxxx.com
	
在Nginx.conf中的顺序如下：Jira的配置在前面，Gitlab的配置在后面。

Jira 配置文件样例：

    server {
        listen    443;
        ssl	     on;
        server_name  jira.xxxx.com;
        
        ssl_protocols TLSv1.2;   # only allow tlsv1.2, 要求的安全级别较高
        
        ssl_certificate       "ssl/server.crt";
        ssl_certificate_key   "ssl/server.key";

        location /{
        	proxy_pass http://jira.xxxx.com;
         }
    }
    

Gitlab 配置文件样例：	
  
    server {
    	listen  443;
    	ssl	     on;
    	server_name  gitlab.xxxx.com;
        
		ssl_protocols TLSv1.2 TLSv1.1 TLSv1;
		
		ssl_certificate       "ssl/server.crt";
		ssl_certificate_key   "ssl/server.key";

        location /{
        	proxy_pass http://gitlab.xxxx.com;
         }
    }
    
 同事配置好上述以后，基本上都工作正常，通过浏览器访问基本上都能正常工作，除了个别浏览器Opera。 但是在Jenkins调用编译代码的过程中不能够正常工作，使用 **curl -v** 调试 **gitlab** 时候发现以下错误：
 
	$ curl -v https://gitlab.xxxxx.com/backend
	*   Trying 192.168.1.206...
	* Connected to gitlab.xxxx.com (192.168.1.206) port 443 (#0)
	* Server aborted the SSL handshake
	* Closing connection 0
	curl: (35) Server aborted the SSL handshake


## 2. 问题排查

### 2.1 端口抓包验证

使用 **tcpudmp** 在443端口上抓包分析：

![](http://www.do1618.com/wp-content/uploads/2017/01/443_ssl_error.png)

通过上图发现 **curl** 默认是用的协议是TLS1.0，在客户端发送了 **Client Hello** 后，gitlab 服务网端直接发送了 **Reset** 复位了客户端的连接，但是后采用Chrome浏览器进行访问则能够正常访问Gitlab。 但是Nginx的配置文件中Gitlab的server项中 *ssl_protocols TLSv1.2 TLSv1.1 TLSv1*, 理论上讲应该可以能够正常访问。

问题的根源还是在于Nginx服务端Gitlab项的配置上。 

### 2.2 更换端口进行测试

将 Gitlab 的端口 从 443 修改为 6443，然后使用 curl 进行测试一切正常，因此初步怀疑是 Jira 和 Gitlab 同时监听在 443 端口上，配置项上不一致导致。

将 Jira server中的配置项 **ssl_protocols TLSv1.2** 修改成  **ssl_protocols TLSv1.2 TLSv1.1 TLSv1**，然后继续使用 curl 测试 gitlab，则一切OK。

## 3. 问题分析

通过以上多次测试初步定位，是 Jira 和 Gitlab 同时监听在 443 端口上，不同的配置造成了干扰。 后仔细分析得知：

	由于多个服务同时监听在443端口，采用 server_name 进行区分，这个行为是发生在 Nginx 应用层的Virtual host，但是SSL的协议握手阶段处于 TCP 之上，位于 应用层协议之下， 在SSL/TLS握手阶段，客户端还未明确发起访问HTTP Resource 的 Request，因此还无法确认到 Virtual Host 的相关配置上，因此 SSL/TLS 发起连接过程中的server相关配置，证书和SSL/TLS的相关配置，因此在 443 端口上第一个配置会生效（上例中的Jira的配置中设定的 **ssl_protocols TLSv1.2** 会直接生效在 443 端口上，而后续配置的 Gitlab 的 SSL/TLS 的配置则被忽略）
	
	
## 4. 总结

在 Nginx 中配置多个 server_name 而共享同一个端口时候的配置，需要格外注意，避免多个配置的不一致性。