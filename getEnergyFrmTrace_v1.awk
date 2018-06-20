#!/usr/bin/awk -f
BEGIN{
#print "energy of node 1"
data=0
}

{
if ($1=="N")
{
    print $3,$5,$7 > "energy_val_3.txt"
}
}
END{
#print "Completed"
}
