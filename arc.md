# Arc 客户端安装和使用
文档目录参见：[phabricator doc](https://secure.phabricator.com/book/phabricator/)

## 1. Mac OS X系统安装
	
依赖项： php + git

Mac OS X系统中一般都默认安装php和git，可以通过一下命令检查：
	
	$ php -v
	PHP 5.5.36 (cli) (built: Jun 12 2016 23:47:46)
	Copyright (c) 1997-2015 The PHP Group
	Zend Engine v2.5.0, Copyright (c) 1998-2015 Zend Technologies
	
	$ git --version
	git version 2.3.8 (Apple Git-58)

如果没有安装的话，建议通过brew包管理工具安装：

[brew安装参考网址](http://brew.sh/index_zh-cn.html)
	
	$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
	$ brew install php55
	
	$ brew install git
	
## 2. 安装arc

参见网址： [Qucik Start](https://secure.phabricator.com/book/phabricator/article/arcanist_quick_start/)  [Arcanist User Guide](https://secure.phabricator.com/book/phabricator/article/arcanist/) 

	$ cd ~ && mkdir arcanist_root && cd arcanist_root
	$ git clone https://github.com/phacility/libphutil.git
    $ git clone https://github.com/phacility/arcanist.git
    
    # 添加到PATH路径中
    $ vim ~/.bash_profile  或者 ~/.zshrc  # 添加以下内容
    export PATH=$PATH:~/arcanist_root/arcanist/bin/
    $ source ~/.bash_profile  或者  source ~/.zshrc
    
    $ arc help
    $ arc upgrade
    
    $ arc set-config editor "vim"
    
## 3. arc 使用

参考： 

1. [git与phabricator](http://www.jianshu.com/p/e8d520f65916)
2. [Phabricator命令行工具Arcanist的基本用法](http://ju.outofmemory.cn/entry/138650)

操作命令：
   
	$ mkdir test && cd test && git init
	$ cat .arcconfig
	{
   		"phabricator.uri" : "https://phab.eengoo.com/"
	}
	
	$ arc install-certificate    # 安装相关证书
	 CONNECT  Connecting to "https://phab.eengoo.com/api/"...
	LOGIN TO PHABRICATOR
	Open this page in your browser and login to Phabricator if necessary:

	https://phab.eengoo.com/conduit/login/  # 在浏览器上输入，获取API Token
	
	Then paste the API Token on that page below.

    Paste API Token from that page: xxxxxx
    Writing ~/.arcrc...
 	 SUCCESS!  API Token installed.
 	 
 	 $ touch README.md  # do some cnt modify
 	 $ git add README.md 
 	 $ git commit -a
 	 
 	 $ arc diff
		Linting...
		No lint engine configured for this project.
		Running unit tests...
		No unit test engine is configured for this project.
		 SKIP STAGING  Unable to determine repository for this change.
		Updating commit message...
		Created a new Differential revision:
		        Revision URI: https://phab.eengoo.com/D1
		
		Included changes:
		  A       .arcconfig
		  A       README.md
		  
		  
	   $ arc amend   # 用于修改上次提交的comment
	   
	   审核通过后，则可以使用使用arc land进行提交了
	   $ arc land
	   
	   
## 3. 配置linter

参见 [Arcanist User Guide: Lint](https://secure.phabricator.com/book/phabricator/article/arcanist_lint/)

	$ arc linters
	
	 AVAILABLE   text (Basic Text Linter)
	 AVAILABLE   csharp (C#)
	 AVAILABLE   cpplint (C++ Google's Styleguide)
	 AVAILABLE   cppcheck (C++ linter)
	 AVAILABLE   lessc (CSS pre-processor)
	 AVAILABLE   csslint (CSSLint)
	 AVAILABLE   chmod (Chmod)
	 AVAILABLE   gjslint (Closure Linter)
	 AVAILABLE   coffeelint (CoffeeLint)
	 AVAILABLE   composer (Composer Dependency Manager)
	 AVAILABLE   filename (Filename)
	 AVAILABLE   generated (Generated Code)
	 AVAILABLE   golint (Golint)
	 AVAILABLE   hlint (Haskell Linter)
	 AVAILABLE   json (JSON Lint)
	 AVAILABLE   jsonlint (JSON Lint)
	 AVAILABLE   jscs (JavaScript Code Style)
	 AVAILABLE   jshint (JavaScript error checking)
	 AVAILABLE   nolint (Lint Disabler)
	 AVAILABLE   merge-conflict (Merge Conflicts)
	 AVAILABLE   phpcs (PHP_CodeSniffer)
	 AVAILABLE   phutil-library (Phutil Library Linter)
	 AVAILABLE   pylint (PyLint)
	 AVAILABLE   flake8 (Python Flake8 multi-linter)
	 AVAILABLE   pep8 (Python PEP 8)
	 AVAILABLE   pyflakes (Python PyFlakes)
	 AVAILABLE   ruby (Ruby)
	 AVAILABLE   rubocop (Ruby static code analyzer)
	 AVAILABLE   script-and-regex (Script and Regex)
	 AVAILABLE   xml (SimpleXML Linter)
	 AVAILABLE   spelling (Spellchecker)
	 AVAILABLE   xhpast (XHPAST Lint)
	 AVAILABLE   php (php -l)
	 AVAILABLE   puppet-lint (puppet-lint)
	(Run `arc linters --verbose` for more details.)
	
	$ pip install pip8
	$ cat .arclint
	{
	  "linters": {
	    "pep8": {
	      "type": "pep8",
	      "include": "(\\.py$)"
	    }
	  }
	}
	
	# 如果运行arc lint出错，则最好导出一下环境变量
	$ export LC_CTYPE="en_US.UTF-8"
   $ export LC_ALL="en_US.UTF-8"
	$ arc lint
	
	例如：
	$ arc lint --severity error --everything  # 全部文件检查，但是只显示error
	
	默认情况下，arc lint 只检查最近修改的问题，如果想要检查全部文件，需要添加 --everything
	$ arc lint --help

      lint [options] [paths]
      lint [options] --rev [rev]
          Supports: git, svn, hg
          Run static analysis on changes to check for mistakes. If no files
          are specified, lint will be run on all files which have been modified.

          --amend-all
              When linting git repositories, amend HEAD with all patches
              suggested by lint without prompting.

          --amend-autofixes
              When linting git repositories, amend HEAD with autofix patches
              suggested by lint without prompting.

          --apply-patches
              Apply patches suggested by lint to the working copy without
              prompting.

          --cache bool
              0 to disable cache, 1 to enable. The default value is
              determined by 'arc.lint.cache' in configuration, which defaults
              to off. See notes in 'arc.lint.cache'.

          --engine classname
              Override configured lint engine for this project.

          --everything
              Lint all files in the project.

          --lintall
              Show all lint warnings, not just those on changed lines. When
              paths are specified, this is the default behavior.

          --never-apply-patches
              Never apply patches suggested by lint.

          --only-changed
              Show lint warnings just on changed lines. When no paths are
              specified, this is the default. This differs from only-new in
              cases where line modifications introduce lint on other
              unmodified lines.

          --only-new bool
              Supports: git, hg
              Display only messages not present in the original code.

          --outfile path
              Output the linter results to a file. Defaults to stdout.

          --output format
              With 'summary', show lint warnings in a more compact format.
              With 'json', show lint warnings in machine-readable JSON
              format. With 'none', show no lint warnings. With 'compiler',
              show lint warnings in suitable for your editor. With 'xml',
              show lint warnings in the Checkstyle XML format.

          --rev revision
              Supports: git, hg
              Lint changes since a specific revision.

          --severity string
              Set minimum message severity. One of: 'advice', 'autofix',
              'warning', 'error', 'disabled'. Defaults to 'advice'.
	
	$ pip install pylint
	
	# 修改 vim src/lint/linter/ArcanistPyLintLinter.php 源码
	$git diff
	diff --git a/src/lint/linter/ArcanistPyLintLinter.php b/src/lint/linter/ArcanistPyLintLinter.php
	index 44793a5..f029fa8 100644
	--- a/src/lint/linter/ArcanistPyLintLinter.php
	+++ b/src/lint/linter/ArcanistPyLintLinter.php
	@@ -82,7 +82,7 @@ final class ArcanistPyLintLinter extends ArcanistExternalLinter {
	     $options = array();
	
	     $options[] = '--reports=no';
	-    $options[] = '--msg-template={line}|{column}|{msg_id}|{symbol}|{msg}';
	+    $options[] = '--msg-template={line},{column},{msg_id},{symbol},{msg}';
	
	     // Specify an `--rcfile`, either absolute or relative to the project root.
	     // Stupidly, the command line args above are overridden by rcfile, so be
	@@ -124,7 +124,7 @@ final class ArcanistPyLintLinter extends ArcanistExternalLinter {
	     $messages = array();
	
	     foreach ($lines as $line) {
	-      $matches = explode('|', $line, 5);
	+      $matches = explode(',', $line, 5);
	
	       if (count($matches) < 5) {
	         continue;
	           
	$ pylint --generate-rcfile > pylintrc
	$ cat .arclint
	{
	  "linters": {
	    "pylinter": {
	      "type": "pylint",
	      "pylint.config": "./pylintrc",
	      "include": "(\\.py$)"
	    }
	  }
	}
	$ arch lint
	
	
	
	
node.js centos7   [jshint.com](http://jshint.com/)

	$ yum install npm  # or brew install npm 
	$ npm install -g jshint
	
	$ cat .arclint
	{
	  "linters": {
	    "linter": {
	      "type": "jshint",
	      "include": "(\\.js$)"
	    }
	  }
	}
	
	$ arc lint --everything
	
	
	
	$ arc unit
	
	
Java linter: [see also](https://secure.phabricator.com/D14632)
	
	
	
[Pycharm 集成Pylint](https://my.oschina.net/niuqingshan/blog/637893)
	
 	 
 	 
 
