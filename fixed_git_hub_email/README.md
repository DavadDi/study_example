修改 Github commit 的作者信息D
http://www.jianshu.com/p/b6add8187c06

Changing author info
 https://help.github.com/articles/changing-author-info/


 1. git clone --bare https://github.com/user/repo.git  # change to your repo
    cd repo.git

 2. change shell info

 3. run

 4. git log > git.log
    cat git.log|grep Author to check email info

 5. git push --force --tags origin 'refs/heads/*'

