# Zabbix 3.0.4 INSTALL  NOTES
## 1. INSTALL MYSQL
	# rpm -Uvh http://dev.mysql.com/get/mysql-community-release-el7-5.noarch.rpm
	# yum -y install mysql-community-server
	
	# systemctl start mysqld
	
	# mysql_secure_installation    # set mysql root password and security option
	# mysql -uroot -p
	

## 2. INSTALL PHP
	# wget http://ar2.php.net/get/php-5.6.23.tar.gz/from/this/mirror
	# tar xzvf mirror
 	# cd php-5.6.23/

	# yum install gcc make gd-devel libjpeg-devel libpng-devel libxml2-devel bzip2-devel libcurl-devel -y

	#./configure --prefix=/usr/local/php-5.5.7 --with-config-file-path=/usr/local/php-5.5.7/etc \
	--with-bz2 --with-curl --enable-ftp --enable-sockets --disable-ipv6 \
	--with-gd --with-jpeg-dir=/usr/local --with-png-dir=/usr/local --with-freetype-dir=/usr/local \
	--enable-gd-native-ttf --with-iconv-dir=/usr/local --enable-mbstring --enable-calendar \
	--with-gettext --with-libxml-dir=/usr/local --with-zlib --with-pdo-mysql=mysqlnd \
    --with-mysqli=mysqlnd --with-mysql=mysqlnd --enable-dom --enable-xml --enable-fpm \ 
    --with-libdir=lib64 --enable-bcmath

	# make
	# make install
	
	# cp php-5.6.23/php.ini-production /usr/local/php-5.5.7/etc/php.ini
	
	# cd /usr/local/php-5.5.7/etc
	# cp php-fpm.conf.default php-fpm.conf
	# vim /usr/local/php-5.5.7/etc/php.ini
	  max_execution_time = 300
	  memory_limit = 128M
	  post_max_size = 16M
	  upload_max_filesize = 2M
	  max_input_time = 300
	  date.timezone = PRC
	  
	  always_populate_raw_post_data = -1
	  
	# /usr/local/php-5.5.7/sbin/php-fpm
	# netstat -lnt | grep 9000
	  tcp     0    0 127.0.0.1:9000    0.0.0.0:*   LISTEN
	  
## 3. INSTALL NGINX

	# vim /etc/yum.repos.d/nginx.repo
	[nginx]
	name=nginx repo
	baseurl=http://nginx.org/packages/centos/$releasever/$basearch/
	gpgcheck=0
	enabled=1
	
	# yum install nginx
		
	# vim /etc/nginx/conf.d/default.conf
	  server {
        listen 80;
        server_name monitor.eengoo.com;
        access_log zabbix.access.log main;
        index index.html index.php index.html;
        root /usr/share/zabbix;
        location / {
                try_files $uri $uri/ /index.php?$args;
        }

        location ~ \.php$ {
                fastcgi_pass 127.0.0.1:9000;
                fastcgi_index index.php;
                fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
                include fastcgi_params;
        }
	  }	
	  
	 # nginx

### phpinfo
	 
	 # cd /usr/share/zabbix 
	 # vim info.php
		<?php
			phpinfo();
		?>
	
	
### check
	In Web Brower: http://A.B.C/info.php 
	
	# tail -f /var/log/nginx/error.log
	
## 4. INSTALL ZABBIX SERVER

### 4.1 install 	zabbix server && agent
	# rpm -ivh http://repo.zabbix.com/zabbix/3.0/rhel/7/x86_64/zabbix-release-3.0-1.el7.noarch.rpm
	
	# yum install zabbix-server-mysql zabbix-web-mysql
	# yum install zabbix-agent

### 4.2 create zabbix database	
	# mysql -uroot -p<password>
	mysql> create database zabbix character set utf8 collate utf8_bin;
	mysql> grant all privileges on zabbix.* to zabbix@localhost identified by '<password>';
	mysql> quit;
	
	# cd /usr/share/doc/zabbix-server-mysql-3.0.4
	# zcat create.sql.gz | mysql -uroot zabbix -p

### 4.3 config database info	
 	# vi /etc/zabbix/zabbix_server.conf
	DBHost=localhost
	DBName=zabbix
	DBUser=zabbix
	DBPassword=<password>
	

### 4.4 start server and agent
	# systemctl start zabbix-server  Log: /var/log/zabbix/zabbix_server.log
	# systemctl start zabbix-agent   Log: /var/log/zabbix/zabbix_agentd.log
	
### 4.5 addtional operations
	# chown -R nginx:nginx /etc/zabbix/web/
	# chmod -R 777 /etc/zabbix/web/  #avoid install gen config save failed.
	
## 5. VALIDATION

### 5.1 setup
   In Web Browser: http://A.B.C/   setup 

### 5.2 login
   After setup process finished, jump to login url:
   
	UserName: Admin
	Password: zabbix
	
## 6. MONITOR TEMPLATE
1. [vicendominguez](https://github.com/vicendominguez) include: nginx mc sockestat
2. [zabbix](https://github.com/kerncai/zabbix)
	

	


	
	
	
	