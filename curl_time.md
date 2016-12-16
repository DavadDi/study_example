curl -w的参数可以获取很多网络访问的细节，其中之一就是可以获取到ssl连接中的tcp handshake和ssl handshake的相关时间：

    $ curl -w "TCP handshake: %{time_connect}, SSL handshake: %{time_appconnect}\n" -so /dev/null https://www.alipay.com
    TCP handshake: 0.031, SSL handshake: 0.146


获取更加详细的连接时间的方式：

	$cat curl-format.txt
	\n
            time_namelookup:  %{time_namelookup}\n
               time_connect:  %{time_connect}\n
            time_appconnect:  %{time_appconnect}\n
           time_pretransfer:  %{time_pretransfer}\n
              time_redirect:  %{time_redirect}\n
         time_starttransfer:  %{time_starttransfer}\n
                            —————\n
                 time_total:  %{time_total}\n
	\n

使用curl：

	$curl -w "@curl-format.txt" -o /dev/null -s http://wordpress.com/

    	-w "@curl-format.txt" tells cURL to use our format file
    	-o /dev/null redirects the output of the request to /dev/null
    	-s tells cURL not to show a progress meter 
    
测试：
	
	$curl -w "@curl-format.txt" -o /dev/null -s http://wordpress.com/

            time_namelookup:  1.521
               time_connect:  2.264
            time_appconnect:  0.000
           time_pretransfer:  2.264
              time_redirect:  0.000
         time_starttransfer:  0.000
                            —————
                 time_total:  2.264
                 

curl帮助文档：<a href="https://curl.haxx.se/docs/manpage.html" target="_blank">curl man</a>

参考：<a href="https://blog.josephscott.org/2011/10/14/timing-details-with-curl/" target="_blank">Timing Details With cURL</a>
