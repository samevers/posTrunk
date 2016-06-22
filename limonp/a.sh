ll *.hpp | awk -F" " '{print "sh post.sh "$9}' > x.sh
