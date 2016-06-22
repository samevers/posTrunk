inputfile=$1

cat ${inputfile} |grep -v include | sed -e "s/string/std::string/g"  |  sed -e "s/map/std::map/g"  |sed -e "s/vector/std::vector/g"> x
#cat ${inputfile} | sed -e "s/Trim/Trim_/g"  > x
mv x ${inputfile}


