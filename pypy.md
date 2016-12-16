# pypy 优化记录

## 1. 安装pypy
官方网址：[pypy.org](pypy.org)  [下载页面](http://pypy.org/download.html)

centos 7上安装比较方便：
	
	$ sudo yum install pypy
	

## 2. 安装相关module

	$ sudo pypy -m ensurepip  # 安装pip
	$ /usr/lib64/pypy-5.0.1/bin/pip install --upgrade pip
	
	$ /usr/lib64/pypy-5.0.1/bin/pip install motor==0.5
	$ /usr/lib64/pypy-5.0.1/bin/pip install tornado==4.2.1
	$ /usr/lib64/pypy-5.0.1/bin/pip install redis==2.10.3
	$ /usr/lib64/pypy-5.0.1/bin/pip install pika==0.10.0
	$ /usr/lib64/pypy-5.0.1/bin/pip install jsonschema==2.5.1
	
	
## 3. 测试

	pypy app.py
	
	
## 4. 总结
	
	pypy比较适用于计算密集型的，对于网络请求占用多的情况，优化效果不明显。
	
	
	
	
	