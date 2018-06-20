set opt(chan)           Channel/WirelessChannel;	# channel layer
set opt(prop)           Propagation/TwoRayGround;	# radio-propagation model
set opt(netif)          Phy/WirelessPhy;		# physical layer
set opt(ant)            Antenna/OmniAntenna;		# antenna model
set opt(macLayer)       Mac;				# mac layer
set opt(ifq)            Queue/DropTail;			# message queue, ifq
set opt(lenIfq)         50;				# max packet in ifq
set opt(linkLayer)      LL;				# link layer
set opt(routing)        DSDV;			        # routing protocol
set opt(agent)		Agent/BAgent;		# simulation agent

Agent/BAgent set myVal_ 0
Agent/BAgent set myState_[] {}
Agent/BAgent set id_ 0

set opt(ArraySize)	100;

# topology
set opt(numOfNodes)   	5;				# number of mobilenodes
set opt(width)		100;				# topography width
set opt(height)		100;				# topography height
set opt(nodeValRange)	255;				# Initial state of nodes between 0 and nodeValRange

set ns_	        [new Simulator]
set chan_	[new $opt(chan)]
set topo_	[new Topography]

#set basic packet tracing
set tracefd     [open "trace.tr" w]
$ns_ trace-all 	$tracefd

#create god object
set god_	[create-god $opt(numOfNodes)]

# create topology object
$topo_ load_flatgrid $opt(width) $opt(height)

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
	-movementTrace OFF \
	-energyModel "EnergyModel" \
	-rxPower 1.0 \
	-txPower 1.0 \
	-initialEnergy 1000 \
	-sleepPower .5 \
	-transitionPower .2 \
	-transitionTime .001 \
	-idlePower .1


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


for {set i 0} {$i < $opt(numOfNodes)} {incr i} {
	for {set j 0} {$j < $opt(ArraySize)} {incr j} {
		#Use this random val expression once working with basic integers
		set initialVal [expr rand()*$opt(nodeValRange)]

		#I did [i+1] so there isnt a 0 value in array
		#set initialVal [expr $i+1]
		$agent_($i) setArray_i $j $i $initialVal
		
	}
}

set sizeRNG [new RNG]
for {set j 0} {$j < $opt(numOfNodes) } {incr j} {
	set randX [$sizeRNG uniform 0 $opt(width)];
	set randY [$sizeRNG uniform 0 $opt(height)];
	$node_($j) set X_ $randX
	$node_($j) set Y_ $randY
	$node_($j) set Z_ 0.000000000000
}

# schedule
for {set i 0} {$i < $opt(numOfNodes)} {incr i} {
	$ns_ at 20 "$agent_($i) sendTo -1"
	#$ns_ at 20 "$agent_(0) sendTo -1"
}

$ns_ at 40 "$ns_ halt"


# start simulation
$ns_ run;

$ns_ flush-trace
close $tracefd

