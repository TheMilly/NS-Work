#!/bin/sh

#this file automatically runs energy simulations with various inputs of topology, nodes, and packets
#it finds the remaining energy and appends it to the end of a file energy.tsv
#energy.tsv will be formatted in each row: [topology_size num_nodes packet_size remaining_energy]

#for j in 10 50 100 #number of nodes 
for j in 3 #number of nodes
do

			#run a simulation with these parameters
			#ns task7_parameters.tcl $i $j $k 
			for i in `seq 1 1`
			do
				ns sending_v6.tcl $j $i
				echo $i 
			done
			
done

