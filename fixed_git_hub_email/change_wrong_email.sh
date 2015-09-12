#/bin/bash

git filter-branch --env-filter '

#OLD_EMAIL="diwh@davaddi.local"
OLD_EMAIL="wrong email"
CORRECT_NAME="correct name"
CORRECT_EMAIL="correct email"

if [ "$GIT_COMMITTER_EMAIL" != "$CORRECT_EMAIL"  ]
then
    export GIT_COMMITTER_NAME="$CORRECT_NAME"
    export GIT_COMMITTER_EMAIL="$CORRECT_EMAIL"
fi
if [ "$GIT_AUTHOR_EMAIL" != "$CORRECT_EMAIL"  ]
then
    export GIT_AUTHOR_NAME="$CORRECT_NAME"
    export GIT_AUTHOR_EMAIL="$CORRECT_EMAIL"
fi
' --tag-name-filter cat -- --branches --tags
