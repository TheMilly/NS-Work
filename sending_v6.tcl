lappend auto_path "/usr/lib/tcllib1.18"
package require struct::matrix
package require math::linearalgebra
package require math::statistics
package require csv

set opt(chan)           Channel/WirelessChannel;	# channel layer
set opt(prop)           Propagation/TwoRayGround;	# radio-propagation model
set opt(netif)          Phy/WirelessPhy;		# physical layer
set opt(ant)            Antenna/OmniAntenna;		# antenna model
set opt(macLayer)       Mac;				# mac layer
set opt(ifq)            Queue/DropTail;			# message queue, ifq
set opt(lenIfq)         50;				# max packet in ifq
set opt(linkLayer)      LL;				# link layer
set opt(routing)        DSDV;			        # routing protocol
set opt(agent)		Agent/PAgent;			# simulation agent

# topology
set opt(numOfNodes)   	[lindex $argv 0];		# number of mobilenodes
set opt(width)		1000;				# topography width
set opt(height)		1000;				# topography height
set SD 			5;				# array length
set distance		249;  				# communication range
set CI [lindex $argv 1];
	
puts "Number of nodes $opt(numOfNodes)"
puts "CI is $CI"
 
set ns_	        [new Simulator]
set chan_	[new $opt(chan)]
set topo_	[new Topography]


#Agent/PAgent set myVal_ 0
#Agent/PAgent set CINum_ 0
Agent/PAgent set waitInterval_ 10

#set basic packet tracing
set tracefd     [open "trace.tr" w]
$ns_ trace-all 	$tracefd

#create god object
set god_	[create-god $opt(numOfNodes)]

# create topology object
#$topo_ load_flatgrid $opt(width) $opt(height)

# general node configuration
$ns_ node-config \
	-adhocRouting $opt(routing) \
	-llType $opt(linkLayer) \
	-macType $opt(macLayer) \
	-ifqType $opt(ifq) \
	-ifqLen $opt(lenIfq) \
	-antType $opt(ant) \
	-propType $opt(prop) \
	-phyType $opt(netif) \
	-topoInstance $topo_ \
	-channel $chan_ \
	-agentTrace OFF \
	-routerTrace OFF \
	-macTrace OFF \
	-movementTrace OFF


for {set i 0} {$i < $opt(numOfNodes)} {incr i} {
	# init mobile nodes
	set node_($i) [$ns_ node]

	# disable random motion
	$node_($i) random-motion 0

	# register node.
	$god_ new_node $node_($i)

	# initial node position
	$ns_ initial_node_pos $node_($i) 10

	# init agent
	set agent_($i) [new $opt(agent)]
		
	# attach agent to node.
	$node_($i) attach $agent_($i)
} 


# file name of topology
set sequence	1; # only used for naming the file
set outputFile [open "top_$sequence\_$opt(numOfNodes)\_$opt(width)\_$opt(height)\.tcl" w]
set sizeRNG [new RNG]
$sizeRNG seed 2
set maxSquareDist [expr $distance*$distance];


# root position
set nodeX(0) [format "%.2f" [expr  $opt(width)/ 2]];
set nodeY(0) [format "%.2f" [expr  $opt(height)/ 2]];
set nodeZ(0) 0.0;

$node_(0) set X_ $nodeX(0)
$node_(0) set Y_ $nodeY(0)
$node_(0) set Z_ 0.000000000000

# distribute unconnected nodes
for {set i 1} {$i < $opt(numOfNodes)} {incr i} {
	set connected 0;
	while { $connected==0 } {
		# you can change distribution function of random variable
		set  nodeX($i) [format "%.2f" [expr [$sizeRNG uniform 0 $opt(width)]]];
		set  nodeY($i) [format "%.2f" [expr [$sizeRNG uniform 0 $opt(height)]]];
	    set  nodeZ($i) 0.0;
		for {set j 0} {$j < $i} {incr j} {
			set dx [expr $nodeX($i)-$nodeX($j)];
			set dy [expr $nodeY($i)-$nodeY($j)];
			set delta [expr $dx*$dx+$dy*$dy];
			if { $maxSquareDist > $delta } {
				set connected 1;
				break;
			}
		}
    }
}

for {set i 0} {$i < $opt(numOfNodes)} {incr i} {
	$node_($i) set X_ $nodeX($i)
	$node_($i) set Y_ $nodeY($i)
	$node_($i) set Z_ $nodeZ($i)
}


set NN $opt(numOfNodes)
set global_time 0

proc myRand {min max} {
    expr {int(rand() * ($max + 1 - $min)) + $min}
}

set nnums $NN
set rmax $NN
set nums {}
set fnod(numOfNodes) 0
if {$nnums > $rmax} {
    puts "You can't get $nnums unique values from a range of 1 to $rmax!"
} else {
    while {[llength $nums] < $nnums} {
        set n [myRand 1 $rmax]
        if {$n ni $nums} {lappend nums $n}
    }
    set nums [linsert $nums 0 {}]
    for {set i 1} {$i <= $nnums} {incr i} {
        set fnod($i) [lindex $nums $i]
	puts $fnod($i)
    }
}


#i_p is power iterations
for {set i_p 1} {$i_p <= 1} {incr i_p} {
	set global_time [expr { $global_time+$i_p-1 }]

	# Timer schedule sorted for one round
	set timerSchedule_1 [::math::statistics::random-poisson $opt(numOfNodes) $opt(numOfNodes)]
	set timerSchedule [lsort -integer $timerSchedule_1]
	
	# initialize each node for a consensus iteration
	set transmitTime [expr { $global_time + 0.1 }]
	for {set i 0} {$i < $opt(numOfNodes)} {incr i} {
		$agent_($i) setNNval $opt(numOfNodes)
		$agent_($i) setDataLen $SD
		$agent_($i) setConsensusIter 1  
		$agent_($i) setPowerIter 1  
		#$agent_($i) setCI $CI  
		$ns_ at $transmitTime "$agent_($i) InitilizeNodes $i"
	}

	
	# broadcast after setting timer time for each node
	for {set i 0} {$i < $opt(numOfNodes)} {incr i} {
		set SendingTime  [expr { 0.2 + $i}]
		set nodeVal [expr {1 + $i}]
		set sendNode [expr {$fnod($nodeVal)-1}]
		puts "Sending node is $sendNode"
		$agent_($sendNode) set waitInterval_ $i
		$ns_ at $SendingTime "$agent_($sendNode) sendData waitInterval_"	
	}
}

# exit
$ns_ at 2000 "$ns_ halt"

# start simulation
$ns_ run;

$ns_ flush-trace
close $tracefd
